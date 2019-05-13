//   New objective: kj
//
// Includes code from (1) Joel Mazer from https://github.com/joelmazer/star-jetframework in July 2018
//                    (2) Tong Liu from ARCAS, also July 2018
//
#include "RunQA.h"
#include <iostream>
#include "TChain.h"
#include <fstream>

ClassImp(RunQA)
//-----------------------------------------------------------------------------
RunQA::RunQA(
        const char*     _name,
        const char*     _outName,
        ofstream&        _log,
        StPicoDstMaker* _picoMaker,
        const char*     _bad_run_list,
        const char*     _bad_tower_list,
        St_db_Maker*    _starDb,
        vector<int>     runIds,
        vector<int>     runDurations,
        vector<string>  beginTimes
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

    // Generate the tree with the run average information
    TTree *tree = new TTree("run_data","Run id, run length (in secs), time start");
    int i_runId;
    int i_runDuration;
    string st_startTime;
    tree->Branch("runId",       &i_runId);
    tree->Branch("length_secs", &i_runDuration);
    tree->Branch("start_time",  &st_startTime);

    for (unsigned int i{0}; i<runIds.size(); ++i){
        i_runId = runIds[i];
        i_runDuration = runDurations[i];
        st_startTime = beginTimes[i];
        tree->Fill();
    }

    vz       = new TH1D("vz","vz, all events;vz;N_{events}",240,-30,30);
    vzVpd    = new TH1D("vzVpd","vzVpd, all events;vzVpd;N_{events}",240,-30,30);
    delta_vz_vzVpd  = new TH1D("delta_vz_vzVpd","vz - vzVpd, all events;vzVpd;N_{events}",480,-60,60);
    vz_vzVpd = new TH2D("vz_vzVpd","All events;vz;vzVpd",240,-30,30,240,-30,30);
    et_gt4  = new TProfile("et_gt4","All events, tower E_{T}>4 GeV;tower Id;E_{T} (for values > 4GeV)",4800,0.5, 4800.5);
};

//----------------------------------------------------------------------------- 
RunQA::~RunQA()
{ };

//----------------------------------------------------------------------------- 
Int_t RunQA::Init() {
    return kStOK;
};

//----------------------------------------------------------------------------- 
Int_t RunQA::Finish() {
    log << " Finished with RunQA object " << endl;
    fout_root->Write();
    fout_root->Close();
    return kStOK;
};
    
//----------------------------------------------------------------------------- 
void RunQA::Clear(Option_t *opt) { };

