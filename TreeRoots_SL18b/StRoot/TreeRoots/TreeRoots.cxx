//   New objective: kj
//
// Includes code from (1) Joel Mazer from https://github.com/joelmazer/star-jetframework in July 2018
//                    (2) Tong Liu from ARCAS, also July 2018
//
#include "TreeRoots.h"
#include <iostream>
#include "TChain.h"
#include <fstream>
#include "TNtuple.h"
#include "TF1.h"
/* #include "fastjet/config.h" */
/* #include "fastjet/PseudoJet.hh" */

/* using namespace fastjet; */

ClassImp(TreeRoots)
//-----------------------------------------------------------------------------
TreeRoots::TreeRoots(
        const char*     _name,
        const char*     _outName,
        ofstream&       _log,
        StPicoDstMaker* _picoMaker,
        const char*     _bad_run_list,
        const char*     _bad_tower_list,
        /* const char*     _runQA_summary, */
        St_db_Maker*    _starDb,
        int             _triggerId
    )
  : StMaker         (_name), 
    log             (_log),
    picoDstMaker    {_picoMaker},
    starDb          {_starDb},
    bemcGeom        { StEmcGeom::getEmcGeom("bemc") },
    triggerId       {_triggerId}
{
    bemc = new StBemcTables();
    
    // read in the bad run and bad tower lists
    log << "Reading in the bad run list" << endl;
    bad_run_list = vector<int>{};
    ifstream file;
    file.open(_bad_run_list);
    if (!file.is_open()) {
        log << "Warning: the bad run file " << _bad_run_list << " could not be opened." << endl;
        log << "Therefore, using no bad runs." << endl;
    } else {
        string line;
        while (getline(file,line)) {
            line.append(" ");
            stringstream words(line);
            TString word;
            while (words >> word) {
                if (word.BeginsWith("//") || word.BeginsWith("#")) break;
                bad_run_list.push_back(word.Atoi());
            }
        }
        std::sort(bad_run_list.begin(), bad_run_list.end());
        log << "- Sucessfully read in bad run list from " << _bad_run_list << endl;
        log << "  Loaded " << bad_run_list.size() << " bad runs" << endl;
    }
    file.close();

    file.open(_bad_tower_list);
    bad_tower_list = vector<int>{};
    if (!file.is_open()) {
        log << "Warning: the bad tower file " << _bad_tower_list << "could not be opened." << endl;
        log << "Therefore, no bad towers are used." << endl;
    } else {
        string line;
        while (getline(file,line)) {
            line.append(" ");
            stringstream words(line);
            TString word;
            while (words >> word) {
                if (word.BeginsWith("//") || word.BeginsWith("#")) break;
                bad_tower_list.push_back(word.Atoi());
            }
        }
        std::sort(bad_tower_list.begin(), bad_tower_list.end());

        log << "- Read bad tower list from " << _bad_tower_list << endl;
        log << "  Loaded " << bad_tower_list.size() << " bad towers" << endl;
    }
    file.close();

    fout_root = new TFile(Form("%s.root",_outName),"RECREATE");
    TString which_SL;
    if (triggerId == 500001) which_SL="SL18h";
    else                     which_SL="SL18b";
    ftree = new TTree("events",Form("tree  w/(%s)events w/triggerId=%i",which_SL.Data(),triggerId));
    ftree->Branch("event", &fevent);

    // Generate the tree with the run average information
    /* TTree *tree = new TTree("run_data","Run id, run length (in secs), time start"); */
    /* int i_runId; */
    /* int i_runDuration; */
    /* string st_startTime; */
    /* tree->Branch("runId",       &i_runId); */
    /* tree->Branch("length_secs", &i_runDuration); */
    /* tree->Branch("start_time",  &st_startTime); */
};

//----------------------------------------------------------------------------- 
TreeRoots::~TreeRoots()
{ };

//----------------------------------------------------------------------------- 
Int_t TreeRoots::Init() {
    return kStOK;
};

//----------------------------------------------------------------------------- 
Int_t TreeRoots::Finish() {
    log << " Finished with TreeRoots object " << endl;
    fout_root->Write();
    fout_root->Close();
    return kStOK;
};
    
//----------------------------------------------------------------------------- 
void TreeRoots::Clear(Option_t *opt) { };

