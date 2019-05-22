#include "Deciles.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TProfile.h>
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
using std::cout;
using std::cin;

void Deciles::Loop()
{

    // read the parameters and make PRE and POST correction decile picks

   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();
   if (user_entries != -1 && user_entries < nentries) nentries = user_entries;

   fout = new TFile(nameout,"RECREATE");
    
   // TH1 to make the quantiles
   TH1D* h_quant = new TH1D("h_quant","histogram to make BBC_E quantiles;BBC_E;N",10000,0,92581.);
   TH1D* h_quant_corr = new TH1D("h_quant_corr","histogram to make BBC_E quantiles, BBC_E corrected;BBC_E;N",10000,0,92581.);
   double xq[9] = {.1,.2,.3,4,.5,.6,.7,.8,.9};
   double yq[9];
   double yq_corr[9];
   /* for (int i = 0; i<9; ++i)  xq[i] = 0.1*(i+1)}; */

   double p[6]; // th
   double p_read;
   ifstream f_in;
   cout << "Trying to read " << pfile_name << endl;
   f_in.open(pfile_name.Data());
   if (f_in.is_open()) cout << " ---- OPEN -----" << endl << endl; 
   int i_read = 0;
   while (f_in >> p_read) { p[i_read++] = p_read; cout << " p_file["<<(i_read-1)<<"] " << p[i_read-1] << endl;; }


   cout << " Starting on " << nentries/1000000 <<"M  entries" << endl;
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      if ((jentry % 1000000) == 0) cout << " finished " << jentry/1000000 << "M entries" << endl;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      h_quant->Fill(bbc_E);
      h_quant_corr->Fill(bbc_E -p[1]*vz   -p[2]*vz*vz
                               -p[3]*ZDCx -p[4]*ZDCx*ZDCx
                               -p[5]*vz*ZDCx);
      // if (Cut(ientry) < 0) continue;
   }
   h_quant->GetQuantiles(9,yq,xq);
   h_quant_corr->GetQuantiles(9,yq,xq_corr);

   ofstream o_file;
   ofstream o_corr;
   o_file.open(Form("%s_quant",pfile_name.Data()));
   o_corr.open(Form("%s_quant_corr",pfile_name.Data()));
   for (int i = 0; i<9; ++i){
       o_file << yq[i] << endl;
       o_corr << yq_corr[i] << endl;
   }
   o_file.close();
   o_corr.close();

   fout->Write();
   fout->Close();
}

Deciles::Deciles(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("test.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("test.root");
      }
      f->GetObject("events",tree);
   }
   Init(tree);
}
Deciles::Deciles(const char* _fin, const char* _fout, TString _pfile_name, Long64_t _user_entries=-1) 
    : nameout(_fout), fChain(0), pfile_name(_pfile_name), user_entries(_user_entries) {
    TFile *f = new TFile(_fin, "read");
    if (!f) { cout << " fatal error, couldn't read TFile\""<<_fin<<"\"." << endl;
              exit(2);
    };
    TTree* tree;
    f->GetObject("events",tree);
    Init(tree);
};
Deciles::~Deciles()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
   /* fout->Write(); */
   /* fout->Close(); */
}

Int_t Deciles::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Deciles::LoadTree(Long64_t entry)
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

void Deciles::Init(TTree *tree)
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

Bool_t Deciles::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Deciles::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Deciles::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
