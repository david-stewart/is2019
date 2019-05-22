//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu May  9 14:21:10 2019 by ROOT version 5.34/30
// from TTree events/tree  w/(SL18b)events w/triggerId=500004
// found on file: test.root
//////////////////////////////////////////////////////////

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <iostream>
using namespace std;
// Header file for the classes stored in the TTree if any.
#include <TObject.h>


// Fixed size dimensions of array or collections stored in the TTree if any.

class Deciles {
public :
   Int_t           fCurrent; //!current Tree number in a TChain
   TFile          *fout; //!current Tree number in a TChain
   const char*    nameout;
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   TString        pfile_name;
   Long64_t       user_entries;
   

   // Declaration of leaf types
 //TreeEvent       *event;
   UInt_t          fUniqueID;
   UInt_t          fBits;
   Int_t           runId;
   Float_t         vz;
   Float_t         vzVpd;
   Int_t           nch;
   Int_t           nchE;
   Int_t           nchW;
   Int_t           nch_ltp5;
   Int_t           ntow_gtp2;
   Int_t           ntow_gtp2_E;
   Int_t           ntow_gtp2_W;
   Int_t           ntow_gt4;
   Int_t           ntow_gt4_E;
   Int_t           ntow_gt4_W;
   Float_t         bbc_E;
   Float_t         bbc_ES;
   Float_t         bbc_EL;
   Float_t         bbc_W;
   Float_t         bbc_WS;
   Float_t         bbc_WL;
   Float_t         ZDCx;
   Float_t         ZdcEastRate;
   Float_t         ZdcWestRate;
   Float_t         ZdcSumAdcEast;
   Float_t         ZdcSumAdcWest;

   // List of branches
   TBranch        *b_event_fUniqueID;   //!
   TBranch        *b_event_fBits;   //!
   TBranch        *b_event_runId;   //!
   TBranch        *b_event_vz;   //!
   TBranch        *b_event_vzVpd;   //!
   TBranch        *b_event_nch;   //!
   TBranch        *b_event_nchE;   //!
   TBranch        *b_event_nchW;   //!
   TBranch        *b_event_nch_ltp5;   //!
   TBranch        *b_event_ntow_gtp2;   //!
   TBranch        *b_event_ntow_gtp2_E;   //!
   TBranch        *b_event_ntow_gtp2_W;   //!
   TBranch        *b_event_ntow_gt4;   //!
   TBranch        *b_event_ntow_gt4_E;   //!
   TBranch        *b_event_ntow_gt4_W;   //!
   TBranch        *b_event_bbc_E;   //!
   TBranch        *b_event_bbc_ES;   //!
   TBranch        *b_event_bbc_EL;   //!
   TBranch        *b_event_bbc_W;   //!
   TBranch        *b_event_bbc_WS;   //!
   TBranch        *b_event_bbc_WL;   //!
   TBranch        *b_event_ZDCx;   //!
   TBranch        *b_event_ZdcEastRate;   //!
   TBranch        *b_event_ZdcWestRate;   //!
   TBranch        *b_event_ZdcSumAdcEast;   //!
   TBranch        *b_event_ZdcSumAdcWest;   //!

   Deciles(TTree *tree=0);
   Deciles(const char* file_in_name, const char* file_out_name, TString _pfile_name, Long64_t _user_entries);

   virtual ~Deciles();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   ClassDef(Deciles,1)
};





