/*********************************************************************************************************
  Example macro to launch analysis plugin for rsn analysis
  Created by fbellini@cern.ch, 19/09/2014

Usage: 
- Modify the main settings (see description below) 
  IMPORTANT: the plugin needs you to configure the global variables,
  please DON'T SKIP THIS PASSAGE!!!
- Launch with AliRoot
  $ aliroot -l -b -q -x RunGridRsnTrain.C"(<mode>, <Grid User>, <suffix>, <ROOT>, <ALIROOT>, <ALIPHYSICS>)"

  <mode> can be "test", "full" or "terminate" - MANDATORY
  <Grid User> is your grid username - MANDATORY
  <suffix> is an additional string which identifies the names of the executables and jdl - OPTIONAL
           if it is empty (e.g., "") all files created by the plugin will have the same name 
  <ROOT>, <ALIROOT>, <ALIPHYSICS> must be compatible versions of the packages (check on Monalisa)
**********************************************************************************************************/

#ifndef __CINT__
#include <TSystem.h>
#include <TROOT.h>
#include <Rtypes.h>
#include <TString.h>
#include <TNamed.h>
#include <TList.h>
//#include <ANALYSIS/AliAnalysisManager.h>
#include <AliAnalysisManager.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <AliMultiInputEventHandler.h>
#include <AliAODHandler.h>
#endif

#include "./AnalysisSetup.C"

class AliAnalysisGrid;


/***************************************************************/
/***   !!! THE USER HAVE TO SET THE FOLLOWING VARIABLES!!!   ***/
/***************************************************************/
/* Define here the input dataset */
Int_t year = 2016;
TString prod = "LHC16k";
TString ppass = "pass2";

/* AOD number. 
   Set aodN = 0 for AOD produced with reconstruction, which have no number.
   Set aodN = -1 for ESDs */
Int_t aodN = 208;

/* Define the run list and the run range for the analysis.
   Example with run list for LHC13c pass2 (p-Pb data) */
Int_t runNmin=2;
Int_t runNmax=150;
Int_t runList[300] = {258537, 258499, 258477, 258456, 258454, 258452, 258426, 258393, 258391, 258387, 258359, 258336, 258332, 258307, 258306, 258303, 258302, 258301, 258299, 258278, 258274, 258273, 258271, 258270, 258258, 258257, 258256, 258204, 258203, 258202, 258198, 258197, 258178, 258117, 258114, 258113, 258109, 258108, 258107, 258063, 258062, 258060, 258059, 258053, 258049, 258045, 258042, 258041, 258039, 258019, 258017, 258014, 258012, 258008, 258003, 257992, 257989, 257986, 257979, 257963, 257960, 257957, 257939, 257937, 257936, 257855, 257853, 257851, 257850, 257804, 257803, 257800, 257799, 257798, 257797, 257773, 257765, 257757, 257754, 257737, 257735, 257734, 257733, 257727, 257725, 257724, 257697, 257694, 257692, 257691, 257689, 257688, 257687, 257685, 257684, 257682, 257644, 257642, 257636, 257635, 257632, 257630, 257606, 257605, 257604, 257601, 257595, 257594, 257592, 257590, 257588, 257587, 257566, 257562, 257561, 257560, 257541, 257540, 257539, 257537, 257531, 257530, 257492, 257491, 257490, 257488, 257487, 257474, 257468, 257457, 257433, 257364, 257358, 257330, 257322, 257320, 257318, 257260, 257224, 257209, 257206, 257204, 257144, 257141, 257139, 257138, 257137, 257136, 257100, 257095, 257092, 257086, 257084, 257082, 257080, 257077, 257012, 257011, 256944, 256942, 256941, 256697, 256695, 256694, 256692, 256691, 256684, 256681, 256677, 256676, 256658, 256620, 256619, 256592, 256591, 256589, 256567, 256565, 256564, 256562, 256560, 256557, 256556, 256554, 256552, 256514, 256512, 256510, 256506, 256504};
//258537, 258499, 258477, 258456, 258454, 258452, 258426, 258393, 258391, 258387, 258359, 258336, 258332, 258307, 258306, 258303, 258302, 258301, 258299, 258278, 258274, 258273, 258271, 258270, 258258, 258257, 258256, 258204, 258203, 258202, 258198, 258197, 258178, 258117, 258114, 258113, 258109, 258108, 258107, 258063, 258062, 258060, 258059, 258053, 258049, 258045, 258042, 258041, 258039, 258019, 258017, 258014, 258012, 258008, 258003, 257992, 257989, 257986, 257979, 257963, 257960, 257957, 257939, 257937, 257936, 257855, 257853, 257851, 257850, 257804, 257803, 257800, 257799, 257798, 257797, 257773, 257765, 257757, 257754, 257737, 257735, 257734, 257733, 257727, 257725, 257724, 257697, 257694, 257692, 257691, 257689, 257688, 257687, 257685, 257684, 257682, 257644, 257642, 257636, 257635, 257632, 257630, 257606, 257605, 257604, 257601, 257595, 257594, 257592, 257590, 257588, 257587, 257566, 257562, 257561, 257560, 257541, 257540, 257539, 257537, 257531, 257530, 257492, 257491, 257490, 257488, 257487, 257474, 257468, 257457, 257433, 257364, 257358, 257330, 257322, 257320, 257318, 257260, 257224, 257209, 257206, 257204, 257144, 257141, 257139, 257138, 257137, 257136, 257100, 257095, 257092, 257086, 257084, 257082, 257080, 257077, 257012, 257011, 256944, 256942, 256941, 256697, 256695, 256694, 256692, 256691, 256684, 256681, 256677, 256676, 256658, 256620, 256619, 256592, 256591, 256589, 256567, 256565, 256564, 256562, 256560, 256557, 256556, 256554, 256552, 256514, 256512, 256510, 256506, 256504

