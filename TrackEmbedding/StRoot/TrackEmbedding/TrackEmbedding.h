#ifndef TrackEmbedding_h
#define TrackEmbedding_h

#include <string>
#include <vector>
#include <set>

#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"

#include "StMaker.h"
#include "StMiniMcEvent/StMiniMcEvent.h"
#include "TChain.h"
#include "TFile.h"
#include "EmbTrack.h"
/* #include "signal_corrections.h" */

class TrackEmbedding : public StMaker {
public:
  TrackEmbedding(TChain* mini_chain, StMuDstMaker* muDstMaker, 
         ofstream& log,  std::string outputName = "embedding_tree");
  
  ~TrackEmbedding();

  
  // loads a new chain
    bool LoadTree(TChain* chain);


    virtual Int_t Init();
    virtual int Make();
    int     Finish();
    void    _fill_EventInfo();
    void    _get_EventInfo();

  
private:
    float _zdcX;
    float _z_vert;
    int   _runId;
    float _bbcAdcES;
    float _bbcAdcES_orig;
    int   _refMult;

    float _ranking;
    float _vpdVz;

    int    _minFit;
    double _maxDCA;
    double _minFitFrac;
    TChain*  _mini_chain;
    StMuDstMaker* _muDstMaker; 
    StMuEvent*    _muEvent;

    TFile*   _fout;
    TTree*   _tree;
    long long int  _ievent;
    long long int  _nChainEvents;
    bool LoadEvent();
  
    StMiniMcEvent* _event;
    EmbTrack    _EmbTrack;
    ofstream& _log;
    string _last_filename;

    /* BbcAdcSignalAdjuster_RunByRun  bbcAdj_rbr; */
    /* BbcAdcSignalAdjuster_AllRunAvg bbcAdj_all; */
  
  ClassDef(TrackEmbedding,1)
};

#endif // STEFFICIENCYMAKERLITE__HH
