#define e_tree_cxx
#include "e_tree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TRandom3.h>

void e_tree::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L e_tree.C
//      Root > e_tree t
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
   int c_break = 0;

   // make the output file to fill with the appropriate data 
   TFile* f_out = new TFile(Form("%s.root",ftype.Data()),"RECREATE");
   const char* name (ftype.Data());
   int nbins = fnbins_per_GeV*15;
   f_out->cd();
   // [A/B/_]_[T/M/TM]_[pi/pin/K/Kn/p/pbar]
   TH1D* T = new TH1D(Form("T_%s",name),   Form("Truth %s;p_{T};N_{MC Tracks}",name), nbins, 0., 15.);
   TH1D* M = new TH1D(Form("M_%s",name),   Form("Measured %s;p_{T};N_{Measured Tracks}",name), nbins, 0., 15.);
   TH2D* TM = new TH2D(Form("TM_%s",name), Form("Paired %s;p_{T,truth};p_{T,measured}",name), nbins, 0.,15.,nbins,0.,15.);
   TH2D* MT = new TH2D(Form("MT_%s",name), Form("Paired %s;p_{T,measured};p_{T,truth}",name), nbins, 0.,15.,nbins,0.,15.);
   T->Sumw2(); M->Sumw2(); TM->Sumw2(); MT->Sumw2();
 
   TH1D* A_T = new TH1D(Form("A_T_%s",name), Form("A_Truth %s;p_{T};N_{MC Tracks}",name), nbins, 0., 15.);
   TH1D* A_M = new TH1D(Form("A_M_%s",name), Form("A_Measured %s;p_{T};N_{Measured Tracks}",name), nbins, 0., 15.);
   TH2D* A_TM = new TH2D(Form("A_TM_%s",name), Form("A_Paired %s;p_{T,truth};p_{T,meas}",name), nbins, 0.,15.,nbins,0.,15.);
   TH2D* A_MT = new TH2D(Form("A_MT_%s",name), Form("A_Paired %s;p_{T,meas};p_{T,truth}",name), nbins, 0.,15.,nbins,0.,15.);
   A_T->Sumw2(); A_M->Sumw2(); A_TM->Sumw2(); A_MT->Sumw2();
       
   TH1D* B_T = new TH1D(Form("B_T_%s",name), Form("B_Truth %s;p_{T};N_{MC Tracks}",name), nbins, 0., 15.);
   TH1D* B_M = new TH1D(Form("B_M_%s",name), Form("B_Measured %s;p_{T};N_{Measured Tracks}",name), nbins, 0., 15.);
   TH2D* B_TM = new TH2D(Form("B_TM_%s",name), Form("B_Paired;p_{T,truth};p_{T,measured}",name), nbins, 0.,15.,nbins,0.,15.);
   TH2D* B_MT = new TH2D(Form("B_MT_%s",name), Form("B_Paired %s;p_{T,meas};p_{T,truth}",name), nbins, 0.,15.,nbins,0.,15.);
   B_T->Sumw2(); B_M->Sumw2(); B_TM->Sumw2(); B_MT->Sumw2();

   fFile->cd();
   TRandom3* r3 = new TRandom3();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      /* cout << "ptMc : " << ptMc << endl; */
        T->Fill(ptMc);
        if (is_matched) {
            M->Fill(ptPr);
            TM->Fill(ptMc, ptPr);
            MT->Fill(ptPr, ptMc);
        }

        if ( r3->Integer(2) == 0 ){
            A_T->Fill(ptMc);
            if (is_matched) {
                A_M->Fill(ptPr);
                A_TM->Fill(ptMc, ptPr);
                A_MT->Fill(ptPr, ptMc);
            }
        } else {
            B_T->Fill(ptMc);
            if (is_matched) {
                B_M->Fill(ptPr);
                B_TM->Fill(ptMc, ptPr);
                B_MT->Fill(ptPr, ptMc);
            }
        }

      /* if (c_break++ > 10) break; */
      // if (Cut(ientry) < 0) continue;
   }
   f_out->Write();
   f_out->Close();
}
