/* In order to compile:
    eventually:
    g++ -std=c++11 -o bin/split_embedding cxx/split_embedding.cxx `root-config --cflags --glibs` -I$ROOUNFOLD/src -L$ROOUNFOLD -lRooUnfold
    for now:
    g++ -std=c++11 -o main cxx/split_embeddings.cxx `root-config --cflags --glibs` -I$ROOUNFOLD/src -L$ROOUNFOLD -lRooUnfold
*/

// make a tree and split it into truth (T) meausred (M) and response (MT)
// give results for two random sub-groups -- a & b
// unfold the M to T in all groups
// unfold M to T in a & b with the other response matrix

#define emb_tree_cxx
#include "emb_tree.h"
#include "myRooUnfoldLib.h"
/* #include "mult_by_response.h" */
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "TRandom3.h"

#include <functional>

#include <iostream>
#include <sstream>

#include "RooUnfoldResponse.h"
#include "RooUnfold.h"
#include "RooUnfoldBayes.h"
#include "signal_corrections.h"
#include "run_map.h"
#include "WhichDeciles.h"


class Filler{
    public:
    TH1D *T;
    TH1D *Tp;
    TH1D *M;
    TH2D *TM;
    TH2D *MT;
    string name;
    string title;
    float& ptMc;
    float& ptPr;
    bool& is_matched;

    const int   nbins;
    const float lobin = 0.;
    const float hibin = 25.;

    /* std::function<bool()> in_group; */
    Filler(string _name, string _title, float& _ptMc, float& _ptPr, bool& _is_matched, int _nbins) :
     name{_name}, title{_title}, ptMc{_ptMc}, ptPr{_ptPr}, is_matched{_is_matched}, nbins{_nbins}
    {
        T = new TH1D( (name+"T").c_str(),   (title + " Truth;p_{T};N_{MC tracks}").c_str(), nbins , lobin , hibin );
        T->Sumw2();
        Tp = new TH1D( (name+"Tp").c_str(), (title + " Truth paired;p_{T};N_{MC tracks which are paired}").c_str(), nbins , lobin , hibin );
        Tp->Sumw2();
        M = new TH1D( (name+"M").c_str(), (title + " Measured;p_{T};N_{Measured tracks}").c_str(), nbins , lobin , hibin );
        M->Sumw2();
        TM = new TH2D( (name+"TM").c_str(), (title + " Measured;p_{T,truth};p_{T,measured}").c_str(), nbins , lobin , hibin , nbins , lobin , hibin );
        TM->Sumw2();
        MT = new TH2D( (name+"MT").c_str(), (title + " Measured;p_{T,measured};p_{T,truth}").c_str(), nbins , lobin , hibin , nbins , lobin , hibin );
        MT->Sumw2();
        cout << " Making filler for: " << name << endl;
    }

    void operator()(){
        T->Fill(ptMc);
        if (is_matched) {
            Tp->Fill(ptMc);
            M->Fill(ptPr);
            TM->Fill(ptMc, ptPr);
            MT->Fill(ptPr, ptMc);
        }
    }
    // Data to get for the first group:
    // : x_E    : Tp ratio to T for x
    // : x_U    : xM unfolded via xMT
    //     example: vz20_U_bbc20
    // : x_rU   : ratio xT to x_U
    //
    // if unfolding against another group
    // : x_U_y   : xM unfolded via yMT
    // : x_rU_y : ratio xT to x_U_y
    //   example: vz20_rU_bbc20
    TH1D* E;
    TH1D* U;
    TH1D* U_y;
    TH1D* rU; 
    TH1D* rU_y; 

