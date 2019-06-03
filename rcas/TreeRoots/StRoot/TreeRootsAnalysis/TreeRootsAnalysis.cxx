#include "TreeRootsAnalysis.h"
#include <TH2.h>
#include <TH2D.h>
#include <TH1D.h>
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

const double TreeRootsAnalysis::p_offset = 1900.;
const double TreeRootsAnalysis::p0 = 13643.1;
const double TreeRootsAnalysis::p1 = 53.9611;
const double TreeRootsAnalysis::p2 = -0.182168;
const double TreeRootsAnalysis::p3 = 1.41106;
const double TreeRootsAnalysis::p4 = -3.40385e-05;
const double TreeRootsAnalysis::p5 = 0.000439491;

double TreeRootsAnalysis::correct_bbc() {
    return bbcE + p0 + p_offset - p1*vz - p2*vz*vz - p3*ZDCx - p4*ZDCx*ZDCx - p5*vz*ZDCx;
}

void TreeRootsAnalysis::Nch(){
   if (fChain == 0) return;
   TFile* f_out = new TFile(o_file_name.c_str(),"RECREATE");

   TH1D *hg_nch = new TH1D("nch","N_{ch} |vz-vzVpd|, |vz|<10, rnk>0, ZDCx#in[7k,24k];N_{ch};",81.,-0.5,80.5);

   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;
   if (nevents != -1 && nevents < nentries) nentries = nevents;
   cout << "nentries " << nentries/1000000 << "M" << endl;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      if (jentry%100000 == 0) cout << "Finished " << Form("%.1f",jentry/1.e6) << "M entries " << endl;
      if (ranking > 0 
              && TMath::Abs(vzVpd-vz)<6. 
              && TMath::Abs(vz)<10 
              && ZDCx > 7.e3 
              && ZDCx < 24.e3) hg_nch->Fill(nch);
      // if (Cut(ientry) < 0) continue;
   }
   f_out->Write();
   f_out->Close();
}

void TreeRootsAnalysis::BBC_dist(){
   if (fChain == 0) return;
   TFile* f_out = new TFile(o_file_name.c_str(),"RECREATE");
   bool pos_ranking;
   options >> pos_ranking;

   double lo_bbc =   -1000.5;
   double hi_bbc =  110000.5;
   int    nbins_bbc = (int)(hi_bbc-lo_bbc);

   TH1D *bbc = new TH1D("bbc","BBC_E for |vz-vzVpd|, |vz|<10; BBC_{east}",1000,-1000.,110000.);
   TH1D *deciles = new TH1D("deciles","BBC_E for |vz-vzVpd|, |vz|<10; BBC_{east}",nbins_bbc,lo_bbc,hi_bbc);

   TH1D *bbc_corr = new TH1D("bbc_corr","BBC_E for |vz-vzVpd|, |vz|<10; BBC_{east}",1000,-1000.,110000.);
   TH1D *deciles_corr = new TH1D("deciles_corr","BBC_E for |vz-vzVpd|, |vz|<10; BBC_{east}",nbins_bbc,lo_bbc,hi_bbc);

    double p_offset = 1900.;
    double p0 = 12612.4;
    double p1 = 53.8662;
    double p2 = -0.874484;
    double p3 = 1.30989;
    double p4 = -3.05535e-05;
    double p5 = 0.000262333;

   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;
   cout << "nentries " << nentries/1000000 << "M" << endl;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      if (jentry%100000 == 0) cout << "Finished " << Form("%.1f",jentry/1.e6) << "M entries " << endl;

      if (pos_ranking) {
          if (ranking <=0) continue;
      } else {
          if (ranking > 0) continue;
      }

      if (TMath::Abs(vzVpd-vz)<6 && TMath::Abs(vz)<10 && ZDCx>7.e3 && ZDCx <24.e3) {
          bbc->Fill(bbcE);
          deciles->Fill(bbcE);
          double val = bbcE + p0 + p_offset - p1*vz - p2*vz*vz - p3*ZDCx - p4*ZDCx*ZDCx - p5*vz*ZDCx;
          bbc_corr->Fill(val);
          deciles_corr->Fill(val);
      }
      // if (Cut(ientry) < 0) continue;
   }
   f_out->Write();
   f_out->Close();
}


