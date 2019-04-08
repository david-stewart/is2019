#include "TrackEmbedding.h"

#include "St_base/StMessMgr.h"
#include "StMiniMcEvent/StMiniMcPair.h"
#include "StMiniMcEvent/StTinyMcTrack.h"
#include "StMiniMcEvent/StContamPair.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StThreeVectorF.hh"

#include "TMath.h"
#include <map>

using namespace std;

ClassImp(TrackEmbedding)

Int_t TrackEmbedding::Init() { return 0; };

void TrackEmbedding::_get_EventInfo() {

    _zdcX    = _muEvent->runInfo().zdcCoincidenceRate();
    _z_vert  = _muDstMaker->muDst()->primaryVertex()->position().z();
    _refMult = _muEvent->refMult();
    _runId   = _muEvent->runId();

    StBbcTriggerDetector &bbc = _muEvent->bbcTriggerDetector();
	_bbcAdcES = 0;
	for (int i{0}; i<16; ++i) _bbcAdcES += bbc.adc(i);
    /* _bbcAdcES_orig = _bbcAdcES; */

    /* if (bbcAdj_rbr.has_run()) { */
        /* _bbcAdcES = bbcAdj_rbr(); */
    /* } else { */
        /* _log << " warning, using run " << _runId << " which isn't in the correction map." << endl; */
        /* _bbcAdcES = bbcAdj_all(); */
    /* } */
      
    // event QA cuts
    if (false) {
        _ranking  = _muDstMaker->muDst()->primaryVertex()->ranking();
        _vpdVz    = _muEvent->vpdVz();
        cout << " ranking: " << _ranking << " " << _vpdVz << endl;
        cout << " | " << _muEvent->vpdVz() << "| " << _zdcX << "|"<<endl;
    }
}

void TrackEmbedding::_fill_EventInfo() {
    _EmbTrack.zdcX    = _zdcX;
    _EmbTrack.z_vert  = _z_vert;
    _EmbTrack.refMult = _refMult;
	_EmbTrack.bbcAdcES = _bbcAdcES;
	_EmbTrack.runId = _runId;
    _EmbTrack.bbcAdcES_orig = _bbcAdcES_orig;
}

TrackEmbedding::TrackEmbedding(TChain* chain, StMuDstMaker* muDstMaker, 
                           ofstream& log, std::string outputName
) : 
    _minFit{20}, _maxDCA{3.0}, _minFitFrac{0.52}, 
    _mini_chain{chain}, _muDstMaker{muDstMaker}, _ievent{0},
    _log(log), _last_filename{""}
    /* bbcAdj_rbr(_bbcAdcES, _z_vert, _runId, "vz_run_corr/a_vz_fit.root"), */
    /* bbcAdj_all(_bbcAdcES, _z_vert, "vz_run_corr/a_vz_fit.root") */
{

    _log << "Starting init with chain:" << endl;
    if (_mini_chain == nullptr) {
        _log << "fatal: input chain does not exist." << endl;
        exit(2);
    }

    if (_mini_chain->GetBranch("StMiniMcEvent") == nullptr) {
        _log << "fatal: chain does not contain StMiniMcEvent branch" << endl;
        exit(2);
    }

    _nChainEvents = chain->GetEntries();
    _log << " reading " << _nChainEvents << " events." << endl;
    _event = new StMiniMcEvent;
    _mini_chain->SetBranchAddress("StMiniMcEvent", &_event);
    _mini_chain->GetEntry(0);
    
    _log << "Opening output file: " << outputName << ".root" << endl;
    _fout = new TFile( (outputName+".root").c_str(), "RECREATE") ;;

    _log << "Opening output tree" << endl;
    _tree = new TTree("tree","Tree of embedding tracks");
    _tree->Branch("EmcTrack", &_EmbTrack);
};

TrackEmbedding::~TrackEmbedding() { };

