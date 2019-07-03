#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLine.h>
#include <TPie.h>
#include <TText.h>
#include <THStack.h>
#include <TMath.h>
#include <iostream>
const static double diamondsize = 1.8;
TH1D* makeRatio(TH1D* hi, TH1D* lo, int nbins = 0, double* edges=nullptr){
    if (nbins == 0) {
        TH1D* rat =  static_cast<TH1D*>(hi->Clone(Form("%s__clone"  ,hi->GetName()))); 
        rat->Divide(lo);
        return rat;
    } else {
        TH1D* hi_rebin = static_cast<TH1D*>(hi->Rebin(nbins, Form("%s__hirebin",hi->GetName()), edges));
        TH1D* lo_rebin = static_cast<TH1D*>(lo->Rebin(nbins, Form("%s__hirebin",lo->GetName()), edges));
        hi_rebin->Divide(lo_rebin);
        return hi_rebin;
    }
}

template <typename T>
void SetMCS(T* val, int marker, int col, double size=1.3) {
    if (marker == kOpenDoubleDiamond || marker == kFullDoubleDiamond) size = diamondsize;
    val->SetMarkerStyle(marker);
    val->SetMarkerColor(col);
    val->SetLineColor  (col);
    val->SetMarkerSize(size);
};

vector<TH1D*> getStripes8(TH2D* hg, const char* name, int shapes[8], int colors[8], double sizes[8]){
    // make a histogram of TH1 in each of the phi bins of hg, starting with the intermost bins
    // (i.e. 8 & 9, then 7 & 10 ... 1 & 16
    TAxis* xpt  = hg->GetXaxis();
    int    npt = xpt->GetNbins();
    TAxis* yphi = hg->GetYaxis();
    int    nphi = yphi->GetNbins();
    vector<TH1D*> vec {};
    for (int phi=0;phi<8;++phi) {
        int i_first = 8-phi;
        int i_second = 9+phi;
        const char* l_title     = Form("|phi(%.2f,%.2f)|;pT;N", yphi->GetBinLowEdge(i_first),yphi->GetBinLowEdge(i_first+1));
        const char* l_name      = Form("%s_%i",name,phi);
        const char* l_name_add  = Form("%s_%i_add",name,phi);
        TH1D* first =  hg->ProjectionX(l_name,i_first,i_first);
        TH1D* second = hg->ProjectionX(l_name_add,i_second,i_second);
        first->Add(second);
        SetMCS(first,shapes[phi],colors[phi],sizes[phi]);
        vec.push_back(first);
        vec[phi]->SetTitle(l_title);
    }
    int last = vec.size()-1;
    SetMCS(vec[0],shapes[0],colors[0]);
    return vec;
}

void drawPie(int colors[8]){
    int c_16[16];
    double vals[16];
    const char* labels[16];
    for (int i=0;i<8;++i) { 
        c_16[i] = colors[i]; 
        c_16[15-i] = colors[i]; 
        labels[i]    = Form("#frac{%i}{8}#pi", i+1);
        labels[15-i] = Form("- #frac{%i}{8}#pi", i+1);

        vals[i]=1.;
        vals[i+8]=1.;
    }
    labels[7] = "-#pi";
    labels[8] = " #pi";
    //roll the pie
    int r_colors[16];
    const char* r_labels[16];
    int roll = 9;
    for (int i=0;i<16;++i){
        int j = (i+roll)%16;
        r_colors[j] = c_16[i];
        r_labels[j] = labels[i];
    }
   TCanvas *c1 = new TCanvas("c1", "",10,67,785,757);
    TPie* pie = new TPie("pie","",16,vals,r_colors,r_labels);
    pie->SetTextFont(42);
    pie->SetLabelsOffset(.02);
    pie->Draw("r");
    /* labels[7] = "#pi"; */
    /* labels[8] = "-#pi"; */
}

