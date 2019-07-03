//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Jun 12 07:17:03 2019 by ROOT version 6.16/00
// from TTree events/Tree of jets events
// found on file: 206_JetTree.root
//////////////////////////////////////////////////////////

#ifndef JetLoop_h
#define JetLoop_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "TObject.h"
#include "TClonesArray.h"

class JetLoop {
public :
    TString name;
    double cutLo;
    double cutHi;
    double etaCut;
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain
   bool  IsNearSide(int i);
   double  IsWithTrigger(int i);

// Fixed size dimensions of array or collections stored in the TTree if any.
   static constexpr Int_t kMaxjet = 24;

   // Declaration of leaf types
 //JetEvent        *events;
   UInt_t          fUniqueID;
   UInt_t          fBits;
   Float_t         ZDCx;
   Float_t         bbc_E;
   Float_t         bbc_C;
   Float_t         vz;
   Float_t         trig_phi;
   Float_t         trig_eta;
   Float_t         trig_Et;
   Float_t         rho;
   Int_t           jet_;
   UInt_t          jet_fUniqueID[kMaxjet];   //[jet_]
   UInt_t          jet_fBits[kMaxjet];   //[jet_]
   Float_t         jet_pt[kMaxjet];   //[jet_]
   Float_t         jet_pt_corr[kMaxjet];   //[jet_]
   Float_t         jet_phi[kMaxjet];   //[jet_]
   Float_t         jet_eta[kMaxjet];   //[jet_]
   Float_t         jet_area[kMaxjet];   //[jet_]

   // List of branches
   TBranch        *b_events_fUniqueID;   //!
   TBranch        *b_events_fBits;   //!
   TBranch        *b_events_ZDCx;   //!
   TBranch        *b_events_bbc;   //!
   TBranch        *b_events_bbc_corr;   //!
   TBranch        *b_events_vz;   //!
   TBranch        *b_events_trig_phi;   //!
   TBranch        *b_events_trig_eta;   //!
   TBranch        *b_events_trig_Et;   //!
   TBranch        *b_events_rho;   //!
   TBranch        *b_jet_;   //!
   TBranch        *b_jet_fUniqueID;   //!
   TBranch        *b_jet_fBits;   //!
   TBranch        *b_jet_pt;   //!
   TBranch        *b_jet_pt_corr;   //!
   TBranch        *b_jet_phi;   //!
   TBranch        *b_jet_eta;   //!
   TBranch        *b_jet_area;   //!

   JetLoop(const char* name, double cutLo, double cutHi, double etaCut=0.6,TTree *tree=0);
   virtual ~JetLoop();
   /* virtual Int_t    Cut(Long64_t entry); */
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef JetLoop_cxx
JetLoop::JetLoop(const char* _name, double _cutLo, double _cutHi, double _etaCut, TTree *tree) :
    name{_name}, cutLo{_cutLo}, cutHi{_cutHi}, etaCut{_etaCut}, fChain(0) 
{

// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("../../206_wRho.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("../../206_wRho.root");
      }
      f->GetObject("events",tree);

   }
   Init(tree);
}

JetLoop::~JetLoop()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t JetLoop::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t JetLoop::LoadTree(Long64_t entry)
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

void JetLoop::Init(TTree *tree)
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

   fChain->SetBranchAddress("fUniqueID", &fUniqueID, &b_events_fUniqueID);
   fChain->SetBranchAddress("fBits", &fBits, &b_events_fBits);
   fChain->SetBranchAddress("ZDCx", &ZDCx, &b_events_ZDCx);
   fChain->SetBranchAddress("bbc_E", &bbc_E, &b_events_bbc);
   fChain->SetBranchAddress("bbc_C", &bbc_C, &b_events_bbc_corr);
   fChain->SetBranchAddress("vz", &vz, &b_events_vz);
   fChain->SetBranchAddress("trig_phi", &trig_phi, &b_events_trig_phi);
   fChain->SetBranchAddress("trig_eta", &trig_eta, &b_events_trig_eta);
   fChain->SetBranchAddress("trig_Et", &trig_Et, &b_events_trig_Et);
   fChain->SetBranchAddress("rho", &rho, &b_events_rho);
   fChain->SetBranchAddress("jet", &jet_, &b_jet_);
   fChain->SetBranchAddress("jet.fUniqueID", jet_fUniqueID, &b_jet_fUniqueID);
   fChain->SetBranchAddress("jet.fBits", jet_fBits, &b_jet_fBits);
   fChain->SetBranchAddress("jet.pt", jet_pt, &b_jet_pt);
   fChain->SetBranchAddress("jet.pt_corr", jet_pt_corr, &b_jet_pt_corr);
   fChain->SetBranchAddress("jet.phi", jet_phi, &b_jet_phi);
   fChain->SetBranchAddress("jet.eta", jet_eta, &b_jet_eta);
   fChain->SetBranchAddress("jet.area", jet_area, &b_jet_area);
   Notify();
}

Bool_t JetLoop::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void JetLoop::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
/* Int_t JetLoop::Cut(Long64_t entry) */
/* { */
/* // This function may be called from Loop. */
/* // returns  1 if entry is accepted. */
/* // returns -1 otherwise. */
/*    return 1; */
/* } */
#endif // #ifdef JetLoop_cxx
