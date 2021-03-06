/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: Remco de Boer, Nicolas Schmidt                                 *
 * Version 1.0                                                            *
 *                                                                        *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

//***************************************************************************************
//This AddTask is supposed to set up the main task
//($ALIPHYSICS/PWGGA/GammaConv/AliAnalysisTaskNeutralMesonToPiPlPiMiNeutralMeson.cxx) for
//pPb together with all supporting classes
//***************************************************************************************


//***************************************************************************************
//main function
//***************************************************************************************
void AddTask_GammaConvNeutralMesonPiPlPiMiNeutralMeson_CaloMode_pPb(
    Int_t trainConfig                 = 1,
    Int_t isMC                        = 0,                           // run MC
    TString   photonCutNumberV0Reader       = "",       // 00000008400000000100000000 nom. B, 00000088400000000100000000 low B
    Int_t selectHeavyNeutralMeson     = 0,                                // run eta prime instead of omega
    Int_t enableQAMesonTask           = 1,                                // enable QA in AliAnalysisTaskNeutralMesonToPiPlPiMiNeutralMeson
    TString fileNameInputForWeighting = "MCSpectraInput.root",            // path to file for weigting input
    Bool_t doWeighting                = kFALSE,                           // enable Weighting
    TString generatorName             = "HIJING",
    Double_t tolerance                = -1,
    TString periodNameV0Reader        = "",                               // period Name for V0Reader
    Int_t runLightOutput              = 0,                                // run light output option 0: no light output 1: most cut histos stiched off 2: unecessary omega hists turned off as well
    TString additionalTrainConfig     = "0"                               // additional counter for trainconfig, this has to be always the last parameter
  ) {

  //parse additionalTrainConfig flag
  TObjArray *rAddConfigArr = additionalTrainConfig.Tokenize("_");
  if(rAddConfigArr->GetEntries()<1){cout << "ERROR during parsing of additionalTrainConfig String '" << additionalTrainConfig.Data() << "'" << endl; return;}
  TObjString* rAdditionalTrainConfig;
  for(Int_t i = 0; i<rAddConfigArr->GetEntries() ; i++){
    if(i==0) rAdditionalTrainConfig = (TObjString*)rAddConfigArr->At(i);
    else{
      TObjString* temp = (TObjString*) rAddConfigArr->At(i);
      TString tempStr = temp->GetString();
      cout << "INFO: nothing to do, no definition available!" << endl;
    }
  }
  TString sAdditionalTrainConfig = rAdditionalTrainConfig->GetString();
  if (sAdditionalTrainConfig.Atoi() > 0){
    trainConfig = trainConfig + sAdditionalTrainConfig.Atoi();
    cout << "INFO: AddTask_GammaConvNeutralMesonPiPlPiMiNeutralMeson_CaloMode_pPb running additionalTrainConfig '" << sAdditionalTrainConfig.Atoi() << "', train config: '" << trainConfig << "'" << endl;
  }

  Int_t isHeavyIon = 2;
  Int_t neutralPionMode = 2;

  // ================== GetAnalysisManager ===============================
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
    Error(Form("AddTask_GammaConvNeutralMesonPiPlPiMiNeutralMeson_CaloMode_pPb_%i",trainConfig), "No analysis manager found.");
    return ;
  }

  // ================== GetInputEventHandler =============================
  AliVEventHandler *inputHandler=mgr->GetInputEventHandler();


  //=========  Set Cutnumber for V0Reader ================================
  TString cutnumberPhoton = photonCutNumberV0Reader.Data();
  TString cutnumberEvent = "80000003";

  AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer();

  //========= Add V0 Reader to  ANALYSIS manager if not yet existent =====
  TString V0ReaderName        = Form("V0ReaderV1_%s_%s",cutnumberEvent.Data(),cutnumberPhoton.Data());
  AliV0ReaderV1 *fV0ReaderV1  =  NULL;
  if( !(AliV0ReaderV1*)mgr->GetTask(V0ReaderName.Data()) ){
    std::cout << "V0Reader: " << V0ReaderName.Data() << " not found!!"<< std::endl;
    return;
  } else {
    std::cout << "V0Reader: " << V0ReaderName.Data() << " found!!"<< std::endl;
  }

  TString PionCuts      = "000000200";            //Electron Cuts
  //================================================
  //========= Add Pion Selector ====================
  if( !(AliPrimaryPionSelector*)mgr->GetTask("PionSelector") ){
    AliPrimaryPionSelector *fPionSelector = new AliPrimaryPionSelector("PionSelector");
    AliPrimaryPionCuts *fPionCuts=0;
    if( PionCuts!=""){
      fPionCuts= new AliPrimaryPionCuts(PionCuts.Data(),PionCuts.Data());
      if(runLightOutput>0) fPionCuts->SetLightOutput(kTRUE);
      //if(runLightOutput>0) fPionCuts->SetLightOutput(kTRUE);
      if(fPionCuts->InitializeCutsFromCutString(PionCuts.Data())){
        fPionSelector->SetPrimaryPionCuts(fPionCuts);
        fPionCuts->SetFillCutHistograms("",kTRUE);
      }
    }

    fPionSelector->Init();
    mgr->AddTask(fPionSelector);

    AliAnalysisDataContainer *cinput1  = mgr->GetCommonInputContainer();
    mgr->ConnectInput (fPionSelector,0,cinput1);
  }

  AliAnalysisTaskNeutralMesonToPiPlPiMiNeutralMeson *task=NULL;
  task= new AliAnalysisTaskNeutralMesonToPiPlPiMiNeutralMeson(Form("GammaConvNeutralMesonPiPlPiMiNeutralMeson_%i_%i",neutralPionMode, trainConfig));
  task->SetIsHeavyIon(isHeavyIon);
  task->SetIsMC(isMC);
  task->SetV0ReaderName(V0ReaderName);
  if(runLightOutput>1) task->SetLightOutput(kTRUE);
  task->SetTolerance(tolerance);

  AliCutHandlerPCM cuts(13);


  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //                                          ETA MESON
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  if( trainConfig == 1 ) {
    // everything open, min pt charged pi = 100 MeV
    cuts.AddCutHeavyMesonCalo("80000113","1111113047032230000","000010400","0103503a00000000","0103503000000000"); // cent 0-100%
    cuts.AddCutHeavyMesonCalo("80200113","1111113047032230000","000010400","0103503a00000000","0103503000000000"); // cent 0-20%
  } else if( trainConfig == 2 ) {
    // closing charged pion cuts, minimum TPC cluster = 80, TPC dEdx pi = \pm 3 sigma, min pt charged pi = 100 MeV
    cuts.AddCutHeavyMesonCalo("80000113","1111100047032230000","002010700","0103503a00000000","0103503000000000"); // cent 0-100%
    cuts.AddCutHeavyMesonCalo("80200113","1111100047032230000","002010700","0103503a00000000","0103503000000000"); // cent 0-20%
  } else if( trainConfig == 3 ) {
    // eta < 0.9
    // closing charged pion cuts, minimum TPC cluster = 80, TPC dEdx pi = \pm 3 sigma, pi+pi- mass cut of 0.65, min pt charged pi = 100 MeV
    // closing neural pion cuts, 0.1 < M_gamma,gamma < 0.15
    // maxChi2 per cluster TPC <4, require TPC refit, DCA XY pT dependend 0.0182+0.0350/pt^1.01, DCA_Z = 3.0
    // timing cluster cut open
    cuts.AddCutHeavyMesonCalo("80010113","1111100047032230000","30a330708","0103503400000000","0153503000000000");
    cuts.AddCutHeavyMesonCalo("80210113","1111100047032230000","30a330708","0103503400000000","0153503000000000");

  } else if( trainConfig == 51 ) { // same as 1, but 0-20% centrality excluded
    cuts.AddCutHeavyMesonCalo("80000113","1111113047032230000","000010400","0103503a00000000","0103503000000000");
    cuts.AddCutHeavyMesonCalo("80000113","1111113047032230000","000010400","0103503a00000000","0103503000000000");
  } else if( trainConfig == 52 ) { // same as 2, but 0-20% centrality excluded
    cuts.AddCutHeavyMesonCalo("80000113","1111100047032230000","002010700","0103503a00000000","0103503000000000");
  } else if( trainConfig == 53 ) { // same as 3, but 0-20% centrality excluded
    cuts.AddCutHeavyMesonCalo("80010113","1111100047032230000","30a330708","0103503400000000","0153503000000000");

  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //                                          OMEGA MESON
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  } else if( trainConfig == 100 ) {
    // everything open, min pt charged pi = 100 MeV
    cuts.AddCutHeavyMesonCalo("80000113","1111100047032230000","000010400","0103503a00000000","0103503000000000"); // cent 0-100%
    cuts.AddCutHeavyMesonCalo("80200113","1111100047032230000","000010400","0103503a00000000","0103503000000000"); // cent 0-20%
  } else if( trainConfig == 101 ) {
    // closing charged pion cuts, minimum TPC cluster = 80, TPC dEdx pi = \pm 3 sigma, min pt charged pi = 100 MeV
    cuts.AddCutHeavyMesonCalo("80000113","1111100047032230000","002010700","0103503a00000000","0103503000000000"); // cent 0-100%
    cuts.AddCutHeavyMesonCalo("80200113","1111100047032230000","002010700","0103503a00000000","0103503000000000"); // cent 0-20%
  } else if( trainConfig == 102 ) {
    // eta < 0.9
    // closing charged pion cuts, minimum TPC cluster = 80, TPC dEdx pi = \pm 3 sigma, pi+pi- mass cut of 0.65, min pt charged pi = 100 MeV
    // closing neural pion cuts, 0.1 < M_gamma,gamma < 0.15
    // maxChi2 per cluster TPC <4, require TPC refit, DCA XY pT dependend 0.0182+0.0350/pt^1.01, DCA_Z = 3.0
    // timing cluster cut open
    cuts.AddCutHeavyMesonCalo("80010113","1111100047032230000","30a330708","0103503400000000","0153503000000000"); // cent 0-100%
    cuts.AddCutHeavyMesonCalo("80210113","1111100047032230000","30a330708","0103503400000000","0153503000000000"); // cent 0-20%

  } else if( trainConfig == 150 ) { // same as 100, but 0-20% centrality excluded
    cuts.AddCutHeavyMesonCalo("80000113","1111100047032230000","000010400","0103503a00000000","0103503000000000");
  } else if( trainConfig == 151 ) { // same as 101, but 0-20% centrality excluded
    cuts.AddCutHeavyMesonCalo("80000113","1111100047032230000","002010700","0103503a00000000","0103503000000000");
  } else if( trainConfig == 152 ) { // same as 102, but 0-20% centrality excluded
    cuts.AddCutHeavyMesonCalo("80010113","1111100047032230000","30a330708","0103503400000000","0153503000000000");

 // pPb 5.02 (run 1)
  } else if( trainConfig == 160)  { // EMCal 0-100%
    cuts.AddCutHeavyMesonCalo("80010113","1111111047032230000","32c010708","0103603700000000","0153503000000000"); // V0AND
    cuts.AddCutHeavyMesonCalo("80052113","1111111047032230000","32c010708","0103603700000000","0153503000000000"); // EMC7
    cuts.AddCutHeavyMesonCalo("80085113","1111111047032230000","32c010708","0103603700000000","0153503000000000"); // EG2
    cuts.AddCutHeavyMesonCalo("80083113","1111111047032230000","32c010708","0103603700000000","0153503000000000"); // EG1
  } else if( trainConfig == 165)  { // PHOS 0-100%
    cuts.AddCutHeavyMesonCalo("80010113","2444411043012300000","32c010708","0103603n00000000","0153503000000000"); // V0AND
    cuts.AddCutHeavyMesonCalo("80052113","2444411043012300000","32c010708","0103603n00000000","0153503000000000"); // EMC7
    cuts.AddCutHeavyMesonCalo("80085113","2444411043012300000","32c010708","0103603n00000000","0153503000000000"); // EG2
    cuts.AddCutHeavyMesonCalo("80083113","2444411043012300000","32c010708","0103603n00000000","0153503000000000"); // EG1
 // pPb 5.02 (run 2)
  } else if( trainConfig == 170)  { // EMCal 0-100%
    cuts.AddCutHeavyMesonCalo("80010113","1111111047032230000","32c010708","0103603700000000","0153503000000000"); // V0AND
  } else if( trainConfig == 175)  { // PHOS 0-100%
    cuts.AddCutHeavyMesonCalo("80010113","2444411043012300000","32c010708","0103603n00000000","0153503000000000"); // V0AND

  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //                                          ETA PRIME MESON
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  } else if( trainConfig == 200 ) {
    // everything open, min pt charged pi = 100 MeV
    cuts.AddCutHeavyMesonCalo("80000113","1111100047032230000","000010400","0103503m00000000","0103503000000000");
    cuts.AddCutHeavyMesonCalo("80200113","1111100047032230000","000010400","0103503m00000000","0103503000000000");
  } else if( trainConfig == 201 ) {
    // closing charged pion cuts, minimum TPC cluster = 80, TPC dEdx pi = \pm 3 sigma, min pt charged pi = 100 MeV
    cuts.AddCutHeavyMesonCalo("80000113","1111100047032230000","002010700","0103503m00000000","0103503000000000");
    cuts.AddCutHeavyMesonCalo("80200113","1111100047032230000","002010700","0103503m00000000","0103503000000000");
  } else if( trainConfig == 202 ) {
    // eta < 0.9
    // closing charged pion cuts, minimum TPC cluster = 80, TPC dEdx pi = \pm 3 sigma, pi+pi- mass cut of 1.5, min pt charged pi = 100 MeV
    // closing neural pion cuts, 0.5 < M_gamma,gamma < 0.6
    // maxChi2 per cluster TPC <4, require TPC refit, DCA XY pT dependend 0.0182+0.0350/pt^1.01, DCA_Z = 3.0
    // timing cluster cut open
    cuts.AddCutHeavyMesonCalo("80010113","1111100047032230000","30a330709","0103503l00000000","0153503000000000"); // all of the above
    cuts.AddCutHeavyMesonCalo("80010113","1111100047032230000","30a330709","0103503m00000000","0153503000000000"); // 0.4-0.7 eta mass cut
    cuts.AddCutHeavyMesonCalo("80210113","1111100047032230000","30a330709","0103503l00000000","0153503000000000"); // all of the above
    cuts.AddCutHeavyMesonCalo("80210113","1111100047032230000","30a330709","0103503m00000000","0153503000000000"); // 0.4-0.7 eta mass cut
  } else if( trainConfig == 204 ) {
    // same as 202 but with mass cut variations
    cuts.AddCutHeavyMesonCalo("80010113","1111100047032230000","30a330700","0103503l00000000","0153503000000000"); // pi+pi- mass cut of 10
    cuts.AddCutHeavyMesonCalo("80010113","1111100047032230000","30a330701","0103503l00000000","0153503000000000"); // pi+pi- mass cut of 1
    cuts.AddCutHeavyMesonCalo("80010113","1111100047032230000","30a330708","0103503l00000000","0153503000000000"); // pi+pi- mass cut of 0.85
    cuts.AddCutHeavyMesonCalo("80210113","1111100047032230000","30a330700","0103503l00000000","0153503000000000"); // pi+pi- mass cut of 10
    cuts.AddCutHeavyMesonCalo("80210113","1111100047032230000","30a330701","0103503l00000000","0153503000000000"); // pi+pi- mass cut of 1
    cuts.AddCutHeavyMesonCalo("80210113","1111100047032230000","30a330708","0103503l00000000","0153503000000000"); // pi+pi- mass cut of 0.85

  } else if( trainConfig == 250 ) { // same as 200, but 0-20% centrality excluded
    cuts.AddCutHeavyMesonCalo("80000113","1111100047032230000","000010400","0103503m00000000","0103503000000000");
  } else if( trainConfig == 251 ) { // same as 201, but 0-20% centrality excluded
    cuts.AddCutHeavyMesonCalo("80000113","1111100047032230000","002010700","0103503m00000000","0103503000000000");
  } else if( trainConfig == 252 ) { // same as 202, but 0-20% centrality excluded
    cuts.AddCutHeavyMesonCalo("80010113","1111100047032230000","30a330709","0103503l00000000","0153503000000000");
    cuts.AddCutHeavyMesonCalo("80010113","1111100047032230000","30a330709","0103503m00000000","0153503000000000");
  } else if( trainConfig == 254 ) { // same as 204, but 0-20% centrality excluded
    cuts.AddCutHeavyMesonCalo("80010113","1111100047032230000","30a330700","0103503l00000000","0153503000000000");
    cuts.AddCutHeavyMesonCalo("80010113","1111100047032230000","30a330701","0103503l00000000","0153503000000000");
    cuts.AddCutHeavyMesonCalo("80010113","1111100047032230000","30a330708","0103503l00000000","0153503000000000");

  } else {
    Error(Form("GammaConvNeutralMeson_CaloMode_%i",trainConfig), "wrong trainConfig variable no cuts have been specified for the configuration");
    return;
  }

  if(!cuts.AreValid()){
    cout << "\n\n****************************************************" << endl;
    cout << "ERROR: No valid cuts stored in CutHandlerNeutralCalo! Returning..." << endl;
    cout << "****************************************************\n\n" << endl;
    return;
  }

  Int_t numberOfCuts = cuts.GetNCuts();

  TList *EventCutList = new TList();
  TList *ClusterCutList  = new TList();
  TList *NeutralPionCutList = new TList();
  TList *MesonCutList = new TList();
  TList *PionCutList  = new TList();

  TList *HeaderList = new TList();
  TObjString *Header1 = new TObjString("pi0_1");
  HeaderList->Add(Header1);
  TObjString *Header3 = new TObjString("eta_2");
  HeaderList->Add(Header3);

  EventCutList->SetOwner(kTRUE);
  AliConvEventCuts **analysisEventCuts = new AliConvEventCuts*[numberOfCuts];
  ClusterCutList->SetOwner(kTRUE);
  AliCaloPhotonCuts **analysisClusterCuts = new AliCaloPhotonCuts*[numberOfCuts];
  NeutralPionCutList->SetOwner(kTRUE);
  AliConversionMesonCuts **analysisNeutralPionCuts   = new AliConversionMesonCuts*[numberOfCuts];
  MesonCutList->SetOwner(kTRUE);
  AliConversionMesonCuts **analysisMesonCuts   = new AliConversionMesonCuts*[numberOfCuts];
  PionCutList->SetOwner(kTRUE);
  AliPrimaryPionCuts **analysisPionCuts     = new AliPrimaryPionCuts*[numberOfCuts];

  for(Int_t i = 0; i<numberOfCuts; i++){
    //create AliCaloTrackMatcher instance, if there is none present
    TString caloCutPos = cuts.GetClusterCut(i);
    caloCutPos.Resize(1);
    TString TrackMatcherName = Form("CaloTrackMatcher_%s",caloCutPos.Data());
    if( !(AliCaloTrackMatcher*)mgr->GetTask(TrackMatcherName.Data()) ){
      AliCaloTrackMatcher* fTrackMatcher = new AliCaloTrackMatcher(TrackMatcherName.Data(),caloCutPos.Atoi());
      fTrackMatcher->SetV0ReaderName(V0ReaderName);
      mgr->AddTask(fTrackMatcher);
      mgr->ConnectInput(fTrackMatcher,0,cinput);
    }

    analysisEventCuts[i] = new AliConvEventCuts();
    analysisEventCuts[i]->SetV0ReaderName(V0ReaderName);
    if(runLightOutput>0) analysisEventCuts[i]->SetLightOutput(kTRUE);
    analysisEventCuts[i]->InitializeCutsFromCutString((cuts.GetEventCut(i)).Data());
    if (periodNameV0Reader.CompareTo("") != 0) analysisEventCuts[i]->SetPeriodEnum(periodNameV0Reader);
    EventCutList->Add(analysisEventCuts[i]);
    analysisEventCuts[i]->SetFillCutHistograms("",kFALSE);

    analysisClusterCuts[i] = new AliCaloPhotonCuts();
    analysisClusterCuts[i]->SetV0ReaderName(V0ReaderName);
    if(runLightOutput>0) analysisClusterCuts[i]->SetLightOutput(kTRUE);
    analysisClusterCuts[i]->SetCaloTrackMatcherName(TrackMatcherName);
    if( ! analysisClusterCuts[i]->InitializeCutsFromCutString((cuts.GetClusterCut(i)).Data()) ) {
      cout<<"ERROR: analysisClusterCuts [" <<i<<"]"<<endl;
      return 0;
    } else {
      analysisClusterCuts[i]->InitializeCutsFromCutString((cuts.GetClusterCut(i)).Data());
      ClusterCutList->Add(analysisClusterCuts[i]);
      analysisClusterCuts[i]->SetFillCutHistograms("");
    }

    analysisNeutralPionCuts[i] = new AliConversionMesonCuts();
    if(runLightOutput>0) analysisNeutralPionCuts[i]->SetLightOutput(kTRUE);
    if( ! analysisNeutralPionCuts[i]->InitializeCutsFromCutString((cuts.GetNDMCut(i)).Data()) ) {
      cout<<"ERROR: analysisMesonCuts [ " <<i<<" ] "<<endl;
      return 0;
    } else {
      NeutralPionCutList->Add(analysisNeutralPionCuts[i]);
      analysisNeutralPionCuts[i]->SetFillCutHistograms("");
    }

    analysisMesonCuts[i] = new AliConversionMesonCuts();
    if(runLightOutput>0) analysisMesonCuts[i]->SetLightOutput(kTRUE);
    if( ! analysisMesonCuts[i]->InitializeCutsFromCutString((cuts.GetMesonCut(i)).Data()) ) {
      cout<<"ERROR: analysisMesonCuts [ " <<i<<" ] "<<endl;
      return 0;
    } else {
      MesonCutList->Add(analysisMesonCuts[i]);
      analysisMesonCuts[i]->SetFillCutHistograms("");
    }
    analysisEventCuts[i]->SetAcceptedHeader(HeaderList);

    TString cutName( Form("%s_%s_%s_%s_%s",(cuts.GetEventCut(i)).Data(), (cuts.GetClusterCut(i)).Data(),(cuts.GetPionCut(i)).Data(),(cuts.GetNDMCut(i)).Data(), (cuts.GetMesonCut(i)).Data() ) );
    analysisPionCuts[i] = new AliPrimaryPionCuts();
    if(runLightOutput>0) analysisPionCuts[i]->SetLightOutput(kTRUE);

        if( !analysisPionCuts[i]->InitializeCutsFromCutString((cuts.GetPionCut(i)).Data())) {
      cout<< "ERROR:  analysisPionCuts [ " <<i<<" ] "<<endl;
      return 0;
    } else {
      PionCutList->Add(analysisPionCuts[i]);
      analysisPionCuts[i]->SetFillCutHistograms("",kFALSE,cutName);
    }
  }

  task->SetNDMRecoMode(neutralPionMode);
  task->SetEventCutList(numberOfCuts,EventCutList);
  task->SetClusterCutList(ClusterCutList);
  task->SetNeutralPionCutList(NeutralPionCutList);
  task->SetMesonCutList(MesonCutList);
  task->SetPionCutList(PionCutList);

  task->SetMoveParticleAccordingToVertex(kTRUE);
  task->SetSelectedHeavyNeutralMeson(selectHeavyNeutralMeson);

  task->SetDoMesonQA(enableQAMesonTask );

  //connect containers
  AliAnalysisDataContainer *coutput =
  mgr->CreateContainer(Form("GammaConvNeutralMesonPiPlPiMiNeutralMeson_%i_%i_%i.root",selectHeavyNeutralMeson,neutralPionMode, trainConfig), TList::Class(),
              AliAnalysisManager::kOutputContainer,Form("GammaConvNeutralMesonPiPlPiMiNeutralMeson_%i_%i_%i.root",selectHeavyNeutralMeson,neutralPionMode, trainConfig));

  mgr->AddTask(task);
  mgr->ConnectInput(task,0,cinput);
  mgr->ConnectOutput(task,1,coutput);

  return;

}
