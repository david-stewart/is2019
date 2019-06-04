#define RunDataAnalysis_cxx
#include "RunDataAnalysis.h"
#include "JetEvent.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <TProfile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <algorithm>
#include <TMath.h>
#include <map>

#include "fastjet/config.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/Selector.hh"
#include "fastjet/tools/Subtractor.hh"
#include "fastjet/tools/JetMedianBackgroundEstimator.hh"


using namespace fastjet;
using namespace std;
   //-----------------------
   // options used by all:
   //----------------------
   // 0: program loop
   // 1: file in
   // 2: file out
   // 3: number of events

const double RunDataAnalysis::p_offset = 1900.;
const double RunDataAnalysis::p0 = 13643.1;
const double RunDataAnalysis::p1 = 53.9611;
const double RunDataAnalysis::p2 = -0.182168;
const double RunDataAnalysis::p3 = 1.41106;
const double RunDataAnalysis::p4 = -3.40385e-05;
const double RunDataAnalysis::p5 = 0.000439491;

double RunDataAnalysis::correct_bbc() {
    return bbcE + p0 + p_offset - p1*vz - p2*vz*vz - p3*ZDCx - p4*ZDCx*ZDCx - p5*vz*ZDCx;
}

float RunDataAnalysis::delta_phi(int i) {
    float rval = track_phi[i] - tower_phi[0];
    while (rval >  TMath::Pi()) rval -= TMath::TwoPi();
    while (rval < -TMath::Pi()) rval += TMath::TwoPi();
    return rval;
};
float RunDataAnalysis::delta_phi(float rval) {
    /* float rval = track_phi[i] - tower_phi[0]; */
    while (rval >  TMath::Pi()) rval -= TMath::TwoPi();
    while (rval < -TMath::Pi()) rval += TMath::TwoPi();
    return rval;
};

void RunDataAnalysis::WriteJetTree()
{
   if (fChain == 0) return;
   //-----------------------
   // options used:
   // double R
   //----------------------
    double jet_R, trig_min, trig_max;
    options >> jet_R >> trig_min >> trig_max;
    if (jet_R < 0.2) {
        cout << "Fatal warning, jet_R="<<jet_R<<endl;
        return;
    }
    double etaMax = 1.0-jet_R;

    TFile *fout     = new TFile(o_file_name.c_str(), "recreate");
    JetEvent     fevent;
    TClonesArray b_jets { "JtJet", 500 };
    TTree *loc_tree = new TTree("events", "Tree of jets events");
    loc_tree->Branch("events", &fevent);
    loc_tree->Branch("jet",    &b_jets);

    // make the background estimator
    // ----------------------------
    //  jet background estimator //
    // ----------------------------
        JetDefinition jet_def_bkgd(kt_algorithm, jet_R); // <--
        float ghost_maxrap = 4.0;
        AreaDefinition area_def_bkgd(active_area_explicit_ghosts,GhostedAreaSpec (ghost_maxrap,1,0.01));
        Selector selector_rm2 = SelectorAbsEtaMax(1.0) * (!SelectorNHardest(2)); // <--
        JetMedianBackgroundEstimator* bge_rm2 = new JetMedianBackgroundEstimator (selector_rm2, jet_def_bkgd, area_def_bkgd); //
        Selector not_pure_ghost = !SelectorIsPureGhost();
        /* Subtractor* subtractor = new Subtractor( bge_rm2 ) ; */
    
    //-----------------
    // run the loop
    //-----------------
    Long64_t nentries = fChain->GetEntriesFast();
    if (nevents != -1 && nevents < nentries) nentries = nevents;
    cout << " Starting " << nentries << endl;
    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry % 1000000 == 0) cout << " ientry " << ientry << endl;
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;
        if (tower_ == 0 || tower_E[0] < trig_min || tower_E[0] > trig_max) continue;
        if (ranking < 0) continue;

        fevent.ZDCx = ZDCx;
        fevent.bbc = bbcE;
        fevent.bbc_corr = correct_bbc();
        fevent.vz = vz;

        fevent.trig_phi = tower_phi[0];
        fevent.trig_eta = tower_eta[0];
        fevent.trig_Et  = tower_E[0];
        
        //Fill in the particles and make jets
        b_jets.Clear();
        vector<PseudoJet> particles;
        for (int j=0;j<track_;++j) {
            particles.push_back(PseudoJet());
            particles[j].reset_PtYPhiM(track_E[j],track_eta[j],delta_phi(j));
        }

        //make the jets
        AreaDefinition area_def(active_area_explicit_ghosts,GhostedAreaSpec(4.0,1,0.01));
        JetDefinition jet_def(antikt_algorithm, jet_R);
        ClusterSequenceArea cs(particles, jet_def, area_def);
        vector<PseudoJet> jets_all = cs.inclusive_jets();
        vector<PseudoJet> jets = sorted_by_pt( not_pure_ghost(jets_all ));
        /* cout << "particles.size: " << particles.size() <<":"<<jets.size()<< endl; */
        bge_rm2->set_particles(particles);

        unsigned int njets = 0;
        for (unsigned int i_jet = 0; i_jet < jets.size(); ++i_jet){
            PseudoJet& jet = jets[i_jet];
            if (jet.pt() < 0.2) break;
            if ( jet.is_pure_ghost() ) { continue; }
            if ( jet.eta() > etaMax || jet.eta() < -etaMax) continue;

            JtJet* b_jet = (JtJet*) b_jets.ConstructedAt(njets); 
            ++njets;

            b_jet->phi = jet.phi_std();
            b_jet->eta = jet.eta();
            b_jet->pt  = jet.pt();
            b_jet->area = jet.area();
            /* if (jet.constituents().size() > 1) */
            /* cout << "  " << jet.pt() << " " << jet.eta() << "  " << jet.constituents().size() << " " << jet.area() << endl; */
        }
        loc_tree->Fill();
   }
   /* cout << "Entries: " << sumpt->GetEntries() << endl; */
   fout->Write();
   fout->Close();
}

