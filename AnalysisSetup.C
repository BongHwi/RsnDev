//fbellini

#include "AliESDInputHandler.h"
#include "AliAODInputHandler.h"
#include "AliVEventHandler.h"
#include "AliAnalysisAlien.h"
#include "AliRsnCutSetDaughterParticle.h"

#include "AliMCEventHandler.h"

//#include "$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C"
//#include "$ALICE_PHYSICS/OADB/macros/AddTaskCentrality.C"
//#include "/Users/aknospe/alice/sw/osx_x86-64/AliPhysics/latest/OADB/macros/AddTaskPIDResponse.C"
//#include "$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C"
#include "./AddMonitorOutput.C"
//#include "/Users/aknospe/Documents/Austin/resonance/dev/AddTaskPhiPP13TeV_PID.C"
//#include "/Users/aknospe/Documents/Houston/dibaryon/1dev/AddTaskPXi0.C"
//#include "./AddTaskRare_pp13.C"
//#include "/Users/aknospe/Documents/Houston/resonances/Xi1820/dev/AddTaskRare_pp13.C"
//#include "/Users/aknospe/Documents/Houston/resonances/penta/3dev/AddTaskResonanceFinder.C"
#include "AddTaskRare_pp13.C"
//#include "AddTaskSigmaStar_pp13TeV.C"
//#include "./AddTaskLstar13TeVpp_II.C"

