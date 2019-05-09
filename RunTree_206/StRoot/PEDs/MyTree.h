#ifndef MyTree_HH
#define MyTree_HH
#include <iostream>
#include <vector>
using namespace std;

    /* Object to store the data for towers with negative ETow energyies */

using namespace std;

class EmcHit : public TObject {
	public:

    EmcHit() {} ;
	virtual ~EmcHit() {};

    int   i_tower; // 1-4800
    float calc_E;
    float calc_Et;
    bool  good_StatUpdate;
    bool  good_Status;
    bool  good_PED;
    bool  good_Gain;
    bool  good_Calib;
    float ped;
    int   adc;

	ClassDef(EmcHit,1);
};

class MyEvent : public TObject {
    public:
    MyEvent() {} ;
    virtual ~MyEvent() {};
    /* int eventid; */
    int runId;
    /* int time; */
    int nHT1;
    /* int nSuspectBtow; */
    /* bool has_only_suspect; */

    ClassDef(MyEvent,1);
};

#endif
