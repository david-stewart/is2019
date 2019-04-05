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
/* #include "bad_runs.h" */
/* #include "bad_towers.h" */
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
/* #include "TreeObjects.h" */
#include <map>

class StPicoDst;
class StPicoDstMaker;
class StPicoEvent;
class TString;
class TH1F;
class TH2F;
class TH3F;
class TH1I;
class TProfile;

class RunQA : public StMaker {

    private:
    // initialized from constructor
    ofstream&       log;
    StPicoDstMaker* picoDstMaker;
    St_db_Maker*    starDb;

    // other
    vector<int> bad_run_list;
    vector<int> bad_tower_list;
    StBemcTables*   bemc;
    StEmcGeom*      bemcGeom;

    TFile*  fout_root;
    TTree*  tree;
        
    map<string, int> m_file_names;
    long long int fEventsProcessed;
    map<int, TProfile*> p_means;
    map<int, TH1D*>     h_means; 

    TH1D* vz;
    TH1D* vzVpd;
    TH1D* delta_vz_vzVpd;
    TH2D* vz_vzVpd;

    public:
    RunQA(
        const char*     _name,
        const char*     _outName,
        ofstream&       _log,
        StPicoDstMaker* _picoMaker,
        const char*     _bad_run_list,
        const char*     _bad_tower_list,
        St_db_Maker    *_starDb,
        vector<int>     _runIds,
        vector<int>     _runDurations,
        vector<string>  _beginTimes
    );
    virtual ~RunQA();
    
    virtual Int_t Init();
    virtual Int_t Make();
    virtual void  Clear(Option_t *opt="");
    virtual Int_t Finish();

    ClassDef(RunQA, 1)
};

#endif
