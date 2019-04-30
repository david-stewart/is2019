#include "TGraph.h"
#include "TGraphErrors.h"
#include "TNtuple.h"
#include "TMath.h"
#include "TF1.h"
#include <iostream>

/* TGraph* fillTGraph() { */
/*     std::cout << " Warning, nothing done!" << std::endl; */
/*     return nullptr; */
/* }; */

TGraphErrors *fill_TGE(TNtuple* tup) {
    int n = tup->Draw("pt:dN:err","","goff");
    return new TGraphErrors(n, tup->GetV1(), tup->GetV2(),0,tup->GetV3());
};

TGraph* fill_TGraph(TNtuple* tup) {
    int n = tup->Draw("pt:dN","","goff");
    return new TGraph(n, tup->GetV1(), tup->GetV2());
};

double StephenHorvat_5p5(double *x, double *p) {
//
    return p[0]*TMath::Power(1-p[1]*(1-p[2])*x[0]*x[0], 1/(1-p[2]));
//
//
};

double WikipediaLevyPDF(double *x, double *p) {
    return TMath::Sqrt(p[0]/2/TMath::Pi())
           * TMath::Exp(-p[0]/(2*(x[0]-p[1])))
           / TMath::Power(x[0]-p[1], 1.5);
};

double LevyFig2(double *x, double *p) {
    /* return TMath::Sqrt(p[0]/2/TMath::Pi()) */
    /*        * TMath::Exp(-p[0]/(2*(x[0]-p[1]))) */
    /*        / TMath::Power(x[0]-p[1], 1.5); */
    double m0 = 0.135;
    return p[0] / TMath::Power( (1+(TMath::Sqrt(x[0]*x[0]+m0*m0)-m0)/(p[1]*p[2])), p[1]);
    /* TF1 *levy = new TF1("levy","([0])/(1 + ((TMath::Sqrt((x*x)+(0.14*0.14))-0.14)/([1]*[2])))**[1]",2.5,8.0); */
};


TF1* make_TFit(const char* name, TGraphErrors* graph) { 
    TF1* rfit = nullptr;

    int which_fit = 1;
    switch( which_fit) {
    case 0:
        rfit = new TF1(name, WikipediaLevyPDF,0.5,10,2);
        rfit->SetParameter(0, 0.0001);
        rfit->SetParameter(1, .001);
    break;

    case 1:
        rfit = new TF1(name, StephenHorvat_5p5,0.1,10,3);
        rfit->SetParameter(0, 62.83);
        rfit->SetParameter(1, 4.56);
        rfit->SetParameter(2, 1.178);
    break;

    case 2:
        rfit = new TF1(name, LevyFig2,.1,10,3);
        rfit->SetParameters(10,1.,1.);
        rfit->SetParLimits(0,0.,500.);
        rfit->SetParLimits(1,0.,40.);
        rfit->SetParLimits(2,0.,6.);
        rfit->SetParNames("B","n","T");
        /* rfit->SetParameter(1, .56); */
        /* rfit->SetParameter(2, .178); */
    break;
    }
    /* TF1 *levy = new TF1("levyi","([0])/(1 + ((TMath::Sqrt((x*x)+(0.14*0.14))-0.14)/([1]*[2])))**[1]",2.5,8.0); */
    /*     levy->SetParameters(10,1.,1.); */
    /*     levy->SetParLimits(0,0.,500.); */
    /*     levy->SetParLimits(1,0.,40.); */
    /*     levy->SetParLimits(2,0.,6.); */
    /*     graph->Fit("levyi","","",2.5,8.); */
    /*     return levy; */

        
    /* rfit->SetParameter(2, 1.178); */
    graph->Fit(name,"","",1,8.);
    return rfit;
};