//----------------------------------------------------------------------------- 
Int_t RunQA::Make() {

    if(!picoDstMaker) {
        log << " No PicoDstMaker! Skip! " << endm;
        return kStWarn;
    }

    StPicoDst* picoDst = picoDstMaker->picoDst();
    if(!picoDst) {
        log << " No PicoDst! Skip! " << endl;
        return kStWarn;
    }

    StPicoEvent* picoEvent = picoDst->event();
    int runId{ picoEvent->runId() } ;

    /* log << " RunID " << runId << endl; */
    // add code to get the Vz of the events as well...
    // 2 bins per centimeter, go -30 to 30 (so 120 bins)
    // in the actual analysis I did -10 to 10k with 40 bins
    fout_root->cd();
    if (!p_means.count(runId)) {
        log << " p_means " << Form("means_%i",runId) << endl;
        p_means[runId] =  new TProfile(
        Form("means_%i",runId),Form(
        "Run %i means;0:zdcX 1:BBCe 2:BBCw 3:vz 4:vzVpd 5:vz-vpd 6:N_{global} 7:N_{ch} 8:meanPt 9:N_{Et>4GeV} 10:meanEt>4;vals",
        runId),
        11,-0.5,10.5);

        bbcVz[runId] = new TProfile(Form("bbcVz_%i",runId),
                Form("Run %i, bbcES(Vz);bbcES;Vz",runId),120,-30.,30.);

        /* bbcZDCx[runId] = new TProfile(Form("bbcZDCx_%i",runId), */
                /* Form("Run %i, bbcES(ZDCx);bbcES;ZDCx",runId), 400, 0., 50000.); */

        et_vals[runId] = new TH2D (
                Form("et_%i",runId), Form(
                "Run %i Tower Et Values;tower Id;Et",runId),
                4800, 0.5, 4800.5,
                42,4.,25.);
        /* p_vals = new TH2D( Form("vals_ */
    }
    TProfile *prof = p_means[runId];
    TH2D     *et_vals_hg2 = et_vals[runId];

    if (!h_means.count(runId)){
        log << " hard_cuts: " << Form("hard_cuts_%i",runId) << endl;
        h_means[runId] = new TH1D(
        Form("hard_cuts_%i",runId),Form(
        "Run %i totals;0:rnk<0 1:rnk>0 2:pT>30 3:500206 4:!500206;Totals per event",runId),
        5,-0.5,4.5);
    }
    TH1D*  hg = h_means[runId];

    // Check the three cuts (trigger, rankings, and pt>30) that would make a bad events
    
    bool cut_event = false;
    float ranking { picoEvent->ranking() };
    if (ranking<0) { 
        hg->Fill(0); 
        cut_event = true; 
    } else {
        hg->Fill(1);
    }

    if (picoEvent->isTrigger(500001)) {
        hg->Fill(3);
    } else {
        /* log << " Not 500001: but Is: "; */
        /* vector<unsigned int> ids = picoEvent->triggerIds(); */
        /* int j = 0; */
        /* for (auto i : ids) cout << "  " << j++ <<":  " << i << endl; */

        hg->Fill(4);
        cut_event = true;
    }

    //loop through each of the tracks to see if there is a greater than 15 GeV track present
    bool has_gt30 { false };
    int nch{0};
    double sumpt;
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
        ++nch;
        sumpt += pt;
    }
    if (has_gt30) {
        cut_event = true;
        hg->Fill(2);
    }
    if (cut_event) return kStOk;

    //loop through all good towers and get the hits with Et > 4
    const int detector{1};
    int    nETgt4  {0};
    double sumETgt4{0};
    
    bemc->loadTables(starDb);
    for (int i_tower = 1; i_tower < 4801; ++i_tower){
        if (std::find(bad_tower_list.begin(), 
                      bad_tower_list.end(),i_tower) != bad_tower_list.end())
        { continue; }

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
            TVector3 towLoc {xT, yT, zT};
            TVector3 relPos { towLoc - picoEvent->primaryVertex() };

            double ped  { bemc->pedestal(detector, i_tower) };
            double eta  { relPos.PseudoRapidity() };
            double calib { bemc->calib(detector,     i_tower) };
            double Et { (bTowHit->adc() - ped)*calib/TMath::CosH(eta)} ;
            /* cout << "ped(" << i_tower <<") " << ped << " eta: " << eta << "  calib: " << calib << " Et: " << Et << endl; */
            if (Et > 4.) {
                et_vals_hg2->Fill(i_tower,Et);
                et_gt4->Fill(i_tower,Et);
                ++nETgt4;
                sumETgt4 += Et;
            }
        }
    }


    // Now fill the TProfile business
    int bbcE{0};
    for (int i = 1; i < 16; ++i) bbcE += picoEvent->bbcAdcEast(i);

    int bbcW{0};
    for (int i = 1; i < 16; ++i) bbcW += picoEvent->bbcAdcWest(i);

    double d_vz{ picoEvent->primaryVertex().z() };
    double d_vzVpd { picoEvent->vzVpd() };
    double d_delta_vzVpd { d_vz - d_vzVpd } ;
    
    vz->Fill(d_vz);
    vzVpd->Fill(d_vzVpd);
    delta_vz_vzVpd->Fill(d_delta_vzVpd);
    vz_vzVpd->Fill(d_vz,d_vzVpd);

    prof->Fill(0., picoEvent->ZDCx());
    prof->Fill(1, bbcE);
    prof->Fill(2, bbcW);
    prof->Fill(3., d_vz);
    prof->Fill(4., d_vzVpd);
    prof->Fill(5., d_delta_vzVpd);
    prof->Fill(6., picoEvent->numberOfGlobalTracks());
    prof->Fill(7., nch);

    if (nch > 0) prof->Fill(8., sumpt/nch);
    prof->Fill(9., nETgt4);
    if (nETgt4 > 0) prof->Fill(10., sumETgt4/nETgt4);

    bbcVz[runId]->Fill(d_vz, bbcE);

    return kStOk;
};
