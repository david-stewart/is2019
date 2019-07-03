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

template<typename T> 
T fGetObject(TFile* f, const char* name, const char* setname) {
    T obj;
    f->GetObject(name, obj);
    obj->SetName(setname);
    return obj;
}

TH1D* TH1_fromTProfile2D(TFile* f, const char* name, const char* setname,
        int i0=0, int i1=0){
    TProfile2D* p2D;
    f->GetObject(name, p2D);
    TProfile *p;
    if (i0==0 && i1 == 0) p = p2D->ProfileX(Form("%s__",setname));
    else                  p = p2D->ProfileX(Form("%s__",setname), i0, i1);
    TH1D* hg = p->ProjectionX(setname);
    /* hg->SetName(setname); */
    return hg;
}

TH1D* NchSumpt_fromTH1D(TFile* f, const char* name, const char* setname, TH1D* nEv,
        bool isSumPt=false ){
	/* cout << "c0 " << name << " " << setname << endl; */
    TH2D* hg2;
    f->GetObject(name, hg2);
	TH1D* rval = hg2->ProjectionX(setname);
	rval->Divide(nEv);
    if (isSumPt) {
        TProfile* p = hg2->ProfileX();
        rval->Multiply(p);
    }
	return rval;
};

void DrawSysErrorBox (TH1D* hg, TH1D* err, int color, bool sqrt=false){
    for (int i=1;i<11;++i){
        double l_box = hg->GetXaxis()->GetBinLowEdge(i);
        double r_box = hg->GetXaxis()->GetBinLowEdge(i+1);
        double err_val = ( sqrt ? TMath::Sqrt(err->GetBinContent(i)) : err->GetBinContent(i));
        double t_box = hg->GetBinContent(i) + err_val;
        double b_box = hg->GetBinContent(i) - err_val;
        TBox *b = new TBox (l_box, b_box, r_box, t_box);
        b->SetFillColorAlpha(color,0.20);
        b->Draw("same");
    }
};

template <class T>
void SetMCS(T* val, int marker, int col, double size=.9) {
    val->SetMarkerStyle(marker);
    val->SetMarkerColor(col);
    val->SetLineColor  (col);
    val->SetMarkerSize(size);
    if (marker == 29) val->SetMarkerSize(1.2);
};

void SetYAxis(TH1D* hg, const char* y_title, double y_min=-1., double y_max=-1., double offset=0.) {
    hg->SetStats(0);
    hg->SetTitle("");
    hg->GetYaxis()->SetTitle(y_title);
    hg->GetYaxis()->SetTitleSize(0.10);
    hg->GetYaxis()->SetTitleFont(42); 
    hg->GetYaxis()->SetTitleOffset(0.45); 
    
    if (y_min != -1) hg->GetYaxis()->SetRangeUser(y_min,y_max);
    hg->GetYaxis()->SetLabelSize(0.10);
    hg->GetYaxis()->SetLabelFont(42);
    hg->GetYaxis()->SetLabelOffset(offset);

};



