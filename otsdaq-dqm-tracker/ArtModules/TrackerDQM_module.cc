// Author: E. Croft, adapted from code by S. Middleton
// This module (should) produce histograms of data from the straw tracker

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "fhiclcpp/types/OptionalAtom.h"
#include <TBufferFile.h>
#include <TH1F.h>

#include "otsdaq-dqm-tracker/ArtModules/HistoContainer.h"
#include "otsdaq-dqm-tracker/ArtModules/TrackerDQM.h"
#include "otsdaq/Macros/CoutMacros.h"
#include "otsdaq/Macros/ProcessorPluginMacros.h"
#include "otsdaq/MessageFacility/MessageFacility.h"
#include "otsdaq/NetworkUtilities/TCPSendClient.h"
#include "otsdaq-mu2e/ArtModules/HistoSender.hh"

#include "mu2e-artdaq-core/Overlays/FragmentType.hh"
#include "mu2e-artdaq-core/Overlays/TrackerFragment.hh"
#include <Offline/RecoDataProducts/inc/StrawDigi.hh>
#include <Offline/RecoDataProducts/inc/StrawDigiCollection.hh>
#include <artdaq-core/Data/Fragment.hh>

#include "Offline/DataProducts/inc/StrawId.hh"
#include "Offline/DataProducts/inc/TrkTypes.hh"

namespace ots {
  class TrackerDQM : public art::EDAnalyzer {
  public:
    struct Config {
      using Name = fhicl::Name;
      using Comment = fhicl::Comment;
      fhicl::Atom<int>             port    { Name("port"),      Comment("This parameter sets the port where the histogram will be sent") };
      fhicl::Atom<std::string>     address { Name("address"),   Comment("This paramter sets the IP address where the histogram will be sent") };
      fhicl::Atom<art::InputTag>   trkTag  { Name("trkTag"),    Comment("trkTag") };
      fhicl::Sequence<std::string> histType{ Name("histType"),  Comment("This parameter determines which quantity is histogrammed") };
      fhicl::Atom<int>             freqDQM { Name("freqDQM"),   Comment("Frequency for sending histograms to the data-receiver") };
      fhicl::Atom<int>             diag    { Name("diagLevel"), Comment("Diagnostic level"), 0 };
    };

    typedef art::EDAnalyzer::Table<Config> Parameters;

    explicit TrackerDQM(Parameters const& conf);

    void analyze(art::Event const& event) override;
    void beginRun(art::Run const&) override;
    void beginJob() override;
    void endJob() override;

    void PlotRate(art::Event const& e);

  private:
    Config          conf_;
    int             port_;
    std::string     address_;
    art::InputTag   trkFragmentsTag_;
    std::vector<std::string> histType_;
    int             freqDQM_,  diagLevel_, evtCounter_;
    art::ServiceHandle<art::TFileService> tfs;
    // ProtoTypeHistos *histos = new ProtoTypeHistos();
    HistoContainer* pedestal_histos = new HistoContainer();
    HistoContainer* straw_histos = new HistoContainer();
    HistoSender*    histSender_;
    
  };
} // namespace ots

ots::TrackerDQM::TrackerDQM(Parameters const& conf)
  : art::EDAnalyzer(conf), conf_(conf()), port_(conf().port()), address_(conf().address()),
    trkFragmentsTag_(conf().trkTag()), histType_(conf().histType()), 
    freqDQM_(conf().freqDQM()), diagLevel_(conf().diag()), evtCounter_(0) {
  histSender_  = new HistoSender(address_, port_);
}

void ots::TrackerDQM::beginJob() {
  __MOUT__ << "[TrackerDQM::beginJob] Beginning job" << std::endl;
  // histos->BookHistos(tfs, "Pedestal");
  for (int station = 0; station < 18; station++) {
    for (int plane = 0; plane < 3; plane++) {
      for (int panel = 0; panel < 7; panel++) {
	for (int straw = 0; straw < 97; straw++) {
	  pedestal_histos->BookHistos(tfs,
				      "Pedestal" + std::to_string(station) +
				      " " + std::to_string(plane) + " " +
				      std::to_string(panel) + " " +
				      std::to_string(straw),
				      station, plane, panel, straw);
	}
      }
    }
  }

  for (int station = 0; station < 18; station++) {
    for (int plane = 0; plane < 3; plane++) {
      for (int panel = 0; panel < 7; panel++) {
	straw_histos->BookHistos(tfs,
				 "Straw Hits" + std::to_string(station) + " " +
				 std::to_string(plane) + " " +
				 std::to_string(panel) + " " +
				 std::to_string(0),
				 station, plane, panel, 0);
      }
    }
  }
}

