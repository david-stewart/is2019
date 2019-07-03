#define JetLoop_cxx
#include "JetLoop.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TProfile2D.h>
#include <iostream>
#include <fstream>

static const double TrigDist { 0.4 }; 
static const double NearSide = TMath::Pi()/8.;

bool JetLoop::IsNearSide(int i) {
    return TMath::Abs( jet_phi[i] ) < NearSide;
}

double JetLoop::IsWithTrigger(int i){
    // +1 if withing trigger
    // -1 if without trigger
    double delta_phi { jet_phi[i] };
    double delta_eta { jet_eta[i] - trig_eta };
    double dist  =  TMath::Sqrt( TMath::Sq(delta_phi) + TMath::Sq(delta_eta) );
    if (dist < TrigDist) return 1;
    else  return -1;
}

void JetLoop::Loop()
{
    /*
        0
        8838.27   10
        12632.5   20
        16631.4   30
        21027.2   40
        25943.8   50
        31506.1   60
        37941.3   70
        45748.4   80
        56303.6   90
        105000   100
        */
    // find the average values
        double loLimit[2]{8838.27, 16631.4};
        double hiLimit[2]{37941.3, 105000.};
        double eta_cut {1.0};
    /* const char* name = "_8up"; */
    TFile* fout = new TFile(Form("hg_Jets%s.root",name.Data()),"recreate");

    //hi events
    Long64_t nLo=0;
    TH2D* jetsLo = new TH2D("jetsLo","70-90% EA_{BBC} (Lowest BBC),N/N_{TT};jet p_{T}[GeV];#Delta#phi",
            45,0.,45.,8,0,TMath::Pi());
    Long64_t nAll=0;
    TH2D* jetsAll = new TH2D("jetsAll","all EA_{BBC} (Lowest BBC),N/N_{TT};jet p_{T}[GeV];#Delta#phi",
            45,0.,45.,8,0,TMath::Pi());
    //low events
    Long64_t nHi=0;
    TH2D* jetsHi = new TH2D("jetsHi","0-30% EA_{BBC} (Highest BBC),N/N_{TT};jet p_{T}[GeV];#Delta#phi",
            45,0.,45.,8,0,TMath::Pi());
    
    //hi events
    TH2D* jetsLo_C = new TH2D("jetsLo_shift","70-90% EA_{BBC} (Lowest BBC),N/N_{TT};jet p_{T}[GeV];#Delta#phi",
            45,0.,45.,8,0,TMath::Pi());
    TH2D* jetsHi_C = new TH2D("jetsHi_shift","0-30% EA_{BBC} (Highest BBC),N/N_{TT};jet p_{T}[GeV];#Delta#phi",
            45,0.,45.,8,0,TMath::Pi());

    TH1D* rho_lo = new TH1D("rho_lo",
            "Jet Median Background Estimator, skip 2 hardest, |#eta_{ghost}|<4,area_{ghost}=0.1;#rho (GeV);#frac{1}{#SigmaN_{ev}}#frac{dN_{ev}}{d#rho}",
            40,0.,4.);
    rho_lo->SetMarkerStyle(kOpenCircle);
    rho_lo->SetMarkerColor(kBlack);
    rho_lo->SetStats(0);
    TH1D* rho_hi = new TH1D("rho_hi",
            "Jet Median Background Estimator, skip 2 hardest, |#eta_{ghost}|<4,area_{ghost}=0.1;#rho (GeV);#frac{1}{#SigmaN_{ev}}#frac{dN_{ev}}{d#rho}",
            40,0.,4.);
    rho_hi->SetMarkerStyle(kFullCircle);
    rho_hi->SetMarkerColor(kBlack);




    /* double shift_factor { 1./(1.2 * TMath::Pi() / 8) * TMath::Pi() * 0.4 * 0.4 }; */
    double shift_hi { 0.450 };
    double shift_lo { 0.150 };

    TProfile *ave_hi = new TProfile("ave_hi","Hi EA Jet average;|#Delta#phi|;p_{T}[GeV]",
            8,0,TMath::Pi());
    TProfile *ave_lo = new TProfile("ave_lo","Lo EA Jet average;|#Delta#phi|;p_{T}[GeV]",
            8,0,TMath::Pi());

   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);

      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;


      /* double cutLo{8}; */
      /* double cutHi{1000}; */

      if ( trig_Et < cutLo || trig_Et > cutHi ) continue;
      ++nAll;
      if (bbc_C > loLimit[0] && bbc_C < loLimit[1]) {
          rho_lo->Fill(rho);
          ++nLo;
          for (int j=0;j<jet_;++j) {
              if (TMath::Abs(jet_eta[j]) > etaCut) continue;
              jetsLo->Fill(jet_pt[j],TMath::Abs(jet_phi[j]));
              jetsLo_C->Fill(jet_pt[j]-shift_lo,TMath::Abs(jet_phi[j]));
              ave_lo->Fill(TMath::Abs(jet_phi[j]),jet_pt[j]);
          }
      }
      if (bbc_C > hiLimit[0] && bbc_C < hiLimit[1]) {
          ++nHi;
          rho_hi->Fill(rho);
          for (int j=0;j<jet_;++j) {
              if (TMath::Abs(jet_eta[j]) > etaCut) continue;
              jetsHi->Fill(jet_pt[j],TMath::Abs(jet_phi[j]));
              jetsHi_C->Fill(jet_pt[j] - shift_hi,TMath::Abs(jet_phi[j]));
              ave_hi->Fill(TMath::Abs(jet_phi[j]),jet_pt[j]);
          }
      }
      // if (Cut(ientry) < 0) continue;
   }
   jetsHi->Scale(1./nHi);
   jetsLo->Scale(1./nLo);
   jetsAll->Scale(1./nAll);

   jetsHi_C->Scale(1./nHi);
   jetsLo_C->Scale(1./nLo);
   jetsAll->Scale(1./nAll);

    rho_lo->Scale(1./nLo);
    rho_hi->Scale(1./nHi);

   ofstream f;
   f.open("rho_correction.txt");
   f << " --- hi events --- " << endl;
   for (int i =1;i<=8;++i){
       double rho { ave_hi->GetBinContent(i) * ave_hi->GetBinEntries(i)
           / nHi / (0.25*TMath::Pi()*1.2)};
       f << "("<<i-1<<"/8,"<<i<<"/8) rho("<<rho<<")  rho x area ("
           <<rho*TMath::Pi()*.16 << "  ave_jet_pt("
         <<ave_hi->GetBinContent(i)<<")"<<endl;
   }
   f << " --- lo events --- " << endl;
   for (int i =1;i<=8;++i){
       double rho { ave_lo->GetBinContent(i) * ave_lo->GetBinEntries(i)
           / nLo / (0.25*TMath::Pi()*1.2)};
       f << "("<<i-1<<"/8,"<<i<<"/8) rho("<<rho<<")  rho x area ("
           <<rho*TMath::Pi()*.16 << "  ave_jet_pt("
         <<ave_lo->GetBinContent(i)<<")"<<endl;
   }
   f << " --- delta shift --- " << endl;
   for (int i =1;i<=8;++i){
       double rholo { ave_lo->GetBinContent(i) * ave_lo->GetBinEntries(i)
           / nLo / (0.25*TMath::Pi()*1.2)};
       double rhohi { ave_hi->GetBinContent(i) * ave_hi->GetBinEntries(i)
           / nHi / (0.25*TMath::Pi()*1.2)};
       f << "("<<i-1<<"/8,"<<i<<"/8) (rho_hi - rho_lo) x area ("
           << (rhohi-rholo)*TMath::Pi()*0.16 << ")" << endl;
   }
   f.close();

   fout->Write();
   fout->Close();
}
