#include "TreeRootsAnalysis.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TProfile.h>
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <TProfile2D.h>
#include <sstream>
using std::cout;
using std::cin;

// required options:
// 1. number of entries required
void TreeRootsAnalysis::Loop() { };
void TreeRootsAnalysis::make_TProfile2D()
{
   if (fChain == 0) return;
   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nevents;
   options >> nevents;
   if (nevents != -1 && nevents < nentries) nentries = nevents;

   string nameout;
   options >> nameout;
   fout = new TFile(nameout.c_str(),"RECREATE");

   TProfile2D *p2All_e = new TProfile2D("p2All_e","BBC with error vs. Vz and ZDCx;vz;ZDCx",   120,-30,30,120,0,35000,"e");
   TProfile2D *p2All_s = new TProfile2D("p2All_s","BBC with err=std vs. Vz and ZDCx;vz;ZDCx", 120,-30,30,120,0,35000,"s");

   TProfile2D *p2Sub_e = new TProfile2D("p2sub_e","BBC with error vs. Vz and ZDCx;vz;ZDCx",   120,-30,30,120,0,35000,"e");
   TProfile2D *p2Sub_s = new TProfile2D("p2sub_s","BBC with err=std vs. Vz and ZDCx;vz;ZDCx", 120,-30,30,120,0,35000,"s");

   TProfile   *pAll_e = new TProfile("pAll_e","BBC with error   vz. Vz, all events", 120,-30,30,"e");
   TProfile   *pAll_s = new TProfile("pAll_s","BBC with err=std vz. Vz, all events", 120,-30,30,"s");

   TProfile   *pSub_e = new TProfile("psub_e","BBC with error   vz. Vz, Vz-Vpd<6 events", 120,-30,30,"e");
   TProfile   *pSub_s = new TProfile("psub_s","BBC with err=std vz. Vz, Vz-Vpd<6 events", 120,-30,30,"s");

   cout << " Starting on " << (double)nentries/1000000 <<"M  entries" << endl;
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      if ((jentry % 1000000) == 0) cout << " finished " << jentry/1000000 << "M entries" << endl;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      p2All_e->Fill(vz, ZDCx, bbc_E);
      p2All_s->Fill(vz, ZDCx, bbc_E);

      pAll_e->Fill(vz,bbc_E);
      pAll_s->Fill(vz,bbc_E);

      if (TMath::Abs(vz - vzVpd) < 6) {
          p2Sub_e->Fill(vz, ZDCx, bbc_E);
          p2Sub_s->Fill(vz, ZDCx, bbc_E);
          pSub_e->Fill(vz,bbc_E);
          pSub_s->Fill(vz,bbc_E);
      }

   }
   fout->Write();
   fout->Close();
}

/* TreeRootsAnalysis::TreeRootsAnalysis(TTree *tree) : fChain(0) */ 
/* { */
/* // if parameter tree is not specified (or zero), connect the file */
/* // used to generate this class and read the Tree. */
/*    if (tree == 0) { */
/*       TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("test.root"); */
/*       if (!f || !f->IsOpen()) { */
/*          f = new TFile("test.root"); */
/*       } */
/*       f->GetObject("events",tree); */
/*    } */
/*    Init(tree); */
/* } */
TreeRootsAnalysis::TreeRootsAnalysis(const char* _myoptions) : options(_myoptions) {
    string _fin;
    options >> _fin; 
    TFile *f = new TFile(_fin.c_str(), "read");
    if (!f) { cout << " fatal error, couldn't read TFile\""<<_fin<<"\"." << endl;
              exit(2);
    };
    TTree* tree;
    f->GetObject("events",tree);
    Init(tree);
};
/* TreeRootsAnalysis::TreeRootsAnalysis(const char* _fin, const char* _fout, Long64_t _nevents) : nameout(_fout), fChain(0), nevents(_nevents) { */
/*     TFile *f = new TFile(_fin, "read"); */
/*     if (!f) { cout << " fatal error, couldn't read TFile\""<<_fin<<"\"." << endl; */
/*               exit(2); */
/*     }; */
/*     TTree* tree; */
/*     f->GetObject("events",tree); */
/*     Init(tree); */
/* }; */
TreeRootsAnalysis::~TreeRootsAnalysis()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
   /* fout->Write(); */
   /* fout->Close(); */
}

Int_t TreeRootsAnalysis::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TreeRootsAnalysis::LoadTree(Long64_t entry)
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

void TreeRootsAnalysis::Init(TTree *tree)
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
   fChain->SetBranchAddress("vz", &vz, &b_event_vz);
   fChain->SetBranchAddress("vzVpd", &vzVpd, &b_event_vzVpd);
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

Bool_t TreeRootsAnalysis::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TreeRootsAnalysis::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TreeRootsAnalysis::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