void RunDataAnalysis::Deciles()
{
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
    bool   use_bbc_corr;
    options >> decile_root_file >> decile_th1d >> et_min >> et_max >> use_bbc_corr;
    bool use_et_cut = (et_max > 0);

    //read in the deciles root file to find the value of the deciles
    TH1D* hg_dec;
    double xq[11], yq[11];
    for (unsigned int i=0; i<11;++i){ xq[i] = 0.1*i; }
    TFile *f_deciles = new TFile(decile_root_file.c_str(),"read");
    f_deciles->GetObject(decile_th1d.c_str(), hg_dec);
    hg_dec->GetQuantiles(11,yq,xq);

    for (unsigned int i=0; i<11;++i) cout << " yq["<<i<<"] " << yq[i]<<endl;

    //run the loop
    TFile *fout    = new TFile(o_file_name.c_str(), "recreate");
    TH1D     *nEvents = new TH1D("nEvents","nEVents;Event Activity_{BBC};#frac{1}{N_{ev}}#frac{dN_{ev}}{dEA_{BBC}}", 10, yq);
    TProfile *nch     = new TProfile("nch","nch;Event Activity_{BBC};#LTN_{cv}#GT", 10, yq);
    TProfile *sumpt   = new TProfile("sumpt","sumpt;Event Activity_{BBC};#LT#Sigmap_T^{ch}#GT",  10, yq);
    TProfile *maxpt   = new TProfile("maxpt","maxpt;Event Activity_{BBC};#LTmax(p_{T}^{nc})#GT", 10, yq);
    TH2D     *tracks  = new TH2D("tracks","distr of tracks per bin for unfolding;bbcE;p_{T}",10,yq,300,0.,30.);

    nEvents->Sumw2();
    nch->Sumw2();
    sumpt->Sumw2();
    maxpt->Sumw2();

    //-----------------
    // run the loop
    //-----------------
    Long64_t nentries = fChain->GetEntriesFast();
    if (nevents != -1 && nevents < nentries) nentries = nevents;
    cout << " Starting " << nentries << endl;
    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry % 1000000 == 0) cout << " ientry " << ientry << endl;
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;
        
        //cut on leading tower Et (if it is a cut, as evidenced by a non-zero et_max)
        if (use_et_cut) if (tower_ == 0 || tower_E[0] < et_min || tower_E[0] > et_max) continue;
        /* if (jentry > 10) break; // TODO TEMP */

        if (TMath::Abs(vz - vzVpd)>6 || ZDCx<7000. || ZDCx>24000. || ranking<=0 || TMath::Abs(vz)>10 ) continue;
        double bbc = (use_bbc_corr) 
            ? correct_bbc()
            : bbcE;

        nEvents->Fill(bbc);
        nch->Fill(bbc,track_);

        double maxpt_val = {0.};
        double sumpt_val = {0.};
        for (int i=0; i<track_; ++i){
            sumpt_val += track_E[i];
            if (track_E[i] > maxpt_val) maxpt_val = track_E[i];
            tracks->Fill(bbc, track_E[i]);
        }
        sumpt->Fill(bbc,sumpt_val);
        maxpt->Fill(bbc,maxpt_val);
   }
   cout << "Entries: " << sumpt->GetEntries() << endl;
   fout->Write();
   fout->Close();
}

