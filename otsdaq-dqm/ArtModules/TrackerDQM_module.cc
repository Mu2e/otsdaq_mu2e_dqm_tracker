//Author: E. Croft, adapted from code by S. Middleton
//This module (should) produce histograms of data from the straw tracker

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Principal/Handle.h"
#include "fhiclcpp/types/OptionalAtom.h"
#include <TH1F.h>
#include <TBufferFile.h>

#include "otsdaq_mu2e_dqm/otsdaq-dqm/ArtModules/HistoContainer.h"
//#include "otsdaq_mu2e_dqm/otsdaq-dqm/ArtModules/ProtoTypeHistos.h"
#include "otsdaq_mu2e_dqm/otsdaq-dqm/ArtModules/TrackerDQM.h"
#include "otsdaq/MessageFacility/MessageFacility.h"
#include "otsdaq/Macros/CoutMacros.h"
#include "otsdaq/Macros/ProcessorPluginMacros.h"
#include "otsdaq/NetworkUtilities/TCPPublishServer.h"

#include <RecoDataProducts/inc/StrawDigi.hh>
#include <RecoDataProducts/inc/StrawDigiCollection.hh>
#include "mu2e-artdaq-core/Overlays/FragmentType.hh"
#include "mu2e-artdaq-core/Overlays/TrackerFragment.hh"
#include <artdaq-core/Data/Fragment.hh>

#include "DataProducts/inc/TrkTypes.hh"
#include "DataProducts/inc/StrawId.hh"


namespace ots {
	class TrackerDQM : public art::EDAnalyzer {
	public:
		struct Config {
			using Name=fhicl::Name;
			using Comment=fhicl::Comment;
			fhicl::Atom<int> port{Name("port"), Comment("This parameter sets the port where the histogram will be broadcast")};
			fhicl::Atom<art::InputTag> trkTag{Name("trkTag"), Comment("trkTag")};
			fhicl::Sequence<std::string> histType{Name("histType"), Comment("This parameter determines which quantity is histogrammed")};
		};

		typedef art::EDAnalyzer::Table<Config> Parameters;


		explicit TrackerDQM(Parameters const& conf);

		void analyze(art::Event const& event) override;
		void beginRun(art::Run const&) override;
		void beginJob() override;
		void endJob() override;

		void PlotRate(art::Event const& e);

			
	private:
		Config _conf;
		int _port;
		art::InputTag trkFragmentsTag_;
		std::vector<std::string> _histType;
		art::ServiceHandle<art::TFileService> tfs;
		//ProtoTypeHistos *histos = new ProtoTypeHistos();
		HistoContainer *pedestal_histos = new  HistoContainer();
		HistoContainer *straw_histos = new HistoContainer();
		TCPPublishServer *tcp = new TCPPublishServer(_port);
	};
}


ots::TrackerDQM::TrackerDQM(Parameters const& conf) : art::EDAnalyzer(conf), _conf(conf()), _port(conf().port()), trkFragmentsTag_(conf().trkTag()), _histType(conf().histType()) {
}

void ots::TrackerDQM::beginJob() {

	std::cout << "Beginning job" << std::endl;
	//histos->BookHistos(tfs, "Pedestal");
	for(int station = 0; station < 18; station++) {
                                for(int plane = 0; plane < 3; plane++) {
                                        for(int panel = 0; panel < 7; panel++) {
                                                for(int straw = 0; straw < 97; straw++) {
                                                        pedestal_histos->BookHistos(tfs, "Pedestal" + std::to_string(station) + " " + std::to_string(plane) + " " + std::to_string(panel) + " " + std::to_string(straw), station, plane, panel, straw);
                                                }
                                        }
                                }
                        }

	for(int station = 0; station < 18; station++) {
                                for(int plane = 0; plane < 3; plane++) {
                                        for(int panel = 0; panel < 7; panel++) {
                                                        straw_histos->BookHistos(tfs, "Straw Hits" + std::to_string(station) + " " + std::to_string(plane) + " " + std::to_string(panel) + " " + std::to_string(0), station, plane, panel, 0);
                                        }
                                }
                        }


}


