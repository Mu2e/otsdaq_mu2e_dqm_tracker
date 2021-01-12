art_make( BASENAME_ONLY
 LIBRARY_NAME otsdaq-dqm_ArtModules
 SUBDIRS detail
 NO_PLUGINS
 DICT_DICTIONARIES
 ${ART_PERSISTENCY_PROVENANCE}
 ${CANVAS_PERSISTENCY_PROVENANCE}
 LIB_LIBRARIES
canvas
 mu2e-artdaq-core_Overlays
 artdaq-core_Utilities
 artdaq_ArtModules
 artdaq-core_Data
 artdaq-core_Core
  art_Framework_Core
  art_Framework_IO_Sources
  art_Framework_Principal
  art_Framework_Services
  art_Persistency_Provenance
  MF_MessageLogger
  ${ROOT_CINTEX}
  ${ROOT_RIO}
  ${ROOT_CORE}
  ${ROOT_NET}
  ${ROOT_REFLEX}
  cetlib
  cetlib_except
  DTCInterface
) #add libs here if want more mu2e stuff $OFFLINE_LIBS - > get name: libNAME.so

simple_plugin(Occupancy "module"
    artdaq-core_Utilities
	NetworkUtilities
	artdaq-core_Data
	artdaq-core_Core
    ${ROOT_CORE}
    ${ROOT_GPAD}
 	${ROOT_HIST}
 	${ROOT_GUI}
 	${ROOT_RIO}
    ${ROOT_NET}
    ${CETLIB_EXCEPT}
    art_Framework_Services_Registry  
    art_root_io
    art_root_io_tfile_support
    art_root_io_TFileService_service
  MF_MessageLogger
    $ENV{OFFLINE_LIB}/libmu2e_ConditionsService.so
    $ENV{OFFLINE_LIB}/libmu2e_Mu2eInterfaces.so
    $ENV{OFFLINE_LIB}/libmu2e_RecoDataProducts.so
    $ENV{OFFLINE_LIB}/libmu2e_MCDataProducts.so
    $ENV{OFFLINE_LIB}/libmu2e_BFieldGeom.so
    $ENV{OFFLINE_LIB}/libmu2e_GeometryService.so
    $ENV{OFFLINE_LIB}/libmu2e_TrackerGeom.so
    $ENV{OFFLINE_LIB}/libmu2e_GlobalConstantsService.so
    $ENV{OFFLINE_LIB}/libmu2e_Mu2eUtilities.so
    $ENV{OFFLINE_LIB}/libmu2e_DataProducts.so
    $ENV{OFFLINE_LIB}/libmu2e_TrackerConditions.so
    $ENV{OFFLINE_LIB}/libmu2e_GlobalConstantsService_GlobalConstantsService_service.so
    $ENV{OFFLINE_LIB}/libmu2e_TrackerMC.so
   $ENV{OFFLINE_LIB}/libmu2e_CaloReco.so
)

#simple_plugin(ProtoType "module"
#       artdaq-core_Utilities
#	NetworkUtilities
#	artdaq-core_Data
#	artdaq-core_Core
#    ${ROOT_CORE}
#    ${ROOT_GPAD}
# 	${ROOT_HIST}
# 	${ROOT_GUI}
# 	${ROOT_RIO}
#    ${ROOT_NET}
#    ${CETLIB_EXCEPT}
#art_Framework_Services_Registry  
#art_root_io
#  art_root_io_tfile_support
#  art_root_io_TFileService_service
#  $ENV{OFFLINE_LIB}/libmu2e_ConditionsService.so
#  $ENV{OFFLINE_LIB}/libmu2e_Mu2eInterfaces.so
#  $ENV{OFFLINE_LIB}/libmu2e_RecoDataProducts.so
#  $ENV{OFFLINE_LIB}/libmu2e_MCDataProducts.so
#  $ENV{OFFLINE_LIB}/libmu2e_BFieldGeom.so
#  $ENV{OFFLINE_LIB}/libmu2e_GeometryService.so
#  $ENV{OFFLINE_LIB}/libmu2e_TrackerGeom.so
#  $ENV{OFFLINE_LIB}/libmu2e_GlobalConstantsService.so
#  $ENV{OFFLINE_LIB}/libmu2e_Mu2eUtilities.so
#  $ENV{OFFLINE_LIB}/libmu2e_DataProducts.so
#  $ENV{OFFLINE_LIB}/libmu2e_TrackerConditions.so
#  $ENV{OFFLINE_LIB}/libmu2e_GlobalConstantsService_GlobalConstantsService_service.so
#  $ENV{OFFLINE_LIB}/libmu2e_TrackerMC.so
#  $ENV{OFFLINE_LIB}/libmu2e_CaloReco.so
#)


