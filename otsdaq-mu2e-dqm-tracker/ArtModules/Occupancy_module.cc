// Author: S Middleton
// Date: 2020
// Purpose: analyzer to plot occupancy.
// Art:
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Services/System/TriggerNamesService.h"
#include "art_root_io/TFileDirectory.h"
#include "art_root_io/TFileService.h"
#include "canvas/Persistency/Common/TriggerResults.h"
#include "canvas/Utilities/InputTag.h"
#include "cetlib_except/exception.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/ParameterSetRegistry.h"

#include "artdaq-core/Data/ContainerFragment.hh"
#include "artdaq-core/Data/Fragment.hh"
//#include "artdaq/DAQdata/Globals.hh"

#include "cetlib_except/exception.h"

// OTS:
#include "otsdaq/Macros/CoutMacros.h"
#include "otsdaq/MessageFacility/MessageFacility.h"

// ROOT:
//#include "art/Framework/Services/Optional/TFileService.h"
#include "art_root_io/TFileService.h"
#include <TBufferFile.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TProfile.h>

// Offline:
#include <Offline/ConditionsService/inc/AcceleratorParams.hh>
#include <Offline/ConditionsService/inc/ConditionsHandle.hh>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#pragma GCC diagnostic ignored "-Wpedantic"
#include <Offline/BFieldGeom/inc/BFieldManager.hh>
#include <Offline/TrackerGeom/inc/Tracker.hh>
//#include <GlobalConstantsService/inc/GlobalConstantsHandle.hh">
#pragma GCC diagnostic pop

#include <Offline/GeometryService/inc/DetectorSystem.hh>
#include <Offline/GeometryService/inc/GeomHandle.hh>
#include <Offline/RecoDataProducts/inc/CaloCluster.hh>
#include <Offline/RecoDataProducts/inc/CaloDigi.hh>
#include <Offline/RecoDataProducts/inc/CaloTrigSeed.hh>
#include <Offline/RecoDataProducts/inc/ComboHit.hh>
#include <Offline/RecoDataProducts/inc/HelixSeed.hh>
#include <Offline/RecoDataProducts/inc/KalSeed.hh>
#include <Offline/RecoDataProducts/inc/StrawDigi.hh>
#include <Offline/RecoDataProducts/inc/TriggerInfo.hh>

#include <Offline/MCDataProducts/inc/ProtonBunchIntensity.hh>

// OTS:
#include "otsdaq-mu2e-dqm-tracker/ArtModules/OccupancyRootObjects.h"
#include "otsdaq/Macros/CoutMacros.h"
#include "otsdaq/Macros/ProcessorPluginMacros.h"
#include "otsdaq/MessageFacility/MessageFacility.h"
#include "otsdaq/NetworkUtilities/TCPPublishServer.h"
// C++:
#include <algorithm>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <numeric>
#include <sstream>
#include <unordered_map>
#include <vector>

#define TRACE_NAME "Occupancy"

namespace ots {
class Occupancy : public art::EDAnalyzer {
public:
  explicit Occupancy(fhicl::ParameterSet const &pset);
  virtual ~Occupancy();

  void analyze(art::Event const &e) override;
  void beginRun(art::Run const &) override;
  void beginJob() override;
  void endJob() override;

  void PlotOccupancu(art::Event const &e);

private:
  art::RunNumber_t current_run_;
  std::string outputFileName_;
  art::ServiceHandle<art::TFileService> tfs;
  bool writeOutput_;
  bool doStreaming_;
  bool overwrite_mode_;

  art::InputTag _trigAlgTag;
  art::InputTag _evtWeightTag;
  art::InputTag _cdTag;
  art::InputTag _sdTag;

  double _duty_cycle;
  string _processName;

  float _nProcess;
  size_t _nTrackTrig;
  size_t _nCaloTrig;
  double _bz0;
  double _nPOT;

  const mu2e::Tracker *_tracker;
  const mu2e::StrawDigiCollection *SDCol;
  const mu2e::CaloDigiCollection *CDCol;

