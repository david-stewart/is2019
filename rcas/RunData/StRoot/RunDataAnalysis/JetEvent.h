#ifndef TreeEvent__h
#define TreeEvent__h
#include <iostream>
#include "TArrayF.h"

struct JetEvent : public TObject {
	virtual ~JetEvent() {} ;
    float     ZDCx;
    float     bbc;
    float     bbc_corr;
    float     vz;
    float     trig_phi;
    float     trig_eta;
    float     trig_Et;
    ClassDef(JetEvent,1);
};

class JtJet : public TObject {
	public:
    JtJet() {} ;
	virtual ~JtJet() {};
    float   pt;
    float   pt_corr;
    float   phi;
    float   eta;
    float   area;
	ClassDef(JtJet,1);
};

#endif
