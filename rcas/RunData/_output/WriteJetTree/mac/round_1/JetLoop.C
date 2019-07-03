#define JetLoop_cxx
#include "JetLoop.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TProfile2D.h>

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
    double bbcLo = 16631.4;
    double bbcHi = 37941.3;

    /* double cutLo {8}; */
    /* double cutHi {100}; */
    /* const char* name = "_8up"; */
    TFile* fout = new TFile(Form("hg_Jets%s.root",name.Data()),"recreate");

    //hi events
    Long64_t nLo=0;
    TH2D* jetsLo = new TH2D("jetsLo","70-100% EA_{BBC} (Lowest BBC),N/N_{TT};jet p_{T};#Delta#phi",
            45,0.,45.,16,-TMath::Pi(),TMath::Pi());

    //low events
    Long64_t nHi=0;
    TH2D* jetsHi = new TH2D("jetsHi","0-30% EA_{BBC} (Highest BBC),N/N_{TT};jet p_{T};#Delta#phi",
            45,0.,45.,16,-TMath::Pi(),TMath::Pi());

    //all events
    Long64_t nAll=0;
    TH2D* jetsAll = new TH2D("jetsAll","All BBC, N/N_{TT},N/N_{TT};jet p_{T};#Delta#phi}",
            45,0.,45.,16,-TMath::Pi(),TMath::Pi());


    //near-side with trigger    (Jet < separate)
    //near-side without trigger (Jet > separate)
    TH2D* nearHi = new TH2D("nearHi","0-30% EA_{BBC} (Highest BBC),N/N_{TT};jet p_{T};-1=w/oTrig,0=w/Trig,1=all",
            45,0.,45., 3,-1.5,1.5);
    TH2D* nearLo = new TH2D("nearLo","70-100% EA_{BBC} (Lowest BBC),N/N_{TT};jet p_{T};-1=w/oTrig,0=w/Trig,1=all",
            45,0.,45., 3,-1.5,1.5);
    TH2D* nearAll = new TH2D("nearAll","All EA_{BBC} (Lowest BBC),N/N_{TT};jet p_{T};-1=w/oTrig,0=w/Trig,1=all",
            45,0.,45., 3,-1.5,1.5);

    TH2D* phieta_hi  = new TH2D("phieta_hi",  "hi;#Delta#phi;#Delta#eta",10,-TMath::Pi()/8, TMath::Pi()/8,20,-2.,2.);
    TH2D* phieta_lo  = new TH2D("phieta_lo",  "lo;#Delta#phi;#Delta#eta",10,-TMath::Pi()/8, TMath::Pi()/8,20,-2.,2.);
    TH2D* phieta_all  = new TH2D("phieta_all","all;#Delta#phi;#Delta#eta",10,-TMath::Pi()/8, TMath::Pi()/8,20,-2.,2.);

    TH2D* phieta_hi_10  = new TH2D("phieta_hi_10","hi Jets>10GeV;#Delta#phi;#Delta#eta",10,-TMath::Pi()/8, TMath::Pi()/8,20,-2.,2.);
    TH2D* phieta_lo_10  = new TH2D("phieta_lo_10","lo Jets>10GeV;#Delta#phi;#Delta#eta",10,-TMath::Pi()/8, TMath::Pi()/8,20,-2.,2.);
    TH2D* phieta_all_10  = new TH2D("phieta_all_10","all Jets>10GeV;#Delta#phi;#Delta#eta",10,-TMath::Pi()/8, TMath::Pi()/8,20,-2.,2.);

    TProfile2D* p2_jet_hi   = new TProfile2D("p2_jet_hi","hi Jets>10GeV;#Delta#phi;#Delta#eta",10,-TMath::Pi()/8, TMath::Pi()/8,20,-2.,2.);
    TProfile2D* p2_jet_lo   = new TProfile2D("p2_jet_lo","lo Jets>10GeV;#Delta#phi;#Delta#eta",10,-TMath::Pi()/8, TMath::Pi()/8,20,-2.,2.);
    TProfile2D* p2_jet_all  = new TProfile2D("p2_jet_all","all Jets>10GeV;#Delta#phi;#Delta#eta",10,-TMath::Pi()/8, TMath::Pi()/8,20,-2.,2.);
        

    /* TH1D* nTrigLo  = new TH1D("nEvLo","70-100% EA_{BBC};",16,-TMath::Pi(),TMath::Pi()); */
    /* TH1D* nTrigHi  = new TH1D("nEvLo","0-30% EA_{BBC};",16,-TMath::Pi(),TMath::Pi()); */
    /* TH1D* nTrigAll = new TH1D("nEvAll","All BBC;",16,-TMath::Pi(),TMath::Pi()); */
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);

      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;


      if ( trig_Et < cutLo || trig_Et > cutHi ) continue;
      ++nAll;
      for (int j=0;j<jet_;++j) {
          if (TMath::Abs(jet_eta[j]) > 0.6) continue;
          jetsAll->Fill(jet_pt[j],jet_phi[j]);

          if (IsNearSide(j)) {
              nearAll->Fill(jet_pt[j], 0);
              nearAll->Fill(jet_pt[j], IsWithTrigger(j));
              //---
              phieta_all                      ->Fill(jet_phi[j],jet_eta[j]-trig_eta);
              if (jet_pt[j]>10) phieta_all_10 ->Fill(jet_phi[j],jet_eta[j]-trig_eta);
              p2_jet_all                      ->Fill(jet_phi[j],jet_eta[j]-trig_eta,jet_pt[j]);
          }
      }

      if (bbc_corr < bbcLo) {
          ++nLo;
          for (int j=0;j<jet_;++j) {
              if (TMath::Abs(jet_eta[j]) > 0.6) continue;
              jetsLo->Fill(jet_pt[j],jet_phi[j]);

              if (IsNearSide(j)) {
                  nearLo->Fill(jet_pt[j], 0);
                  nearLo->Fill(jet_pt[j], IsWithTrigger(j));
                  //---
                  phieta_lo                      ->Fill(jet_phi[j],jet_eta[j]-trig_eta);
                  if (jet_pt[j]>10) phieta_lo_10 ->Fill(jet_phi[j],jet_eta[j]-trig_eta);
                  p2_jet_lo                      ->Fill(jet_phi[j],jet_eta[j]-trig_eta,jet_pt[j]);
              }
          }
      }
      if (bbc_corr > bbcHi) {
          ++nHi;
          for (int j=0;j<jet_;++j) {
              if (TMath::Abs(jet_eta[j]) > 0.6) continue;
              jetsHi->Fill(jet_pt[j],jet_phi[j]);

              if (IsNearSide(j)) {
                  nearHi->Fill(jet_pt[j], 0);
                  nearHi->Fill(jet_pt[j], IsWithTrigger(j));
                  //---
                  phieta_hi                      ->Fill(jet_phi[j],jet_eta[j]-trig_eta);
                  if (jet_pt[j]>10) phieta_hi_10 ->Fill(jet_phi[j],jet_eta[j]-trig_eta);
                  p2_jet_hi                      ->Fill(jet_phi[j],jet_eta[j]-trig_eta,jet_pt[j]);
              }
          }
      }
      // if (Cut(ientry) < 0) continue;
   }
   jetsHi->Scale(1./nHi);
   jetsLo->Scale(1./nLo);
   jetsAll->Scale(1./nAll);

   nearHi->Scale(1./nHi);
   nearLo->Scale(1./nLo);
   nearAll->Scale(1./nAll);


   TH2D* jetRatio = (TH2D*) jetsHi->Clone("hilo_ratio");
   jetRatio->SetTitle("Ratio (0-30%)/(70-100%)");
   jetRatio->Divide(jetsLo);

   TH2D* nearRatio = (TH2D*) nearHi->Clone("near_hilo_ratio");
   nearRatio->SetTitle("Ratio (0-30%)/(70-100%), Jet(#phi<#pi/8)");
   nearRatio->Divide(nearLo);

   TH2D* phietaRatio = (TH2D*) phieta_hi->Clone("phietaRatio");
   phietaRatio->SetTitle("nearside to trigger;#DeltaPhi;#Delta#eta");
   phietaRatio->Divide(phieta_lo);

   TH2D* phieta_10_Ratio = (TH2D*) phieta_hi_10->Clone("phieta10Ratio");
   phieta_10_Ratio->SetTitle("nearside to trigger;#DeltaPhi;#Delta#eta");
   phieta_10_Ratio->Divide(phieta_lo_10);

   TH2D* p2_jet_ratio = (TH2D*) p2_jet_hi->ProjectionXY("p2_jet_ratio");
   TH2D* p2_copy = (TH2D*) p2_jet_lo->ProjectionXY("copy");
   p2_jet_ratio->SetTitle("nearside to trigger;#DeltaPhi;#Delta#eta");
   p2_jet_ratio->Divide(p2_copy);



   fout->Write();
   fout->Close();
}
