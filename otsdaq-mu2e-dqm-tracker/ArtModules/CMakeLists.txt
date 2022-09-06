

# cet_build_plugin(BeamMonitor art::module LIBRARIES REG
# art_root_io::TFileService_service
# otsdaq::NetworkUtilities
# ${ROOT_BASIC_LIB_LIST}
# mu2e_Mu2eUtilities
# )


cet_build_plugin(Occupancy art::module LIBRARIES REG
art_root_io::TFileService_service
artdaq_core::artdaq-core_Data
otsdaq::NetworkUtilities
ROOT::RIO
ROOT::Gui
)

cet_build_plugin(TrackerDQM art::module LIBRARIES REG
art_root_io::TFileService_service
artdaq_core_mu2e::Overlays
#otsdaq_mu2e::ArtModules
otsdaq_mu2e::otsdaq-mu2e_ArtModules
# ROOT::Hist
)

#cet_build_plugin(TriggerRates art::module LIBRARIES REG
#art_root_io::TFileService_service
#artdaq_core::artdaq-core_Data
#artdaq::DAQdata
#otsdaq::Macros
#)

