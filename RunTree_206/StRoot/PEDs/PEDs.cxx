//
// Includes code from (1) Joel Mazer from https://github.com/joelmazer/star-jetframework in July 2018
//                    (2) Tong Liu from ARCAS, also July 2018
//
#include "PEDs.h"
#include <iostream>
#include <algorithm>
#include "TProfile2D.h"
#include <fstream>
/* #include <TVector3.h> */
#define zmin -30
#define zmax 30


ClassImp(PEDs)
    //
//-----------------------------------------------------------------------------
PEDs::PEDs(
        const char     *_name,
        StPicoDstMaker *_picoMaker,
        const char     *_outName,
        const char     *_fUpdateStatus_list,
        St_db_Maker    *_starDb ,
        const char     *_index_runId,
        const char*     fbad_tower_file
) : StMaker(_name), 
    mPicoDstMaker(_picoMaker),
    mOutName(_outName),
    status_update{_fUpdateStatus_list, &runId},
    starDb(_starDb),
    ncalls{0},
    emcPosition{new StEmcPosition()},
    bemcGeom{StEmcGeom::getEmcGeom("bemc")},
    max_bbc_val{0},
    min_bbc_val{0},
    old_runId{0},
    p0{0},p1{0},p2{0}
{ 
    ifstream file;
    file.open(fbad_tower_file);
    if (!file.is_open()) {
        fbad_tower_list = vector<int>{};
    } else {
        string line;
        while (getline(file,line)) {
            line.append(" ");
            stringstream words(line);
            TString word;
            while (words >> word) {
                if (word.BeginsWith("//") || word.BeginsWith("#")) break;
                fbad_tower_list.push_back(word.Atoi());
            }
        }
        std::sort(fbad_tower_list.begin(), fbad_tower_list.end());
    }
    file.close();
    bbc_edges = get_bbc_edges();
    flog.open(TString::Format("%s.log",mOutName.Data()).Data());
    flog << "Making PEDs object" << endl;
    flog << "Using status file " << _fUpdateStatus_list << endl;
    /* flog << " Initializing log " << endl; */
    bemc = new StBemcTables();

    // fill in the index_runId
    flog << "Reading in index map " << _index_runId << endl;
    ifstream ifs(_index_runId);
    if (!ifs.is_open()) {
        cout << "fatal error. Could not open file " << _index_runId << endl;
        exit(2);
    }
    int id;
    /* int runId; */
    while (ifs >> id >> runId) {
        index_runId[runId] = id;
    }
    flog << "Finished reading in index map " << _index_runId << endl;
    ifs.close();
    /* flog << "Reading in updated status table list from " << fUpdateStatus_list << endl; */
};

//----------------------------------------------------------------------------- 
PEDs::~PEDs()
{ /*  */ };

//----------------------------------------------------------------------------- 
Int_t PEDs::Init() {
    flog << "Opening output file " << mOutName.Data() << ".root" << endl;
    fout = new TFile(TString::Format("%s.root",mOutName.Data()).Data(),"RECREATE") ;

    inout = new TH1D("bbc_inout",
            "if bbc val in/out of range;-1 small, 0 ok, 1 large;N_{events}",
            2, -1.5, 1.5);

    hg_Etmax = new TH1D("hg_Etmax","max Et value in each collision;Et;N.",
            250, 0, 25);

    bbc_dist = new TH1D("bbc_distr","bbc distribution per event;bbc;N_{events}",
            500, -6853.43, 69119.);

    /* p2_Et = new TProfile2D("Et", */
    /*         "mean(calib (ADC_{tower} - ADC_{pedestal})/cosh(#eta));index_{runId};i_{Tower} (1-4800)", */
    /*         700,0.5,700.5,4800,0.5,4800.5); */

    /* p2_peds       = new TProfile2D("peds", */
    /*         "pedestal (first recorded in run);index_{runId};i_{Tower} (1-4800)", */
    /*         700,0.5,700.5,4800,0.5,4800.5); */

    /* nevents_perTow = new TH2D("nEvents_perTow", */
    /*         "Number of nevents per tower in good status;index_{runId};i_{Tower}", */
    /*         700,0.5,700.5,4800,0.5,4800.5); */

    nevents_perRun = new TH1D("nEvents_perRun",
            "Number of good ssdmb events per run;index_{runId};N_{events}",
            700,0.5,700.5);

    for (int i : {0, 4, 8, 12}){
        pt_all[i] = vector<TH1D*>{};
        pt_max[i] = vector<TH1D*>{};
        nevents[i] = new TH1D(Form("nEv%i",i),Form("Number of events for Et>%i;bbc_bin;N_{events}",i),10,-0.5,9.5);
        for (int j{0};j<10;++j) {
            pt_all[i].push_back(new TH1D(Form("ptAll%i_bin%i",i,j),Form("pT tracks in events Et>%i, bbc_bin(%i);p_{T};N_{events}",i,j),
                        250,0.,25.));
            pt_max[i].push_back(new TH1D(Form("ptMax%i_bin%i",i,j),Form("Max pT tracks in events Et>%i, bbc_bin(%i);p_{T};N_{events}",i,j),
                        250,0.,25.));
        }
    }
    r_map = return_map();

    // open up the ttree
    /* ftree = new TTree("tree","Tree of Suspect StPicoBTowHit objects"); */
    /* ftree->Branch("emchit", &b_EmcHit); */
    /* ftree->Branch("event",  &fevent); */
  return kStOK;
}

