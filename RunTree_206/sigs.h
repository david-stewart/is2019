#ifndef SIGNAL_CORRECTIONS_H
#define SIGNAL_CORRECTIONS_H

#include <TF1.h>
#include "run_map.h"
#include "TProfile.h"


class BbcSigAdjuster {
    public:
        double&    bbc;
        float&     vz;
        int&       runId;
        float p0;
        float p1;
        float p2;

        const float p0_all =  17582.505;
        const float p1_all =  392.41000;
        const float p2_all = -8.0006738;
        std::map<int, TripleFloat>  p_map; // fit parameters per run
        int old_runId;

    BbcSigAdjuster(
        double& _bbc,
        float&  _vz,
        int&    _runId
    ) : bbc{_bbc}, vz{_vz}, runId{_runId}, p_map{get_runmap()}, old_runId(0) {};

    float operator()() {
        if (p_map.count(runId)){
            if (old_runId != runId) {
                old_runId = runId;
                p0 = p_map[runId].p0;
                p1 = p_map[runId].p1;
                p2 = p_map[runId].p2;
            }
            return bbc - vz*p1 - vz*vz*p2 + 17580. - p0;
        } else {
            return bbc - vz*p1_all - vz*vz*p2_all;
        }
    }
    ClassDef(BbcSigAdjuster,1)
};
#endif
