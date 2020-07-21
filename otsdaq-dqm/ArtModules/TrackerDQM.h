//Some functions for producing histograms in trackerDQM. Author E. Croft
#include "DataProducts/inc/TrkTypes.hh"

namespace ots {

	int pedestal_est(mu2e::TrkTypes::ADCWaveform adc) {
		int sum{0};
		for(int i = 0; i < 3; ++i) {
			sum += adc[i];
		}

		return sum/3;
	}

	//int hits
}
