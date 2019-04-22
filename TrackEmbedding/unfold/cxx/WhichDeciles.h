#ifndef WHICH_DECILE__H
#define WHICH_DECILE__H

#include <vector>
#include <algorithm>
#include "signal_corrections.h"

using std::vector;
using std::lower_bound;

class WhichDeciles{
    BbcSigAdjuster* adj;
    TH1D* inout;
    vector<float> edges {
        -6853.43, 3042.34, 5737.51, 8463.37, 11526.7, 15007.8,
        18992.7,  23623.1, 29202.3, 36585.7, 69119 
    };
    float minval;
    float maxval;

    public:
    WhichDeciles(BbcSigAdjuster* inval, TH1D* _inout) 
        : adj{inval}, inout{_inout} {};
    //: //adj{_adj} {};
    int operator()(bool count=false) {
        double bbc { (*adj)() };
        if (bbc < minval) minval = bbc;
        if (bbc > maxval) minval = bbc;
        int rval = (int)(lower_bound(edges.begin(), edges.end(), 
                    bbc)-edges.begin())-1;
        if (rval > 9) {
            /* cout << "warning: bbc val("<<bbc<<") is bigger than last bin!"<<endl; */
            if (count) inout->Fill(1);
            rval = 9;
        } else if (rval < 0) {
            /* cout << "warning: bbc val("<<bbc<<") is less than first bin!"<<endl; */
            rval = 0;
            if (count) inout->Fill(-1);
        } else { if (count) inout->Fill(0); }
        return rval;
    }
    void printminmax() {
        cout << " min max vals of bbc: " << minval << " " << maxval << endl;
    };
};
#endif