//----------------------------------------------------------------------------- 
Int_t PEDs::Finish() {
    fout->Write();
    fout->Close();
    flog << "Closing log at normal end of program." << endl;
    flog.close();
    delete emcPosition;
  return kStOK;
}
    
//----------------------------------------------------------------------------- 
void PEDs::Clear(Option_t *opt) { }

//----------------------------------------------------------------------------- 
Int_t PEDs::Make() {
    /* cout << "::a1 " << endl; */
    ++ncalls;
    if (!(ncalls % 10000)) flog << " called " << ncalls/10000 << "K times" << endl;
    if(!mPicoDstMaker) {
        LOG_WARN << " No PicoDstMaker! Skip! " << endm;
        return kStWarn;
    }
    /* cout << "::a2 " << endl; */

    mPicoDst = mPicoDstMaker->picoDst();
    if(!mPicoDst) {
        LOG_WARN << " No PicoDst! Skip! " << endm;
        return kStWarn;
    }
    StPicoEvent* mevent = mPicoDst->event();
    /* cout << "::a3 " << endl; */

    float rank = mevent->ranking();
    int sig = rank > 0 ? +1 : -1;
    if (sig < 0) return kStOK;
    vz = mevent->primaryVertex().z();

    bemc->loadTables(starDb);
    if (TMath::Abs(vz) > 10) return kStOk;
	/* cout << " vz " <<  vz << " " << TMath::Abs(vz - mevent->vzVpd()) << endl; */
    if (TMath::Abs(vz - mevent->vzVpd())>3.0) return kStOk;

    runId = mevent->runId(); // there is a pointer to runId in status_update
    int index;
    if (index_runId.count(runId)) {
        index = index_runId[runId];
    } else {
        flog << "Error, did not find run " << runId << " in the runId index" << endl;
        return kStWarn;
    } 

    nevents_perRun->Fill(index);

    /* cout << "a9" << endl; */
    /* cout << status_update.is_good(10) << endl; */
    /* cout << "a10" << endl; */
    /* StEmcPosition *mPosition = new StEmcPosition(); */

    // get the position of tower 10
    /* StEmcPosition *mPosition = new StEmcPosition(); */

    // Thanks to J. Mazer, see https://github.com/joelmazer/star-jetframework/ StEmcPosition2.cxx

    //for pedestal values, only read the first 
    bool new_run { runs_read.find(runId) == runs_read.end() };
    if (new_run) flog << " Reading data from runId:"<<runId << "  index: " << index << endl;
    runs_read.insert(runId);

    int detector {1};
    /* cout << "i_tower calib(0): " <<  bemc->calib(detector, 2, 0) */ 
         /* << " (1) "  <<              bemc->calib(detector, 2, 1) */
         /* << " (2) "  <<              bemc->calib(detector, 2, 2) */ 
         /* << "   |   " <<             bemc->calib(detector, 2) << endl; */
	/* cout <<  bemc->calib(detector,     15) ; */

    float maxEt{0};
    for (unsigned int i=0 ; i < mPicoDst->numberOfEmcTriggers(); ++i){
        StPicoEmcTrigger* emcTrig = mPicoDst->emcTrigger(i);
        if (! emcTrig->isHT1()) continue;
        int   trigId  = emcTrig->id();
        if (trigId < 1) { continue; }
        if (std::binary_search(fbad_tower_list.begin(), 
            fbad_tower_list.end(), trigId)) continue;

        StPicoBTowHit* bTowHit = mPicoDst->btowHit(trigId-1); 
        StEmcPosition *mPosition = new StEmcPosition();
        StThreeVectorF towLoc = mPosition->getPosFromVertex( 
                mevent->primaryVertex(), trigId);

        /* float xT, yT, zT; */
        /* bemcGeom->getXYZ(trigId, xT, yT, zT); */
        /* TVector3 relPos { TVector3{xT, yT, zT} - mevent->primaryVertex() }; */

        double eta  { towLoc.pseudoRapidity() };
        int i_tower = trigId;
        double ped  { bemc->pedestal(detector, i_tower) };
        double calib { bemc->calib(detector,     i_tower) };
/* cout << "calib " << calib << endl; */
        double Et   { (bTowHit->adc() - ped)*calib/TMath::CosH(eta)} ;
        /* cout << "::a9 " << Et << endl; */
        if (Et > maxEt) maxEt = Et;
        if (Et > 30) return kStOk;
        if (Et < 0.2) {
            continue;
        }
    }
    if (maxEt < 4.) return kStOk;

    //--------------------
    // LION lion 
    // get the bbc values
    float bbcAdcES = mevent->bbcAdcEast(0);
    for (int i = 1; i < 16; ++i) bbcAdcES += mevent->bbcAdcEast(i);
    float bbc_cor = get_bbc_correction(bbcAdcES);
    bbc_dist -> Fill(bbc_cor);
    int i_bbc = which_bbc(bbc_cor);


    // get which group it is in, 0, 4, 8, 12
    hg_Etmax->Fill(maxEt);
    bool in4 { maxEt > 4. };
    bool in8 { (maxEt > 8.) && (maxEt < 12.) };
    bool in12{ (maxEt > 12) };

    nevents[0]->Fill(i_bbc);
    if (in4) nevents[4]->Fill(i_bbc); 
    if (in8) nevents[8]->Fill(i_bbc); 
    if (in12) nevents[12]->Fill(i_bbc); 


    // now loop through all of the primary tracks
    /* cout << "a11" << endl; */
    float max_track_pt = 0;
    float sum_track_pt = 0;
    int ntracks = 0;
    for (unsigned int i = 0; i < mPicoDst->numberOfTracks(); ++i){
        StPicoTrack* track = static_cast<StPicoTrack*>(mPicoDst->track(i));
        if (!track->isPrimary()) continue;

        StThreeVectorF Ptrack = track->pMom();
        /* TVector3 Ptrack = track->pMom(); */
        float pt  = Ptrack.perp();

        if (pt > 30)   return kStOK; // cut out this event
        if (pt < 0.2)  continue;
        // thanks to joel mazer
        float dca = (track->dcaPoint() - mevent->primaryVertex()).mag();
        /* float dca = (track->origin()).Mag(); */
        /* cout << "::: dca: " << dca << endl; */
        if (dca > 3.0)  continue;
        float eta = Ptrack.pseudoRapidity();
        if (TMath::Abs(eta)  >= 1.0) continue;

        float nhit_ratio = ((float)track->nHitsFit()) / (float)track->nHitsMax();
        if (nhit_ratio <= 0.52) continue;
        if (track->bTofPidTraitsIndex() != -1) continue;

        if (pt > max_track_pt) max_track_pt = pt;
        sum_track_pt += pt;

        pt_all[0][i_bbc]->Fill(pt);
        if (in4) pt_all[4][i_bbc]->Fill(pt);
        if (in8) pt_all[8][i_bbc]->Fill(pt);
        if (in12) pt_all[12][i_bbc]->Fill(pt);
        ++ntracks;
    }
    pt_max[0][i_bbc]->Fill(max_track_pt);
    if (in4) pt_max[4][i_bbc]->Fill(max_track_pt);
    if (in8) pt_max[8][i_bbc]->Fill(max_track_pt);
    if (in12) pt_max[12][i_bbc]->Fill(max_track_pt);
        
    /* cout << max_track_pt << " " << sum_track_pt << " " << n_tracks << endl; */
    return kStOk;
}
