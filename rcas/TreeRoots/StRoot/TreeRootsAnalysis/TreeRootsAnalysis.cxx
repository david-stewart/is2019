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

void TreeRootsAnalysis::Deciles(){
   if (fChain == 0) return;
  
   //-----------------------
   // options used:
   //----------------------
   // 4. decile_root_file
   // 5. decile_root_name
   // 6. min Et value
   // 7. max Et value
   //     note: yes it would be more efficienct to fill the Et values 
   //           in one go -- but it is not worth the extra loop of logic
   //           and naming hassle. Therefore, one set of reading per loop it is.
    string decile_root_file;
    string decile_th1d;
    /* string towerEt_file; */
    double et_min;
    double et_max;
    options >> decile_root_file >> decile_th1d >> et_min >> et_max;

    //read in the deciles root file to find the value of the deciles
    TH1D* hg_dec;
    double xq[11], yq[11];
    for (unsigned int i=0; i<11;++i){ xq[i] = 0.1*i; }
    TFile *f_deciles = new TFile(decile_root_file.c_str(),"read");
    f_deciles->GetObject(decile_th1d.c_str(), hg_dec);
    hg_dec->GetQuantiles(11,yq,xq);

    //run the loop
    TFile *fout    = new TFile(o_file_name.c_str(), "recreate");
    TH1D     *nEvents = new TH1D("nEvents","nEVents;Event Activity_{BBC};#frac{1}{N_{ev}}#frac{dN_{ev}}{dEA_{BBC}}", 10, yq);
    TProfile *hg_nch     = new TProfile("hg_nch","nch;Event Activity_{BBC};#LTN_{cv}#GT", 10, yq);

    nEvents->Sumw2();
    hg_nch->Sumw2();
    /* sumpt->Sumw2(); */
    /* maxpt->Sumw2(); */
    
    //-----------------
    // run the loop
    //-----------------
    Long64_t nentries = fChain->GetEntriesFast();
    if (nevents != -1 && nevents < nentries) nentries = nevents;
    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;
        // correct the bbc signal for vz and zdcX
        double bbc = bbc_E-53.82*vz-0.4371*vz*vz-0.6826*ZDCx+1.15e-5/ZDCx*ZDCx;
        if (TMath::Abs(vz - vzVpd)>6) continue;

        nEvents->Fill(bbc);
        hg_nch->Fill(bbc,nch);
   }
    /* cout << "Entries: " << sumpt->GetEntries() << endl; */
   fout->Write();
   fout->Close();
}


