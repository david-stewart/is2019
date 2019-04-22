//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Mar 23 15:11:22 2019 by ROOT version 5.34/30
// from TTree tree/Tree of embedding tracks
// found on file: sample_full.root
//////////////////////////////////////////////////////////

#ifndef emb_tree_h
#define emb_tree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <string>
#include <iostream>

// Header file for the classes stored in the TTree if any.
using std::string;
using std::cout;
using std::endl;
#include <TObject.h>

// Fixed size dimensions of array or collections stored in the TTree if any.

class emb_tree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
 //EmbTrack        *EmcTrack;
   UInt_t          fUniqueID;
   UInt_t          fBits;
   Bool_t          is_matched;
   Float_t         ptMc;
   Float_t         ptPr;
   Float_t         phiMc;
   Float_t         phiPr;
   Float_t         etaMc;
   Float_t         etaPr;
   Int_t           nHitMc;
   Float_t         commonFrac;
   Double_t        zdcX;
   Float_t         z_vert;
   Int_t           runId;
   Int_t           refMult;
   Double_t        bbcAdcES;


   // List of branches
   TBranch        *b_EmcTrack_fUniqueID;   //!
   TBranch        *b_EmcTrack_fBits;   //!
   TBranch        *b_EmcTrack_is_matched;   //!
   TBranch        *b_EmcTrack_ptMc;   //!
   TBranch        *b_EmcTrack_ptPr;   //!
   TBranch        *b_EmcTrack_phiMc;   //!
   TBranch        *b_EmcTrack_phiPr;   //!
   TBranch        *b_EmcTrack_etaMc;   //!
   TBranch        *b_EmcTrack_etaPr;   //!
   TBranch        *b_EmcTrack_nHitMc;   //!
   TBranch        *b_EmcTrack_commonFrac;   //!
   TBranch        *b_EmcTrack_zdcX;   //!
   TBranch        *b_EmcTrack_z_vert;   //!
   TBranch        *b_EmcTrack_runId;   //!
   TBranch        *b_EmcTrack_refMult;   //!
   TBranch        *b_EmcTrack_bbcAdcES;   //!

   emb_tree(TString in_file_name);
   virtual ~emb_tree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(TString, int);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef emb_tree_cxx
emb_tree::emb_tree(TString root_file) : fChain(0) 
{
    cout << "root_file: " << root_file << endl;
  TTree *tree = 0;
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
  TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(root_file.Data());
  if (!f || !f->IsOpen()) f = new TFile(root_file.Data());

  f->GetObject("tree",tree);
  if (!tree) {
      cout << " Fatal: cannot open file " << root_file << endl;
      exit(1);
  }
  Init(tree);
}

emb_tree::~emb_tree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t emb_tree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t emb_tree::LoadTree(Long64_t entry)
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

void emb_tree::Init(TTree *tree)
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

   fChain->SetBranchAddress("fUniqueID", &fUniqueID, &b_EmcTrack_fUniqueID);
   fChain->SetBranchAddress("fBits", &fBits, &b_EmcTrack_fBits);
   fChain->SetBranchAddress("is_matched", &is_matched, &b_EmcTrack_is_matched);
   fChain->SetBranchAddress("ptMc", &ptMc, &b_EmcTrack_ptMc);
   fChain->SetBranchAddress("ptPr", &ptPr, &b_EmcTrack_ptPr);
   fChain->SetBranchAddress("phiMc", &phiMc, &b_EmcTrack_phiMc);
   fChain->SetBranchAddress("phiPr", &phiPr, &b_EmcTrack_phiPr);
   fChain->SetBranchAddress("etaMc", &etaMc, &b_EmcTrack_etaMc);
   fChain->SetBranchAddress("etaPr", &etaPr, &b_EmcTrack_etaPr);
   fChain->SetBranchAddress("nHitMc", &nHitMc, &b_EmcTrack_nHitMc);
   fChain->SetBranchAddress("commonFrac", &commonFrac, &b_EmcTrack_commonFrac);
   fChain->SetBranchAddress("zdcX", &zdcX, &b_EmcTrack_zdcX);
   fChain->SetBranchAddress("z_vert", &z_vert, &b_EmcTrack_z_vert);
   fChain->SetBranchAddress("runId", &runId, &b_EmcTrack_runId);
   fChain->SetBranchAddress("refMult", &refMult, &b_EmcTrack_refMult);
   fChain->SetBranchAddress("bbcAdcES", &bbcAdcES, &b_EmcTrack_bbcAdcES);
   Notify();
}

Bool_t emb_tree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void emb_tree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t emb_tree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef emb_tree_cxx