//----------------------------------------------------------------------------- 
Int_t TreeRoots::Make() {
    /* cout << " a0 " << endl; */

    if(!picoDstMaker) {
        log << " No PicoDstMaker! Skip! " << endm;
        return kStWarn;
    }
    StPicoDst* picoDst = picoDstMaker->picoDst();
    if(!picoDst) {
        log << " No PicoDst! Skip! " << endl;
        return kStWarn;
    }

    /* cout << " a1 " << endl; */
    StPicoEvent* picoEvent = picoDst->event();

    /* vector<unsigned int> tids { picoEvent->triggerIds() }; */
    /* for (unsigned int i = 0; i < tids.size(); ++i) */
    /*     cout << "tr("<<i<<") : " << tids[i] << endl; */
    if (!picoEvent->isTrigger(triggerId)) return kStOk;
    /* cout << "-----------------------------------------MADE IT--------------------" << endl; */

    fevent.runId = picoEvent->runId() ;
    fevent.b_rank = (picoEvent->ranking() > 0);
    fevent.ZDCx = picoEvent->ZDCx();

    // cut on bad_run_list
    if (std::find(bad_run_list.begin(), bad_run_list.end(), fevent.runId)
            != bad_run_list.end()) return kStOK;


    /* cout << " a2 " << endl; */
    //*// cut on vz
    fevent.vz = picoEvent->primaryVertex().z();
    fevent.vzVpd = picoEvent->vzVpd();
    
    fevent.b_vz10   = TMath::Abs(fevent.vz) < 10;
    fevent.b_vzVpd6 = TMath::Abs(fevent.vz-fevent.vzVpd) < 6;
    fevent.b_all    = (fevent.b_rank && fevent.b_vz10 && fevent.b_vzVpd6);
    // cout << " oo oo oo  -------- oo oo oo dst: " << picoDst << " event: <<" << picoEvent << ">>  vertex: <<" /*<< picoEvent->primaryVertex() */<< ">>    vz: " << picoEvent->primaryVertex().z() << "  runId " << picoEvent->runId() << endl;
    /* cout << " a3 " << endl; */
    /* if (TMath::Abs(fevent.vz) > 10) return kStOK; */
    /* cout << " a4 " << endl; */

    fevent.nVpdHitsEast = picoEvent->nVpdHitsEast();
    fevent.nVpdHitsWest = picoEvent->nVpdHitsWest();

    fevent.nch_tof = 0;
    fevent.nch = 0;
    fevent.nchE = 0;
    fevent.nchW = 0;
    fevent.nch_ltp5 = 0;

    for (unsigned int i = 0; i < picoDst->numberOfTracks(); ++i) {
        StPicoTrack* track = picoDst->track(i);
        float dca = (track->dcaPoint() - picoEvent->primaryVertex()).mag();
        StThreeVectorF Ptrack = track->pMom();
        if (dca > 3.0) continue;
        double eta  { Ptrack.pseudoRapidity() };
        if (TMath::Abs(eta)  >= 1.0) continue;
        float nhit_ratio = ((float)track->nHitsFit()) / (float)track->nHitsMax();
        if (nhit_ratio <= 0.52) continue;
        double pt {Ptrack.perp() };
        if (pt > 30.)  return kStOK;
        if (pt < 0.2) continue;

        if (!track || !(track->isPrimary()))   continue;
        ++fevent.nch;
        if (track->bTofPidTraitsIndex()>=0) ++fevent.nch_tof;
        if (eta < 0) { ++fevent.nchE; }
        else         { ++fevent.nchW; }
        if (TMath::Abs(eta) < 0.5) ++fevent.nch_ltp5;
    }

    const int detector{1};

    // use fastjet for both charged and nuetral hits
    fevent.ntow_gtp2   = 0;
    fevent.ntow_gtp2_E  = 0; // ditto for east
    fevent.ntow_gtp2_W  = 0; // ditto for west

    fevent.ntow_gt4    = 0;   // # towers > 4 GeV total
    fevent.ntow_gt4_E  = 0;  // ditto for east
    fevent.ntow_gt4_W  = 0;  // ditto for west
    
    /* vector<PseudoJet> towers; */
    bemc->loadTables(starDb);
    /* cout << " z0 " << endl; */
    for (int i_tower = 1; i_tower < 4801; ++i_tower){
        /* cout << " z1 " << i_tower <<  endl; */
        if (std::find(bad_tower_list.begin(), 
                      bad_tower_list.end(),i_tower) != bad_tower_list.end())
        { continue; }
        /* cout << " z2 " << i_tower <<  endl; */

        if ( 
            bemc->status(detector, i_tower)             == 1
         && bemc->status(detector, i_tower, "pedestal") == 1
         && bemc->status(detector, i_tower, "gain")     == 1
         && bemc->status(detector, i_tower, "calib")    == 1
         ) {
            /* StThreeVectorF towLoc = mPosition->getPosFromVertex( */ 
            /*         (StVertex*) mevent->primaryVertex(), i_tower); */
            StPicoBTowHit* bTowHit = picoDst->btowHit(i_tower-1); 

            if (!bTowHit) { 
                log << " NO BTOWHIT " << endl;
                continue;
            }
            StThreeVectorF towLoc = StEmcPosition().getPosFromVertex(picoEvent->primaryVertex(), i_tower);

            /* cout << " version A("<<i_tower<<"): " << endl; */
            /* StEmcPosition *mPosition = new StEmcPosition(); */
            /* StThreeVectorF towLoc_0 = mPosition->getPosFromVertex(picoEvent->primaryVertex(), i_tower); */
            /* cout << " are the same? " << (towLoc_0 == towLoc_1) << endl; */
            /* cout << " loc: " << towLoc_0.pseudoRapidity() << "  " << towLoc_1.pseudoRapidity() << endl; */
            /* if (i_tower > 100) break; */
            /* StThreeVectorF towLoc = mPosition->getPosFromVertex( */ 
            /*         picoEvent->primaryVertex(), i_tower); */
            /* StThreeVectorF towLoc = StEmcPosition().getPosFromVertex( picoEvent->primaryVertex(), i_tower); */
            /* delete mPosition; */
            /* float xT, yT, zT; */
            /* bemcGeom->getXYZ(i_tower, xT, yT, zT); */
            /* TVector3 towLoc {xT, yT, zT}; */
            /* TVector3 relPos { towLoc - picoEvent->primaryVertex() }; */

            double ped  { bemc->pedestal(detector, i_tower) };
            /* double eta  { relPos.PseudoRapidity() }; */
            /* double phi  { relPos.Phi() }; */
            double eta  { towLoc.pseudoRapidity() };
            double calib { bemc->calib(detector,     i_tower) };
            double Et { (bTowHit->adc() - ped)*calib/TMath::CosH(eta)} ;
            /* cout << " Et("<<i_tower<<": " << Et << endl; */
            /* cout << "ped(" << i_tower <<") " << ped << " eta: " << eta << "  calib: " << calib << " Et: " << Et << endl; */
            /* if (Et > 0) cout << " Et("<<i_tower<<": " << Et << endl; */
            if (Et > 0.2) {
                ++fevent.ntow_gtp2;
                if (eta < 0) { ++fevent.ntow_gtp2_E; }
                else         { ++fevent.ntow_gtp2_W; }
            }
            if (Et > 4.0) {
                ++fevent.ntow_gt4;
                if (eta < 0) { ++fevent.ntow_gt4_E; }
                else         { ++fevent.ntow_gt4_W; }
            }

        }
    }

    fevent.bbcE  = 0;
    fevent.bbcES = 0;
    fevent.bbcEL = 0;

    fevent.bbcW  = 0;
    fevent.bbcWS = 0;
    fevent.bbcWL = 0;
    
    for (int i = 0; i < 16; ++i)  fevent.bbcES += picoEvent->bbcAdcEast(i);
    for (int i = 16; i < 24; ++i) fevent.bbcEL += picoEvent->bbcAdcEast(i);
    fevent.bbcE = fevent.bbcES + fevent.bbcEL;

    for (int i = 0; i < 16; ++i)  fevent.bbcWS += picoEvent->bbcAdcWest(i);
    for (int i = 16; i < 24; ++i) fevent.bbcWL += picoEvent->bbcAdcWest(i);
    fevent.bbcW = fevent.bbcWS + fevent.bbcWL;

    fevent.ZDCx = picoEvent->ZDCx();
    fevent.ZdcEastRate = picoEvent->zdcEastRate();
    fevent.ZdcWestRate = picoEvent->zdcWestRate();

    fevent.ZdcSumAdcEast = picoEvent->ZdcSumAdcEast();
    fevent.ZdcSumAdcWest = picoEvent->ZdcSumAdcWest();

    ftree->Fill();

    return kStOk;
};