/* void RunDataAnalysis::TOF_dist() */
/* { */
/*     if (fChain == 0) return; */
/*     TFile *f_deciles = new TFile(decile_root_file.c_str(),"read"); */

/*     TH1D* tof = new TH1D("tof","N_{tof};N_{tof};N_{events}",100,-0.5,99.5); */
/*     TH1D* notof = new TH1D("no_tof","N_{tof};N_{tof};N_{events}",100,-0.5,99.5); */

/*    Long64_t nbytes = 0, nb = 0; */
/*    for (Long64_t jentry=0; jentry<nentries;jentry++) { */
/*       Long64_t ientry = LoadTree(jentry); */
/*       if (ientry < 0) break; */
/*       nb = fChain->GetEntry(jentry);   nbytes += nb; */
/*       cout << "tracks: " << track_ << endl; */
/*       if (jentry > 10) break; */
/*       // if (Cut(ientry) < 0) continue; */
/*    } */
/* } */
void RunDataAnalysis::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L RunDataAnalysis.C
//      Root > RunDataAnalysis t
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

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      cout << "tracks: " << track_ << endl;
      if (jentry > 10) break;
      // if (Cut(ientry) < 0) continue;
   }
}

/* RunDataAnalysis::RunDataAnalysis(TTree *tree) : fChain(0) */ 
RunDataAnalysis::RunDataAnalysis(const char* _myoptions) : options(_myoptions) {
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

    if (which_program == "Deciles") {
        cout << which_program << endl;
        Deciles(); 
    } else if (which_program == "WriteJetTree") {
        cout << which_program << endl;
        WriteJetTree(); 
    /* } else if (which_program == "nch_in_deciles") { */
    /*     cout << "Running nch_in_deciles" << endl; */
    /*     nch_in_deciles(); */
    /* } else if (which_program == "normAcceptanceTo004") { */
    /*     cout << "Running normAcceptanceTo004" << endl; */
    /*     normAcceptanceTo004(); */
    } else {
        cout << "analysis option not recognized, not running." << endl;
    }
};

RunDataAnalysis::~RunDataAnalysis()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t RunDataAnalysis::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t RunDataAnalysis::LoadTree(Long64_t entry)
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

void RunDataAnalysis::Init(TTree *tree)
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
   fChain->SetBranchAddress("bbcES", &bbcES, &b_event_bbcES);
   fChain->SetBranchAddress("bbcE", &bbcE, &b_event_bbcE);
   fChain->SetBranchAddress("ZDCx", &ZDCx, &b_event_ZDCx);
   fChain->SetBranchAddress("vz", &vz, &b_event_vz);
   fChain->SetBranchAddress("vzVpd", &vzVpd, &b_event_vzVpd);
   fChain->SetBranchAddress("track", &track_, &b_track_);
   fChain->SetBranchAddress("track.fUniqueID", track_fUniqueID, &b_track_fUniqueID);
   fChain->SetBranchAddress("track.fBits", track_fBits, &b_track_fBits);
   fChain->SetBranchAddress("track.E", track_E, &b_track_E);
   fChain->SetBranchAddress("track.phi", track_phi, &b_track_phi);
   fChain->SetBranchAddress("track.eta", track_eta, &b_track_eta);
   fChain->SetBranchAddress("tower", &tower_, &b_tower_);
   fChain->SetBranchAddress("tower.fUniqueID", tower_fUniqueID, &b_tower_fUniqueID);
   fChain->SetBranchAddress("tower.fBits", tower_fBits, &b_tower_fBits);
   fChain->SetBranchAddress("tower.E", tower_E, &b_tower_E);
   fChain->SetBranchAddress("tower.phi", tower_phi, &b_tower_phi);
   fChain->SetBranchAddress("tower.eta", tower_eta, &b_tower_eta);
   Notify();
}

Bool_t RunDataAnalysis::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void RunDataAnalysis::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t RunDataAnalysis::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
