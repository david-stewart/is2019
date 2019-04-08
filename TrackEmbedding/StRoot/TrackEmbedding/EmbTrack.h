#ifndef EmbTrack_h
#define EmbTrack_h

class EmbTrack : public TObject {
    public:

    EmbTrack() : is_matched{false},
        ptMc{0.}, ptPr{0.}, phiMc{0.}, phiPr{0.}, etaMc{0.}, etaPr{0.},
        nHitMc{0}, commonFrac{0}, zdcX{0.}, z_vert{0.}, refMult{0}, bbcAdcES{0} {} ;

	virtual ~EmbTrack() {};

    bool is_matched; 

    float ptMc;  // StTinyMcTrack.h
    float ptPr;  // StTinyRcTrack.h

    float phiMc;  // StTinyMcTrack.h
    float phiPr;  // StTinyRcTrack.h

    float etaMc;  // StTinyMcTrack.h
    float etaPr;  // StTinyRcTrack.h

    /* int    nhitMc;     //StTinyMcTrack.h */
    /* int    NCommonHit; //StMiniMcPair.h */
    
    int    nHitMc;     // StTinyMcTrack.h
    float  commonFrac; // StMiniMcPair.h

    double zdcX;     // TODO: get from the StMuDst
    float z_vert;   // TODO: ''''''''''''''''''''


    int refMult; // TODO: '''''''''''''''''''

    double bbcAdcES;
    double bbcAdcES_orig;

    int runId;

    // use the constructor to clear events 
    // (I timed it; it is comparable, if not a little faster, to just deleting everything)

	ClassDef(EmbTrack,1);
};

#endif
