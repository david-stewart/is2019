#ifndef EA_bmetric_h
#define EA_bmetric_h
#include <iostream>
#include <vector>
using namespace std;

/** @file    tree
    @author  David Stewart
    @version draft 0.2
    @brief   simple tree into which to sort physics runs
    @details put in hadron-triggered jet events
    @date    2018.03.15
*/

using namespace std;

// for now, do not store jet tracks, just jets themselves.

/* struct ZeTrack : public TObject { */
/* 	virtual ~ZeTrack() { }; */
/*     double t_phi; */
/*     double t_eta; */
/*     double t_pt; */
/*     /1* bool  isCentral; // is central *1/ */
/*     /1* void check_isCentral() { isCentral = (eta <= 0.5); } *1/ */
/* 	ClassDef(ZeTrack,1); */
/* }; */

struct EA_bmetric  : public TObject {
    // Et is the leading tower's Et
    // pt is the leading tracks' pt
    /* PreCut() {}; */
    double bbcAdcES; //
    double bbcAdcWS; //
    double bbcAdcEL; //
    double bbcAdcWL; //
    double zdcSumAdcEast; //
    double zdcSumAdcWest; //
    int vpdE;     //
    int vpdW;     //
    double zdcX;  //
    double vz;    //
    double vzVpd; //
    int    nch;   //
    int    sumpt;   //
    double sumETgtp2; //
    int    ntow_gt_p2GeV; //
    int    ntow_gt_4GeV; //
    int    ntow_gt_8GeV; //
    int    ntow_gt_12GeV; //
    ClassDef(EA_bmetric,1);
};

#endif
