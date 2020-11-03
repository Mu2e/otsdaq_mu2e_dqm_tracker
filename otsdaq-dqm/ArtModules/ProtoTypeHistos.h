#ifndef _ProtoTypeHistos_h_
#define _ProtoTypeHistos_h_


#include "otsdaq/NetworkUtilities/TCPPublishServer.h"
#include "art_root_io/TFileService.h"
#include "art_root_io/TFileDirectory.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include <string>
#include <TH1F.h>

namespace ots
{

class ProtoTypeHistos
{
    public:
        ProtoTypeHistos(){};
	    virtual ~ProtoTypeHistos(void){};
        struct  summaryInfoHist_  {
            TH1F *_FirstHist;
            summaryInfoHist_() {
            _FirstHist = NULL;
            }
         };
        const std::string _title;
        summaryInfoHist_ Test;

        void BookHistos(art::ServiceHandle<art::TFileService>  tfs, std::string Title){
            art::TFileDirectory TestDir = tfs->mkdir("TestingHistos");
	    this->Test._FirstHist = TestDir.make<TH1F>(Title.c_str(), Title.c_str(), 1000, 0,110);
        }

        void BookHistos(TDirectory *dir, std::string Title){
            dir->mkdir("TestingHistos", "TestingHistos");
	    this->Test._FirstHist = new TH1F(Title.c_str(), Title.c_str(), 1000, 0,110);
        }
};

}

#endif