void TreeRootsAnalysis::LumiProfiles(){
   if (fChain == 0) return;
   TFile* f_out = new TFile(o_file_name.c_str(),"RECREATE");
   bool pos_ranking;
   options >> pos_ranking;

   TProfile *bbc = new TProfile("bbc","bbc vz ZDCx;ZDCx;bbc",1000,0,50000);
   TProfile *bbc_i10 = new TProfile("bbc_i10","bbc vz ZDCx, |vz|<10;ZDCx;bbc",1000,0,50000);
   TProfile *bbc_i10i6 = new TProfile("bbc_i10i6","bbc vz ZDCx, |vz|<10, |vpd-vz|<6;ZDCx;bbc",1000,0,50000);
   TProfile *bbc_i6 = new TProfile("bbc_i6","bbc vz ZDCx, |vpd-vz|<6;ZDCx;bbc",1000,0,50000);
   TProfile *bbc_i10i6zRange = new TProfile("bbc_i10i6zRange","bbc vz ZDCx, |vpd-vz|<6, |vz|<10, bbc#in(7k,24k);ZDCx;bbc",1000,0,50000);
   TProfile2D* b_vzZDCz = new TProfile2D("b_vzZDCz","bbcE(vz,ZDCx);vz;ZDCx",20,-10,10,100,7000.,24000.);
   TH1D *hg_bbc = new TH1D("hg_bbc","BBC_E for |vz-vzVpd|, |vz|<10; BBC_{east}",1000,-1000.,110000.);
   TH1D *deciles = new TH1D("bbc_deciles","BBC_E for |vz-vzVpd|, |vz|<10; BBC_{east}",100000,-1000.,110000.);

   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;
   cout << "nentries " << nentries/1000000 << "M" << endl;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      if (jentry%100000 == 0) cout << "Finished " << Form("%.1f",jentry/1.e6) << "M entries " << endl;

      if (pos_ranking) {
          if (ranking <=0) continue;
      } else {
          if (ranking > 0) continue;
      }

      bbc->Fill(ZDCx, bbcE);
      if (TMath::Abs(vz)<10) bbc_i10->Fill(ZDCx, bbcE);
      if (TMath::Abs(vzVpd-vz)<6) bbc_i6->Fill(ZDCx, bbcE);
      if (TMath::Abs(vzVpd-vz)<6 && TMath::Abs(vz)<10) bbc_i10i6->Fill(ZDCx, bbcE);
      if (TMath::Abs(vzVpd-vz)<6 && TMath::Abs(vz)<10 && ZDCx>7.e3 && ZDCx <24.e3) {
          bbc_i10i6zRange->Fill(ZDCx, bbcE);
          b_vzZDCz->Fill(vz,ZDCx,bbcE);
          hg_bbc->Fill(bbcE);
          deciles->Fill(bbcE);
      }
      // if (Cut(ientry) < 0) continue;
   }
   f_out->Write();
   f_out->Close();
}

