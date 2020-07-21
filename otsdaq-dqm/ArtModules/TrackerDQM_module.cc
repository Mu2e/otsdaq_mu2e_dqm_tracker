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

#include "otsdaq_mu2e_dqm/otsdaq-dqm/ArtModules/ProtoTypeHistos.h"
#include "otsdaq_mu2e_dqm/otsdaq-dqm/ArtModules/TrackerDQM.h"
#include "otsdaq/MessageFacility/MessageFacility.h"
#include "otsdaq/Macros/CoutMacros.h"
#include "otsdaq/Macros/ProcessorPluginMacros.h"
#include "otsdaq/NetworkUtilities/TCPPublishServer.h"

#include <RecoDataProducts/inc/StrawDigi.hh>
#include <RecoDataProducts/inc/StrawDigiCollection.hh>
#include "mu2e-artdaq-core/Overlays/FragmentType.hh"
#include "mu2e-artdaq-core/Overlays/ArtFragmentReader.hh"
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
		art::ServiceHandle<art::TFileService> tfs;
		ProtoTypeHistos *histos = new ProtoTypeHistos("Hits");
		TCPPublishServer *tcp = new TCPPublishServer(_port);
	};
}


ots::TrackerDQM::TrackerDQM(Parameters const& conf) : art::EDAnalyzer(conf), _conf(conf()), _port(conf().port()), trkFragmentsTag_(conf().trkTag()) {
}

void ots::TrackerDQM::beginJob() {
	std::cout << "Beginning job" << std::endl;
	histos->BookHistos(tfs);
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
		
	//if(!trkFragment.isValid()){
	//	__MOUT__ << "Invalid fragment" << std::endl;
	//	return;
	//}

	auto numTrkFrags = trkFragment->size();
	
	__MOUT__ << "number of trkFrags = " << numTrkFrags << std::endl;
	
	for (size_t idx = 0; idx < numTrkFrags; ++idx) {
		auto curHandle = trkFragment;
		size_t curIdx = idx;
		const auto& fragment((*curHandle)[curIdx]);

		mu2e::ArtFragmentReader cc(fragment);
		
		//this bit doesn't work, backpressure condition
		//int sindex = cc.GetTrackerData(0)->StrawIndex;

		//histos->Test._FirstHist->Fill(sindex);

		for(size_t curBlockIdx=0; curBlockIdx<cc.block_count(); curBlockIdx++) {
			auto hdr = cc.GetHeader(curBlockIdx);
			if(hdr == nullptr) {
				mf::LogError("TrackerDQM") << "Unable to retrieve header from block " << curBlockIdx << "!" << std::endl;
				continue;
			}
			__MOUT__ << "hdr->PacketCount = " << hdr->PacketCount << std::endl;	
			if(hdr->PacketCount>0) {
				auto trkData = cc.GetTrackerData(curBlockIdx);
				if(trkData == nullptr) {
					mf::LogError("TrackerDQM") << "Error retrieving Tracker data from DataBlock " << curBlockIdx << "!";
					continue;
				}
				mu2e::StrawId sid(trkData->StrawIndex);
				mu2e::TrkTypes::TDCValues tdc = {trkData->TDC0, trkData->TDC1};
				mu2e::TrkTypes::TOTValues tot = {trkData->TOT0, trkData->TOT1};
				mu2e::TrkTypes::ADCWaveform adcs = trkData->Waveform();

				int sum{0};
				unsigned short maxadc{0};
				//for (auto adc : adcs ) {
				//	sum += adc;
				//	maxadc = std::max(maxadc, adc);
				//}
				//__MOUT__ << "adc = " << sum << std::endl;
				//__MOUT__ << "TOT cal = " << tot[0] << std::endl;
				//__MOUT__ << "TOT hv = " << tot[1] << std::endl;

				//compute the average of the first 3 adc samples for pedestal estimate
				int PedEst = pedestal_est(adcs);

				histos->Test._FirstHist->Fill(PedEst);
			}
		}
	}
	
	TBufferFile message(TBuffer::kWrite);
	message.WriteObject(histos->Test._FirstHist);

	tcp->broadcastPacket(message.Buffer(), message.Length());
}

void ots::TrackerDQM::endJob() {

}

void ots::TrackerDQM::beginRun(const art::Run& run) {

}



DEFINE_ART_MODULE(ots::TrackerDQM)
