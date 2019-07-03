#include <TPad.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TCanvas.h>
#include <TFile.h>
#include "RooUnfoldResponse.h"
#include "RooUnfold.h"
#include "RooUnfoldBayes.h"
#include "TLegend.h"
#include <iostream>
#include <TStyle.h>
#include <TExec.h>
#include <map>
#include <TMath.h>
#include <TH3.h>

static const double TOF_efficiency = 0.60;

// unfold hg3 into sets of hg2:
//     T0  (any towers)
//     T1  (4-8 trigs)
//     T2  (8-12 trigs)
//     T3  (12 & up, trigs)

class Unfolder {
    private:
    TH2D* MT;
    TH1D* T;
    TH1D* M;
    /* RooUnfoldResponse* rMatrix; */  
    int nIter;
    int nUsed;

    public:
    Unfolder(const char* tfile, const char* tag) :
    nIter(7), nUsed(0) 
    {
        TFile* f = new TFile(tfile,"read");
        f->GetObject("MT_piKp",MT);
        f->GetObject("T_piKp",T);
        M = MT->ProjectionX(Form("M_%s",tag));

        T ->SetName(Form("T_%s", tag));
        M ->SetName(Form("M_%s" ,tag));
        MT->SetName(Form("MT_%s",tag));
    };
    TH2D* Unfold(TH2D* clip, const char* name){
        int n_y = clip->GetYaxis()->GetNbins();
        int n_x = clip->GetXaxis()->GetNbins();
        TH2D *rval = static_cast<TH2D*>( clip->Clone(name) );
        RooUnfoldResponse* rMatrix = new RooUnfoldResponse(M,T,MT);
        for (int x=1;x<=n_x;++x) {
            TH1D* raw = rval->ProjectionY(Form("temp_%i",nUsed++),x,x);
            //TOF update: increase all pT bins by pT percentage
            if (false) {
                TAxis* axis = raw->GetXaxis();
                int nbins = axis->GetNbins();
                for (int i=1;i<=nbins;++i){
                    double val { raw->GetBinContent(i) };
                    double pos { raw->GetBinCenter(i) };
                    int n_add { static_cast<int>(val*(1/TOF_efficiency-1)+0.5) }; 
                    for (int j{0};j<n_add;++j) raw->Fill(pos); 
                }
            }
            
            /* rMatrix->SetName(Form("rmatrix_%s",tag)); */
            RooUnfoldBayes* bayes = new RooUnfoldBayes(rMatrix,raw,nIter); 
            TH1D* hg1 = (TH1D*) bayes->Hreco();
            for (int y=1;y<=n_y;++y) {
                rval->SetBinContent(x,y,hg1->GetBinContent(y));
                rval->SetBinError  (x,y,hg1->GetBinError(y));
            }
            delete raw;
            delete bayes;
            delete hg1;
        }
        return rval;
    };
};

void Unfold_pT_hg3(TFile* f_in, TFile *f_out, TString name, Unfolder& pp, Unfolder& dAu) {
    vector<TH2D*> proj;
    TH3D* hg3;
    f_in->GetObject(name.Data(), hg3);
    const char* tag = name.Contains("_E") ? "E" : "C";

    int nIter = 5;

    cout << " name: " << name << " " << hg3->GetEntries() << endl;

    // get all the TH2 projections that will be unfolded
    /* proj.push_back((TH2D*) hg3->Project3D("xy")); */
    /* proj[0]->SetName(Form("%s_T0_",tag)); */

    // get the projections in T0, T1, T2, T3 // T4=Trig>4GeV in lo/md/hi group
    //                                       // T5=all       in lo/md/hi group
    int i_tag[6] {0,1,2,3,4,5};
    int i_bot[6] {0,3,4,5,3,0};
    int i_top[6] {6,3,4,6,6,6};
    for (int i=0;i<6;++i){
        // T3 needs to include the overflow bin
        hg3->GetZaxis()->SetRange(i_bot[i],i_top[i]);
        TH2D* p =  (TH2D*) hg3->Project3D("xy");
        /* cout << " i " << i_tag[i] << "  " << p->GetEntries() << endl; */
        p->SetName(Form("%s_T%i_", tag, i));
        proj.push_back(p);
    }

    // clip and unfold
    double x_edges[11];
    for (int i=0;i<11;++i) x_edges[i] = proj[0]->GetXaxis()->GetBinLowEdge(i+1);

    double x_4_edges[4];
    int four_edges[4] {1,4,8,11};
    for (int i =0;i<4; ++i ) x_4_edges[i] = proj[0]->GetXaxis()->GetBinLowEdge(four_edges[i]);

    f_out->cd();
    for (int i=0;i<6;++i) {
        TH2D* tracks = static_cast<TH2D*> (proj[i]->Clone(Form("%s_T%i",tag,i)));
        tracks->Write();
        TH2D* clip;
        if (i<4) {
            clip = new TH2D( Form("%s_T%i_clip",tag,i), ";BBC;pT", 10,x_edges,150,0,15.);
            for (int x = 1; x<=10; ++x){
                for (int y = 1; y<=150; ++y){
                    clip->SetBinContent(x,y, tracks->GetBinContent(x,y));
                    clip->SetBinError(x,y,    tracks->GetBinError(x,y));
                }
            }
            clip->Write();
        } else {
            clip = new TH2D( Form("%s_T%i_clip",tag,i), ";BBC;pT", 3,x_4_edges,150,0,15.);
            int i0[3] {1,4,8};
            int i1[3] {3,7,10};
            for (int j=0;j<3;++j) {
                TH1D* temp = tracks->ProjectionY("temp",i0[j],i1[j]);
                for (int y = 1; y<=150; ++y){
                    clip->SetBinContent(j+1,y, temp->GetBinContent(y));
                    clip->SetBinError  (j+1,y, temp->GetBinError  (y));
                }
                delete temp;
            }
            clip->Write();
        }
        TH2D* unf_pp = pp.Unfold(clip, Form("%s_T%i_unf_pp",tag,i));
        unf_pp->Write();

        TH2D* unf_dAu = dAu.Unfold(clip, Form("%s_T%i_unf_dAu",tag,i));
        unf_dAu->Write();
    }
    return;
};


void pT_unfold_set(const char* nameTFile="../../206_ZDCx0.root", const char* name_out="004_h2pt.root"){
    TFile *f_in  = new TFile (nameTFile);
    if (!f_in->IsOpen()) { cout << "Fatal, input file not open." << endl; exit(2); }
    
    TFile *f_out = new TFile(name_out,"recreate");
/*
    E_T0
    E_T0_clip
    C_T0_unf_pp
    E_T0_unf_dAu
*/
    Unfolder unfolder_pp("../unfold/pp_rMatrix.root","_pp");
    Unfolder unfolder_dAu("../unfold/dAu_rMatrix.root","_dAu");

    Unfold_pT_hg3(f_in, f_out, "h3_pt_E", unfolder_pp, unfolder_dAu);
    Unfold_pT_hg3(f_in, f_out, "h3_pt_C", unfolder_pp, unfolder_dAu);

    f_out->Save();
};

/* void pt_unfold() { */
void pt_unfold(const char* tag) {
    pT_unfold_set(Form("../../004_%s.root",tag), "pT_unfold_004.root");
    pT_unfold_set(Form("../../206_%s.root",tag), "pT_unfold_206.root");
};

