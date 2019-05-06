#include "TGraph.h"
#include "TGraphErrors.h"
#include "TNtuple.h"
#include "TMath.h"
#include "TF1.h"
#include "TF1.h"
#include "TPad.h"
#include "TAxis.h"
#include "TLegend.h"
#include <iostream>

// This is a local root library with some simple functions to fill TGraphErrors from TNtuples
TGraphErrors *tup_to_TGE(TNtuple* tup) {
    int n = tup->Draw("pt:dN:err","","goff");
    return new TGraphErrors(n, tup->GetV1(), tup->GetV2(),0,tup->GetV3());
};

enum EnumFitFn    { Horvat5p5, LevySTAR };
enum EnumParticle { pi, K, p };
enum posneg      { pos, neg };

double get_p_mass ( EnumParticle particle ) {
    switch(particle) 
    {
        case EnumParticle::pi : return 0.135  ; break;
        case EnumParticle::K  : return 493.677; break;
        case EnumParticle::p  : return 938.272; break;
    }
};

pair<TGraphErrors*, TF1*> tup_to_fit(
       TNtuple*      tup, 
       /* TGraphErrors*& TGE, */ 
       /* TF1*&          fit, */ 
       EnumParticle    particle=EnumParticle::pi, 
       TPad* pad=nullptr,
       EnumFitFn fit_function=EnumFitFn::LevySTAR
) {
    const char* name { tup->GetName() };
    const char* fit_name { Form("%s_fit",name) };
    TGraphErrors* TGE = tup_to_TGE(tup);
    TF1* fit;
    double x_min { tup->GetMinimum("pT")-0.001 };
    double x_max { tup->GetMaximum("pT")-0.001 };
    switch (fit_function)
    {
    case Horvat5p5 :
        fit = new TF1(fit_name, 
        [](double*x,double*p){ return
        p[0]*TMath::Power(1.-p[1]*(1.-p[2])*x[0]*x[0],(1./(1.-p[2])));},
        x_min, x_max, 3);
        fit->SetParameters(62.83, 4.56, 1.178);
        fit->SetParNames("p0","p1","p2");
        /* rfit->SetParameter(1, 4.56); */
        /* rfit->SetParameter(2, 1.178); */

    break;
    case LevySTAR :
        double m0 { get_p_mass(particle) };
        fit = new TF1(
                fit_name,
                [m0](double*x,double*p){return 
                p[0] / TMath::Power( (1+(TMath::Sqrt(x[0]*x[0]+m0*m0)-m0)/(p[1]*p[2])), p[1]);},
                x_min,x_max,3);
        fit->SetParameters(0.18,4.85,2.83e-4);
        fit->SetParLimits(0,0.,500.);
        fit->SetParLimits(1,0.,40.);
        fit->SetParLimits(2,0.,6.);
        fit->SetParNames("B","n","T");
    break;
    }

    TGE->Fit(fit_name,"","",x_min,x_max);
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