  const art::Event *_event;
  OccupancyRootObjects *rootobjects = new OccupancyRootObjects("occ_plots");
  TCPPublishServer *tcp;
};
} // namespace ots

ots::Occupancy::Occupancy(fhicl::ParameterSet const &pset)
    : art::EDAnalyzer(pset), current_run_(0),
      outputFileName_(
          pset.get<std::string>("fileName", "otsdaqOccupancyDQM.root")),
      writeOutput_(pset.get<bool>("write_to_file", true)),
      doStreaming_(pset.get<bool>("stream_to_screen", true)),
      overwrite_mode_(pset.get<bool>("overwrite_output_file", true)),
      _evtWeightTag(pset.get<art::InputTag>("protonBunchIntensity",
                                            "protonBunchIntensity")),
      _cdTag(
          pset.get<art::InputTag>("caloDigiCollection", "CaloDigiFromShower")),
      _sdTag(pset.get<art::InputTag>("strawDigiCollection", "makeSD")),
      _duty_cycle(pset.get<float>("dutyCycle", 1.)),
      _processName(pset.get<string>("processName", "globalTrigger")),
      _nProcess(pset.get<float>("nEventsProcessed", 1.)),
      _nTrackTrig(pset.get<size_t>("nTrackTriggers", 4)),
      _nCaloTrig(pset.get<size_t>("nCaloTriggers", 4)),
      tcp(new TCPPublishServer(pset.get<int>("listenPort", 6000))) {
  TLOG(TLVL_INFO) << "Occuapncy Plotter construction is beginning ";

  TLOG(TLVL_DEBUG) << "TriggerRate Plotter construction complete";
}

ots::Occupancy::~Occupancy() {}

void ots::Occupancy::beginJob() {
  TLOG(TLVL_INFO) << "Started";
  rootobjects->BookHistos(tfs, _nTrackTrig, _nCaloTrig);
}

void ots::Occupancy::analyze(art::Event const &event) {
  TLOG(TLVL_INFO) << "Occupancy Plotting Module is Analyzing Event #  "
                  << event.event();
  //double value = 1;
  // get the StrawDigi Collection
  art::Handle<mu2e::StrawDigiCollection> sdH;
  event.getByLabel(_sdTag, sdH);
  // const StrawDigiCollection* sdCol(0);
  if (sdH.isValid()) {
    SDCol = sdH.product();
  }

  // get the CaloDigi Collection
  art::Handle<mu2e::CaloDigiCollection> cdH;
  event.getByLabel(_cdTag, cdH);
  // const CaloDigiCollection* cdCol(0);
  if (cdH.isValid()) {
    CDCol = cdH.product();
  }
  _nPOT = -1.;
  art::Handle<mu2e::ProtonBunchIntensity> evtWeightH;
  event.getByLabel(_evtWeightTag, evtWeightH);
  if (evtWeightH.isValid()) {
    _nPOT = (double)evtWeightH->intensity();
  }

  if (_nPOT < 0)
    return;
  int nSD(-1), nCD(-1);
  if (SDCol)
    nSD = SDCol->size();
  if (CDCol)
    nCD = CDCol->size(); // TODO - Index
  int Index = _nTrackTrig + _nCaloTrig;
  rootobjects->Hist._hOccInfo[Index][0]->Fill(_nPOT);

  rootobjects->Hist._h2DOccInfo[Index][0]->Fill(_nPOT, nSD);
  rootobjects->Hist._h2DOccInfo[Index][1]->Fill(_nPOT, nCD);

  TBufferFile message(TBuffer::kWrite);
  message.WriteObject(rootobjects->Hist._hOccInfo[0][0]);
  tcp->broadcastPacket(message.Buffer(), message.Length());
}

void ots::Occupancy::endJob() { TLOG(TLVL_INFO) << "Completed"; }

void ots::Occupancy::beginRun(const art::Run &run) {}

DEFINE_ART_MODULE(ots::Occupancy)
