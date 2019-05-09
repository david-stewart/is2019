#ifndef RUNQA_H
#define RUNQA_H

#include "TTree.h"
#include "St_db_Maker/St_db_Maker.h"
#include "StEmcUtil/database/StBemcTables.h"
#include "StRoot/StPicoDstMaker/StPicoDst.h"
/* #include "StRoot/StPicoEvent/StPicoDst.h" */
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

class RunData : public StMaker {

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

    int             triggerId;

    TClonesArray    b_tracks;
    TClonesArray    b_towers;

    TFile*          fout_root;
    TTree*          ftree;
    TreeEvent       fevent;

    map<int, double> p0;
    map<int, double> p1;
    map<int, double> p2;

    double           p0_arb; // the zero of the overall fit.
    double           p1_arb; // the 1 of the overall fit.
    double           p2_arb; // the 2 of the overall fit.

    /* map<string, int> m_file_names; */
    long long int fEventsProcessed;
    /* map<int, TProfile*> p_means; */
    /* map<int, TProfile*> bbcVz; */
    /* map<int, TH2D*>     et_vals; */
    /* map<int, TH1D*>     h_means; */ 

    /* TH1D* vz; */
    /* TH1D* vzVpd; */
    /* TH1D* delta_vz_vzVpd; */
    /* TH2D* vz_vzVpd; */
    /* TProfile* et_gt4; */

    public:
    RunData(
        const char*     _name,
        const char*     _outName,
        ofstream&       _log,
        StPicoDstMaker* _picoMaker,
        const char*     _bad_run_list,
        const char*     _bad_tower_list,
        const char*     _runQA_summary_root,
        St_db_Maker*    _starDb,
        int             _triggerId
    );
    virtual ~RunData();
    
    virtual Int_t Init();
    virtual Int_t Make();
    virtual void  Clear(Option_t *opt="");
    virtual Int_t Finish();

    ClassDef(RunData, 1)
};

#endif
