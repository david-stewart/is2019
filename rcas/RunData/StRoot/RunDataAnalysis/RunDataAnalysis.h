//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue May 14 15:38:17 2019 by ROOT version 5.34/30
// from TTree events/tree  w/(SL18b)events w/triggerId=500206
// found on file: hadd_file.root
//////////////////////////////////////////////////////////

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <iostream>
#include <sstream>

// Header file for the classes stored in the TTree if any.
#include <TObject.h>
#include <TClonesArray.h>
using namespace std;

// Fixed size dimensions of array or collections stored in the TTree if any.
   const Int_t kMaxtrack = 77;
   const Int_t kMaxtower = 2;

class RunDataAnalysis {
public :
   /* TTree          *fChain;   //!pointer to the analyzed TTree or TChain */
   /* Int_t           fCurrent; //!current Tree number in a TChain */
   /* istringstream   options; */

   Int_t           fCurrent; //!current Tree number in a TChain
   TFile          *fout; //!current Tree number in a TChain
   istringstream   options;
   /* const char*    nameout; */
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Long64_t       nevents;
   string         o_file_name;

   // Declaration of leaf types
 //TreeEvent       *event;
   UInt_t          fUniqueID;
   UInt_t          fBits;
   Int_t           runId;
   Int_t           bbcES;
   Double_t        bbcE;
   Float_t         zdcX;
   Float_t         vz;
   Float_t         vzVpd;
   Int_t           track_;
   UInt_t          track_fUniqueID[kMaxtrack];   //[track_]
   UInt_t          track_fBits[kMaxtrack];   //[track_]
   Float_t         track_E[kMaxtrack];   //[track_]
   Float_t         track_phi[kMaxtrack];   //[track_]
   Float_t         track_eta[kMaxtrack];   //[track_]
   Int_t           tower_;
   UInt_t          tower_fUniqueID[kMaxtower];   //[tower_]
   UInt_t          tower_fBits[kMaxtower];   //[tower_]
   Float_t         tower_E[kMaxtower];   //[tower_]
   Float_t         tower_phi[kMaxtower];   //[tower_]
   Float_t         tower_eta[kMaxtower];   //[tower_]

   // List of branches
   TBranch        *b_event_fUniqueID;   //!
   TBranch        *b_event_fBits;   //!
   TBranch        *b_event_runId;   //!
   TBranch        *b_event_bbcES;   //!
   TBranch        *b_event_bbcE;   //!
   TBranch        *b_event_zdcX;   //!
   TBranch        *b_event_vz;   //!
   TBranch        *b_event_vzVpd;   //!
   TBranch        *b_track_;   //!
   TBranch        *b_track_fUniqueID;   //!
   TBranch        *b_track_fBits;   //!
   TBranch        *b_track_E;   //!
   TBranch        *b_track_phi;   //!
   TBranch        *b_track_eta;   //!
   TBranch        *b_tower_;   //!
   TBranch        *b_tower_fUniqueID;   //!
   TBranch        *b_tower_fBits;   //!
   TBranch        *b_tower_E;   //!
   TBranch        *b_tower_phi;   //!
   TBranch        *b_tower_eta;   //!

   /* RunDataAnalysis(TTree *tree=0); */
   RunDataAnalysis(const char* _options);
   virtual ~RunDataAnalysis();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual void     Deciles();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   ClassDef(RunDataAnalysis,1)
};