void drawStripes2D (TH2D* h, const char* name, int colors[8]){
    TAxis *x = h->GetXaxis();
    int nx = x->GetNbins();

    // make 8 different TH2D's, and zero out all but the i and 16-i stripe,
    // and plot them with the colors given;
    THStack* stack = new THStack("stack",name);
    for (int i=0;i<8;++i){
        TH2D* z = new TH2D(Form("%s_stripe_%i",name,i), ";;", nx, x->GetBinLowEdge(1), x->GetBinLowEdge(nx+1),
                16, 0, 2*TMath::Pi());
        for (int pt=0;pt<nx;++pt){
            int i0 = i+1;
            int i1 = 16-i;
            z->SetBinContent(pt,i0,h->GetBinContent(pt,i0));
            z->SetBinError  (pt,i0,h->GetBinError  (pt,i0));
            z->SetBinContent(pt,i1,h->GetBinContent(pt,i1));
            z->SetBinError  (pt,i1,h->GetBinError  (pt,i1));
        }
        z->SetLineColor(kBlack);
        z->SetFillColor(colors[i]);
        z->SetMarkerColor(colors[i]);
        stack->Add(z);

        /* if (i==0) { z->Draw("box"); gPad->SetLogz(); } */
        /* else      z->Draw("box same"); */
    }
    TCanvas *c1 = new TCanvas("c1");
    c1->SetLogz();
    stack->Draw("lego4");
    /* stack->GetXaxis()->SetRangeUser(1e-5,.6); */
    /* stack->Draw("lego1"); */
    /* c1->Update(); */
    return;
}