void TreeRootsAnalysis::normAcceptanceTo004(){
   if (fChain == 0) return;
   /* cout << "a1" << endl; */


   //---------------------------------------------------------------------------
   // fill in options: rebin_factor (5), first_norm_bin (30), last_norm_bin (31)
   //---------------------------------------------------------------------------
   string hg2_root_file, hg2_004_root_file;
   int rebin_factor, first_norm_bin, last_norm_bin; // 5, 30, 31
   /* options >> hg2_root_file >> hg2_004_root_file >> rebin_factor >> first_norm_bin >> last_norm_bin; */
   options >> hg2_004_root_file >> hg2_root_file >> rebin_factor >> first_norm_bin >> last_norm_bin;

   vector<int> vz_normal_bins;
   for (int i = first_norm_bin; i<=last_norm_bin; ++i) vz_normal_bins.push_back(i);

   // -- make the new histogram --
   cout << "opening file " << o_file_name << endl;
   fout = new TFile(o_file_name.c_str(),"RECREATE");
   TH2D* hg2_temp;
   TH2D* hg2_004_temp;


   TFile *f004 = new TFile(hg2_004_root_file.c_str(),"read");
   f004->GetObject("hg2",hg2_004_temp);


   TFile *f_hg2 = new TFile(hg2_root_file.c_str(),"read");
   f_hg2->GetObject("hg2",hg2_temp);

   fout->cd();
   TH2D* hg2     = (TH2D*) hg2_temp->Clone("hg2");
   TH2D* hg2_004 = (TH2D*) hg2_004_temp->Clone("hg2_004");


   int n_bbc = hg2->GetXaxis()->GetNbins();
   int n_vz  = hg2->GetYaxis()->GetNbins();
   double min_bbc = hg2->GetXaxis()->GetBinLowEdge(1);
   double max_bbc = hg2->GetXaxis()->GetBinLowEdge(n_bbc+1);
   double min_vz  = hg2->GetYaxis()->GetBinLowEdge(1);
   double max_vz  = hg2->GetYaxis()->GetBinLowEdge(n_vz+1);


   //normalize in bbc per vz
   TH1D* scale_factors = hg2->ProjectionY("scale_factors");
   for (int i_vz=1; i_vz<n_vz+1; ++i_vz) {
       double factor = scale_factors->GetBinContent(i_vz); 
       cout << " factor(" << i_vz << "): " << factor << endl;
       if (factor < 1.) cout << " factor("<<i_vz<<") " << factor << endl;
       for (int i_bbc=1; i_bbc<n_bbc+1; ++i_bbc) {
           double val = hg2->GetBinContent(i_bbc, i_vz) / factor;
           /* if (val > 1) cout << " (i_vz,i_bbc, val) " << i_vz<<" " << i_bbc<<" " << val << "   " << factor << endl; */
           hg2->SetBinContent(i_bbc, i_vz, val);
       }
   }
   //normalize in bbc per vz do the same for hg2_004
   scale_factors = hg2_004->ProjectionY("scale_factors");
   for (int i_vz=1; i_vz<n_vz+1; ++i_vz) {
       double factor = scale_factors->GetBinContent(i_vz); 
       if (factor < 1.) cout << " factor("<<i_vz<<") " << factor << endl;
       for (int i_bbc=1; i_bbc<n_bbc+1; ++i_bbc) {
           double val =                    hg2_004->GetBinContent(i_bbc, i_vz) / factor;
           /* if (val > 1) cout << " (i_vz,i_bbc, val) " << i_vz<<" " << i_bbc<<" " << val << "   " << factor << endl; */
           hg2_004->SetBinContent(i_bbc, i_vz, val);
       }
   }


   for (int i_vz=1; i_vz<n_vz+1; ++i_vz) { 
       hg2->SetBinContent(0, i_vz,0.);
       hg2->SetBinContent(n_bbc+1, i_vz, 0.);
       hg2_004->SetBinContent(0, i_vz,0.);
       hg2_004->SetBinContent(n_bbc+1, i_vz, 0.);
   }
   for (int i_vz=1; i_vz<n_vz+1; ++i_vz) { 
       hg2->SetBinContent(0, i_vz,0.);
       hg2->SetBinContent(n_bbc+1, i_vz, 0.);
       hg2_004->SetBinContent(0, i_vz,0.);
       hg2_004->SetBinContent(n_bbc+1, i_vz, 0.);
   }
        
   /* hg2_004->Rebin(rebin_factor); */
   cout << " n_bbc & nvz " << n_bbc << " " << n_vz << endl;


   TH1D* standard = hg2_004->ProjectionX("stand_004", first_norm_bin, last_norm_bin);
   standard->SetTitle(Form("standard bbc_E distr: 500004 at bins %i-%i;bbc_E;N/N_{total}",first_norm_bin, last_norm_bin));
   standard->Rebin(rebin_factor);
   standard->Scale(1./standard->Integral());

   TH2D     *nEv_n = new TH2D("nEv_n","nEvents per decile no weighting;bbc_E;vz",
           n_bbc, min_bbc, max_bbc, n_vz, min_vz, max_vz);
   TH2D     *nEv_w = new TH2D("nEv_w","nEvents per decile weighted;bbc_E;vz",
           n_bbc, min_bbc, max_bbc, n_vz, min_vz, max_vz);

   TProfile2D *nch_n = new TProfile2D("nch_n", "Nch |#eta|<1.0, no weighting;bbc_E;vz",
           n_bbc, min_bbc, max_bbc, n_vz, min_vz, max_vz);
   TProfile2D *nch_w = new TProfile2D("nch_w", "Nch |#eta|<1.0, weighted;bbc_E;vz",
           n_bbc, min_bbc, max_bbc, n_vz, min_vz, max_vz);

   TProfile2D *nchE_n = new TProfile2D("nchE_n", "Nch #eta<0, no weighting;bbc_E;vz",
           n_bbc, min_bbc, max_bbc, n_vz, min_vz, max_vz);
   TProfile2D *nchE_w = new TProfile2D("nchE_w", "Nch #eta<0, weighted;bbc_E;vz",
       n_bbc, min_bbc, max_bbc, n_vz, min_vz, max_vz);

   TProfile2D *nchW_n = new TProfile2D("nchW_n", "Nch #eta>0, no weighting;bbc_E;vz",
       n_bbc, min_bbc, max_bbc, n_vz, min_vz, max_vz);
   TProfile2D *nchW_w = new TProfile2D("nchW_w", "Nch #eta>0, weighted;bbc_E;vz",
       n_bbc, min_bbc, max_bbc, n_vz, min_vz, max_vz);

   TProfile2D *nchC_n = new TProfile2D("nchC_n", "Nch |#eta|<0.5, no weighting;bbc_E;vz",
       n_bbc, min_bbc, max_bbc, n_vz, min_vz, max_vz);
   TProfile2D *nchC_w = new TProfile2D("nchC_w", "Nch |#eta|<0.5, weighted;bbc_E;vz",
       n_bbc, min_bbc, max_bbc, n_vz, min_vz, max_vz);

   TProfile2D *ntow_n = new TProfile2D("ntow_n", "Number of towers with E_{T}>0.2N, no weighting;bbc_E;vz",
       n_bbc, min_bbc, max_bbc, n_vz, min_vz, max_vz);
   TProfile2D *ntow_w = new TProfile2D("ntow_w", "Number of towers with E_{T}>0.2N, weighted;bbc_E;vz",
       n_bbc, min_bbc, max_bbc, n_vz, min_vz, max_vz);

   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;
   if (nevents != -1 && nevents < nentries) nentries = nevents;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      if ((jentry % 1000000) == 0) cout << " finished " << jentry/1000000 << "M entries" << endl;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      if (vz > max_vz || vz < min_vz) continue;
      if (ZDCx<7000 || ZDCx >24000) continue;
      if (TMath::Abs(vz-vzVpd)>5) continue;

      /* if (TMath::Abs(vz) > 10) continue; */
      //find the weighting
      int i_vz  = hg2->GetYaxis()->FindBin(vz);
      int i_bbc = hg2->GetXaxis()->FindBin(bbc_E);
      int i_standard = standard->GetXaxis()->FindBin(bbc_E);
      double w = standard->GetBinContent(i_standard) / hg2->GetBinContent(i_bbc, i_vz) / rebin_factor;
      /* if (hg2->GetBinContent(i_bbc, i_vz) > 1) cout <<    " SHOCK content:  " << hg2->GetBinContent(i_bbc, i_vz); */
      /* if (jentry < 10){ */
      /* cout << "------------"<<endl; */
      /* cout <<    " standard: " << standard->GetBinContent(i_standard) << "  " << i_bbc << "  " << i_vz << endl; */
      /* cout <<    " content:  " << hg2->GetBinContent(i_bbc, i_vz); */
      /* cout <<    " weight:   " << w << endl; */
      /* cout << "w: " << w << endl; */
      /* } */

      nEv_n->Fill(bbc_E, vz);
      nEv_w->Fill(bbc_E, vz, w);

      nch_n->Fill(bbc_E, vz, nch);
      nch_w->Fill(bbc_E, vz, nch, w);

      nchE_n->Fill(bbc_E, vz, nchE);
      nchE_w->Fill(bbc_E, vz, nchE, w);

      nchW_n->Fill(bbc_E, vz, nchW);
      nchW_w->Fill(bbc_E, vz, nchW, w);

      nchC_n->Fill(bbc_E, vz, nch_ltp5);
      nchC_w->Fill(bbc_E, vz, nch_ltp5, w);

      ntow_n->Fill(bbc_E, vz, ntow_gtp2);
      ntow_w->Fill(bbc_E, vz, ntow_gtp2, w);
   }

   /* cout << "nbins :: :: :: " << nEv_n->GetXaxis()->GetNbins() << endl; */
   /* cout << "nbins :: :: :: " << nEv_n->GetEntries() << endl; */
   /* for (int i=0; i<n; ++i) cout << " low bound"<<i<<": " << nEv_n->GetXaxis()->GetBinLowEdge(i+1) */ 
       /* << "  " << nEv_n->GetBinContent(i+1) << endl; */
   /* nEv_n->Write(); */
   fout->Write();
   fout->Close();
}
void TreeRootsAnalysis::nch_in_deciles(){
   /* if (fChain == 0) return; */
   /* /1* cout << "a1" << endl; *1/ */

   /* string deciles_list, deciles_root, which_hg; */
   /* options >> deciles_root >> deciles_list >> which_hg; */

   /* double* bounds = new double[11]; */
   /* vector<double> v_bounds; */
   /* int n = 10; */
   
   /* /1* cout << "a2" << endl; *1/ */
   /* // read in deciles histograms */
   /* // (this was generated in djs232/is2019/rcas/TreeRoots/mac on my pc) */
   /* ifstream i_file; */
   /* i_file.open(deciles_list.c_str()); */
   /* /1* if (! i_file.is_open() ) cout << " aaargh! " << endl; *1/ */
   /* string line; */
   /* /1* cout << "a3" << endl; *1/ */
   /* int i_lines = 0; */
   /* /1* cout << "a4" << endl; *1/ */
   /* while (getline(i_file, line)) { */
   /*     /1* cout << "a5" << " " << line << endl; *1/ */
   /*     istringstream iss (line); */
   /*     /1* cout << "a6" << " " << iss.str() << endl; *1/ */
   /*     double percentile, val, integral_bin, integral_sum; */
   /*     iss >> percentile >> val >> integral_bin >> integral_sum; */
   /*     bounds[i_lines] = val; */
   /*     v_bounds.push_back(val); */
   /*     i_lines += 1; */
   /* } */
   /* for (int i=0;i<11;++i) cout << " "<<i<<": " << bounds[i] << endl; */
   /* /1* cout << "a7" << endl; *1/ */

   /* fout = new TFile(o_file_name.c_str(),"RECREATE"); */
   /* cout << "opening file " << o_file_name << endl; */
   /* TH2D* hg2; */
   /* TH1D* standard; */

   /* TFile *f = new TFile(deciles_root.c_str(),"read"); */
   /* f->GetObject("bbc_cenVz",standard); */
   /* f->GetObject(which_hg.c_str(), hg2); */

   /* /1* cout << " hg2 " << hg2->GetEntries() << " " << hg2 << endl; *1/ */


   /* //make the new root file and output histograms */
   /* fout->cd(); */

   /* TH1D     *nEv_n = new TH1D("nEv_n","nEvents per decile no weighting",n, bounds); */
   /* /1* cout << "nbins :: :: :: " << nEv_n->GetXaxis()->GetNbins() << endl; *1/ */
   /* TH1D     *nEv_w = new TH1D("nEv_w","nEvents per decile weighted",    n, bounds); */

   /* TProfile *nch_n = new TProfile("nch_n", "Nch |#eta|<1.0, no weighting",n,bounds); */
   /* TProfile *nch_w = new TProfile("nch_w", "Nch |#eta|<1.0, weighted",n,bounds); */

   /* TProfile *nchE_n = new TProfile("nchE_n", "Nch #eta<0, no weighting",n,bounds); */
   /* TProfile *nchE_w = new TProfile("nchE_w", "Nch #eta<0, weighted",n,bounds); */

   /* TProfile *nchW_n = new TProfile("nchW_n", "Nch #eta>0, no weighting",n,bounds); */
   /* TProfile *nchW_w = new TProfile("nchW_w", "Nch #eta>0, weighted",n,bounds); */

   /* TProfile *nchC_n = new TProfile("nchC_n", "Nch |#eta|<0.5, no weighting",n,bounds); */
   /* TProfile *nchC_w = new TProfile("nchC_w", "Nch |#eta|<0.5, weighted",n,bounds); */

   /* TProfile *ntow_n = new TProfile("ntow_n", "Number of towers with E_{T}>0.2N, no weighting",n,bounds); */
   /* TProfile *ntow_w = new TProfile("ntow_w", "Number of towers with E_{T}>0.2N, weighted",    n,bounds); */

   /* Long64_t nentries = fChain->GetEntriesFast(); */
   /* Long64_t nbytes = 0, nb = 0; */
   /* if (nevents != -1 && nevents < nentries) nentries = nevents; */
   /* for (Long64_t jentry=0; jentry<nentries;jentry++) { */
   /*    Long64_t ientry = LoadTree(jentry); */
   /*    if (ientry < 0) break; */
   /*    if ((jentry % 1000000) == 0) cout << " finished " << jentry/1000000 << "M entries" << endl; */
   /*    nb = fChain->GetEntry(jentry);   nbytes += nb; */

   /*    if (TMath::Abs(vz) > 10) continue; */

   /*    //find the weighting */
   /*    int i_vz  = hg2->GetYaxis()->FindBin(vz); */
   /*    int i_bbc = hg2->GetXaxis()->FindBin(bbc_E); */
   /*    double w = standard->GetBinContent(i_bbc) / hg2->GetBinContent(i_bbc, i_vz); */
   /*    if (jentry < 10){ */
   /*    cout << "------------"<<endl; */
   /*    cout <<    " standard: " << standard->GetBinContent(i_bbc) << endl; */
   /*    cout <<    " content:  " <<                 hg2->GetBinContent(i_bbc, i_vz); */
   /*    cout <<    " weight:   " << w << endl; */
   /*    cout << "w: " << w << endl; */
   /*    } */

   /*    nEv_n->Fill(bbc_E); */
   /*    nEv_w->Fill(bbc_E, w); */

   /*    nch_n->Fill(bbc_E, nch); */
   /*    nch_w->Fill(bbc_E, nch, w); */

   /*    nchE_n->Fill(bbc_E, nchE); */
   /*    nchE_w->Fill(bbc_E, nchE, w); */

   /*    nchW_n->Fill(bbc_E, nchW); */
   /*    nchW_w->Fill(bbc_E, nchW, w); */

   /*    nchC_n->Fill(bbc_E, nch_ltp5); */
   /*    nchC_w->Fill(bbc_E, nch_ltp5, w); */

   /*    ntow_n->Fill(bbc_E, ntow_gtp2); */
   /*    ntow_w->Fill(bbc_E, ntow_gtp2, w); */
   /* } */

   /* cout << "nbins :: :: :: " << nEv_n->GetXaxis()->GetNbins() << endl; */
   /* cout << "nbins :: :: :: " << nEv_n->GetEntries() << endl; */
   /* for (int i=0; i<n; ++i) cout << " low bound"<<i<<": " << nEv_n->GetXaxis()->GetBinLowEdge(i+1) */ 
   /*     << "  " << nEv_n->GetBinContent(i+1) << endl; */
   /* /1* nEv_n->Write(); *1/ */
   /* fout->Write(); */
   /* fout->Close(); */
}