void TreeRootsAnalysis::nch_TProfile2D(){
   if (fChain == 0) return;
   TFile* f_out = new TFile(o_file_name.c_str(),"RECREATE");

   bool pos_ranking;
   bool corr_bbc;
   bool cut_on_4GeV;
   options >> pos_ranking >> corr_bbc >> cut_on_4GeV;

   // use two tags:
   //   i6 for |vz-vzVpd|<6cm
   //   o6 for |vz-vzVpd|>6cm  
   //   yT for yes tof matched
   //   nT for not tof matched
   //   b for bbc
   //   z for vz
   //   x for ZDCx

    double p0 = 12612.4; 
    double p_offset = 1900.;
    double p1 = 53.8662;
    double p2 = -0.874484;
    double p3 = 1.30989;
    double p4 = -3.05535e-05;
    double p5 = 0.000262333;

   TH2D       *hg2_bz_i6   = new TH2D("hg2_bz_i6", "bbcE(vz) |vz-vpd|<6;  bbc;Vz",  500,0.,100000.,60,-30,30);
   TH2D       *hg2_bz_o6   = new TH2D("hg2_bz_o6", "bbcE(vz) |vz-vpd|>6;  bbc;Vz",  500,0.,100000.,60,-30,30);

   TH2D       *hg2_bx_i6   = new TH2D("hg2_bx_i6", "bbcE(ZDCx) |vz-vpd|<6;bbc;ZDCx",500,0.,100000.,100,7000.,24000.);
   TH2D       *hg2_bx_o6   = new TH2D("hg2_bx_o6", "bbcE(ZDCx) |vz-vpd|>6;bbc;ZDCx",500,0.,100000.,100,7000.,24000.);


   TProfile2D *bpr2_zx       = new TProfile2D("pr2_bbc_vz_ZDCx_noCorr", "bbcE(ZDCx,vZ);Vz;ZDCx",60,-30,30,100,7000.,24000.);
   TProfile2D *bpr2_zx_corr  = new TProfile2D("pr2_bbc_vz_ZDCx", "bbcE(ZDCx,vZ);Vz;ZDCx",60,-30,30,100,7000.,24000.);
   //----------------
   // nch sets below:
   //----------------
   // nch vs vz vz vpd
   TProfile2D *vz_v_vpd = new TProfile2D("vz_v_vpd", 
    "nch(vz,vzVpd) (w/vzVpd:underflow=-50.5,vzVpd:over=50.5);vz;vzVpd", 60,-30,30, 102,-51.,51.);

   TH2D *hg_vz_v_vpd = new TH2D("hg_vz_v_vpd", 
    "vz vz vzVpd (w/vzVpd:underflow=-50.5,vzVpd:over=50.5);vz;vzVpd", 60,-30,30, 102,-51.,51.);

   // bbc vz sets
   TProfile2D *bz_i6yT = new TProfile2D("bz_i6yT", "n_{ch,tof-matched},|vz-vpd|<6,;bbc_e;vz",500,0.,100000.,60,-30,30);
   TProfile2D *bz_o6yT = new TProfile2D("bz_o6yT", "n_{ch,tof-matched},|vz-vpd|>6,;bbc_e;vz",500,0.,100000.,60,-30,30);

   TProfile2D *bz_i6nT = new TProfile2D("bz_i6nT", "n_{ch,!tof-matched},|vz-vpd|<6,;bbc_e;vz",500,0.,100000.,60,-30,30);
   TProfile2D *bz_o6nT = new TProfile2D("bz_o6nT", "n_{ch,!tof-matched},|vz-vpd|>6,;bbc_e;vz",500,0.,100000.,60,-30,30);

   // bbc ZDCx sets
   TProfile2D *bx_i6yT = new TProfile2D("bx_i6yT", "n_{ch,tof-matched},|vz-vpd|<6,;bbc_e;ZDCx",500,0.,100000.,100,7000.,24000.);
   TProfile2D *bx_o6yT = new TProfile2D("bx_o6yT", "n_{ch,tof-matched},|vz-vpd|>6,;bbc_e;ZDCx",500,0.,100000.,100,7000.,24000.);

   TProfile2D *bx_i6nT = new TProfile2D("bx_i6nT", "n_{ch,!tof-matched},|vz-vpd|<6,;bbc_e;ZDCx",500,0.,100000.,100,7000.,24000.);
   TProfile2D *bx_o6nT = new TProfile2D("bx_o6nT", "n_{ch,!tof-matched},|vz-vpd|>6,;bbc_e;ZDCx",500,0.,100000.,100,7000.,24000.);

   // ZDCx vz sets
   TProfile2D *xz_i6yT = new TProfile2D("xz_i6yT", "n_{ch,tof-matched},|vz-vpd|<6,;bbc_e;ZDCx", 100,7000.,24000.,60,-30,30);
   TProfile2D *xz_o6yT = new TProfile2D("xz_o6yT", "n_{ch,tof-matched},|vz-vpd|>6,;bbc_e;ZDCx", 100,7000.,24000.,60,-30,30);

   TProfile2D *xz_i6nT = new TProfile2D("xz_i6nT", "n_{ch,!tof-matched},|vz-vpd|<6,;bbc_e;ZDCx",100,7000.,24000.,60,-30,30);
   TProfile2D *xz_o6nT = new TProfile2D("xz_o6nT", "n_{ch,!tof-matched},|vz-vpd|>6,;bbc_e;ZDCx",100,7000.,24000.,60,-30,30);

   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;
   if (nevents != -1 && nevents < nentries) nentries = nevents;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        if ((jentry % 1000000) == 0) cout << " finished " << jentry/1000000 << "M entries" << endl;
        nb = fChain->GetEntry(jentry);   nbytes += nb;

        if ( pos_ranking && ranking <= 0) continue;
        if (!pos_ranking && ranking >  0) continue;
        if (ZDCx<7000 || ZDCx > 24000) continue;
        if (cut_on_4GeV && (ntow_gt4 == 0)) continue;

        double t_vzVpd = (vzVpd < -50) ? -50.5 :
                         (vzVpd >  50) ?  50.5 :
                         vzVpd;
        double bbc_val = (corr_bbc) 
            ? bbcE + p0 + p_offset - p1*vz - p2*vz*vz - p3*ZDCx - p4*ZDCx*ZDCx - p5*vz*ZDCx
            : bbcE;

        vz_v_vpd->Fill(vz,t_vzVpd,bbc_val);
        hg_vz_v_vpd->Fill(vz,t_vzVpd);
        int no_tof = nch-nch_tof;
        
        if (is_vz_vzVpd){
            bpr2_zx->Fill(vz,ZDCx,bbcE);
            bpr2_zx_corr->Fill(vz,ZDCx,bbc_val);

            hg2_bz_i6->Fill(bbc_val,vz);
            hg2_bx_i6->Fill(bbc_val,ZDCx);

            //-------nch and nch_tof profiles-------
            bz_i6yT->Fill(bbc_val,vz,nch_tof);
            bz_i6nT->Fill(bbc_val,vz,no_tof);

            bx_i6yT->Fill(bbc_val,ZDCx,nch_tof);
            bx_i6nT->Fill(bbc_val,ZDCx,no_tof);

            xz_i6yT->Fill(ZDCx,vz,nch_tof);
            xz_i6nT->Fill(ZDCx,vz,no_tof);
        } else {
            hg2_bz_o6->Fill(bbc_val,vz);
            hg2_bx_o6->Fill(bbc_val,ZDCx);
            
            //-------nch and nch_tof profiles-------
            bz_o6yT->Fill(bbc_val,vz,nch_tof);
            bz_o6nT->Fill(bbc_val,vz,no_tof);

            bx_o6yT->Fill(bbc_val,ZDCx,nch_tof);
            bx_o6nT->Fill(bbc_val,ZDCx,no_tof);

            xz_o6yT->Fill(ZDCx,vz,nch_tof);
            xz_o6nT->Fill(ZDCx,vz,no_tof);
        }
   }
   f_out->Write();
   f_out->Close();
};

