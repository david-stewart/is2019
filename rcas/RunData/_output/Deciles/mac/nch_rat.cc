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
#include <TMath.h>
#include <TStyle.h>
#include <TExec.h>
#include <map>
#include <TH3.h>
#include <TProfile2D.h>

void nch_rat() {
    vector<const char*> files {"ZDCx0","TOFmatch", "TOFflat","TOF40","TOF-15"};
    vector<TH1D*> hg;
    int col6[6] {924, 975, 1026, 1077, 1128, 1178};
    int mark6[6] {kFullCircle, kFullSquare, kFullStar, kFullTriangleUp, kFullDiamond};

    int i{0};
    TCanvas *c = new TCanvas("c");
    TLegend*leg = new TLegend();
    bool first {true};
    for (auto name : files) {
        TFile *f = new TFile(Form("%s/decile_%s.root",name,name),"read");
        TH1D* get;
        f->GetObject("nch_dAu_0",get);
        get->SetName(name);
        get->SetMarkerStyle(mark6[i]);
        /* get->SetMarkerColor(col6[i]); */
        get->SetMarkerColor(kBlue);
        if (first) { 
            get->GetYaxis()->SetTitleSize(0.05);
            get->GetYaxis()->SetTitleOffset(0.71);
            get->GetYaxis()->SetLabelSize(0.04);
            get->GetYaxis()->SetLabelOffset(0.007);
            get->Draw("PE"); first = false; 
        }
        else       get->Draw("PE same");
        leg->AddEntry( get, Form("MB(%s)",name));

        f->GetObject("nch_dAu_1",get);
        get->SetName(Form("HT1-%s",name));
        get->SetMarkerStyle(mark6[i]);
        /* get->SetMarkerColor(col6[i]); */
        get->SetMarkerColor(kRed);
        get->Draw("PE same");
        leg->AddEntry( get, Form("HT(%s)",name));
        ++i;
    }
    leg->Draw();
    /* C->WaitPrimitive(); */
}

