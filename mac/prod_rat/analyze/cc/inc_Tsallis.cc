#include "TGraph.h"
#include "TGraphErrors.h"
#include "TNtuple.h"
#include "TMath.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TPad.h"
#include "TAxis.h"
#include "TLegend.h"
#include <iostream>
#include <TCanvas.h>

// This is a local root library with some simple functions to fill TGraphErrors from TNtuples
TGraphErrors *tup_to_TGE(TNtuple* tup) {
    int n = tup->Draw("pt:dN:err","","goff");
    return new TGraphErrors(n, tup->GetV1(), tup->GetV2(),0,tup->GetV3());
};


enum EnumFitFn    { Horvat5p5, LevySTAR, Tsallis };
enum EnumParticle { t_pi, t_K, t_p };
enum posneg      { pos, neg };

double get_p_mass ( EnumParticle particle ) {
    switch(particle) 
    {
        case EnumParticle::t_pi : return 0.135  ; break;
        case EnumParticle::t_K  : return 0.493677; break;
        case EnumParticle::t_p  : return 0.938272; break;
    }
};

TF1* TsallisFn(double A, double T, double n, EnumParticle particle=t_pi, const char* fit_name="name", 
        int color = kBlack, int linestyle=1, double x_min=0., double x_max=15.) {
    double m0 { get_p_mass(particle) };
    cout << " This is m: " << m0 << endl;
    TF1* fit = new TF1(
                fit_name,
                [m0](double*x,double*p){
                return p[0] / TMath::Power( 
                        (1+(TMath::Sqrt(x[0]*x[0]+m0*m0)-m0)/(p[1]*p[2])), 
                        p[2]);},
                x_min,x_max,3);
    fit->SetParameters(A, T, n);
    fit->SetParNames("A","T","n");
    fit->GetYaxis()->SetTitle("#frac{d^{2}N}{2#pip_{T}d#etadp_{T}}");
    fit->GetXaxis()->SetTitle("p_{T}");
    fit->SetLineColor(color);
    fit->SetLineStyle(linestyle);
    return fit;
};

TF1* FunctionRatio(const char* name, TF1*& num, TF1*& den, int color= kBlack, int linestyle=1, double x_min=0., double x_max=15.) {
    TF1* tf = new TF1(name, 
            [&num, &den](double *x, double *p=nullptr){ 
            if (p) x[0] = x[0]; 
            return num->Eval(x[0]) / den->Eval(x[0]); }, 
            x_min, x_max, 0);
    tf->SetLineColor(color);
    tf->SetLineStyle(linestyle);
    return tf;
    
};

/* TGraphErrors *tup_to_TGE(TNtuple* tup, int color=kBlack) { */
/*     int n = tup->Draw("pt:dN:err","","goff"); */
/*     TGraphErrors* r_class = new TGraphErrors(n, tup->GetV1(), tup->GetV2(),0,tup->GetV3()); */
/*     r_class->SetMarkerStyle(kOpenCircle); */
/*     r_class->SetMarkerColor(color); */
/*     return r_class; */
/* }; */