void TreeRootsAnalysis::TH2_bbc_to_track(){
    if (fChain == 0) return;
   TFile* f_out = new TFile(o_file_name.c_str(),"RECREATE");

    TH2D* nch_bbcES = new TH2D("nch_bbcES",
            "East Small |vz-vzVpd|<6, |vz|<10, ranking>0;bbcE_{#PMT<16};nch",200,0.,100000.,80,-0.5,79.5);
    TH2D* nch_bbcE = new TH2D("nch_bbcE",
            "East All |vz-vzVpd|<6, |vz|<10, ranking>0;bbcE_{#PMT<24};nch",200,0.,100000.,80,-0.5,79.5);
    TH2D* nch_bbcWS = new TH2D("nch_bbcWS",
            "West Small |vz-vzVpd|<6, |vz|<10, ranking>0;bbcW_{#PMT<16+24};nch",200,0.,100000.,80,-0.5,79.5);
    TH2D* nch_bbcW = new TH2D("nch_bbcW",
            "West All |vz-vzVpd|<6, |vz|<10, ranking>0;bbcW_{#PMT<24+24};nch",200,0.,100000.,80,-0.5,79.5);

    TH2D* tof_bbcES = new TH2D("tof_bbcES",
            "East Small |vz-vzVpd|<6, |vz|<10, ranking>0;bbcE_{#PMT<16};nch-TOF",200,0.,100000.,80,-0.5,79.5);
    TH2D* tof_bbcE = new TH2D("tof_bbcE",
            "East All |vz-vzVpd|<6, |vz|<10, ranking>0;bbcE_{#PMT<24};nch-TOF",200,0.,100000.,80,-0.5,79.5);
    TH2D* tof_bbcWS = new TH2D("tof_bbcWS",
            "West Small |vz-vzVpd|<6, |vz|<10, ranking>0;bbcW_{#PMT<16+24};nch-TOF",200,0.,100000.,80,-0.5,79.5);
    TH2D* tof_bbcW = new TH2D("tof_bbcW",
            "West All |vz-vzVpd|<6, |vz|<10, ranking>0;bbcW_{#PMT<24+24};nch-TOF",200,0.,100000.,80,-0.5,79.5);

    TH2D* notof_bbcES = new TH2D("notof_bbcES",
            "East Small |vz-vzVpd|<6, |vz|<10, ranking>0;bbcE_{#PMT<16};nch-noTOF",200,0.,100000.,80,-0.5,79.5);
    TH2D* notof_bbcE = new TH2D("notof_bbcE",
            "East All |vz-vzVpd|<6, |vz|<10, ranking>0;bbcE_{#PMT<24};nch-noTOF",200,0.,100000.,80,-0.5,79.5);
    TH2D* notof_bbcWS = new TH2D("notof_bbcWS",
            "West Small |vz-vzVpd|<6, |vz|<10, ranking>0;bbcW_{#PMT<16+24};nch-noTOF",200,0.,100000.,80,-0.5,79.5);
    TH2D* notof_bbcW = new TH2D("notof_bbcW",
            "West All |vz-vzVpd|<6, |vz|<10, ranking>0;bbcW_{#PMT<24+24};nch-noTOF",200,0.,100000.,80,-0.5,79.5);

   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;
   if (nevents != -1 && nevents < nentries) nentries = nevents;
   cout << "nentries " << nentries/1000000 << "M" << endl;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      if ( ranking <= 0 || ZDCx<7000. || ZDCx>24000. || !is_vz_vzVpd || TMath::Abs(vz)<10) continue;
        nch_bbcES->Fill(bbcES,nch);
        nch_bbcE->Fill(bbcE,nch);
        nch_bbcWS->Fill(bbcWS,nch);
        nch_bbcW->Fill(bbcW,nch);

        tof_bbcES->Fill(bbcES,nch_tof);
        tof_bbcE->Fill(bbcE,nch_tof);
        tof_bbcWS->Fill(bbcWS,nch_tof);
        tof_bbcW->Fill(bbcW,nch_tof);

        int notof = nch - nch_tof;
        notof_bbcES->Fill(bbcES,notof);
        notof_bbcE->Fill(bbcE,notof);
        notof_bbcWS->Fill(bbcWS,notof);
        notof_bbcW->Fill(bbcW,notof);
   }
   f_out->Write();
   f_out->Close();
}
void TreeRootsAnalysis::TOF_dist(){
    if (fChain == 0) return;
   TFile* f_out = new TFile(o_file_name.c_str(),"RECREATE");

    TH1D* tof   = new TH1D("tof","N_{tof};N_{tof};N_{events}",50,-0.5,49.5);
    TH1D* notof = new TH1D("no_tof","N_{tof};N_{tof};N_{events}",50,-0.5,49.5);
    TH2D* bbc_swing = new TH2D("bbc_swing","bbc correction swing;bbc_{raw};bbc_{corrected}",
            500,0.,110000.,500,0.,110000.);
    TProfile2D* tof_ZDCx = new TProfile2D("tofnch_zdcX",
            "nch_{TOF} in bins of vz-vzvpd;vz-vpd;ZDCx",48,-6.,6.,40,7000.,24000.);

   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;
   if (nevents != -1 && nevents < nentries) nentries = nevents;
   cout << "nentries " << nentries/1000000 << "M" << endl;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      if ( ranking <= 0) continue;
      if (ZDCx<7000 || ZDCx > 24000) continue;
      if (!is_vz_vzVpd) continue;
      if (TMath::Abs(vz) > 0) continue;

      tof->Fill(nch_tof);
      notof->Fill(nch-nch_tof);
      bbc_swing->Fill(bbcE, correct_bbc());
      tof_ZDCx->Fill(vz-vzVpd, ZDCx, nch_tof);


      // if (Cut(ientry) < 0) continue;
   }
   f_out->Write();
   f_out->Close();
}