Int_t TrackEmbedding::Make() {
    if (_event == nullptr) {
        _log << "StMiniMcEvent Branch not loaded properly: exiting run loop" 
              << endl;
        return kStFatal;
    }

    if (_ievent >= _nChainEvents) { 
        _log << " i_event("<<_ievent<<") > _nChainEvents("<<_nChainEvents<<")" << endl;
        cout << " i_event("<<_ievent<<") > _nChainEvents("<<_nChainEvents<<")" << endl;
        return kStEOF;
    }
    _mini_chain->GetEntry(_ievent);
    _muEvent = _muDstMaker->muDst()->event();
    _ievent++;
    if (!_event) { 
        cout  << "Reached end of chain " << endl;
        _log << "Reached end of chain" << endl;
        return kStEOF;
    }

    if (_last_filename != _muDstMaker->GetFile()) {
        _last_filename = _muDstMaker->GetFile();
        _log << " Starting file: " << _muDstMaker->GetFile() << endl;
        cout << " Starting file: " << _muDstMaker->GetFile() << endl;
    }
    

    if (_muEvent->eventId() != _event->eventId()) {
        ostringstream msg;
        msg << "Fatal: The StMiniMc and StMuDst eventId's got out of sync" << endl
            << "MuDst  runId/eventId: " << _muEvent->runId()<<"/"<<_muEvent->eventId() << endl
            << "MiniMc runId/eventId: " << _event  ->runId()<<"/"<<_event  ->eventId() << endl;
        cout << msg.str() << endl;
        _log << msg.str() << endl;
        return kStFatal;
    }
    _runId = _event->runId();
    _get_EventInfo();

    // ----------------------
    // make the event cuts:
    // ranking > 0
    // z_Vert < 30
    // |z_vert-z_vpd| < 3 cm
    // ----------------------
    
    // Use the following blcok if cutting on events. 
    // However, I cannot get the MuDst's to read
    // the values; the primary vertex ranking is always -5, and the 
    // vpdVz is zero (or -1.58819e-12, which is the same thing).
    // I am not sure if I need them. It is possible that they are not needed
    // (nobody bins on them), and therefore the data is just not
    // put in for the embedding. In any case, I am going to ignore
    // these cuts for now.
    if (false) {
        if (_ranking < 0) { 
            _log << "Skip event " << _event->eventId() << " b/c ranking("
            << _ranking << ") < 0." << endl;
            return kStWarn;
        }
        if (TMath::Abs(_z_vert) > 30) {
            _log << "Skip event " << _event->eventId() << " b/c |z_vert|("
                << _z_vert << ") > 30cm" << endl;
            return kStWarn;
        }
        if (TMath::Abs(_z_vert - _vpdVz) > 3){
            _log << "Skip event " << _event->eventId() << "b/c |z_vert-vpdz|("
                <<"("<<_z_vert<<"-"<<_vpdVz<<") >3cm" << endl;
            return kStWarn;
        }
    }

    // ----------------------------------------------------------------
    // Not all MC tracks are matched with a reconstructed track (RC)
    // Loop through the RC tracks and input all of the matched values,
    // and keep track of the keys.
    // ----------------------------------------------------------------

    std::set<int> keys;
    TClonesArray* arrayMatch = _event->tracks(MATCHED);
    TIter iterMatch (arrayMatch);
    while (StMiniMcPair* pair { (StMiniMcPair*) iterMatch()}) {
        if (pair->parentGeantId()) continue; // will loop for those with parents
        _EmbTrack = EmbTrack{}; // clear out the event and get data
        /* cout << " pair key: " << pair->key() << endl; */
        keys.insert(pair->key());

        _EmbTrack.is_matched = true;
        _fill_EventInfo();

        _EmbTrack.ptMc = pair->ptMc();
        _EmbTrack.ptPr = pair->ptPr();

        _EmbTrack.phiMc = pair->phiMc();
        _EmbTrack.phiPr = pair->phiPr();

        _EmbTrack.etaMc = pair->etaMc();
        _EmbTrack.etaPr = pair->etaPr();

        _EmbTrack.nHitMc = pair->nHitMc();
        _EmbTrack.commonFrac = pair->commonFrac();

        /* cout << " pt " << _EmbTrack.ptMc << " " << _EmbTrack.ptPr << endl; */
        _tree->Fill();
    }

    // pick up the tracks without matching values
    TClonesArray* arrayMc = _event->tracks(MC);
    TIter iterMc (arrayMc);
    while (StTinyMcTrack* track { (StTinyMcTrack*) iterMc() }) {
        if (track->parentGeantId()) continue; // will loop for those with parents
        if (keys.count(track->key())) continue;
        _EmbTrack = EmbTrack{}; // clear out the event and get data
        _fill_EventInfo();
        _EmbTrack.ptMc   = track->ptMc();
        _EmbTrack.phiMc  = track->phiMc();
        _EmbTrack.etaMc  = track->etaMc();
        _EmbTrack.nHitMc = track->nHitMc();
        _tree->Fill();
    }
    return kStOK;
}

Int_t TrackEmbedding::Finish() {
    _log << " Finished running. Generated " << _ievent << " events." << endl;
    /* _tree->Write(); */
    _fout->Write();
    _fout->Close();
  /* if (_fout == nullptr) { */
    /* _fout = new TFile("embedding_tree.root", "RECREATE"); */
  /* } */
    /* _fout->Close(); */
  return kStOk;
}
