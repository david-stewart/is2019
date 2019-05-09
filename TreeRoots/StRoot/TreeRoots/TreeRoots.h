#ifndef RUNQA_H
#define RUNQA_H

#include "TTree.h"
#include "St_db_Maker/St_db_Maker.h"
#include "StEmcUtil/database/StBemcTables.h"
#include "StRoot/StPicoEvent/StPicoDst.h"
#include "StRoot/StPicoEvent/StPicoTrack.h"
#include "StRoot/StPicoEvent/StPicoEvent.h"
#include "StRoot/StPicoDstMaker/StPicoDstMaker.h"
#include "StRoot/StPicoEvent/StPicoEmcTrigger.h"
#include "StRoot/StPicoEvent/StPicoBTowHit.h"
#include "StPicoEvent/StPicoBEmcPidTraits.h"
#include "StMaker.h"
#include "StThreeVectorF.hh"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TH1I.h"
#include "TProfile.h"
#include "TFile.h"
#include <iostream>
#include "StThreeVectorF.hh"
#include "StEmcUtil/geometry/StEmcGeom.h"
#include "StEmcUtil/projection/StEmcPosition.h"
#include "TreeEvent.h"
/* #include "TreeObjects.h" */
#include <map>

#define MaxTracks 100;

class StPicoDst;
class StPicoDstMaker;
class StPicoEvent;
class TString;
class TH1F;
class TH2F;
class TH3F;
class TH1I;
class TProfile;

class TreeRoots : public StMaker {

    private:
    // initialized from constructor
    ofstream&       log;
    StPicoDstMaker* picoDstMaker;
    St_db_Maker*    starDb;

    // other
    vector<int>     bad_run_list;
    vector<int>     bad_tower_list;
    StBemcTables*   bemc;
    StEmcGeom*      bemcGeom;

    TFile*          fout_root;
    TTree*          ftree;
    TreeEvent       fevent;

    int             triggerId;
    
    long long int fEventsProcessed;

    public:
    TreeRoots(
        const char*     _name,
        const char*     _outName,
        ofstream&       _log,
        StPicoDstMaker* _picoMaker,
        const char*     _bad_run_list,
        const char*     _bad_tower_list,
        /* const char*     _runQA_summary_root, */
        St_db_Maker*    _starDb,
        int             trigger
    );
    virtual ~TreeRoots();
    
    virtual Int_t Init();
    virtual Int_t Make();
    virtual void  Clear(Option_t *opt="");
    virtual Int_t Finish();

    ClassDef(TreeRoots, 1)
};

#endif