void draw_deciles(const char* tag) {
    /* vector<int> colors {kBlack, kBlue-7, kMagenta+3, kRed}; */
    vector<int> colors {kBlack, kBlue-4, kGreen+3, kRed};
    vector<int> shapes {kFullTriangleUp, kFullCircle, kFullSquare, kFullStar};
    vector<const char*> titles{"Minimum Bias Events",
            "HT1 Triggered Events w/BEMC Hit E_{T}#in(4,8) GeV",
            "HT1 Triggered Events w/BEMC Hit E_{T}#in(8,12) GeV",
            "HT1 Triggered Events w/BEMC Hit E_{T}> 12 GeV"};

    TCanvas *c = new TCanvas("c", "",10,45,946,968);
    c->SetFillStyle(4000);
    c->Divide(1,5,0.1,0);

    const char* name_004 =   Form("../../004_%s.root",tag);
    TFile* f_004 = new TFile(name_004,"read");
    TFile* unf_004 = new TFile("pT_unfold_004.root","read");

    const char* name_206 =   Form("../../206_%s.root",tag);
    TFile* f_206 = new TFile(name_206,"read");
    TFile* unf_206 = new TFile("pT_unfold_206.root","read");

    TFile* f_out = new TFile(Form("decile_%s.root",tag),"recreate");

    if (!f_004->IsOpen() || !f_206->IsOpen()) {
        cout << Form("fatal error, input file %s or %s not open",name_004, name_206) << endl;
        exit(2);
    }

    //--------------------------
    //  TPad 1L -- distributions
    //--------------------------
    double y_lo = 1e-9;
    double y_hi = 4e-5;
    c->cd(1);
    gPad->SetLogy();

    f_out->cd();
    TH1D* MB_dist = (fGetObject<TH2D*>(f_004,"h2_bbcDist_C", "MB_dist_h1"))->ProjectionX();
    MB_dist->Rebin(100);
    SetMCS(MB_dist,kDot,colors[0]);
    MB_dist->Scale(1./MB_dist->Integral("width"));

    TH1D* HT_dist = (fGetObject<TH2D*>(f_206,"h2_bbcDist_C", "HT_dist_h1"))->ProjectionX("HT_dist",3,5);
    HT_dist->Rebin(100);
    HT_dist->Scale(1./HT_dist->Integral("width"));

    /* SetMCS(HT_dist,kDot,colors[1]); */
    int HT1_line_color = kMagenta+2;
    SetMCS(HT_dist,kDot,HT1_line_color);

    SetYAxis(HT_dist,"#frac{1}{N_{ev}}  #frac{dN_{ev}}{dEA_{est}}",y_lo, y_hi);
    HT_dist->Draw("PE");
    MB_dist->Draw("PE same");
    HT_dist->Write();
    MB_dist->Write();

    TLegend *leg = new TLegend(0.2106874,0.1422688,0.7615348,0.6275574,NULL,"brNDC");
    leg->SetFillColor(4000);
    leg->SetLineColor(4000);
    vector<TH1D*> dummies;
    for (int i=0;i<5;++i) {
        TString title;
        int color;
        int shape;
        if (i == 1) {
            color = HT1_line_color;
            shape = kDot;
            title = "HT1 Triggered Events w/BEMC Hit E_{T}> 4 GeV";
        } else {
            int k = (i == 0) ? 0 : i-1;
            color = colors[k];
            shape = shapes[k];
            title = titles[k];
        }
        dummies.push_back(new TH1D(Form("dummy_%i",i),";;",1,0.,1.));
        SetMCS(dummies[i], shape, color);
        leg->AddEntry(dummies[i], title.Data());
    }
    leg->Draw("same");
    
    //-----------------------
    //  TPad 2 -- 
    //  -- add errors in distributiosn for 500206
    //-----------------------
    c->cd(2);
    vector<TH1D*> nEvents_C;
    vector<TH1D*> nEvents_E;

    vector<TH1D*> nEv_C; // same as above, but not normalized
    vector<TH1D*> nEv_E;

    vector<int> x0 {0,3,4,5};
    vector<int> x1 {0,3,4,6};

    f_out->cd();
    nEvents_C.push_back( fGetObject<TH2D*>(f_004, "h2_bbcDec_C", "nEvents_C_0")->ProjectionX() );
    nEv_C.push_back( fGetObject<TH2D*>(f_004, "h2_bbcDec_C", "nEv_C_0")->ProjectionX() );
    nEvents_E.push_back( fGetObject<TH2D*>(f_004, "h2_bbcDec_E", "nEvents_E_0")->ProjectionX() );
    nEv_E.push_back( fGetObject<TH2D*>(f_004, "h2_bbcDec_E", "nEv_E_0")->ProjectionX() );
    for (int i{1};i<4;++i) {
        nEvents_C.push_back( fGetObject<TH2D*>(f_206, "h2_bbcDec_C", Form("_nEvents_C_%i",i))
                ->ProjectionX(Form("nEvents_C_%i",i),x0[i],x1[i]));
        nEvents_E.push_back( fGetObject<TH2D*>(f_206, "h2_bbcDec_E", Form("_nEvents_E_%i",i))
                ->ProjectionX(Form("nEvents_E_%i",i),x0[i],x1[i]));

        nEv_C.push_back( fGetObject<TH2D*>(f_206, "h2_bbcDec_C", Form("_nEv_C_%i",i))
                ->ProjectionX(Form("nEv_C_%i",i),x0[i],x1[i]));
        nEv_E.push_back( fGetObject<TH2D*>(f_206, "h2_bbcDec_E", Form("_nEv_E_%i",i))
                ->ProjectionX(Form("nEv_E_%i",i),x0[i],x1[i]));
    }

    for (int i{0};i<4;++i) {
        SetMCS(nEvents_C[i],shapes[i],colors[i]);

        nEvents_C[i]->Scale(1./nEvents_C[i]->Integral());
        nEvents_E[i]->Scale(1./nEvents_E[i]->Integral());
    }
    // plot the error boxes (make error = (orig - corr)
    
    SetYAxis(nEvents_C[0], "#frac{1}{N_{ev}}  #frac{dN_{ev}}{dEA_{est}}", -0.04, 0.19, 0.007);
    nEvents_C[0]->Draw("PE");

    for (int i=1;i<4;++i) {
        TH1D* err = static_cast<TH1D*>(nEvents_C[i]->Clone(Form("err_%i",i)));
		for (int k{1};k<=10;++k){
				err->SetBinContent(k, nEvents_E[i]->GetBinContent(k));
				err->SetBinError  (k, nEvents_E[i]->GetBinError  (k));
		}
        err->Add(nEvents_C[i],-1);
        DrawSysErrorBox(nEvents_C[i], err, colors[i]);
    }
    nEvents_C[1]->Draw("PE same");
    nEvents_C[2]->Draw("PE same");
    nEvents_C[3]->Draw("PE same");
    for (int i=0;i<4;++i) nEvents_C[i]->Write();


    //-----------------------
    //  TPad 3 - nch
    //-----------------------
    //  Get Systematic Errors from 0.2-15 GeV
    //     errors from cutting, errors from unfolding (add each)
    //     errors from difference in _orig and _corr
    //-----------------------
    c->cd(3);
    f_out->cd();
    vector<TH1D*> nch;      // original value (to use with nch_E_clip)
    vector<TH1D*> nch_clip; // to get percent error between orig and discretize+clip to 1-15 GeV
    vector<TH1D*> nch_dAu;  // unfolded dAu -- plotted value
    vector<TH1D*> nch_pp;   // unfolded pp : for error in pp vs dAu
    vector<TH1D*> nch_E_dAu;  // to get error between C and E in dAu

    nch.push_back( TH1_fromTProfile2D(f_004, "p2_nch_C", "nch_C_0") );
    for (int i{1};i<4;++i) nch.push_back(TH1_fromTProfile2D(f_206, "p2_nch_C", Form("nch_C_%i",i), x0[i],x1[i]));

    nch_clip.push_back( NchSumpt_fromTH1D(unf_004, "C_T0_clip", "nch_C_clip_0", nEv_C[0]) );
    for (int i{1};i<4;++i) nch_clip.push_back( NchSumpt_fromTH1D(
		unf_206, Form("C_T%i_clip",i), Form("nch_C_clip_%i",i), nEv_C[i]) );

	nch_dAu.push_back( NchSumpt_fromTH1D(unf_004, "C_T0_unf_dAu", "nch_dAu_0", nEv_C[0] ) );
    for (int i{1};i<4;++i) nch_dAu.push_back( NchSumpt_fromTH1D(
		unf_206, Form("C_T%i_unf_dAu",i), Form("nch_dAu_%i",i), nEv_C[i]) );

	nch_pp.push_back( NchSumpt_fromTH1D(unf_004, "C_T0_unf_pp", "nch_pp_0", nEv_C[0] ) );
    for (int i{1};i<4;++i) nch_pp.push_back( NchSumpt_fromTH1D(
		unf_206, Form("C_T%i_unf_pp",i), Form("nch_pp_%i",i), nEv_C[i]) );

	nch_E_dAu.push_back( NchSumpt_fromTH1D(unf_004, "E_T0_unf_dAu", "nch_E_dAu_0", nEv_E[0] ) );
    for (int i{1};i<4;++i) nch_E_dAu.push_back( NchSumpt_fromTH1D(
		unf_206, Form("E_T%i_unf_dAu",i), Form("nch_E_dAu_%i",i), nEv_E[i]) );

	for (int i{0};i<4;++i){
        SetMCS(nch_dAu[i],shapes[i],colors[i]);
        SetMCS(nch_clip[i],shapes[i],colors[i]);
	}

    SetYAxis(nch_dAu[0],"#LTN_{ch}#GT",5,32,0.007);
    nch_dAu[0]->Draw("PE");

	// draw all teh error boxes
	// calculte the various errors:
	// 1. percent error between nch and nch_clip
	// 2. total error between nch_pp and nch_dAu
	// 3. total error between nch_dAu C and nch_dAu E
    for (int i{0};i<4;++i) {
		//error in clipping and discretizing
		nch_clip[i]->Add(nch[i],-1);
		nch_clip[i]->Divide(nch[i]);
		nch_clip[i]->Multiply(nch_dAu[i]);

		//error in nch_pp and nch_dAu
		nch_pp[i]->Add(nch_dAu[i],-1);

		//error in nch_E_dAu and nch_dAu
		TH1D* nch_diff = static_cast<TH1D*>(nch_dAu[i]->Clone());
		for (int k{1};k<=10;++k){
				nch_diff->SetBinContent(k, nch_E_dAu[i]->GetBinContent(k));
				nch_diff->SetBinError  (k, nch_E_dAu[i]->GetBinError  (k));
		}
		nch_diff->Add(nch_dAu[i],-1);

		//sum up the errors in quadrature
		nch_clip[i]->Multiply(nch_clip[i]);
		nch_pp[i]->Multiply(nch_pp[i]);
		nch_diff->Multiply(nch_diff);
	
		nch_diff->Add(nch_clip[i]);
		nch_diff->Add(nch_pp[i]);
        DrawSysErrorBox(nch_dAu[i], nch_diff, colors[i], true);
	}

    // draw all the TH1Ds
    nch_dAu[1]->Draw("PE same");
    nch_dAu[2]->Draw("PE same");
    nch_dAu[3]->Draw("PE same");
    for (int i=0;i<4;++i) nch_dAu[i]->Write();

    //-----------------------
    //  TPad 4 - sumpt
    //-----------------------
    //  Get Systematic Errors from 0.2-15 GeV
    //     errors from cutting, errors from unfolding (add each)
    //     errors from difference in _orig and _corr
    //     same logic as for nch
    //-----------------------
    c->cd(4);
    f_out->cd();

    vector<TH1D*> sumpt;      // original value (to use with sumpt_E_clip)
    vector<TH1D*> sumpt_clip; // to get percent error between orig and discretize+clip to 1-15 GeV
    vector<TH1D*> sumpt_dAu;  // unfolded dAu -- plotted value
    vector<TH1D*> sumpt_pp;   // unfolded pp : for error in pp vs dAu
    vector<TH1D*> sumpt_E_dAu;  // to get error between C and E in dAu

    sumpt.push_back( TH1_fromTProfile2D(f_004, "p2_sumpt_C", "sumpt_C_0") );
    for (int i{1};i<4;++i) sumpt.push_back(TH1_fromTProfile2D(f_206, "p2_sumpt_C", Form("sumpt_C_%i",i), x0[i],x1[i]));

    sumpt_clip.push_back( NchSumpt_fromTH1D(unf_004, "C_T0_clip", "sumpt_C_clip_0", nEv_C[0],true) );
    for (int i{1};i<4;++i) sumpt_clip.push_back( NchSumpt_fromTH1D(
		unf_206, Form("C_T%i_clip",i), Form("sumpt_C_clip_%i",i), nEv_C[i],true) );

	sumpt_dAu.push_back( NchSumpt_fromTH1D(unf_004, "C_T0_unf_dAu", "sumpt_dAu_0", nEv_C[0] ,true) );
    for (int i{1};i<4;++i) sumpt_dAu.push_back( NchSumpt_fromTH1D(
		unf_206, Form("C_T%i_unf_dAu",i), Form("sumpt_dAu_%i",i), nEv_C[i],true) );

	sumpt_pp.push_back( NchSumpt_fromTH1D(unf_004, "C_T0_unf_pp", "sumpt_pp_0", nEv_C[0] ,true) );
    for (int i{1};i<4;++i) sumpt_pp.push_back( NchSumpt_fromTH1D(
		unf_206, Form("C_T%i_unf_pp",i), Form("sumpt_pp_%i",i), nEv_C[i],true) );
	
	sumpt_E_dAu.push_back( NchSumpt_fromTH1D(unf_004, "E_T0_unf_dAu", "sumpt_E_dAu_0", nEv_E[0] ,true) );
    for (int i{1};i<4;++i) sumpt_E_dAu.push_back( NchSumpt_fromTH1D(
		unf_206, Form("E_T%i_unf_dAu",i), Form("sumpt_E_dAu_%i",i), nEv_E[i],true) );

	for (int i{0};i<4;++i){
        SetMCS(sumpt_dAu[i],shapes[i],colors[i]);
        SetMCS(sumpt_clip[i],shapes[i],colors[i]);
	}

    SetYAxis(sumpt_dAu[0],"#LT#Sigma p_{T}^{ch}#GT (GeV/c)", 3.,25.,0.007);
    sumpt_dAu[0]->Draw("PE");

    for (int i{0};i<4;++i) {
		//error in clipping and discretizing
		sumpt_clip[i]->Add(sumpt[i],-1);
		sumpt_clip[i]->Divide(sumpt[i]);
		sumpt_clip[i]->Multiply(sumpt_dAu[i]);

		//error in sumpt_pp and sumpt_dAu
		sumpt_pp[i]->Add(sumpt_dAu[i],-1);

		//error in sumpt_E_dAu and sumpt_dAu
		TH1D* sumpt_diff = static_cast<TH1D*>(sumpt_dAu[i]->Clone());
		for (int k{1};k<=10;++k){
				sumpt_diff->SetBinContent(k, sumpt_E_dAu[i]->GetBinContent(k));
				sumpt_diff->SetBinError  (k, sumpt_E_dAu[i]->GetBinError  (k));
		}
		sumpt_diff->Add(sumpt_dAu[i],-1);

		//sum up the errors in quadrature
		sumpt_clip[i]->Multiply(sumpt_clip[i]);
		sumpt_pp[i]->Multiply(sumpt_pp[i]);
		sumpt_diff->Multiply(sumpt_diff);
	
		sumpt_diff->Add(sumpt_clip[i]);
		sumpt_diff->Add(sumpt_pp[i]);
        DrawSysErrorBox(sumpt_dAu[i], sumpt_diff, colors[i], true);
	}

    // draw all the TH1Ds
    sumpt_dAu[1]->Draw("PE same");
    sumpt_dAu[2]->Draw("PE same");
    sumpt_dAu[3]->Draw("PE same");
    for (int i=0;i<4;++i) sumpt_dAu[i]->Write();
    
    //-----------------------
    //  TPad 5 - sumpt
    //-----------------------
    //  Get Systematic Errors from 0.2-15 GeV
    //     errors from cutting, errors from unfolding (add each)
    //     errors from difference in _orig and _corr
    //-----------------------
    c->cd(5); 
    gPad->SetBottomMargin(0.24);
    f_out->cd();
   
    vector<TH1D*> maxpt_C;      // dAu
    vector<TH1D*> maxpt_E;   // for error
    maxpt_C.push_back( TH1_fromTProfile2D(f_004, "p2_maxpt_C", "maxpt_C_0") );
    maxpt_E.push_back( TH1_fromTProfile2D(f_004, "p2_maxpt_E", "maxpt_E_0") );
    for (int i{1};i<4;++i) {
        maxpt_C.push_back( TH1_fromTProfile2D(f_206, "p2_maxpt_C", Form("maxpt_C_%i",i),x0[i],x1[i]));
        maxpt_E.push_back( TH1_fromTProfile2D(f_206, "p2_maxpt_E", Form("maxpt_E_%i",i),x0[i],x1[i]));
    }
    for (int i{0};i<4;++i)  SetMCS(maxpt_C[i],shapes[i],colors[i]);

    SetYAxis(maxpt_C[0], "#LTmax(p_{T}^{ch})#GT (GeV/c)",.5,6.0, 0.007);
    maxpt_C[0]->GetXaxis()->SetTitle("Event Activity_{BBC}");
    maxpt_C[0]->GetXaxis()->SetTitleSize(0.10);
    maxpt_C[0]->GetXaxis()->SetTitleFont(42); 
    maxpt_C[0]->GetXaxis()->SetTitleOffset(1.); 
    maxpt_C[0]->GetXaxis()->SetLabelSize(0.10);
    maxpt_C[0]->GetXaxis()->SetLabelFont(42);
    maxpt_C[0]->GetXaxis()->SetLabelOffset(0.007);
    maxpt_C[0]->Draw("PE");
    maxpt_C[0]->Write();


    for (int i=0;i<4;++i) {
        TH1D* err = static_cast<TH1D*>(maxpt_C[i]->Clone(Form("maxpt__%i",i)));
		for (int k{1};k<=10;++k){
				err->SetBinContent(k, maxpt_E[i]->GetBinContent(k));
				err->SetBinError  (k, maxpt_E[i]->GetBinError  (k));
		}
        err->Add(maxpt_C[i],-1);
        DrawSysErrorBox(maxpt_C[i], err, colors[i]);
    }
    maxpt_C[1]->Draw("PE same");
    maxpt_C[2]->Draw("PE same");
    maxpt_C[3]->Draw("PE same");
    maxpt_C[1]->Write();
    maxpt_C[2]->Write();
    maxpt_C[3]->Write();

    c->SaveAs(Form("../pdf/Deciles_%s.pdf",tag));
    f_out->Save();

    TCanvas *d = new TCanvas("d");
    d->WaitPrimitive();
};