/* The following flags are needed to configure the analysis */
Bool_t isPP = 1;
Bool_t isMC = 1;
Bool_t isESD = 0;
Bool_t useTender = kFALSE;
Int_t nmix=10; 

/* Define data path */
TString myGridDataDir="";
TString myDataPattern="";

/* Define output directory and names of the files generated by the plugin.
Omit the extension of the files, it will be automatically set by the SetupIO function */
TString myWorkDir = "RsnOut";
TString myOutDir  = "Example";
TString myJDLname = "jobRsnAnalysisExample";
TString myExecutableName = "RunRsnAnalysisExample";
TString myMacroName = "RunRsnAnalysisExample";

/* the macros are normally in the Rsn Package */
TString loadMacroPath = "${ALICE_PHYSICS}/PWGLF/RESONANCES/macros/mini";

/* The analysis mode can be "local" or "grid"
Use "grid" if you need to access any file on the grid. 
This is optional in test mode, mandatory for full and terminate mode.
Use "local" instead if you do not need the grid connection at all */
TString analysisMode  = "local";

/* Set the usage of the alien plugin to kTRUE to run the analysis on grid or in test mode,
   set it to kFALSE to run test locally*/
Bool_t useAlienPlugin = 0;

/* If you want to run merging via plugin, disable the local merge when running in "full" mode or "terminate" mode (multiple merging stages are possible).
   Enable the local merge at the last step of the "terminate" to download locally the final merged output.
   Otherwise, local merge is always possible. */
Bool_t isLocalMerge = 1;

/* Run Rsn monitoring for QA only - keep disabled as default to run also the analysis */
Bool_t runMonOnly= kFALSE;

AliAnalysisGrid* CreateAlienHandler(TString,TString,TString,TString,TString);
void SetupIO(TString);