void ots::TrackerDQM::analyze(art::Event const& event) {
  ++evtCounter_;
  //__MOUT__ << "I am Alive!" << std::endl;
  art::Handle<artdaq::Fragments> trkFragment;

  event.getByLabel(trkFragmentsTag_, trkFragment);

  if (!trkFragment.isValid()) {
    __MOUT__ << "[TrackerDQM::analyze] Invalid fragment" << std::endl;
    return;
  }

  auto numTrkFrags = trkFragment->size();

  if (diagLevel_>0){
    __MOUT__ << "[TrackerDQM::analyze] number of trkFrags = " << numTrkFrags << std::endl;
  }
  for (size_t idx = 0; idx < numTrkFrags; ++idx) {
    auto curHandle = trkFragment;
    size_t curIdx = idx;
    const auto& fragment((*curHandle)[curIdx]);

    mu2e::TrackerFragment cc(fragment);

    for (size_t curBlockIdx = 0; curBlockIdx < cc.block_count();
	 curBlockIdx++) { // iterate over straws
      auto block_data = cc.dataAtBlockIndex(curBlockIdx);
      if (block_data == nullptr) {
	mf::LogError("TrackerDQM") << "Unable to retrieve header from block "
				   << curBlockIdx << "!" << std::endl;
	continue;
      }
      auto hdr = block_data->GetHeader();
      if (hdr->GetPacketCount() > 0) {
	auto trkDatas = cc.GetTrackerData(curBlockIdx);
	if (trkDatas.empty()) {
	  mf::LogError("TrackerDQM")
	    << "Error retrieving Tracker data from DataBlock " << curBlockIdx
	    << "!";
	  continue;
	}

	for (auto& trkData : trkDatas) {
	  mu2e::StrawId sid(trkData.first->StrawIndex);
	  //mu2e::TrkTypes::TDCValues tdc = {	    static_cast<uint16_t>(trkData.first->TDC0()),	    static_cast<uint16_t>(trkData.first->TDC1()) };
	  //mu2e::TrkTypes::TOTValues tot = { trkData.first->TOT0,					    trkData.first->TOT1 };
	  mu2e::TrkTypes::ADCWaveform adcs(trkData.second.begin(), trkData.second.end());

	  for (std::string name : histType_) {
	    if (name == "pedestal") {
	      straw_fill(pedestal_histos, pedestal_est(adcs), "Pedestal", tfs, sid);
	    }
	    else if (name == "strawHits") {
	      panel_fill(straw_histos, sid.straw(), "Straw Hits", tfs, sid);
	    } // else if(name == "maxadc") {
	    //	straw_fill(histos, max_adc(adcs), "Maximum ADC Value", tfs,
	    //sid); } else if(name == "deltaT") { 	straw_fill(histos,
	    //tdc[1]-tdc[0], "delta T", tfs, sid); } else if(name ==
	    //"peak_pedestal") { 	straw_fill(histos, max_adc(adcs) -
	    //pedestal_est(adcs), "Peak - Pedestal", tfs, sid);
	    else {
	      __MOUT_ERR__ << "Unrecognized histogram type" << std::endl;
	    }
	  }
	}
      }
    }
  }

  if (evtCounter_ % freqDQM_  != 0) return;
  
  if (diagLevel_>0){
    __MOUT__ << "[TrackerDQM::analyze] preparing the BUFFER..."<< std::endl;
  }

  //send a packet AND reset the histograms
  for (std::string name : histType_) {
    if (diagLevel_>0){
      __MOUT__ << "[TrackerDQM::analyze] collecting histograms from the block: "<< name << std::endl;
    }
    std::vector<TH1*>   hists_to_send;
    if (name == "pedestal") {   
      //prepare the vector of histograms
      for (size_t i = 0; i < pedestal_histos->histograms.size(); i++) {
	hists_to_send.push_back((TH1*)pedestal_histos->histograms[i]._Hist->Clone());
	pedestal_histos->histograms[i]._Hist->Reset();
      }
    }else if(name == "strawHits") {
      //prepare the vector of histograms
      for (size_t i = 0; i < straw_histos->histograms.size(); i++) {
	hists_to_send.push_back((TH1*)straw_histos->histograms[i]._Hist->Clone());
	straw_histos->histograms[i]._Hist->Reset();
      }
    }
    if (hists_to_send.size() != 0) {
      std::string  tagName = "Tracker_"+name;
      histSender_->sendHistograms(tagName, hists_to_send);
      if (diagLevel_>0){
	__MOUT__ << "[TrackerDQM::analyze] sent histograms with tag: "<< tagName << std::endl;
      }
    }
  }
  
}

void ots::TrackerDQM::endJob() {}

void ots::TrackerDQM::beginRun(const art::Run& run) {}

DEFINE_ART_MODULE(ots::TrackerDQM)
