//   New objective: kj
//
// Includes code from (1) Joel Mazer from https://github.com/joelmazer/star-jetframework in July 2018
//                    (2) Tong Liu from ARCAS, also July 2018
//
#include "RunData.h"
#include <iostream>
#include "TChain.h"
#include <fstream>
#include "TNtuple.h"
#include "TF1.h"
#include "fastjet/config.h"
#include "fastjet/PseudoJet.hh"

#define  MaxNTracks 150
#define  MaxNTowers 50
using namespace fastjet;

struct MyInfoTracks : public PseudoJet::UserInfoBase {
    MyInfoTracks(bool in_isTOF, bool in_isBEMC) 
        : _isTOF{in_isTOF}, _isBEMC{in_isBEMC} {};
    bool isTOF() const  { return _isTOF; };
    bool isBEMC() const { return _isBEMC; };
    bool _isTOF;
    bool _isBEMC;
};


ClassImp(RunData)
//-----------------------------------------------------------------------------
RunData::RunData(
        const char*     _name,
        const char*     _outName,
        ofstream&       _log,
        StPicoDstMaker* _picoMaker,
        const char*     _bad_run_list,
        const char*     _bad_tower_list,
        const char*     _runQA_summary,
        St_db_Maker*    _starDb,
        int             _triggerId=500001
    )
  : StMaker         (_name), 
    log             (_log),
    picoDstMaker    {_picoMaker},
    starDb          {_starDb},
    bemcGeom        { StEmcGeom::getEmcGeom("bemc") },
    triggerId       { _triggerId },
    b_tracks        { "myTrack", MaxNTracks },
    b_towers        { "myTower", MaxNTowers  }
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
    ftree->Branch("track", &b_tracks);
    ftree->Branch("tower", &b_towers);

};

//----------------------------------------------------------------------------- 
RunData::~RunData()
{ };

//----------------------------------------------------------------------------- 
Int_t RunData::Init() {
    return kStOK;
};

//----------------------------------------------------------------------------- 
Int_t RunData::Finish() {
    log << " Finished with RunData object " << endl;
    fout_root->Write();
    fout_root->Close();
    return kStOK;
};
    
