
# cet_build_plugin(Occupancy art::module LIBRARIES REG
#   )

# cet_build_plugin(TrackerDQM art::module LIBRARIES REG
#   )

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
otsdaq_mu2e::otsdaq-mu2e_ArtModules
otsdaq::NetworkUtilities
ROOT::Hist
ROOT::Tree
ROOT::Core
ROOT::RIO
ROOT::Gui
)



install_headers()
install_source()