pair<TGraphErrors*, TF1*> tup_to_fit(
       TNtuple*      tup, 
       /* TGraphErrors*& TGE, */ 
       /* TF1*&          fit, */ 
       EnumParticle    particle=EnumParticle::t_pi, 
       TPad* pad=nullptr
) {
    const char* name { tup->GetName() };
    /* cout << " name " << name << endl; */
    const char* fit_name { Form("%s_fit",name) };
    
    TString massname(name);
    if (massname.BeginsWith("pbar")) particle = t_p;
    else if (massname.BeginsWith("p_")) particle = t_p;
    else if (massname.BeginsWith("K"))  particle = t_K;
    else if (massname.BeginsWith("pi")) particle = t_pi;


    cout << "fit_name: " << fit_name << endl;
    TGraphErrors* TGE = tup_to_TGE(tup);
    /* double x_min { tup->GetMinimum("pT")-0.001 }; */
    /* double x_max { tup->GetMaximum("pT")+0.001 }; */
    double x_min {0.0};
    double x_max {15.0};
    /* cout << "x_min " << x_min << " x_max " << x_max << endl; */
    double m0 { get_p_mass(particle) };
    cout << "This is mass: " << m0 << endl;
    TF1* fit = new TF1(
            fit_name,
            [m0](double*x,double*p){
                return p[0] / TMath::Power( 
                        (1+(TMath::Sqrt(x[0]*x[0]+m0*m0)-m0)/(p[1]*p[2])), 
                        p[2]);},
            x_min,x_max,3);
    /* fit->SetParameters(0.58,0.15,10.0); */
    fit->SetParameters(0.776,0.214,8.89);
    /* fit->SetParameters(0.568,0.221,11.97); */
    fit->SetParLimits(0,0.,100.);
    fit->SetParLimits(1,0.,2.);
    fit->SetParLimits(2,0.,30.);
    fit->SetParNames("A","T","n");

    /* cout << "A1" << endl; */
    TGE->Fit(fit_name,"","",x_min,x_max);
    /* TCanvas *c = new TCanvas("c"); */
    /* TGE->Draw(); */
    /* TCanvas *c1 = new TCanvas("c1"); */
    /* c1->WaitPrimitive(); */
    /* cout << "A2" << endl; */
    TGE->SetMarkerStyle(kOpenCircle);
    TGE->SetMarkerColor(kBlack);
    TGE->SetLineColor(kBlack);

    TGE->GetXaxis()->SetTitle("p_{T}");
    TGE->GetYaxis()->SetTitle("#frac{d^{2}N}{2#pip_{T}d#etadp_{T}}");

    fit->SetLineColor(kRed);
    fit->SetLineStyle(6);

    if (pad) {
        /* pad1->Draw(); */
        pad->cd();
        TGE->Draw("ALPE");
        fit->Draw("same");
        TLegend *leg = new TLegend();
        leg->AddEntry(TGE, Form("%s Data", name));
        leg->AddEntry(fit, Form("%s Fit", name));
    }
    return {TGE, fit};
};

TGraphErrors *ctup(TNtuple* tup, int marker, int color, int line) {
    int n = tup->Draw("pt:dN:err","","goff");
    TGraphErrors* rval;
    TF1* f;
    tie(rval, f) = tup_to_fit(tup);
    /* TGraphErrors* rval = new TGraphErrors(n, tup->GetV1(), tup->GetV2(),0,tup->GetV3()); */
    /* rval->SetLineStyle(line); */
    /* rval->SetLineWidth(1); */
    /* rval->SetLineColor(color); */
    const char* fit_name { Form("%s_fit",tup->GetName()) };
    rval->GetFunction(fit_name)->SetLineStyle(line);
    rval->GetFunction(fit_name)->SetLineWidth(1);
    rval->GetFunction(fit_name)->SetLineColor(color);
    rval->SetLineStyle(line);
    rval->SetLineWidth(1);
    rval->SetLineColor(color);
    rval->SetMarkerStyle(marker);
    rval->SetMarkerColor(color);
    return rval;
};

// input: file name and particle name of TM matrices; return weighted TF Matrix
void xWeight_TH2(TH2D* hg2, TH1D* w){
    TAxis* ax = w->GetXaxis();
    int nbins = ax->GetNbins();
    for (int i{1};i<nbins+1;++i) {
        double val { w->GetBinContent(i) };
        for (int j{1};j<nbins+1;++j) {
            hg2->SetBinContent( i, j, hg2->GetBinContent(i,j) * val );
            hg2->SetBinError  ( i, j, hg2->GetBinError(i,j) * val );
        }
    }
}
void yWeight_TH2(TH2D* hg2, TH1D* w){
    TAxis* ax = w->GetXaxis();
    int nbins = ax->GetNbins();
    for (int i{1};i<nbins+1;++i) {
        double val { w->GetBinContent(i) };
        for (int j{1};j<nbins+1;++j) {
            hg2->SetBinContent(j,i, hg2->GetBinContent(j,i) * val );
            hg2->SetBinError  (j,i, hg2->GetBinError(j,i) * val );
        }
    }
}

void weight_TH1(TH1D* hg, TH1D* w) {
    TAxis* ax = w->GetXaxis();
    int nbins = ax->GetNbins();
    for (int i{1};i<nbins+1;++i) {
        double val { w->GetBinContent(i) };
        hg->SetBinContent(i, hg->GetBinContent(i)*val);
        hg->SetBinError(i, hg->GetBinError(i)  *val);
    }
}