void TreeRootsAnalysis::Loop() { };
// a backup of the old sets are in arch/_x.bkup and arch/_h.bkup
TreeRootsAnalysis::TreeRootsAnalysis(const char* _myoptions) : options(_myoptions) {
    string infile;
    // ---------------------------------------------------------------
    // get first three options
    // ---------------------------------------------------------------
    string which_program;
    options >> which_program >> infile >> o_file_name >> nevents; 
    TFile *f = new TFile(infile.c_str(), "read");
    if (!f) { cout << " fatal error, couldn't read TFile\""<<infile<<"\"." << endl;
              exit(2);
    };
    TTree* tree;
    f->GetObject("events",tree);
    Init(tree);

    // ---------------------------------------------------------------
    // select and run program
    // ---------------------------------------------------------------
    if (which_program == "nch_TProfile2D") {
        cout << "Running make_TProfile2D"<<endl;
        nch_TProfile2D(); 
    } else if ( which_program == "LumiProfiles") {
        cout << "Running " << which_program << endl;
        LumiProfiles(); 
    } else if ( which_program == "BBC_dist") {
        cout << "Running " << which_program << endl;
        BBC_dist(); 
    } else if ( which_program == "Nch") {
        cout << "Running " << which_program << endl;
        Nch(); 
    } else if ( which_program == "nch_gt4GeV_TProfile2D") {
        cout << "Running " << which_program << endl;
        nch_gt4GeV_TProfile2D(); 
    } else if ( which_program == "TOF_dist") {
        cout << "Running " << which_program << endl;
        TOF_dist(); 
    } else if ( which_program == "TH2_bbc_to_track") {
        cout << "Running " << which_program << endl;
        TH2_bbc_to_track(); 
    } else {
        cout << "analysis option not recognized; -> no analysis run!" << endl;
    }
};
TreeRootsAnalysis::~TreeRootsAnalysis()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
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
   fChain->SetBranchAddress("bbcE", &bbcE, &b_event_bbcE);
   fChain->SetBranchAddress("bbcES", &bbcES, &b_event_bbcES);
   fChain->SetBranchAddress("bbcEL", &bbcEL, &b_event_bbcEL);
   fChain->SetBranchAddress("bbcW", &bbcW, &b_event_bbcW);
   fChain->SetBranchAddress("bbcWS", &bbcWS, &b_event_bbcWS);
   fChain->SetBranchAddress("bbcWL", &bbcWL, &b_event_bbcWL);
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