TString AnalysisSetup(Int_t       nmix,
		      const char *options,
		      const char *outputFileName,
		      const char *macroPath,
		      Bool_t      isAOD49=kFALSE,
		      Bool_t      enableMon=kTRUE,
		      Bool_t      runMonOnly=kFALSE,
		      Bool_t      isMcTrueOnly=kFALSE,
		      TString     monitorOpt = "NoSIGN")
{
   // prepare output
   TString out("");
   
   // === EXAMINE OPTIONS ==========================================================================
   TString opt(options);
   opt.ToUpper();

   cerr<<"********** "<<options<<endl;
   
   Bool_t isMC      = opt.Contains("MC") || (!opt.Contains("DATA"));
   Bool_t isPP      = opt.Contains("PP") || (!opt.Contains("PBPB"));
   Bool_t isESD     = opt.Contains("ESD");
   Bool_t useTender = opt.Contains("TENDER");
   Bool_t noV0      = opt.Contains("NOV0");
   
   //
   // === LOAD LIBRARIES ===========================================================================
   // uncomment only if not already done in the steering macro, eg. RunGrid.C
   // load analysis libraries
   gSystem->AddIncludePath("-I$ALICE_ROOT/STEER/STEER -I$ALICE_ROOT/STEER/STEERBase -I$ALICE_ROOT/ANALYSISalice");
   gSystem->Load("libTree.so");
   gSystem->Load("libGeom.so");
   gSystem->Load("libVMC.so");
   gSystem->Load("libPhysics.so");
   gSystem->Load("libSTEERBase.so");
   gSystem->Load("libESD.so");
   gSystem->Load("libAOD.so");
   gSystem->Load("libANALYSIS.so");
   gSystem->Load("libOADB");
   gSystem->Load("libANALYSISalice.so");
   gSystem->Load("libEventMixing.so");
   gSystem->Load("libPWGLFresonances.so");
   
   // tender-related libraries
   if (isESD && useTender) {
      ::Info("AnalysisSetup", "Loading tender libraries");
      gSystem->Load("libTENDER.so");
      gSystem->Load("libTENDERSupplies.so");
   } else if (!isESD) {
      useTender = kFALSE;
   }
   
   // load development RSN library
   // uncomment only for par files usage (not recommended)
   //if (!AliAnalysisAlien::SetupPar("PWGLFresonances.par")) return "";

   //
   // === CREATE ANALYSIS MANAGER ==================================================================
   //

   AliAnalysisManager *mgr = new AliAnalysisManager("RsnAnalysisManager");
   mgr->SetCommonFileName(outputFileName);
   ::Info("AnalysisSetup", "Common file name: %s", outputFileName);

   //
   // === INPUT / OUTPUT HANDLER CONFIGURATION =====================================================
   //

   if (isESD) {
      out = "esdTree";
      ::Info("AnalysisSetup", "Creating ESD handler");
      AliESDInputHandler *esdHandler = new AliESDInputHandler();
      mgr->SetInputEventHandler(esdHandler);
      esdHandler->SetNeedField();
      if (isMC) {
         ::Info("AnalysisSetup", "Creating MC handler");
         AliMCEventHandler *mcHandler  = new AliMCEventHandler();
         mgr->SetMCtruthEventHandler(mcHandler);
      }
   } else {
      out = "aodTree";
      ::Info("AnalysisSetup", "Creating AOD handler");
      AliAODInputHandler *aodHandler = new AliAODInputHandler();
      mgr->SetInputEventHandler(aodHandler);
   }
  
   //
   // === PHYSICS SELECTION =============================================================
   // Check that the correct trigger class is selected!!! USER DEFINED!!!

   if (isESD) {
     ::Info("AnalysisSetup", "Add physics selection");
     //gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
     //AliPhysicsSelectionTask* physSelTask = AddTaskPhysicsSelection(isMC);
     AliPhysicsSelectionTask *physSelTask = reinterpret_cast<AliPhysicsSelectionTask *>(gInterpreter->ExecuteMacro(Form("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C(%d)", isMC)));
     physSelTask->SelectCollisionCandidates(AliVEvent::kMB); 
     if (noV0) {
       ::Info("AnalysisSetup", "Skip of V0 info is required");
       //physSelTask->GetPhysicsSelection()->SetSkipV0(kTRUE);
     }
   }
   
   //
   // === CENTRALITY/PLANE ==============================================================
   //
   
     ::Info("AnalysisSetup", "Add Multiplicity and event plane computation tasks");
     //gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskCentrality.C");
     //AliCentralitySelectionTask *taskCentrality = (AliCentralitySelectionTask *)AddTaskCentrality();

     AliMultSelectionTask* MultSlection =
        reinterpret_cast<AliMultSelectionTask*>(gInterpreter->ExecuteMacro(
            "$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/"
            "AddTaskMultSelection.C"));
   
   
   //
   // === PID RESPONSE =============================================================================
   //   
   ::Info("AnalysisSetup", "Add task for PID response");
   //gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C");
   /*
   AddTaskPIDResponse(isMC, 
		      isMC, //autoMCesd
		      kFALSE,//isMC, //tuneOnData
		      2, //recopass
                      kFALSE, //cachePID, default
		      "",//detResponse, default
                      kTRUE,//useTPCEtaCorrection, // Please use default value! Otherwise splines can be off
                      kFALSE, //useTPCMultiplicityCorrection --> default was kTRUE, but not avail for LHC13b2_efix_p1 
		      -1); //recodatapass, default=-1
   */
   //AliAnalysisTask *fPIDResponse = reinterpret_cast<AliAnalysisTask *>(gInterpreter->ExecuteMacro(Form("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C(%d,%d,%d,%d,%s,%f,%d,%d)", isMC, 0, 2, 0, "", 1, 0, -1)));

   // PID response
   AliAnalysisTask *fPIDResponse = reinterpret_cast<AliAnalysisTask *>(gInterpreter->ExecuteMacro(Form("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C(%d)", isMC)));
   if (!fPIDResponse)
   {
      Printf("no fPIDResponse");
   }
   //
   // === PID QA ===================================================================================
   //   
   //gROOT->LoadMacro("$(ALICE_ROOT)/ANALYSIS/macros/AddTaskPIDqa.C ");
   //AddTaskPIDqa();
   
   //
   // === RSN TASKS ==============================================================================
   //    
   ::Info("AnalysisSetup", "Add task for XiStar");
   //gROOT->LoadMacro("./AddTaskPhiPP13TeV_PID.C");
   //AddTaskPhiPP13TeV_PID(isMC,isPP,"def",0,0,3,0,1,AliRsnCutSetDaughterParticle::kTPCTOFpidphipp2015,3002.);
   //AddTaskPhiPP13TeV_PID(isMC,isPP,"trig",6,0,3,0,1,AliRsnCutSetDaughterParticle::kTPCTOFpidphipp2015,3002.);
   //AddTaskPhiPP13TeV_PID(isMC,isPP,"vzoff",7,0,3,0,1,AliRsnCutSetDaughterParticle::kTPCTOFpidphipp2015,3002.);
   //AddTaskPhiPP13TeV_PID(isMC,isPP,"inel",8,0,3,0,1,AliRsnCutSetDaughterParticle::kTPCTOFpidphipp2015,3002.);
   //AddTaskPhiPP13TeV_PID(isMC,isPP,"inel10",9,0,3,0,1,AliRsnCutSetDaughterParticle::kTPCTOFpidphipp2015,3002.);
   //AddTaskPhiPP13TeV_PID(isMC,isPP,"igz10",11,0,3,0,1,AliRsnCutSetDaughterParticle::kTPCTOFpidphipp2015,3002.);
   //gROOT->LoadMacro("./AddTaskRare_pp13.C");
   //AddTaskRare_pp13("pipi",isMC,0,AliRsnDaughter::kPion,AliRsnDaughter::kPion,0,0,225);//9010221);
   //AddTaskRare_pp13("pipi_alt",isMC,0,AliRsnDaughter::kPion,AliRsnDaughter::kPion,0,2030,225);
   //AddTaskRare_pp13("pikx_tpc2s",kFALSE,0,AliRsnDaughter::kPion,AliRsnDaughter::kKaon,10,10020,10020);
   //AddTaskRare_pp13("pikx_tof2s",kFALSE,0,AliRsnDaughter::kPion,AliRsnDaughter::kKaon,10,22000,22000);
   //AddTaskRare_pp13("pkx_tpc2s",kFALSE,0,AliRsnDaughter::kProton,AliRsnDaughter::kKaon,10,10020,10020);
   //AddTaskRare_pp13("pkx_tof2s",kFALSE,0,AliRsnDaughter::kProton,AliRsnDaughter::kKaon,10,22000,22000);
   //AddTaskRare_pp13("pikx",isMC,0,AliRsnDaughter::kPion,AliRsnDaughter::kKaon);
   //AddTaskRare_pp13("pik0",isMC,0,AliRsnDaughter::kPion,AliRsnDaughter::kKaon0);
   //AddTaskRare_pp13("kxkx",isMC,0,AliRsnDaughter::kKaon,AliRsnDaughter::kKaon);
   //AddTaskRare_pp13("kxkx_alt",kTRUE,0,AliRsnDaughter::kKaon,AliRsnDaughter::kKaon,0,2020,2020);
   //AddTaskRare_pp13("kxk0",isMC,0,AliRsnDaughter::kKaon,AliRsnDaughter::kKaon0);
   //AddTaskRare_pp13("pkx",isMC,0,AliRsnDaughter::kProton,AliRsnDaughter::kKaon);
   //AddTaskRare_pp13("pk0",isMC,0,AliRsnDaughter::kProton,AliRsnDaughter::kKaon0);
   //AddTaskRare_pp13("Lambdapi",isMC,0,AliRsnDaughter::kLambda,AliRsnDaughter::kPion,0,0,10030);
   //AddTaskRare_pp13("Lambdakx",isMC,0,AliRsnDaughter::kLambda,AliRsnDaughter::kKaon);
   //AddTaskRare_pp13("Lambdakx_c1",kFALSE,0,AliRsnDaughter::kLambda,AliRsnDaughter::kKaon,0,1);
   //AddTaskRare_pp13("Lambdakx_c2",kFALSE,0,AliRsnDaughter::kLambda,AliRsnDaughter::kKaon,0,2);
   //AddTaskRare_pp13("Lambdak0",isMC,0,AliRsnDaughter::kLambda,AliRsnDaughter::kKaon0);
   //AddTaskRare_pp13("Lambdak0_c1",kFALSE,0,AliRsnDaughter::kLambda,AliRsnDaughter::kKaon0,0,1);
   //AddTaskRare_pp13("Lambdak0_c2",kFALSE,0,AliRsnDaughter::kLambda,AliRsnDaughter::kKaon0,0,2);
   //AddTaskRare_pp13("Lambdak0_c3",kFALSE,0,AliRsnDaughter::kLambda,AliRsnDaughter::kKaon0,0,3);
   //AddTaskRare_pp13("Lambdap",isMC,0,AliRsnDaughter::kLambda,AliRsnDaughter::kProton,0,0,10030);
   //gROOT->LoadMacro("$ALICE_PHYSICS/PWGLF/RESONANCES/macros/mini/AddTaskF0.C");
   //AddTaskF0("f0",kTRUE,AliPIDResponse::kPP,AliVEvent::kINT7,kFALSE,0,0,5,AliRsnCutSetDaughterParticle::kTPCpidTOFveto3s,2,0.3,1.3,100,kFALSE);
    
    
    //AddTaskResonanceFinder("phipi",isMC,0,AliRsnDaughter::kPhi,AliRsnDaughter::kPion);
    //AddTaskResonanceFinder("piphi",isMC,0,AliRsnDaughter::kPion,AliRsnDaughter::kPhi);
    //AddTaskResonanceFinder("kxphi",isMC,0,AliRsnDaughter::kKaon,AliRsnDaughter::kPhi);
    //AddTaskResonanceFinder("k0phi",isMC,0,AliRsnDaughter::kKaon0,AliRsnDaughter::kPhi);
    //AddTaskResonanceFinder("pphi",isMC,0,AliRsnDaughter::kProton,AliRsnDaughter::kPhi);
    //AddTaskResonanceFinder("phiphi",isMC,0,AliRsnDaughter::kPhi,AliRsnDaughter::kPhi);
    //AddTaskResonanceFinder("Lambdaphi",isMC,0,AliRsnDaughter::kLambda,AliRsnDaughter::kPhi);
    //AddTaskResonanceFinder("pikstar0",isMC,0,AliRsnDaughter::kPion,AliRsnDaughter::kKstar0);
    //AddTaskResonanceFinder("kxSigmastar",isMC,0,AliRsnDaughter::kKaon,AliRsnDaughter::kSigmastarp);
    //AddTaskResonanceFinder("k0Sigmastar",isMC,0,AliRsnDaughter::kKaon0,AliRsnDaughter::kSigmastarp);
    //AddTaskResonanceFinder("Kstar0Sigmastar",isMC,0,AliRsnDaughter::kKstar0,AliRsnDaughter::kSigmastarp);
    //AddTaskResonanceFinder("Kstar0SBSigmastar",isMC,0,AliRsnDaughter::kKstar0,AliRsnDaughter::kSigmastarp,0,10000,0);
    //AddTaskResonanceFinder("KstarxSigmastar",isMC,0,AliRsnDaughter::kKstarpm,AliRsnDaughter::kSigmastarp);
    //AddTaskResonanceFinder("piKstar0",isMC,0,AliRsnDaughter::kPion,AliRsnDaughter::kKstar0);     
    //AddTaskResonanceFinder("piKsrarx",isMC,0,AliRsnDaughter::kPion,AliRsnDaughter::kKstarpm);

    //AddTaskLstar13TeVpp(isMC,isPP,5,0,0,0,1);
    AddTaskRare_pp13("Xi1530",isMC,0,AliRsnDaughter::kXi,AliRsnDaughter::kPion);
    //AddTaskPXi0("test",isMC,isPP);
    //AddTaskSigmaStar_pp13TeV("Sigam1385",isMC,0,20010,0,0);

     ::Info("AnalysisSetup", "Setup successful");
   return out;
}
