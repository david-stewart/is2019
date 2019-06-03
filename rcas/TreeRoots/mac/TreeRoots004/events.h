//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri May 24 07:23:45 2019 by ROOT version 5.34/30
// from TTree events/tree  w/(SL18b)events w/triggerId=500004
// found on file: hadd_004.root
//////////////////////////////////////////////////////////

#ifndef events_h
#define events_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <TObject.h>

// Fixed size dimensions of array or collections stored in the TTree if any.

class events {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
 //TreeEvent       *event;
   UInt_t          fUniqueID;
   UInt_t          fBits;
   Int_t           runId;
   Float_t         ranking;
   Float_t         vz;
   Float_t         vzVpd;
   Int_t           nVpdHitsEast;
   Int_t           nVpdHitsWest;
   Bool_t          is_vz_vzVpd;
   Int_t           nch_tof;
   Int_t           nch;
   Int_t           nchE;
   Int_t           nchW;
   Int_t           nch_ltp5;
   Int_t           ntow_gtp2;
   Int_t           ntow_gtp2_E;
   Int_t           ntow_gtp2_W;
   Int_t           ntow_gt4;
   Int_t           ntow_gt4_E;
   Int_t           ntow_gt4_W;
   Float_t         bbc_E;
   Float_t         bbc_ES;
   Float_t         bbc_EL;
   Float_t         bbc_W;
   Float_t         bbc_WS;
   Float_t         bbc_WL;
   Float_t         ZDCx;
   Float_t         ZdcEastRate;
   Float_t         ZdcWestRate;
   Float_t         ZdcSumAdcEast;
   Float_t         ZdcSumAdcWest;

   // List of branches
   TBranch        *b_event_fUniqueID;   //!
   TBranch        *b_event_fBits;   //!
   TBranch        *b_event_runId;   //!
   TBranch        *b_event_ranking;   //!
   TBranch        *b_event_vz;   //!
   TBranch        *b_event_vzVpd;   //!
   TBranch        *b_event_nVpdHitsEast;   //!
   TBranch        *b_event_nVpdHitsWest;   //!
   TBranch        *b_event_is_vz_vzVpd;   //!
   TBranch        *b_event_nch_tof;   //!
   TBranch        *b_event_nch;   //!
   TBranch        *b_event_nchE;   //!
   TBranch        *b_event_nchW;   //!
   TBranch        *b_event_nch_ltp5;   //!
   TBranch        *b_event_ntow_gtp2;   //!
   TBranch        *b_event_ntow_gtp2_E;   //!
   TBranch        *b_event_ntow_gtp2_W;   //!
   TBranch        *b_event_ntow_gt4;   //!
   TBranch        *b_event_ntow_gt4_E;   //!
   TBranch        *b_event_ntow_gt4_W;   //!
   TBranch        *b_event_bbc_E;   //!
   TBranch        *b_event_bbc_ES;   //!
   TBranch        *b_event_bbc_EL;   //!
   TBranch        *b_event_bbc_W;   //!
   TBranch        *b_event_bbc_WS;   //!
   TBranch        *b_event_bbc_WL;   //!
   TBranch        *b_event_ZDCx;   //!
   TBranch        *b_event_ZdcEastRate;   //!
   TBranch        *b_event_ZdcWestRate;   //!
   TBranch        *b_event_ZdcSumAdcEast;   //!
   TBranch        *b_event_ZdcSumAdcWest;   //!

   events(TTree *tree=0);
   virtual ~events();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef events_cxx
events::events(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("hadd_004.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("hadd_004.root");
      }
      f->GetObject("events",tree);

   }
   Init(tree);
}

events::~events()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t events::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t events::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void events::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("fUniqueID", &fUniqueID, &b_event_fUniqueID);
   fChain->SetBranchAddress("fBits", &fBits, &b_event_fBits);
   fChain->SetBranchAddress("runId", &runId, &b_event_runId);
   fChain->SetBranchAddress("ranking", &ranking, &b_event_ranking);
   fChain->SetBranchAddress("vz", &vz, &b_event_vz);
   fChain->SetBranchAddress("vzVpd", &vzVpd, &b_event_vzVpd);
   fChain->SetBranchAddress("nVpdHitsEast", &nVpdHitsEast, &b_event_nVpdHitsEast);
   fChain->SetBranchAddress("nVpdHitsWest", &nVpdHitsWest, &b_event_nVpdHitsWest);
   fChain->SetBranchAddress("is_vz_vzVpd", &is_vz_vzVpd, &b_event_is_vz_vzVpd);
   fChain->SetBranchAddress("nch_tof", &nch_tof, &b_event_nch_tof);
   fChain->SetBranchAddress("nch", &nch, &b_event_nch);
   fChain->SetBranchAddress("nchE", &nchE, &b_event_nchE);
   fChain->SetBranchAddress("nchW", &nchW, &b_event_nchW);
   fChain->SetBranchAddress("nch_ltp5", &nch_ltp5, &b_event_nch_ltp5);
   fChain->SetBranchAddress("ntow_gtp2", &ntow_gtp2, &b_event_ntow_gtp2);
   fChain->SetBranchAddress("ntow_gtp2_E", &ntow_gtp2_E, &b_event_ntow_gtp2_E);
   fChain->SetBranchAddress("ntow_gtp2_W", &ntow_gtp2_W, &b_event_ntow_gtp2_W);
   fChain->SetBranchAddress("ntow_gt4", &ntow_gt4, &b_event_ntow_gt4);
   fChain->SetBranchAddress("ntow_gt4_E", &ntow_gt4_E, &b_event_ntow_gt4_E);
   fChain->SetBranchAddress("ntow_gt4_W", &ntow_gt4_W, &b_event_ntow_gt4_W);
   fChain->SetBranchAddress("bbc_E", &bbc_E, &b_event_bbc_E);
   fChain->SetBranchAddress("bbc_ES", &bbc_ES, &b_event_bbc_ES);
   fChain->SetBranchAddress("bbc_EL", &bbc_EL, &b_event_bbc_EL);
   fChain->SetBranchAddress("bbc_W", &bbc_W, &b_event_bbc_W);
   fChain->SetBranchAddress("bbc_WS", &bbc_WS, &b_event_bbc_WS);
   fChain->SetBranchAddress("bbc_WL", &bbc_WL, &b_event_bbc_WL);
   fChain->SetBranchAddress("ZDCx", &ZDCx, &b_event_ZDCx);
   fChain->SetBranchAddress("ZdcEastRate", &ZdcEastRate, &b_event_ZdcEastRate);
   fChain->SetBranchAddress("ZdcWestRate", &ZdcWestRate, &b_event_ZdcWestRate);
   fChain->SetBranchAddress("ZdcSumAdcEast", &ZdcSumAdcEast, &b_event_ZdcSumAdcEast);
   fChain->SetBranchAddress("ZdcSumAdcWest", &ZdcSumAdcWest, &b_event_ZdcSumAdcWest);
   Notify();
}

Bool_t events::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void events::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t events::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef events_cxx
