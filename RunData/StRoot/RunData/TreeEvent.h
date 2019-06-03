#ifndef TreeEvent__h
#define TreeEvent__h
#include <iostream>
#include "TArrayF.h"

struct TreeEvent : public TObject {
	virtual ~TreeEvent() {} ;
    int       runId;      
    int       bbcES;  // bbc East adc
    double    bbcE;  // bbc East adc
    float     ZDCx;
    float     vz;
    float     vzVpd;

    int       nch;
    int       n_isTOF;
    int       n_isBEMC;

    bool      b_rank; // true for > 0 neg for < 0
    bool      b_vz10; // is |vz|<10
    bool      b_vzVpd6; // is |vz-vpdvz|<6
    bool      b_all; // b_rank && b_vz10 && b_vzVpd6

    ClassDef(TreeEvent,1);
};

class myTrack : public TObject {
	public:
    myTrack() {} ;
	virtual ~myTrack() {};
    float   pt;
    float   phi;
    float   eta;
    bool    isTOF;
    bool    isBEMC;
	ClassDef(myTrack,1);
};

class myTower : public TObject {
	public:
    myTower() {} ;
	virtual ~myTower() {};
    float   Et;
    float   phi;
    float   eta;
	ClassDef(myTower,1);
};

#endif