//----------------------------------------------------------------------------- 
void RunData::Clear(Option_t *opt) { };
//----------------------------------------------------------------------------- 
Int_t RunData::Make() {
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

    if (!picoEvent->isTrigger(triggerId)) return kStOk;

    b_tracks.Clear();
    b_towers.Clear();

    fevent.runId = picoEvent->runId() ;
    fevent.ZDCx = picoEvent->ZDCx();

    // cut on bad_run_list
    if (std::find(bad_run_list.begin(), bad_run_list.end(), fevent.runId)
            != bad_run_list.end()) return kStOK;

    // cut on vz
    fevent.vz = picoEvent->primaryVertex().z();
    fevent.vzVpd = picoEvent->vzVpd();

    fevent.b_rank = (picoEvent->ranking() > 0);
    fevent.b_vz10 = (TMath::Abs(fevent.vz)<10);
    fevent.b_vzVpd6 = TMath::Abs(fevent.vz-fevent.vzVpd) < 6;
    fevent.b_all = (fevent.b_rank && fevent.b_vz10 && fevent.b_vzVpd6);
    /* if (TMath::Abs(fevent.vz) > 10) return kStOK; */

    // eventually cut on the vz-vpd, but not for now...
    /* double d_vzVpd { picoEvent->vzVpd() }; */
    /* double d_delta_vzVpd { d_vz - d_vzVpd } ; */
    /* if (TMath::Abs("d_vzVpd") */

    vector<PseudoJet> particles;
    /* int nch{0}; */
    fevent.nch = 0;
    fevent.n_isTOF  = 0;
    fevent.n_isBEMC = 0;
    for (unsigned int i = 0; i < picoDst->numberOfTracks(); ++i) {
        StPicoTrack* track = picoDst->track(i);
        if (!track || !(track->isPrimary()))   continue;
        /* StPicoTrack* track = static_cast<StPicoTrack*>(picoDst->track(i)); */
        if (!track->isPrimary()) continue;
        /* TVector3 Ptrack { track->pMom() }; */
        /* double dca { (track->origin() - picoEvent->primaryVertex()).Mag() }; */
        /* double eta {Ptrack.Eta()}; */
        /* double phi {Ptrack.Phi()}; */
        StThreeVectorF Ptrack = track->pMom();
        float dca = (track->dcaPoint() - picoEvent->primaryVertex()).mag();
        double eta  { Ptrack.pseudoRapidity() };
        double phi  { Ptrack.phi() };
        if (dca > 3.0) continue;
        if (TMath::Abs(eta)  >= 1.0) continue;
        float nhit_ratio = ((float)track->nHitsFit()) / (float)track->nHitsMax();
        if (nhit_ratio <= 0.52) continue;
        /* if (track->bTofPidTraitsIndex() != -1) continue; */
        /* double pt {Ptrack.Perp() }; */
        double pt {Ptrack.perp() };
        if (pt > 30.)  return kStOK;
        if (pt < 0.2) continue;

        particles.push_back (PseudoJet());
        unsigned int j = particles.size() - 1;
        particles[j].reset_PtYPhiM(pt, eta, phi);


        bool tofmatch  = (track->bTofPidTraitsIndex() != -1);
        bool bemcmatch = (track->bemcPidTraitsIndex() != -1);
        particles[j].set_user_info( new MyInfoTracks( tofmatch, bemcmatch) );
        if (tofmatch)  ++fevent.n_isTOF;
        if (bemcmatch) ++fevent.n_isBEMC;
        ++fevent.nch;
    }

    //loop through all good towers and get the hits with Et > 4
    const int detector{1};
    
    vector<PseudoJet> towers;
    bemc->loadTables(starDb);
    for (int i_tower = 1; i_tower < 4801; ++i_tower){
    /* for (int i_tower = 1; i_tower < 101; ++i_tower){ */
        if (std::find(bad_tower_list.begin(), 
                      bad_tower_list.end(),i_tower) != bad_tower_list.end())
        { continue; }

        if ( 
            bemc->status(detector, i_tower)             == 1
         && bemc->status(detector, i_tower, "pedestal") == 1
         && bemc->status(detector, i_tower, "gain")     == 1
         && bemc->status(detector, i_tower, "calib")    == 1
         ) {
            StPicoBTowHit* bTowHit = picoDst->btowHit(i_tower-1); 
            if (!bTowHit) { 
                continue;
            }

            /* StEmcPosition *mPosition = new StEmcPosition(); */
            /* StThreeVectorF towLoc = mPosition->getPosFromVertex( */ 
            /*         picoEvent->primaryVertex(), i_tower); */
            StThreeVectorF towLoc = StEmcPosition().getPosFromVertex(picoEvent->primaryVertex(), i_tower);
            /* float xT, yT, zT; */
            /* bemcGeom->getXYZ(i_tower, xT, yT, zT); */
            /* TVector3 towLoc {xT, yT, zT}; */
            /* TVector3 relPos { towLoc - picoEvent->primaryVertex() }; */

            double ped  { bemc->pedestal(detector, i_tower) };
            /* double eta  { relPos.PseudoRapidity() }; */
            /* double phi  { relPos.Phi() }; */
            double calib { bemc->calib(detector,     i_tower) };
            double eta  { towLoc.pseudoRapidity() };
            double Et { (bTowHit->adc() - ped)*calib/TMath::CosH(eta)} ;
            if (Et > 4.) {
                towers.push_back (PseudoJet());
                double phi  { towLoc.phi() };
                unsigned int j = towers.size() - 1;
                towers[j].reset_PtYPhiM(Et, eta, phi);
            }
        }
    }


    // Now fill the tracks and towers
    particles = sorted_by_pt(particles);
    towers    = sorted_by_pt(towers);

    /* fevent.nch = particles.size(); */
    unsigned int nrec = (particles.size() > MaxNTracks) ? MaxNTracks : particles.size();
    for (unsigned int i{0}; i<nrec; ++i){
        myTrack* track = (myTrack*) b_tracks.ConstructedAt(i);
        track->pt = particles[i].pt();
        track->phi = particles[i].phi();
        track->eta = particles[i].eta();
        track->isTOF  = particles[i].user_info<MyInfoTracks>().isTOF();
        track->isBEMC = particles[i].user_info<MyInfoTracks>().isBEMC();
    }

    /* fevent.ntow = towers.size(); */
    nrec = (towers.size() > MaxNTowers) ? MaxNTowers : towers.size();
    for (unsigned int i{0}; i<nrec; ++i){
        myTower* tower = (myTower*) b_towers.ConstructedAt(i);
        tower->Et  = towers[i].Et();
        tower->phi = towers[i].phi();
        tower->eta = towers[i].eta();
    }

    fevent.bbcES = 0;
    for (int i = 0; i < 16; ++i) fevent.bbcES += picoEvent->bbcAdcEast(i);

    fevent.bbcE = fevent.bbcES;
    for (int i = 16; i < 24; ++i) fevent.bbcE += picoEvent->bbcAdcEast(i);
    
    ftree->Fill();

    return kStOk;
};