/***************************************************************/
void LoadLibraries()
{
  gSystem->SetIncludePath("-I. -I$ROOTSYS/include  -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include -I$ALICE_ROOT/STEER/STEER -I$ALICE_ROOT/STEER/STEERbase -I$ALICE_ROOT/ANALYSIS -I$ALICE_ROOT/ANALYSISalice -I$ALICE_PHYSICS/EVENTMIX -g"); 
  printf("%s\n",gSystem->GetIncludePath());
  Printf("Loading libs");
  gSystem->Load("libTree.so");
  gSystem->Load("libGeom.so");
  gSystem->Load("libVMC.so");
  gSystem->Load("libPhysics.so");
  gSystem->Load("libSTEERBase");
  gSystem->Load("libESD");
  gSystem->Load("libAOD");
  gSystem->Load("libANALYSIS");
  gSystem->Load("libOADB");
  gSystem->Load("libANALYSISalice.so");
  gSystem->Load("libEventMixing.so");
  gSystem->Load("libPWGLFresonances.so");
}

/***************************************************************/
void RunGridRsnTrain(Int_t flag=101,TString dir="test",TString pluginmode="terminate",
		     TString username="blim",
		     TString suffix="_Example",
		     TString rootVer = "v6-14-04-4", 
		     TString aliRootVer = "v5-09-43-1", 
		     TString aliPhysicsVer = "vAN-20181219-1") {
    
    //isMC=(flag>1000);
    //isESD=((flag%1000)/100);
    //Int_t fid=flag%100;
    Int_t fid = 1;
    suffix.Form("%s%i",suffix.Data(),fid);
  
  Long64_t nentries=100000, firstentry=0; //needed to read input when running locally
  
  // Connect to AliEn only if needed
  if(analysisMode=="grid") {
    TGrid::Connect("alien://");
  } 
  
  //load include path for necessary libraries to configure analysis and plugin
  LoadLibraries();
  SetupIO(suffix.Data());

  // Create Alien plugin, if requested
  AliAnalysisGrid *alienHandler=0;
  if(useAlienPlugin) {  
    alienHandler = CreateAlienHandler(pluginmode.Data(),username.Data(), rootVer.Data(), aliRootVer.Data(), aliPhysicsVer.Data());
    if(!alienHandler) return;
  }
  
  //AliLog::SetGlobalDebugLevel(2);
    
  //read input file for local tests
  //The file added to the chain can also be a file on the grid, e.g. alien://<full path on grid>/AliESDs.root
  TChain *chain = 0;
  if(!useAlienPlugin) {
    if (isESD){
      chain=new TChain("esdTree");
        if(!isMC){
            //chain->Add("/Users/aknospe/Documents/Austin/resonance/dev/test/AliESDs.root");
            if(fid==1) chain->Add("/Users/blim/data/2015/LHC15f/000225106/pass2/15000225106019.10/AliESDs.root");
            //else if(fid==2) chain->Add("/Users/blim/data/2015/LHC15f/000225106/pass2/15000225106019.100/AliESDs.root");
            //else if(fid==3) chain->Add("/Users/blim/data/2015/LHC15f/000225106/pass2/15000225106019.101/AliESDs.root");
            //else if(fid==4) chain->Add("/Users/blim/data/2015/LHC15f/000225106/pass2/15000225106019.1010/AliESDs.root");
            //else if(fid==5) chain->Add("/Users/blim/data/2015/LHC15f/000225106/pass2/15000225106019.1011/AliESDs.root");
            //else if(fid==6) chain->Add("/Users/blim/data/2015/LHC15f/000225106/pass2/15000225106019.1012/AliESDs.root");
            //else if(fid==7) chain->Add("/Users/blim/data/2015/LHC15f/000225106/pass2/15000225106019.1013/AliESDs.root");
            //else if(fid==8) chain->Add("/Users/blim/data/2015/LHC15f/000225106/pass2/15000225106019.1014/AliESDs.root");
            //else if(fid==9) chain->Add("/Users/blim/data/2015/LHC15f/000225106/pass2/15000225106019.103/AliESDs.root");
            //else if(fid==10) chain->Add("/Users/blim/data/2015/LHC15f/000225106/pass2/15000225106019.104/AliESDs.root");
        }else{
            //chain->Add("/Users/aknospe/Documents/Austin/resonance/dev/test/sim/AliESDs.root");
            //if(fid==1) chain->Add("/Users/blim/sim/2015/LHC15g3a3/225106/001/AliESDs.root");
            if(fid==1) chain->Add("/Users/blim/sim/LHC18c8b_cent/282367/999/AliESDs.root");
            //else if(fid==2) chain->Add("/Users/blim/data/LHC15g3a3/225106/002/AliESDs.root");
            //else if(fid==3) chain->Add("/Users/blim/data/LHC15g3a3/225106/003/AliESDs.root");
            //else if(fid==4) chain->Add("/Users/blim/data/LHC15g3a3/225106/004/AliESDs.root");
            //else if(fid==5) chain->Add("/Users/blim/data/LHC15g3a3/225106/005/AliESDs.root");
            //else if(fid==6) chain->Add("/Users/blim/data/LHC15g3a3/225106/006/AliESDs.root");
            //else if(fid==7) chain->Add("/Users/blim/data/LHC15g3a3/225106/007/AliESDs.root");
            //else if(fid==8) chain->Add("/Users/blim/data/LHC15g3a3/225106/008/AliESDs.root");
            //else if(fid==9) chain->Add("/Users/blim/data/LHC15g3a3/225106/009/AliESDs.root");
            //else if(fid==10) chain->Add("/Users/blim/data/LHC15g3a3/225106/010/AliESDs.root");
        }
    } else {
        chain=new TChain("aodTree");
        if(!isMC){
        //chain->Add("/opt/alice/data/2015/LHC15f/000225106/pass2/15000225106019.1010/AliAOD.root");
        if(fid==1) chain->Add("/Users/blim/data/2015/LHC15f/000225106/pass2/AOD184/0001/AliAOD.root");
        else if(fid==2) chain->Add("/Users/blim/data/2015/LHC15f/000225106/pass2/AOD184/0002/AliAOD.root");
        //else if(fid==3) chain->Add("/Users/blim/data/2015/LHC15f/000225106/pass2/AOD184/0003/AliAOD.root");
        //else if(fid==4) chain->Add("/Users/blim/data/2015/LHC15f/000225106/pass2/AOD184/0004/AliAOD.root");
        //else if(fid==5) chain->Add("/Users/blim/data/2015/LHC15f/000225106/pass2/AOD184/0005/AliAOD.root");
        //else if(fid==6) chain->Add("/Users/blim/data/2015/LHC15f/000225106/pass2/AOD184/0006/AliAOD.root");
        //else if(fid==7) chain->Add("/Users/blim/data/2015/LHC15f/000225106/pass2/AOD184/0007/AliAOD.root");
        //else if(fid==8) chain->Add("/Users/blim/data/2015/LHC15f/000225106/pass2/AOD184/0008/AliAOD.root");
        }else{
            if(fid==1) chain->Add("/Users/blim/sim/2015/LHC15g3a3/225106/AOD176/0001/AliAOD.root");
            //else if(fid==2) chain->Add("/Users/blim/data/LHC15g3a3/225106/AOD176/0002/AliAOD.root");
            //else if(fid==3) chain->Add("/Users/blim/data/LHC15g3a3/225106/AOD176/0003/AliAOD.root");
            //else if(fid==4) chain->Add("/Users/blim/data/LHC15g3a3/225106/AOD176/0004/AliAOD.root");
            //else if(fid==5) chain->Add("/Users/blim/data/LHC15g3a3/225106/AOD176/0005/AliAOD.root");
            //else if(fid==6) chain->Add("/Users/blim/data/LHC15g3a3/225106/AOD176/0006/AliAOD.root");
            //else if(fid==7) chain->Add("/Users/blim/data/LHC15g3a3/225106/AOD176/0007/AliAOD.root");
           //else if(fid==8) chain->Add("/Users/blim/data/LHC15g3a3/225106/AOD176/0008/AliAOD.root");
            //else if(fid==9) chain->Add("/Users/blim/data/LHC18c6b/252235/AOD/001/AliAOD.root");
        }
    }
  }
  if (chain) Printf("Input chain has %d entries\n",(Int_t)chain->GetEntries());
  
  /***************************************************************
    call the AnalysisSetup.C macro which:
    - loads missing libraries for the analysis
    - set config params (beam type, ESD/AOD, w/wo tender)
    - add tasks (including non-rsn tasks)
    - creates analysis manager and input handler
  ****************************************************************/
  Bool_t isAOD049 = 0;
  (aodN==49) ? isAOD049 = kTRUE :  isAOD049 = kFALSE;
  
  //  gROOT->LoadMacro("$ALICE_PHYSICS/PWGLF/RESONANCES/macros/mini/AnalysisSetup.C");
  //gROOT->LoadMacro("./AnalysisSetup.C");
  TString options;
  if (isMC) options="MC"; else options="DATA";
  if (isPP) options.Append("_PP"); else options.Append("_PBPB");
  if (isESD) options.Append("_ESD");
  if (useTender) options.Append("_TENDER");
  
  TString out = "";
  out = AnalysisSetup(nmix, options, Form("%s_RsnTask%s.root",dir.Data(),suffix.Data()), loadMacroPath, isAOD049, kTRUE, runMonOnly, isMC, "NoSIGN");
  if (out.Length() < 1) return;
  
  // add plugin to analysis manager
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) return;
  if (useAlienPlugin)
    mgr->SetGridHandler(alienHandler);
  
  if(!mgr->InitAnalysis()) {
    printf("Error: Analysis manager failed to be initialized. Nothing done!\n");
    return;
  }
  
  mgr->PrintStatus();
  if(analysisMode=="grid" && !useAlienPlugin) 
    analysisMode="local";
 
  //start analysis
  mgr->StartAnalysis(analysisMode.Data(),chain,nentries,firstentry);
  return;
}


