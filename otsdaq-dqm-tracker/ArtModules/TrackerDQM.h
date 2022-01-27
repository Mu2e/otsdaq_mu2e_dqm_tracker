// Some functions for producing histograms in trackerDQM. Author E. Croft
#include "Offline/DataProducts/inc/StrawId.hh"
#include "Offline/DataProducts/inc/TrkTypes.hh"
#include "art/Framework/Core/ModuleMacros.h"
#include "art_root_io/TFileService.h"
#include "otsdaq-dqm-tracker/ArtModules/TrackerDQMHistoContainer.h"
#include "otsdaq/Macros/ProcessorPluginMacros.h"

namespace ots {

int pedestal_est(mu2e::TrkTypes::ADCWaveform adc) {
  int sum{0};

  if (adc.size() == 0) return 0;

  size_t i_max = adc.size() > 3 ? 3 : adc.size();

  for (size_t i = 0; i < i_max; ++i) {
    sum += adc[i];
  }
  int average = sum / i_max;
  return average;
}

unsigned short max_adc(mu2e::TrkTypes::ADCWaveform adcs) {
  unsigned short maxadc{0};

  for (auto adc : adcs) {
    maxadc = std::max(maxadc, adc);
  }

  return maxadc;
}





void summary_fill(TrackerDQMHistoContainer *histos,  art::ServiceHandle<art::TFileService> tfs, mu2e::StrawId sid) {
  //  __MOUT__ << "filling Summary histograms..."<< std::endl;

  if (histos->histograms.size() == 0) {
    __MOUT__ << "No histograms booked. Should they have been created elsewhere?"
             << std::endl;
  } else {
    
    histos->histograms[0]._Hist->Fill(sid.uniquePanel());
    histos->histograms[1]._Hist->Fill(sid.plane());
    
  }
}


void panel_fill(TrackerDQMHistoContainer *histos, int data, std::string title,
                art::ServiceHandle<art::TFileService> tfs, mu2e::StrawId sid) {
  __MOUT__ << title.c_str() + std::to_string(sid.plane()) + " " +
                  std::to_string(sid.panel()) + " " +
                  std::to_string(sid.straw())
           << std::endl;

  if (histos->histograms.size() == 0) {
    __MOUT__ << "No histograms booked. Should they have been created elsewhere?"
             << std::endl;
  } else {
    for (int histIdx = 0; histIdx < int(histos->histograms.size()); ++histIdx) {
      if ((sid.straw() == histos->histograms[histIdx].straw) &&
          (sid.panel() == histos->histograms[histIdx].panel) &&
          (sid.plane() == histos->histograms[histIdx].plane) ) { // if the histogram does exist, fill it
        histos->histograms[histIdx]._Hist->Fill(data);
        __MOUT__ << "number of histos: " << histos->histograms.size()
                 << std::endl;
        break;
      }

      if (histIdx == int(histos->histograms.size() - 1)) { 
        __MOUT__ << "Cannot find histogram: "
                 << title + std::to_string(sid.plane()) + " " +
                        std::to_string(sid.panel()) + " " +
                        std::to_string(sid.straw())
                 << std::endl;
      }
    }
  }
}

void pedestal_fill(TrackerDQMHistoContainer *histos, int data, std::string title,
                art::ServiceHandle<art::TFileService> tfs, mu2e::StrawId sid) {
  if (histos->histograms.size() == 0) {
    __MOUT__ << "No histograms booked. Should they have been created elsewhere?"
             << std::endl;
  } else {
    bool   foundHist(false);
    for (int histIdx = 0; histIdx < int(histos->histograms.size()); ++histIdx) {
      //__MOUT__ << "[TrackerDWM::panel_fill] hist block:  "<<histos->histograms[histIdx].plane <<", "<< histos->histograms[histIdx].panel<< std::endl;

      if ((sid.panel()   == histos->histograms[histIdx].panel) &&
          (sid.plane()   == histos->histograms[histIdx].plane) ) {
        histos->histograms[histIdx]._Hist->Fill(data);
	//	__MOUT__ << "[TrackerDWM::panel_fill] filled hist "<<sid.plane() <<", "<< sid.panel()<< std::endl;
	foundHist = true;
        break;
      }
      
    }
    if (!foundHist){
      __MOUT__ << "Cannot find histogram: "
	       << title + "_"+std::to_string(sid.plane()) + "_" +
	std::to_string(sid.panel())
	       << std::endl;
    }
  }
}

} // namespace ots
