#define cdec_cxx
#include "cdec.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void cdec::Loop()
{
//   In a ROOT session, you can do:
//      root> .L cdec.C
//      root> cdec t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   TFile* f_out = new TFile("bbc_dist.root","recreate");
   
   TH1D*     o_dist = new TH1D("o_dist",";bbcE;N",100000,0, 93000.);
   TProfile* o_bbc_ZDCx = new TProfile("o_bbc_ZDCx",";bbcE;ZDCx",1000,0, 93000.);
   TProfile* o_ZDCx_bbc = new TProfile("o_ZDCx_bbc",";ZDCx;bbc", 1000,0, 50000.);
   
   TH1D*     d_dist = new TH1D("d_dist",";bbcE;N",100000,-48000, 93000.);
   TProfile* d_bbc_ZDCx = new TProfile("d_bbc_ZDCx",";bbcE;ZDCx",1000,-48000, 93000.);
   TProfile* d_ZDCx_bbc = new TProfile("d_ZDCx_bbc",";ZDCx;bbc", 1000,0, 50000.);

   /* TH1D* corr_dist = new TH1D("corr_dist",";bbcE;N",100000,-48000, 93000.); */
   /* TProfile* corr_bbc_v_ZDCx = new TProfile("corr_ZDCx_v_bbc",";bbcE;ZDCx",1000,0, 50000.); */
   /* TProfile* corr_ZDCx_v_bbc = new TProfile("ZDCx_v_bbc",";ZDCx;bbc",1000,-48000, 93000.); */

   double vmax=0;
   double vmin=0;

   double o_vmax=0;
   double o_vmin=0;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      if (TMath::Abs(vz - vzVpd)>6) continue;
      double bbc = bbc_E-8.699*vz-0.4383*vz*vz-0.6851*ZDCx+1.161e-5*ZDCx*ZDCx-0.003482*ZDCx*vz;
      if (bbc > vmax) vmax = bbc;
      if (bbc < vmin) vmin = bbc;
      if (bbc_E > o_vmax) o_vmax = bbc_E;
      if (bbc_E < o_vmin) o_vmin = bbc_E;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

       o_dist->Fill(bbc_E);
       o_bbc_ZDCx->Fill(bbc_E,ZDCx);
       o_ZDCx_bbc->Fill(ZDCx,bbc_E);

       d_dist->Fill(bbc);
       d_bbc_ZDCx->Fill(bbc,ZDCx);
       d_ZDCx_bbc->Fill(ZDCx,bbc);
   }
   cout << " max:min " << vmin << "  " << vmax << endl;
   cout << " orig max:min " << o_vmin << "  " << o_vmax << endl;
   f_out->Write();
   f_out->Close();
}