/***************************************************************/
AliAnalysisGrid* CreateAlienHandler(TString pluginmode, 
				    TString username, 
				    TString rootVer = "v5-34-08-7", 
				    TString aliRootVer = "v5-06-03", 
				    TString aliPhysicsVer = "vAN-20150222")
{
  
  AliAnalysisAlien *plugin = new AliAnalysisAlien();
  plugin->SetRunMode(pluginmode.Data());
  plugin->SetUser(username.Data());
  plugin->SetAPIVersion("V1.1x");
  plugin->SetROOTVersion(rootVer.Data()); 
  plugin->SetAliROOTVersion(aliRootVer.Data()); 
  plugin->SetAliPhysicsVersion(aliPhysicsVer.Data()); 
  plugin->SetGridWorkingDir(myWorkDir.Data()); 
  plugin->SetGridOutputDir(myOutDir.Data()); 
  plugin->SetCheckCopy(kTRUE);
  plugin->SetGridDataDir(myGridDataDir.Data());
  plugin->SetDataPattern(myDataPattern.Data());
  //plugin->EnablePackage("PWGLFresonances.par");
  if (!isMC) plugin->SetRunPrefix("000");
  
  for (Int_t irun=runNmin;irun<runNmax;irun++){
    plugin->AddRunNumber((Int_t )runList[irun]);
  }

  /****change this to se the number of files to be used in "test" mode ****/
  plugin->SetNtestFiles(2); 
  
  /**** change this not to have the output run-by-run ****/
  plugin->SetOutputToRunNo(1); 
  plugin->SetNrunsPerMaster(1);
  
  /**** feel free to modify the list of include paths and libraries according to your need! ****/
  plugin->AddIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include -I$ALICE_ROOT/STEER -I$ALICE_ROOT/TOF -I/$ALICE_ROOT/ANALYSIS -I/$ALICE_PHYSICS/EVENTMIX -I/$ALICE_PHYSICS/PWGLF/RESONANCES -g"); 
  plugin->SetAdditionalLibs("AddTaskRare_pp13.C AnalysisSetup.C libGui.so libProof.so libMinuit.so libANALYSIS.so libOADB.so libANALYSISalice.so libCDB.so libVZERObase.so libVZEROrec.so libTENDER.so libTENDERSupplies.so libEventMixing.so libPWGLFresonances.so");

  /**** these have to be customized ****/
  plugin->SetAnalysisMacro(myMacroName.Data()); 
  plugin->SetExecutable(myExecutableName.Data()); 
  plugin->SetJDLName(myJDLname);
  
  /**** more job option ****/
  plugin->SetDefaultOutputs(kTRUE);
  plugin->SetExecutableCommand("aliroot -b -q");
  plugin->SetMaxInitFailed(15);
  plugin->SetMasterResubmitThreshold(10);
  plugin->SetTTL(50000);
  plugin->SetInputFormat("xml-single");
  plugin->SetSplitMaxInputFileNumber(30);
  plugin->SetSplitMode("se");
  
  /**** Set properly the global flac to enable/disable local or plugin merging ****/     
  plugin->SetMergeViaJDL(!isLocalMerge);
  
  /**** Force a single merging stage or define multiple stages ****/     
  // plugin->SetOneStageMerging(kTRUE);                          
  plugin->SetMaxMergeStages(2); // adapt n to your expected number of files              
  
  return plugin;
}

