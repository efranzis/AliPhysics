#include "TString.h"
#include "TMath.h"
#include "AliForwardSettings.h"
#include "TFile.h"

//________________________________________________________________________
AliForwardSettings::AliForwardSettings() :
  fDataType(-1),
  fPhiAcceptanceLowEdge(0),
  fPhiAcceptanceUpEdge(2*TMath::Pi()),
  fEtaLowEdge(-4.0),
  fEtaUpEdge(6.0),
  fNPhiBins(20),
  fZVtxAcceptanceLowEdge(-10),
  fZVtxAcceptanceUpEdge(10),
  fNZvtxBins(10),
  qctype("std"),
  fnoSamples(10),
  fNRefEtaBins(1),
  fNDiffEtaBins(50),
  fCentBins(10),
  nuacentral(),
  nuaforward(),
  doNUA(false),
  gap(0.0),
  maxpt(5),
  mc(kTRUE),
  esd(kTRUE),
  tracktype(kHybrid),
  nua_mode(kNormal),
  useTPC{kTRUE},
  useSPD(kFALSE),
  use_primaries(kTRUE),
  use_primaries_cen(kTRUE),
  use_primaries_fwd(kTRUE),
  centrality_estimator('SPDTracklets'),//CL0, V0M
  etagap(kTRUE),
  makeFakeHoles(kFALSE)
{
}