simple_plugin(TrackerDQM "module"
	MF_MessageLogger
	art_Utilities
       artdaq-core_Utilities
        NetworkUtilities
        artdaq-core_Data
        artdaq-core_Core
	mu2e-artdaq-core_Overlays
    DTCInterface
    ${ROOT_CORE}
    ${ROOT_GPAD}
    ${ROOT_TREE}
        ${ROOT_HIST}
        ${ROOT_GUI}
        ${ROOT_RIO}
    ${ROOT_NET}
    ${CETLIB_EXCEPT}
art_Framework_Services_Registry  
art_root_io
  art_root_io_tfile_support
  art_root_io_TFileService_service
  MF_MessageLogger
  $ENV{OFFLINE_LIB}/libmu2e_ConditionsService.so
  $ENV{OFFLINE_LIB}/libmu2e_Mu2eInterfaces.so
  $ENV{OFFLINE_LIB}/libmu2e_RecoDataProducts.so
  $ENV{OFFLINE_LIB}/libmu2e_MCDataProducts.so
  $ENV{OFFLINE_LIB}/libmu2e_BFieldGeom.so
  $ENV{OFFLINE_LIB}/libmu2e_GeometryService.so
  $ENV{OFFLINE_LIB}/libmu2e_TrackerGeom.so
  $ENV{OFFLINE_LIB}/libmu2e_GlobalConstantsService.so
  $ENV{OFFLINE_LIB}/libmu2e_Mu2eUtilities.so
  $ENV{OFFLINE_LIB}/libmu2e_DataProducts.so
  $ENV{OFFLINE_LIB}/libmu2e_TrackerConditions.so
  $ENV{OFFLINE_LIB}/libmu2e_GlobalConstantsService_GlobalConstantsService_service.so
  $ENV{OFFLINE_LIB}/libmu2e_TrackerMC.so
  $ENV{OFFLINE_LIB}/libmu2e_CaloReco.so
)


simple_plugin(BeamMonitor "module"
       artdaq-core_Utilities
	NetworkUtilities
	artdaq-core_Data
	artdaq-core_Core
    ${ROOT_CORE}
    ${ROOT_GPAD}
 	${ROOT_HIST}
 	${ROOT_GUI}
 	${ROOT_RIO}
    ${ROOT_NET}
    ${CETLIB_EXCEPT}
  art_Framework_Services_Registry  
  art_Framework_Services_System_TriggerNamesService_service
  art_root_io
  art_root_io_tfile_support
  art_root_io_TFileService_service
  art_Persistency_Common
  art_Persistency_Provenance
  art_Utilities
  canvas
  MF_MessageLogger
  $ENV{OFFLINE_LIB}/libmu2e_ConditionsService.so
  $ENV{OFFLINE_LIB}/libmu2e_Mu2eInterfaces.so
  $ENV{OFFLINE_LIB}/libmu2e_RecoDataProducts.so
  $ENV{OFFLINE_LIB}/libmu2e_MCDataProducts.so
  $ENV{OFFLINE_LIB}/libmu2e_BFieldGeom.so
  $ENV{OFFLINE_LIB}/libmu2e_GeometryService.so
  $ENV{OFFLINE_LIB}/libmu2e_TrackerGeom.so
  $ENV{OFFLINE_LIB}/libmu2e_GlobalConstantsService.so
  $ENV{OFFLINE_LIB}/libmu2e_Mu2eUtilities.so
  $ENV{OFFLINE_LIB}/libmu2e_DataProducts.so
  $ENV{OFFLINE_LIB}/libmu2e_TrackerConditions.so
  $ENV{OFFLINE_LIB}/libmu2e_GlobalConstantsService_GlobalConstantsService_service.so
  $ENV{OFFLINE_LIB}/libmu2e_TrackerMC.so
  $ENV{OFFLINE_LIB}/libmu2e_CaloReco.so
)