/***************************************************************/
TChain *CreateESDChain(TString esdpath=".",Int_t ifirst=-1,Int_t ilast=-1) {


  TChain *chainESD = new TChain("esdTree");

  if(ifirst<0) {
    chainESD->Add("AliESDs.root");
  } else {
    for(Int_t i=ifirst; i<=ilast; i++) {
      TString esdfile=esdpath; esdfile+=i; esdfile.Append("/AliESDs.root");
      chainESD->Add(esdfile.Data());
    }
  }
  
  return chainESD;
}

/***************************************************************/
void SetupIO(TString suffix="_Example")
{
  //Setup I/O paths and file names
  if (!isESD && aodN<0) {
    Printf("WARNING: AOD analysis is required but no valid AOD number is provided. Switching to ESD analysis");
    isESD = kTRUE;
  }
  
  myGridDataDir = "/alice";
  if (isMC) myGridDataDir.Append(Form("/sim"));
  else myGridDataDir.Append(Form("/data/%i",year));
  if (isMC && year>=2013) myGridDataDir.Append(Form("/%i",year));
  myGridDataDir.Append(Form("/%s",prod.Data()));
  
  if (isESD) {
    if (isMC) myDataPattern="*AliESDs.root";
    else myDataPattern=Form("*ESDs/%s/*AliESDs.root",ppass.Data());
  } else {
    if (aodN>0) myDataPattern=Form("AOD%03i/*AliAOD.root",aodN);
    else myDataPattern="AOD/*AliAOD.root";
    if (!isMC) myDataPattern.Prepend(Form("%s/",ppass.Data())); 
  }
  
  myJDLname.Append(suffix.Data()); myJDLname.Append(".jdl");
  myExecutableName.Append(suffix.Data()); myExecutableName.Append(".sh");
  myMacroName.Append(suffix.Data()); myMacroName.Append(".C");

  Printf("=========================================  Setup I/O:");
  Printf("myGridDataDir = %s", myGridDataDir.Data());
  Printf("myDataPattern = %s", myDataPattern.Data());
  Printf("myWorkDir = %s", myWorkDir.Data());
  Printf("myOutDir = %s", myOutDir.Data());
  Printf("myJDLname = %s", myJDLname.Data());
  Printf("myExecutableName = %s", myExecutableName.Data());
  Printf("myMacroName = %s", myMacroName.Data());
  Printf("=======================================================");
}