// updating for second round on 2019.05.21 for |vz-vzVpd|
void TreeRootsAnalysis::bbcVz_slices(){
   if (fChain == 0) return;
   fout = new TFile(o_file_name.c_str(),"RECREATE");
   double delta_vzVpd;
   options >> delta_vzVpd;
   bool use_delta_vzVpd = (delta_vzVpd != -1.);

   TH2D       *hg2 = new TH2D("hg2", "bbc_E(vz) in one centimeter strips;bbc;Vz",500,0.,100000.,60,-30,30);
   TProfile2D *pr2   = new TProfile2D("pr2", "bbcE(ZDCx,vZ);Vz;ZDCx",60,-30,30, 100,7000.,24000.);
   TProfile2D *p_nch  = new TProfile2D("nch", "Nch |#eta|<1.0, no weighting;bbc_E;vz",500,0.,100000.,60,-30,30);
   TProfile2D *p_nchE = new TProfile2D("nchE", "Nch #eta<0, no weighting;bbc_E;vz",500,0.,100000.,60,-30,30);
   TProfile2D *p_nchW = new TProfile2D("nchW", "Nch #eta>0, no weighting;bbc_E;vz",500,0.,100000.,60,-30,30);
   TProfile2D *p_nchC = new TProfile2D("nchC", "Nch |#eta|<0.5, no weighting;bbc_E;vz",500,0.,100000.,60,-30,30);
   TProfile2D *p_ntow = new TProfile2D("ntow", "Number of towers with E_{T}>0.2N, no weighting;bbc_E;vz",500,0.,100000.,60,-30,30);

   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;
   if (nevents != -1 && nevents < nentries) nentries = nevents;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      if ((jentry % 1000000) == 0) cout << " finished " << jentry/1000000 << "M entries" << endl;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      if (use_delta_vzVpd) if (TMath::Abs(vz-vzVpd) > delta_vzVpd) continue;
      hg2->Fill(bbc_E, vz);
      pr2->Fill(vz,ZDCx,bbc_E);
      p_nch->Fill(bbc_E,  vz, nch);
      p_nchE->Fill(bbc_E, vz, nchE);
      p_nchW->Fill(bbc_E, vz, nchW);
      p_nchC->Fill(bbc_E, vz, nch_ltp5);
      p_ntow->Fill(bbc_E, vz, ntow_gtp2);
   }
   fout->Write();
   fout->Close();
};

