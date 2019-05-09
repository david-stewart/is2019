#ifndef TreeEvent__h
#define TreeEvent__h
#include <iostream>
#include "TArrayF.h"

struct TreeEvent : public TObject {
	virtual ~TreeEvent() {} ;

    int          runId;      
    float        vz;
    float        vzVpd;

    /* int          eventId; */
    /* int          i_file_name; */
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


    float       bbc_E;
    float       bbc_ES;
    float       bbc_EL;

    float       bbc_W;
    float       bbc_WS;
    float       bbc_WL;

    float       ZDCx;
    float       ZdcEastRate;
    float       ZdcWestRate;
    float       ZdcSumAdcEast;
    float       ZdcSumAdcWest;


    ClassDef(TreeEvent,1);
};

#endif