    void unfold() {
        cout << " unfolding " << name << endl;
        /* cout << "::0e name:" << name << endl; */
        // get x_E
        E = (TH1D*) Tp->Clone((name+"_E").c_str());
        E->Sumw2();
        /* cout << "::1e name:" << name << endl; */
        E->SetTitle((title + " embedded to matched ratio}").c_str());
        /* cout << "::2e name:" << name << endl; */
        E->GetYaxis()->SetTitle("N_{MC tracks}/N_{MC tracks matched}");
        /* cout << "::3e name:" << name << endl; */
        E->Divide(T);
        /* cout << "::4e name:" << name << endl; */

        // get x_U
        RooUnfoldResponse* response_selfMT = new RooUnfoldResponse(M, T, MT);
        /* cout << "::5e name:" << name << endl; */
        RooUnfoldBayes*    bayes_x_U    = new RooUnfoldBayes(response_selfMT,M,5); // This isn't making the renaming
        /* cout << "::6e name:" << name << endl; */
        TH1D* U =  (TH1D*) bayes_x_U->Hreco();
        U->Sumw2();
        /* cout << "::7e name:" << name << endl; */
        U->SetName( (name+"_U").c_str());
        /* cout << "::8e name:" << name << endl; */
        U->SetTitle( (title + " M unfolded").c_str());
        /* cout << "::9e name:" << name << endl; */
        delete response_selfMT;
        delete bayes_x_U;
        /* cout << "::10e name:" << name << endl; */

        // get x_rU 
        TH1D* rU = (TH1D*) U->Clone( (name+"_rU").c_str() );
        rU->Sumw2();
        rU->SetTitle( (title+" ratio: (M unfolded)/T").c_str());
        rU->GetYaxis()->SetTitle("(N_{M unfolded})/N_{T}");
        rU->Divide(T);
        /* cout << "::11e name:" << name << endl; */
    };

    void unfold(Filler& first, Filler& second) {
        unfold(first);
        unfold(second, false);
    }

    void unfold(Filler& unzip, bool unfold_self=true) {
       /* cout << " ::1b" << endl; */
       /* cout << " my name " << name << " unzip name " << unzip.name << endl; */
       /* cout << " ::2b" << endl; */
       /* cout << " ::3b" << endl; */

        if (unfold_self) unfold();
        cout << " unfolding " << name << " with " << unzip.name << endl;
        /* cout << "::f0"<<endl; */
        // get x_U_y
        RooUnfoldResponse* resp_y_MT 
            = new RooUnfoldResponse(unzip.M, unzip.T, unzip.MT);
        /* cout << "::f1"<<endl; */
        RooUnfoldBayes*    bayes_x_U_y   = new RooUnfoldBayes(resp_y_MT,M,5);
        /* cout << "::f2"<<endl; */
        TH1D* U_y = (TH1D*) bayes_x_U_y->Hreco();
        U_y->Sumw2();
        /* cout << "::f3"<<endl; */
        U_y->SetName( (name+"_U_"+unzip.name).c_str());
        /* cout << "::f4"<<endl; */
        U_y->SetTitle( (title + " M unfolded with " + unzip.name).c_str());
        /* cout << "::f5"<<endl; */
        delete resp_y_MT;
        delete bayes_x_U_y;
        /* cout << "::f6"<<endl; */

        // get x_rU_y
        TH1D* rU_y = (TH1D*) U_y->Clone( (name+"_rU"+unzip.name).c_str() );
        rU_y->Sumw2();
        rU_y->SetName((name+"_rU_" + unzip.name).c_str());
        rU_y->SetTitle(("Ratio M unfolded with "+unzip.name+" to T").c_str());
        rU_y->GetYaxis()->SetTitle(
                ("N_{M unfolded w. "+unzip.name+"}/N_{T}").c_str());
        rU_y->Divide(T);
    }
};

class FillerFactory {
    float& ptMc;
    float& ptPr;
    bool&  is_matched;
    int    nbins;
    public:
    FillerFactory(float& _mc, float& pr, bool& matched, int _nbins) :
        ptMc{_mc}, ptPr{pr}, is_matched{matched}, nbins{_nbins*25}
    {};
    Filler operator()(string prepend, string title) {
        return Filler{prepend, title, ptMc, ptPr, is_matched, nbins};
    };
};