// required options:
// 1. number of entries required
void TreeRootsAnalysis::Loop() { };

void TreeRootsAnalysis::make_TProfile2D()
{
    // -------------------------------
    //  input
    // -------------------------------
    // 0. input file name
    // 1. output file name
    // 2. nevents
    // -------------------------------
   if (fChain == 0) return;
   Long64_t nentries = fChain->GetEntriesFast();

   /* Long64_t nevents; */
   /* string nameout; */
   /* options >> nevents >> nameout; */
   if (nevents != -1 && nevents < nentries) nentries = nevents;

   fout = new TFile(o_file_name.c_str(),"RECREATE");

   TProfile2D *p2All_e = new TProfile2D("p2All_e","BBC with error vs. Vz and ZDCx;vz;ZDCx",   120,-30,30,120,0,35000,"e");
   TProfile2D *p2All_s = new TProfile2D("p2All_s","BBC with err=std vs. Vz and ZDCx;vz;ZDCx", 120,-30,30,120,0,35000,"s");

   TProfile2D *p2Sub_e = new TProfile2D("p2sub_e","BBC with error vs. Vz and ZDCx;vz;ZDCx",   120,-30,30,120,0,35000,"e");
   TProfile2D *p2Sub_s = new TProfile2D("p2sub_s","BBC with err=std vs. Vz and ZDCx;vz;ZDCx", 120,-30,30,120,0,35000,"s");

   TProfile   *pAll_e = new TProfile("pAll_e","BBC with error   vz. Vz, all events", 120,-30,30,"e");
   TProfile   *pAll_s = new TProfile("pAll_s","BBC with err=std vz. Vz, all events", 120,-30,30,"s");

   TProfile   *pSub_e = new TProfile("psub_e","BBC with error   vz. Vz, Vz-Vpd<6 events", 120,-30,30,"e");
   TProfile   *pSub_s = new TProfile("psub_s","BBC with err=std vz. Vz, Vz-Vpd<6 events", 120,-30,30,"s");

   TH1D* bbc_dist = new TH1D("bbc_dist",";bbc;N_{events}",100000,0,98000.);
   TH1D* c_bbc_dist = new TH1D("c_bbc_dist",";bbc;N_{events}",100000,0,98000.);

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

          double c_bbc =  bbc_E-8.699*vz-0.4383*vz*vz-0.6851*ZDCx+1.161e-5*ZDCx*ZDCx-0.003482*ZDCx*vz;
          bbc_dist->Fill(bbc_E);
          c_bbc_dist->Fill(c_bbc);
      }

   }
   fout->Write();
   fout->Close();
}
void TreeRootsAnalysis::make_Deciles(){
   if (fChain == 0) return;

   // options:
   // 0. TFile in
   // 1. nevents
   // 2. TFile out name
   // 3. file with corrections
   // 3. file quantiles ratios in
   // 4. file quantiles calculated out
   /* Long64_t nevents; */
   string  fileout_name;
   string p_file;
   string file_quant_in;
   string file_quant_out;

   /* options >> nevents >> fileout_name >> file_quant_in >> file_quant_out; */
   fout = new TFile(o_file_name.c_str(),"RECREATE");

   //----- read in the correction parameters

   //----- read the quant_in_file
   double xq[50]; // max array size (probably use ten)
   double yq[50];
    int nq = 0;
    //TODO remove this junk line that just keeps warnings down for now...
    if (false) cout << " this is xq and yq " << xq << " " << yq << endl;

    string line;
    ifstream i_file;
    i_file.open(file_quant_in.c_str());
    while (getline(i_file, line)){
        istringstream iss (line);
        double val;
        iss >> val;
        xq[nq] = val;
        ++nq;
    }
    i_file.close();

    // read the p_file
    double p[6];
    i_file.open(p_file.c_str());
    int i_p;
    while (getline(i_file,line)) {
        istringstream iss (line);
        double val;
        iss >> val;
        p[i_p++] = val;
    };

    TH1D *h_quant = new TH1D("h_quant","BBC signal for quantiles;BBC;Number", 30000, -100000., 200000.);
    TH1D *h_quant_corr = new TH1D("h_quant_corr","BBC signal for quantiles;BBC;Number", 30000, -100000., 200000.);

   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;
   if (nevents != -1 && nevents < nentries) nentries = nevents;
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
   fout->Write();
   fout->Close();
};

TreeRootsAnalysis::TreeRootsAnalysis(const char* _myoptions) : options(_myoptions) {
    string infile;
    string which_program;
    options >> which_program >> infile >> o_file_name >> nevents; 
    TFile *f = new TFile(infile.c_str(), "read");
    if (!f) { cout << " fatal error, couldn't read TFile\""<<infile<<"\"." << endl;
              exit(2);
    };
    TTree* tree;
    f->GetObject("events",tree);
    Init(tree);

    // get the outfile name and number of events and run the analysis
    if (which_program == "make_TProfile2D") {
        cout << "Running make_TProfile2D"<<endl;
        make_TProfile2D(); 
    } else if (which_program == "bbcVz_slices") {
        cout << "Running bbcVz_slices" << endl;
        bbcVz_slices(); 
    } else if (which_program == "nch_in_deciles") {
        cout << "Running nch_in_deciles" << endl;
        nch_in_deciles();
    } else if (which_program == "normAcceptanceTo004") {
        cout << "Running normAcceptanceTo004" << endl;
        normAcceptanceTo004();
    } else if (which_program == "Deciles") {
        cout << "Deciles" << endl;
        Deciles();
    } else {
        cout << "analysis option not recognized, not running." << endl;
    }
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
