#define events_cxx
#include "events.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void events::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L events.C
//      Root > events t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
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
   Long64_t nentries = fChain->GetEntriesFast();
   TFile *f = new TFile("TProfile004_to_fit_negrank.root","recreate");
   TProfile *bbc = new TProfile("bbc","bbc vz ZDCx;ZDCx;bbc",1000,0,50000);
   TProfile *bbc_i10 = new TProfile("bbc_i10","bbc vz ZDCx, |vz|<10;ZDCx;bbc",1000,0,50000);
   TProfile *bbc_i10i6 = new TProfile("bbc_i10i6","bbc vz ZDCx, |vz|<10, |vpd-vz|<6;ZDCx;bbc",1000,0,50000);
   TProfile *bbc_i6 = new TProfile("bbc_i6","bbc vz ZDCx, |vpd-vz|<6;ZDCx;bbc",1000,0,50000);
   TProfile *bbc_i10i6zRange = new TProfile("bbc_i10i6zRange","bbc vz ZDCx, |vpd-vz|<6, |vz|<10, bbc#in(7k,24k);ZDCx;bbc",1000,0,50000);
   TProfile2D* b_vzZDCz = new TProfile2D("b_vzZDCz","bbc_E(vz,ZDCx);vz;ZDCx",20,-10,10,100,7000.,24000.);
   TH1D *hg_bbc = new TH1D("hg_bbc","BBC_E for |vz-vzVpd|, |vz|<10; BBC_{east}",1000,-1000.,98000.);
   TH1D *deciles = new TH1D("bbc_deciles","BBC_E for |vz-vzVpd|, |vz|<10; BBC_{east}",100000,-1000.,98000.);

   Long64_t nbytes = 0, nb = 0;
   cout << "nentries " << nentries/1000000 << "M" << endl;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      if (jentry%100000 == 0) cout << "Finished " << Form("%.1f",jentry/1.e6) << "M entries " << endl;

      if (ranking>0) continue;
      bbc->Fill(ZDCx, bbc_E);
      if (TMath::Abs(vz)<10) bbc_i10->Fill(ZDCx, bbc_E);
      if (TMath::Abs(vzVpd-vz)<6) bbc_i6->Fill(ZDCx, bbc_E);
      if (TMath::Abs(vzVpd-vz)<6 && TMath::Abs(vz)<10) bbc_i10i6->Fill(ZDCx, bbc_E);
      if (TMath::Abs(vzVpd-vz)<6 && TMath::Abs(vz)<10 && ZDCx>7.e3 && ZDCx <24.e3) {
          bbc_i10i6zRange->Fill(ZDCx, bbc_E);
          b_vzZDCz->Fill(vz,ZDCx,bbc_E);
          hg_bbc->Fill(bbc_E);
          deciles->Fill(bbc_E);
      }
      // if (Cut(ientry) < 0) continue;
   }
   f->Write();
   f->Close();

   //-----------------------------
   //-----------------------------
   //-----------------------------
   //-----------------------------
}