void sets_hg(int which_set=0, const char* tag=""){

    TFile* f_in = new TFile(Form("hg_Jets%s.root",tag),"read");
    TH2D *hg2_hi, *hg2_lo, *hg2_all;

    f_in->GetObject("jetsHi", hg2_hi);
    f_in->GetObject("jetsLo", hg2_lo);

    /* int col8[8] {924, 960, 996, 1032, 1068, 1104, 1140, 1176}; */
    int c_8[8] {924, 960, 996, 1032, 1068, 1104, 1140, kRed};
    /* int black_8[8] {kBlack,kBlack,kBlack,kBlack,kBlack,kBlack,kBlack,kBlack}; */
    int m_8_lo[8] {kOpenCross, kOpenSquare, kOpenTriangleDown, kOpenCrossX, kOpenDiamond, kOpenStar, kOpenTriangleUp, kOpenCircle};
    int m_8_hi[8] {kFullCross, kFullSquare, kFullTriangleDown, kFullCrossX, kFullDiamond, kFullStar, kFullTriangleUp, kFullCircle};
    double sizes[9] {1.3,       1.3,                 1.3,           1.3,         1.7,          1.7,       1.3,         1.3};
    int col4[4] {924, 1009, 1094, 1178};

    vector<TH1D*> hi_8  = getStripes8(hg2_hi,"hi", m_8_hi, c_8,sizes);
    vector<TH1D*> lo_8  = getStripes8(hg2_lo,"lo", m_8_lo, c_8,sizes);
    /* vector<TH1D*> all_8 = getStripes8(hg2_all,"all",m_8_lo, black_8,sizes); */


    //-------------------------------------------------
    // Draw a pie color chart
    //-------------------------------------------------
    if (false) drawPie(c_8);

    //-------------------------------------------------
    // Draw a TH2D color chart
    //-------------------------------------------------
    /* if (false) drawStripes2D(ratio, "ratio", c_8); */
    
    //-------------------------------------------------
    // Plot the axes
    //-------------------------------------------------
   TCanvas *c1 = new TCanvas("c1", "",10,67,785,757);
   c1->Range(0,0,1,1);
   c1->SetFillColor(0);
   c1->SetBorderMode(0);
   c1->SetBorderSize(0);
   c1->SetFrameBorderMode(0);

   TPad *pad1 = new TPad("pad1", "pad1",0,0.4981685,0.9983498,0.997558);
   pad1->Draw();
   pad1->cd();
   pad1->Range(-5.781388,-6.723629,38.09586,0);
   pad1->SetFillColor(0);
   pad1->SetBorderMode(0);
   pad1->SetBorderSize(2);
   pad1->SetLogy();
   pad1->SetLeftMargin(0.1317628);
   pad1->SetRightMargin(0.002184823);
   pad1->SetTopMargin(0);
   pad1->SetBottomMargin(0.003667478);
   pad1->SetFrameBorderMode(0);
   pad1->SetFrameBorderMode(0);

    //---------------------------------------
    // rebinning
    vector<int> plots;
    double *bot_edge[8];
    /* int     bot_nbins[8]{ 27, 21, 16, 10, 10, 8, 9, 27 }; */
    int     bot_nbins[8]{ 27, 9, 8, 10, 10, 16, 21, 27 };

    double *top_edge[8];
    int    *top_nbins = new int[8];
    for (int i{0};i<8;++i){
        top_edge[i] = new double[39];
        top_nbins[i] = 38;
        for (int j{0};j<39;++j){
            top_edge[i][j] = j*1.0;
        }
        bot_edge[i] = new double[22] {0,1,2,3,4,5,6,7,8,9,10,11,12,13,15,17,19,21,23,25,28,38};
        bot_nbins[i] = 21;
    }

    // seven
    int draw = -1;
    top_edge[7] = new double[28] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15,16,17,18,19, 20,21,23,25,27,29,33,37};
    top_nbins[7] = 27;

    top_edge[6] = new double[21] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15,17,21,25,29,37};
    top_nbins[6] = 20;

    top_edge[5] = new double[18] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,17,20,23};
    top_nbins[5] = 17;

    top_edge[4] = new double[16] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,14,17,20};
    top_nbins[4] = 15;
    
    top_edge[3] = new double[17] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,14,17,20,24};
    top_nbins[3] = 16;
    
    top_edge[2] = new double[14] { 0, 1, 2, 3, 4, 5, 6, 7, 8,10,12,15,18,21};
    top_nbins[2] = 13;
    
    top_edge[1] = new double[14] { 0, 1, 2, 3, 4, 5, 6, 7, 8,10,12,15,18,21};
    top_nbins[1] = 13;

    top_edge[0] = new double[28] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15,16,17,18,19, 20,21,23,25,27,29,33,38};
    top_nbins[0] = 27;



    double *save = new double[39] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15,16,17,18,19, 20,21,22,23,24,25,26,27,28,29, 30,31,32,33,34,35,36,37,38};
    int n_save = 38;

    bot_edge[6] = new double[17] {0,1,2,3,4,5,6,7,8,9,10,11,12,15,18,21,27};
    bot_nbins[6] = 16;

    bot_edge[5] = new double[14] {0,1,2,3,4,5,6,7,8,9,10,12,15,27};
    bot_nbins[5] = 13;

    bot_edge[4] = new double[12] {0,1,2,3,4,5,6,7,8,9,11,15};
    bot_nbins[4] = 11;

    bot_edge[3] = new double[10] {0,1,2,3,4,5,6,8,11,16};
    bot_nbins[3] = 9;

    bot_edge[2] = new double[10] {0,1,2,3,4,5,6,9,14,20};
    bot_nbins[2] = 9;

    bot_edge[1] = new double[10] {0,1,2,3,4,5,6,10,15,21};
    bot_nbins[1] = 9;

    /* bot_edge[3] = new double[10] {0,1,2,3,4,5,6,9,14,20}; */
    /* bot_nbins[2] = 9; */

    switch (which_set){
        case 0:
        case 3:
            plots = vector<int> {1,2,3,4,7};
            break;
        case 1:
        case 4:
            /* plots = vector<int> {1,2,3,4,5,6,7}; */
            plots = vector<int> {1,2,3,4,7};
            break;
        case 2:
        case 5:
            /* plots = vector<int> {0,1,2,3,4,5,6,7}; */
            plots = vector<int> {0,1,2,3,4,7};
            break;
    }

    switch (which_set){
        case 101:
            plots = vector<int> {0,1,2,3,4,5,6,7};
        break;

        case 0: // 7, 1,2,3,4 only up to 9 GeV
        case 1:
        case 2:

            top_edge[4] = new double[10] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            top_nbins[4] = 9;
            
            top_edge[3] = new double[10] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            top_nbins[3] = 9;
            
            top_edge[2] = new double[9] { 0, 1, 2, 3, 4, 5, 6, 7, 8};
            top_nbins[2] = 8;
            
            top_edge[1] = new double[9] { 0, 1, 2, 3, 4, 5, 6, 7, 8};
            top_nbins[1] = 8;

            top_edge[7] = new double[10] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            top_nbins[7] = 9;


            bot_edge[4] = new double[10] {0,1,2,3,4,5,6,7,8,9};
            bot_nbins[4] = 9;

            bot_edge[3] = new double[8] {0,1,2,3,4,5,6,8};
            bot_nbins[3] = 7;

            bot_edge[2] = new double[8] {0,1,2,3,4,5,6,9};
            bot_nbins[2] = 7;

            bot_edge[1] = new double[7] {0,1,2,3,4,5,6};
            bot_nbins[1] = 6;

            bot_edge[7] = new double[10] {0,1,2,3,4,5,6,7,8,9};
            bot_nbins[7] = 9;

            bot_edge[6] = new double[10] {0,1,2,3,4,5,6,7,8,9};
            bot_nbins[6] = 9;

            bot_edge[5] = new double[10] {0,1,2,3,4,5,6,7,8,9};
            bot_nbins[5] = 9;

            top_edge[6] = new double[10] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            top_nbins[6] = 9;

            top_edge[5] = new double[10] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            top_nbins[5] = 9;

            top_edge[0] = new double[10] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            top_nbins[0] = 9;

            bot_edge[0] = new double[10] {0,1,2,3,4,5,6,7,8,9};
            bot_nbins[0] = 9;


            /* bot_nbins = new int[8]{ 27, 21, 16, 10, 10, 8, 9, 27 }; */
        break;
        /* /1* case 1: // away side only, up to 9 *1/ */
        /* /1*     plots = vector<int> { 6, 7 }; *1/ */
        /* /1*     for (auto i : plots) { *1/ */
        /* /1*         top_nbins[i] = 9; *1/ */
        /* /1*         for (int j=0; j<10;++j) top_edge[i][j] = j*1.0; *1/ */
        /* /1*     } *1/ */
        /* /1*     bot_nbins[7]=9; *1/ */
        /* /1* break; *1/ */
        /* case 2: // away side only, up to 9 */
        /*     plots = vector<int> { 3,4,5, 6, 7 }; */
        /*     for (auto i : plots) { */
        /*         top_nbins[i] = 9; */
        /*         for (int j=0; j<10;++j) top_edge[i][j] = j*1.0; */
        /*     } */
        /*     bot_nbins[3] = 8; */
        /*     bot_nbins[4] = 8; */
        /*     bot_nbins[5] = 7; */
        /*     bot_nbins[7] = 9; */
        /* break; */
        /* case 3: // away side only, up to 9 */
        /*     plots = vector<int> { 1,2,3,4,5, 6, 7 }; */
        /*     for (auto i : plots) { */
        /*         top_nbins[i] = 9; */
        /*         for (int j=0; j<10;++j) top_edge[i][j] = j*1.0; */
        /*     } */
        /*     bot_nbins[3] = 8; */
        /*     bot_nbins[4] = 8; */
        /*     bot_nbins[5] = 7; */
        /*     bot_nbins[7]=9; */
        /*     bot_nbins[2]=9; */
        /*     bot_nbins[1]=9; */
        /* break; */
        /* case 4: // away side only, up to 9 */
        /*     plots = vector<int> { 0,1,2,3,4,5, 6, 7 }; */
        /*     for (auto i : plots) { */
        /*         top_nbins[i] = 9; */
        /*         for (int j=0; j<10;++j) top_edge[i][j] = j*1.0; */
        /*     } */
        /*     bot_nbins[3] = 8; */
        /*     bot_nbins[4] = 8; */
        /*     bot_nbins[5] = 7; */
        /*     bot_nbins[7]=9; */
        /*     bot_nbins[2]=9; */
        /*     bot_nbins[1]=9; */
        /*     bot_nbins[0]=9; */
        /* break; */
        /* case 5: // away side only, up to 9 */
        /*     plots = vector<int> { 4,5, 6, 7 }; */
        /* break; */
        /* case 6: // away side only, up to 9 */
        /*     plots = vector<int> { 0, 3,4,5, 6, 7 }; */
        /* break; */
        /* case 7: // away side only, up to 9 */
        /*     plots = vector<int> {0,1,2,3,4,5,6,7}; */
        /* break; */
    }

    hi_8[0]->SetMarkerSize(diamondsize);
    lo_8[0]->SetMarkerSize(diamondsize);


    bool first{true};
    for (auto i : plots) {
    /* for (int i=0;i<8;++i) { */
        if (draw != -1 && i!=draw) continue;
        TH1D* hi = static_cast<TH1D*>(hi_8[i]->Rebin(top_nbins[i],Form("hi__%i",i),top_edge[i]));
        TH1D* lo = static_cast<TH1D*>(lo_8[i]->Rebin(top_nbins[i],Form("lo__%i",i),top_edge[i]));

        // reweight the bins according to bin width
        for (int j=1;j<=top_nbins[i];++j){
            double w { hi->GetBinWidth(j) };
            hi->SetBinContent(j, hi->GetBinContent(j)/w);
            hi->SetBinError  (j, hi->GetBinError  (j)/w);

            lo->SetBinContent(j, lo->GetBinContent(j)/w);
            lo->SetBinError  (j, lo->GetBinError  (j)/w);
        }

        
        if (first) {
            first = false; 
            TH1D* blank = new TH1D("blank",";;",45,0.,45);
            blank->SetMarkerStyle(34);
            blank->SetMarkerSize(1.3);
            blank->GetXaxis()->SetTitle("pT");
            blank->GetXaxis()->SetRange(1,38);
            blank->GetXaxis()->SetLabelFont(42);
            blank->GetXaxis()->SetLabelSize(0.035);
            blank->GetXaxis()->SetTitleSize(0.035);
            blank->GetXaxis()->SetTitleOffset(1);
            blank->GetXaxis()->SetTitleFont(42);
            blank->GetYaxis()->SetTitle("d(N^{ch}_{jets-raw}/N_{trig})/dp^{ch}_{T,jets-raw}");
            blank->GetYaxis()->SetLabelFont(42);
            blank->GetYaxis()->SetLabelOffset(0);
            blank->GetYaxis()->SetLabelSize(0.06);
            blank->GetYaxis()->SetTitleSize(0.06);
            blank->GetYaxis()->SetTitleOffset(0.83);
            blank->GetYaxis()->SetTitleFont(42);
            blank->GetYaxis()->SetRangeUser(2.e-7,1);
            blank->GetZaxis()->SetLabelFont(42);
            blank->GetZaxis()->SetLabelSize(0.035);
            blank->GetZaxis()->SetTitleSize(0.035);
            blank->GetZaxis()->SetTitleOffset(1);
            blank->GetZaxis()->SetTitleFont(42);
            blank->SetStats(0);
            blank->SetTitle("");
            blank->Draw("PE");
        
        }
        hi->Draw("PE same");
        lo->Draw("PE same");
    }
   TText *text = new TText(25.9,0.5250454,"Detector level uncorrected");
   text->SetTextFont(42);
   text->SetTextSize(0.04878063);
   text->Draw();

   text = new TText(25.9,0.2548169,"Open Markers: 70-100% EA");
   text->SetTextFont(42);
   text->SetTextSize(0.04878063);
   text->Draw();

   text = new TText(25.9,0.1179758,"Full Markers:      0 - 30% EA");
   text->SetTextFont(42);
   text->SetTextSize(0.04878063);
   text->Draw();
   TLegend *L = new TLegend(0.7470821,0.3246073,0.9952567,0.8457916,NULL,"brNDC");
   const char* leg_entries[8] {
              "#left|#Delta#phi#right|<(1/8)#pi",
     "(1/8)#pi< #left|#Delta#phi#right|<(2/8)#pi",
     "(2/8)#pi< #left|#Delta#phi#right|<(3/8)#pi",
     "(3/8)#pi< #left|#Delta#phi#right|<(4/8)#pi",
     "(4/8)#pi< #left|#Delta#phi#right|<(5/8)#pi",
     "(5/8)#pi< #left|#Delta#phi#right|<(6/8)#pi",
     "(6/8)#pi< #left|#Delta#phi#right|<(7/8)#pi",
     "(7/8)#pi< #left|#Delta#phi#right|<#pi"};
    L->SetLineColor(4000);
    L->SetFillColor(4000);
    int psize =plots.size();
    for (int i=0; i<psize;++i) L->AddEntry(hi_8[plots[psize-i-1]], leg_entries[plots[psize-i-1]]);
    L->Draw();

    c1->cd();
    /* pad2->Draw(); */
   /* TPad *pad1 = new TPad("pad1", "pad1",0,0.4981685,0.9983498,0.997558); */
   TPad *pad2 = new TPad("pad2", "pad2",0,0,1,0.5);
   pad2->Draw();
   pad2->cd();
   pad2->Range(-5.781388,-0.5518394,38.16839,2.5);
   pad2->SetFillColor(0);
   pad2->SetBorderMode(0);
   pad2->SetBorderSize(2);
   pad2->SetLeftMargin(0.1315453);
   pad2->SetRightMargin(0.003831418);
   pad2->SetTopMargin(1.455153e-16);
   pad2->SetBottomMargin(0.1808219);
   pad2->SetFrameBorderMode(0);
   pad2->SetFrameBorderMode(0);

    //rebin the ratios
    TH1D *blank__17 = new TH1D("blank__17","",45,0,45);
    blank__17->SetMinimum(0);
    blank__17->SetMaximum(2.5);
    blank__17->SetStats(0);

    /* Int_t ci;      // for color index setting */
    /* ci = TColor::GetColor("#352a86"); */
    /* hi_1__hirebin__18->SetLineColor(ci); */
    /* ci = TColor::GetColor("#000099"); */
    /* blank__17->SetLineColor(ci); */
    blank__17->GetXaxis()->SetTitle("p^{ch}_{T,jets-raw}");
    blank__17->GetXaxis()->SetRange(1,38);
    blank__17->GetXaxis()->SetLabelFont(42);
    blank__17->GetXaxis()->SetLabelOffset(0.02);
    blank__17->GetXaxis()->SetLabelSize(0.06);
    blank__17->GetXaxis()->SetTitleSize(0.06);
    blank__17->GetXaxis()->SetTitleOffset(1.15);
    blank__17->GetXaxis()->SetTitleFont(42);
    blank__17->GetYaxis()->SetTitle("#frac{ d(N^{ch}_{jets-raw}/N_{trig})_{EA(0-30%)   }}{ d(N^{ch}_{jets-raw}/N_{trig})_{EA(70-100%) }}");
    blank__17->GetYaxis()->SetLabelFont(42);
    blank__17->GetYaxis()->SetLabelOffset(0.006);
    blank__17->GetYaxis()->SetLabelSize(0.06);
    blank__17->GetYaxis()->SetTitleSize(0.06);
    blank__17->GetYaxis()->SetTitleOffset(0.89);
    blank__17->GetYaxis()->SetTitleFont(42);
    blank__17->GetZaxis()->SetLabelFont(42);
    blank__17->GetZaxis()->SetLabelSize(0.035);
    blank__17->GetZaxis()->SetTitleSize(0.035);
    blank__17->GetZaxis()->SetTitleOffset(1);
    blank__17->GetZaxis()->SetTitleFont(42);
    blank__17->Draw("PE");
    TLine *line = new TLine(0,1.,38,1.);
    line->SetLineColor(kGray+3);
    line->SetLineStyle(2);
    line->SetLineWidth(1);
    line->Draw();

    line = new TLine(0,.5,38,0.5);
    line->SetLineColor(kRed-9);
    line->SetLineStyle(2);
    line->SetLineWidth(1);
    line->Draw("same");
    line = new TLine(0,1.5,38,1.5);
    line->SetLineColor(kRed-9);
    line->SetLineStyle(2);
    line->SetLineWidth(1);
    line->Draw("same");
    /* line = new TLine(0,2.,38,2.); */
    /* line->SetLineColor(kRed-9); */
    /* line->SetLineStyle(2); */
    /* line->SetLineWidth(1); */
    /* line->Draw("same"); */


    /* draw = 5; */
    /* for (int i=0;i<8;++i) { */
    for (auto i : plots) {
        if (draw != -1 && i!=draw) continue;
        TH1D* rat = makeRatio(hi_8[i], lo_8[i], bot_nbins[i], bot_edge[i]);
        rat->Draw("PE same");
    }

    c1->SaveAs(Form("pdf_sets_hg/sets_%s%i.pdf",tag,which_set));
    /* c1->WaitPrimitive(); */

    return;
}
