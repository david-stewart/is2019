//   New objective: kj
//
// Includes code from (1) Joel Mazer from https://github.com/joelmazer/star-jetframework in July 2018
//                    (2) Tong Liu from ARCAS, also July 2018
//
#include "EA_metrics.h"
#include <iostream>
#include "TChain.h"
#include <fstream>

ClassImp(EA_metrics)
//-----------------------------------------------------------------------------
EA_metrics::EA_metrics(
        const char*     _name,
        const char*     _outName,
        ofstream&        _log,
        StPicoDstMaker* _picoMaker,
        const char*     _bad_run_list,
        const char*     _bad_tower_list,
        St_db_Maker*    _starDb
    )
  : StMaker         (_name), 
    log             (_log),
    picoDstMaker    {_picoMaker},
    starDb          {_starDb},
    bemcGeom        { StEmcGeom::getEmcGeom("bemc") }
{
    fout_root = new TFile(Form("%s.root",_outName),"RECREATE");
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
    tree = new TTree("EA_metrics","metrics for EA in ssd_mb events");
    tree->Branch("EA_metric", &metrics);
};

//----------------------------------------------------------------------------- 
EA_metrics::~EA_metrics()
{ };

//----------------------------------------------------------------------------- 
Int_t EA_metrics::Init() {
    return kStOK;
};

//----------------------------------------------------------------------------- 
Int_t EA_metrics::Finish() {
    log << " Finished with EA_metrics object " << endl;
    fout_root->Write();
    fout_root->Close();
    return kStOK;
};
    
//----------------------------------------------------------------------------- 
void EA_metrics::Clear(Option_t *opt) { };

//----------------------------------------------------------------------------- 
Int_t EA_metrics::Make() {

    if(!picoDstMaker) {
        log << " No PicoDstMaker! Skip! " << endm;
        return kStWarn;
    }

    StPicoDst* picoDst = picoDstMaker->picoDst();
    if(!picoDst) {
        log << " No PicoDst! Skip! " << endl;
        return kStWarn;
    }

    // cut on vz
    StPicoEvent* picoEvent = picoDst->event();
    int runId{ picoEvent->runId() } ;

    if (std::binary_search(bad_tower_list.begin(), bad_tower_list.end(), runId))
        return kStOk;

    /* log << " RunID " << runId << endl; */
    fout_root->cd();
    
    // Check the three cuts (trigger, rankings, and pt>30) that would make a bad events
    bool cut_event = false;
    float ranking { picoEvent->ranking() };
    if (ranking<0) return kStOk;
    if (!picoEvent->isTrigger(500001)) return kStOk;

    int bbcE{0};
    for (int i = 1;  i < 16; ++i) bbcE += picoEvent->bbcAdcEast(i);
    metrics.bbcAdcES = bbcE;
    for (int i = 16; i < 24; ++i) bbcE += picoEvent->bbcAdcEast(i);
    metrics.bbcAdcEL = bbcE;

    int bbcW{0};
    for (int i = 1; i < 16; ++i) bbcW += picoEvent->bbcAdcEast(i);
    metrics.bbcAdcWS = bbcW;
    for (int i = 16; i < 24; ++i) bbcW += picoEvent->bbcAdcEast(i);
    metrics.bbcAdcWL = bbcW;

    metrics.zdcX = picoEvent->ZDCx();
    metrics.zdcSumAdcEast = picoEvent->ZdcSumAdcEast();
    metrics.zdcSumAdcWest = picoEvent->ZdcSumAdcWest();

    metrics.vpdE = picoEvent->nVpdHitsEast();
    metrics.vpdW = picoEvent->nVpdHitsWest();

    metrics.vz = picoEvent->primaryVertex().z();
    metrics.vzVpd = picoEvent->vzVpd();

    metrics.nch = 0;
    metrics.sumpt = 0;
    

    //loop through each of the tracks to see if there is a greater than 15 GeV track present
    bool has_gt30 { false };
    for (unsigned int i = 0; i < picoDst->numberOfTracks(); ++i){
        StPicoTrack* track = static_cast<StPicoTrack*>(picoDst->track(i));
        if (!track->isPrimary()) continue;
        TVector3 Ptrack { track->pMom() };
        double dca { (track->origin() - picoEvent->primaryVertex()).Mag() };
        if (dca > 3.0) continue;
        if (TMath::Abs(Ptrack.Eta())  >= 1.0) continue;
        float nhit_ratio = ((float)track->nHitsFit()) / (float)track->nHitsMax();
        if (nhit_ratio <= 0.52) continue;
        if (track->bTofPidTraitsIndex() != -1) continue;

        double pt {Ptrack.Perp() };
        if (pt > 30.) {
            has_gt30 = true;
            break;
        }
        if (pt < 0.2) continue;
        ++metrics.nch;
        metrics.sumpt += pt;
    }
    if (has_gt30) {
        cut_event = true;
    }
    if (cut_event) return kStOk;

    //loop through all good towers and get the hits with Et > 4
    const int detector{1};
    metrics.sumETgtp2 = 0;
    metrics.ntow_gt_p2GeV = 0;
    metrics.ntow_gt_4GeV = 0;
    metrics.ntow_gt_8GeV = 0;
    metrics.ntow_gt_12GeV = 0;
    
    bemc->loadTables(starDb);
    for (int i_tower = 1; i_tower < 4801; ++i_tower){
        if (std::binary_search(bad_tower_list.begin(), bad_tower_list.end(),i_tower)) continue;
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
            float xT, yT, zT;
            bemcGeom->getXYZ(i_tower, xT, yT, zT);
            TVector3 towLow {xT, yT, zT};
            TVector3 relPos { towLow - picoEvent->primaryVertex() };

            double ped  { bemc->pedestal(detector, i_tower) };
            double eta  { relPos.PseudoRapidity() };
            double calib { bemc->calib(detector,     i_tower) };
            double Et { (bTowHit->adc() - ped)*calib/TMath::CosH(eta)} ;
            if (Et < 0.2) continue;
            ++metrics.ntow_gt_p2GeV;
            metrics.sumETgtp2 += Et;
            if (Et > 4.) ++metrics.ntow_gt_4GeV;
            if (Et > 8.) ++metrics.ntow_gt_8GeV;
            if (Et > 12.) ++metrics.ntow_gt_12GeV;
        }
    }
    tree->Fill();

    return kStOk;
};
