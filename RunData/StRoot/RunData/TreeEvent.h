#ifndef TreeEvent__h
#define TreeEvent__h
#include <iostream>
#include "TArrayF.h"

struct TreeEvent : public TObject {
	virtual ~TreeEvent() {} ;
    int       runId;      
    /* int       nch;  // doesn't *have* to match sum of jet tracks; */
    /* int       ntow; // number of towers */
    int       bbcES;  // bbc East adc
    /* int       bbcES_corr;  // bbc East adc */
    double    bbcE;  // bbc East adc
    float     zdcX;
    float     vz;
    float     vzVpd;
    /* bool      has_corr; */

    ClassDef(TreeEvent,1);
};

class EPhiEta : public TObject {
	public:
    EPhiEta() {} ;
	virtual ~EPhiEta() {};
    float   E;
    float   phi;
    float   eta;
	ClassDef(EPhiEta,1);
};

#endif
