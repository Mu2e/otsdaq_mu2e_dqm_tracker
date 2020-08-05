#ifndef _ProtoTypeHistos_h_
#define _ProtoTypeHistos_h_


#include "otsdaq/NetworkUtilities/TCPPublishServer.h"
#include "art_root_io/TFileService.h"
#include "art_root_io/TFileDirectory.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include <string>
#include <TH1F.h>
#include "DataProducts/inc/StrawId.hh"

namespace ots
{

class HistoContainer
{
    public:
        HistoContainer(){};
	    virtual ~HistoContainer(void){};
        struct  summaryInfoHist_  {
        	TH1F *_Hist;
		int station;
		int plane;
		int panel;
		int straw;
        	summaryInfoHist_() {
        		_Hist = NULL;
         	}
         };
	
	std::vector<summaryInfoHist_> histograms;

        void BookHistos(art::ServiceHandle<art::TFileService>  tfs, std::string Title, mu2e::StrawId sid){
        	histograms.push_back(summaryInfoHist_());
		art::TFileDirectory TestDir = tfs->mkdir("TestingHistos");
		this->histograms[histograms.size() - 1]._Hist = TestDir.make<TH1F>(Title.c_str(), Title.c_str(), 1000,0,4000);
		this->histograms[histograms.size() - 1].station = sid.station();
		this->histograms[histograms.size() - 1].plane = sid.plane();
		this->histograms[histograms.size() - 1].panel = sid.panel();
		this->histograms[histograms.size() - 1].straw = sid.straw();
        }

        void BookHistos(TDirectory *dir, std::string Title, mu2e::StrawId sid){
        	dir->mkdir("TestingHistos", "TestingHistos");
		histograms.push_back(summaryInfoHist_());
		this->histograms[histograms.size() - 1]._Hist = new TH1F(Title.c_str(), Title.c_str(), 1000, 0,4000);
		this->histograms[histograms.size() - 1].station = sid.station();
		this->histograms[histograms.size() - 1].plane = sid.plane();
		this->histograms[histograms.size() - 1].panel = sid.panel();
		this->histograms[histograms.size() - 1].straw = sid.straw();
        }

	void BookHistos(art::ServiceHandle<art::TFileService>  tfs, std::string Title){
                histograms.push_back(summaryInfoHist_());
                art::TFileDirectory TestDir = tfs->mkdir("TestingHistos");
                this->histograms[histograms.size() - 1]._Hist = TestDir.make<TH1F>(Title.c_str(), Title.c_str(), 1000,0,4000);
	}

	void BookHistos(TDirectory *dir, std::string Title){
                dir->mkdir("TestingHistos", "TestingHistos");
                histograms.push_back(summaryInfoHist_());
                this->histograms[histograms.size() - 1]._Hist = new TH1F(Title.c_str(), Title.c_str(), 1000, 0,4000);
	}
};

}

#endif