void emb_tree::Loop(TString outname, int nbins)
{
   if (fChain == 0) return;


   // make the adjusters

   const float  v_bbc20 { 9780. };
   const float  v_bbc80 { 36800. };

   const float  v_zdc20 { 11300. };
   const float  v_zdc80 { 20200. };

   const float  v_vz20 { -11.6 };
   const float  v_vz80 { 17.8  };

   const float  rmult20 {10.0 };
   const float  rmult80 {20.0 };

   BbcSigAdjuster bbc_adj{bbcAdcES, z_vert, runId, 
       "vz_corr/a_vzfit.root"};
   FillerFactory factory{ptMc, ptPr, is_matched, nbins};

   /* int j = 0; */
   /* for (auto i : bbc_adj.p_map) { */
   /*     cout << " run id " << i.first << " " << i.second.p0 */ 
   /*                  << " " << i.second.p1 */
   /*                  << " " << i.second.p2 */
   /*                  << endl; */
   /* } */
   /* return; */

   TFile* fout = new TFile(outname.Data(), "RECREATE");

   TH1D* bbcInOut = new TH1D("bbcInOut","In or out corrected bbc;-1:0:1 = too small, in, too large",3,-1.5,1.5);
   bbcInOut->Sumw2();
   TH1D* bbcAdc18b = new TH1D("bbcAdc18b", "bbcAdcDistribution;bbcAdcEs;1/N_total x N",500,-6853.4263, 69119.016);
   bbcAdc18b->Sumw2();

   WhichDeciles i_dec{&bbc_adj, bbcInOut};

   /* vector<Filler> v_bbcdec; */
   TH2D* all_decM = new TH2D("all_decM",
       "Each bbc decile M p_T;bbc decile; p_{T,M}",10,-0.5,9.5,
        25*nbins, 0, 25.);

   TH2D* all_decT = new TH2D("all_decT",
       "Each bbc decile T p_T;bbc decile; p_{T,truth}",10,-0.5,9.5,
        25*nbins, 0, 25.);

   /* for (int i{0};i<10;++i) { */
   /*     const char* name {Form("%i_dec",i)}; */
   /*     cout << " This name: " << name << endl; */
   /*     v_bbcdec.push_back( */
   /*     factory(Form("bbcDec_%i",i),Form("bbcDec_%i",i)) */
   /*     ); */
   /* }; */


    Filler all     { factory("all",   "all") };
    /* Filler zdc20   { factory("zdc20", "zdc20") }; */
    /* Filler zdc80   { factory("zdc80", "zdc80") }; */
    Filler bbc20   { factory("bbc20", "zdc80") };
    Filler bbc80   { factory("bbc80", "zdc80") };
    /* Filler mult20  { factory("mult20","mult20") }; */
    /* Filler mult80  { factory("mult80","mult80") }; */
    /* Filler vz20    { factory("vz20",  "vz20") }; */
    /* Filler vz80    { factory("vz80",  "vz80") }; */
    /* Filler vzIn    { factory("vzIn",  "vzIn") }; */
    /* Filler vzOut   { factory("vzOut", "(group B) vzOut") }; */

    Filler A       { factory("A","(group A)") };
    Filler Abbc20  { factory("Abbc20", "(group A) zdc80") };
    Filler Abbc80  { factory("Abbc80", "(group A) zdc80") };

    Filler B       { factory("B","(group B)") };
    /* Filler Bzdc20  { factory("Bzdc20", "(group B) zdc20") }; */
    /* Filler Bzdc80  { factory("Bzdc80", "(group B) zdc80") }; */
    /* Filler Bmult20 { factory("Bmult20","(group B) mult20") }; */
    /* Filler Bmult80 { factory("Bmult80","(group B) mult80") }; */
    /* Filler Bvz20   { factory("Bvz20",  "(group B) vz20") }; */
    /* Filler Bvz80   { factory("Bvz80",  "(group B) vz80") }; */
    /* Filler BvzIn   { factory("BvzIn",  "(group B) vzIn") }; */
    /* Filler BvzOut  { factory("BvzOut", "(group B) vzOut") }; */

   TRandom3* r3 = new TRandom3();
   Long64_t nbytes = 0, nb = 0;

   Long64_t nentries = fChain->GetEntriesFast();
   cout << " There are " << nentries << " entries" << endl;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;

        /* v_bbcdec[i_dec(true)](); */
        all_decT->Fill(i_dec(),ptMc);

        bbcAdc18b->Fill(bbc_adj());

        if (is_matched) all_decM->Fill(i_dec(),ptPr);

        all();
        /* if (zdcX < v_zdc20)                 zdc20(); */
        /* if (zdcX > v_zdc80)                 zdc80(); */
        if (bbcAdcES < v_bbc20)             bbc20();
        if (bbcAdcES > v_bbc80)             bbc80();
        /* if (refMult < rmult20)              mult20(); */
        /* if (refMult > rmult80)              mult80(); */
        /* if (z_vert < -11.6)                 vz20(); */
        /* if (z_vert > 17.8 )                 vz80(); */
        /* if ((z_vert < 10)  || (z_vert>24))  vzOut(); */
        /* if ((z_vert >= 13) || (z_vert<=16)) vzIn(); */

        if ( r3->Integer(2) == 0 )
            A();
            if (bbcAdcES < v_bbc20)             Abbc20();
            if (bbcAdcES > v_bbc80)             Abbc80();
        else {
            B();
            /* if (zdcX < v_zdc20)                 Bzdc20(); */
            /* if (zdcX > v_zdc80)                 Bzdc80(); */
            /* if (refMult < rmult20)              Bmult20(); */
            /* if (refMult > rmult80)              Bmult80(); */
            /* if (z_vert < -11.6)                 Bvz20(); */
            /* if (z_vert > 17.8 )                 Bvz80(); */
            /* if ((z_vert < 10)  || (z_vert>24))  BvzOut(); */
            /* if ((z_vert >= 13) || (z_vert<=16)) BvzIn(); */
        }
   }
   i_dec.printminmax();

   /* for (auto& v : v_bbcdec) v.unfold(); */
   all.unfold();
   bbc20 .unfold();
   bbc80 .unfold();
   /* zdc20 .unfold(); */
   /* zdc80 .unfold(); */
   /* mult20.unfold(); */
   /* mult80.unfold(); */
   /* vz20  .unfold(); */
   /* vz80  .unfold(); */
   /* vzOut .unfold(); */
   /* vzIn  .unfold(); */

   A.unfold(B);

   B      .unfold(Abbc20, Abbc80);
   /* Bzdc20 .unfold(Abbc20, Abbc80); */
   /* Bzdc80 .unfold(Abbc20, Abbc80); */
   /* Bmult20.unfold(Abbc20, Abbc80); */
   /* Bmult80.unfold(Abbc20, Abbc80); */
   /* Bvz20  .unfold(Abbc20, Abbc80); */
   /* Bvz80  .unfold(Abbc20, Abbc80); */
   /* BvzOut .unfold(Abbc20, Abbc80); */
   /* BvzIn  .unfold(Abbc20, Abbc80); */

   fout->Write();
}

int main (int argc, const char** argv) {
    std::ostringstream fout_name;
    int nbins_per_GeV;

    std::istringstream iss_nbins_per_GeV    { (argc > 1) ? argv[1] : "10." } ;
    TString in_root_file  { (argc > 2) ? argv[2] : "PiPlusMinus.root"};
    /* string in_root_file  { (argc > 2) ? argv[2] : "short.root"}; */
    /* fout_name << ( (argc > 3) ? argv[3] : Form("unf_%s", (in_root_file(0,in_root_file.Length()-5)).Data())); */
    fout_name << Form("unf_%s", (in_root_file(0,in_root_file.Length()-5)).Data());

    iss_nbins_per_GeV >> nbins_per_GeV;

    cout << " in file: " << in_root_file << endl;
    fout_name << "_" << nbins_per_GeV << "perGeV.root" ;
    cout << "Writing file: " << fout_name.str() << endl;

    emb_tree myLoop(in_root_file);
    myLoop.Loop(fout_name.str(), nbins_per_GeV);
}