void ots::TrackerDQM::analyze(art::Event const& event) {
	//double value = 2;
	
	//histos->Test._FirstHist->Fill(value);
	//TBufferFile message(TBuffer::kWrite);
	//message.WriteObject(histos->Test._FirstHist);
	
	//tcp->broadcastPacket(message.Buffer(), message.Length());

	//__MOUT__ << "I am Alive!" << std::endl;
	art::Handle<artdaq::Fragments> trkFragment;
	
	event.getByLabel(trkFragmentsTag_, trkFragment);
		
	if(!trkFragment.isValid()){
		__MOUT__ << "Invalid fragment" << std::endl;
		return;
	}

	auto numTrkFrags = trkFragment->size();
	
	__MOUT__ << "number of trkFrags = " << numTrkFrags << std::endl;
	
	for (size_t idx = 0; idx < numTrkFrags; ++idx) {
		auto curHandle = trkFragment;
		size_t curIdx = idx;
		const auto& fragment((*curHandle)[curIdx]);

		mu2e::TrackerFragment cc(fragment);
		
		for(size_t curBlockIdx=0; curBlockIdx<cc.block_count(); curBlockIdx++) { //iterate over straws
		  auto block_data = cc.dataAtBlockIndex(curBlockIdx);
			if(block_data == nullptr) {
				mf::LogError("TrackerDQM") << "Unable to retrieve header from block " << curBlockIdx << "!" << std::endl;
				continue;
			}
			auto hdr = block_data->GetHeader();
			if(hdr.GetPacketCount()>0) {
				auto trkDatas = cc.GetTrackerData(curBlockIdx);
				if(trkDatas.empty()) {
					mf::LogError("TrackerDQM") << "Error retrieving Tracker data from DataBlock " << curBlockIdx << "!";
					continue;
				}

				for(auto& trkData : trkDatas) {
				mu2e::StrawId sid(trkData.first.StrawIndex);
				mu2e::TrkTypes::TDCValues tdc = {static_cast<uint16_t>(trkData.first.TDC0()), static_cast<uint16_t>(trkData.first.TDC1())};
				mu2e::TrkTypes::TOTValues tot = {trkData.first.TOT0, trkData.first.TOT1};
				mu2e::TrkTypes::ADCWaveform adcs;
				std::copy_n(trkData.second.begin(), adcs.size(), adcs.begin());
					
				for(std::string name : _histType) {
					if(name == "pedestal") {
						straw_fill(pedestal_histos, pedestal_est(adcs), "Pedestal", tfs, sid);
					} else if(name == "strawHits") {
						panel_fill(straw_histos, sid.straw(), "Straw Hits", tfs, sid);
					} //else if(name == "maxadc") {
					//	straw_fill(histos, max_adc(adcs), "Maximum ADC Value", tfs, sid);
					//} else if(name == "deltaT") {
					//	straw_fill(histos, tdc[1]-tdc[0], "delta T", tfs, sid);
					//} else if(name == "peak_pedestal") {
 					//	straw_fill(histos, max_adc(adcs) - pedestal_est(adcs), "Peak - Pedestal", tfs, sid);
					 else {
						__MOUT_ERR__ << "Unrecognized histogram type" << std::endl;
					}

				}
				}

			}
		}
	}
	
	TBufferFile message(TBuffer::kWrite);
	//message.WriteObject(histos->Test._FirstHist);

	for(int idx = 0; idx < int(pedestal_histos->histograms.size()); idx ++) {
		message.WriteObject(pedestal_histos->histograms[idx]._Hist);
		tcp->broadcastPacket(message.Buffer(), message.Length());

		message.Reset();
	}
	for(int idx = 0; idx < int(straw_histos->histograms.size()); idx ++) {
                message.WriteObject(straw_histos->histograms[idx]._Hist);
                tcp->broadcastPacket(message.Buffer(), message.Length());

                message.Reset();

		//std::this_thread::sleep_for(std::chrono::milliseconds(100));

	//message.WriteObject(histo);
        //tcp->broadcastPacket(message.Buffer(), message.Length());
	}
}

void ots::TrackerDQM::endJob() {

}

void ots::TrackerDQM::beginRun(const art::Run& run) {

}



DEFINE_ART_MODULE(ots::TrackerDQM)
