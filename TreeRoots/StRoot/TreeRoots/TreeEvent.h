#ifndef TreeEvent__h
#define TreeEvent__h
#include <iostream>
#include "TArrayF.h"

struct TreeEvent : public TObject {
	virtual ~TreeEvent() {} ;

    int          runId;      
    bool         b_rank; // true for > 0 neg for < 0
    bool         b_vz10; // is |vz|<10
    bool         b_vzVpd6; // is |vz-vpdvz|<6
    bool         b_all; // b_rank && b_vz10 && b_vzVpd6
    float        vz;
    float        vzVpd;

    int         nVpdHitsEast;
    int         nVpdHitsWest;

    int         nch_tof; 
    int         nch;  // 
    int         nchE;  // 
    int         nchW;  // 
    int         nch_ltp5;  // N_{charged tracks w/ |eta|<0.5}

    int         ntow_gtp2;  // # towers > 0.2 GeV total
    int         ntow_gtp2_E; // ditto for east
    int         ntow_gtp2_W; // ditto for west

    int         ntow_gt4;   // # towers > 4 GeV total
    int         ntow_gt4_E;  // ditto for east
    int         ntow_gt4_W;  // ditto for west


    float       bbcE;
    float       bbcES;
    float       bbcEL;

    float       bbcW;
    float       bbcWS;
    float       bbcWL;

    float       ZDCx;
    float       ZdcEastRate;
    float       ZdcWestRate;
    float       ZdcSumAdcEast;
    float       ZdcSumAdcWest;


    ClassDef(TreeEvent,1);
};

#endif
