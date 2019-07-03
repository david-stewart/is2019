TH2D* GetTH2D(const char* f_name, const char* name, TString setname=""){
    if (setname == "") setname = name;
    TH2D* hg;
    TFile *f = new TFile(f_name,"read");
    f->GetObject(name, hg);
    hg->SetName(setname);
    return hg;
}
/* void SetMCS(T* val, int marker, int col, double size=.9) { */
/*     val->SetMarkerStyle(marker); */
/*     val->SetMarkerColor(col); */
/*     val->SetLineColor  (col); */
/*     val->SetMarkerSize(size); */
/* }; */


void draw_jets(const char* tag=""){
    TH2D* hg2_hi  = GetTH2D(Form("hg_Jets%s.root",tag), "jetsHi");
    TH2D* hg2_lo  = GetTH2D(Form("hg_Jets%s.root",tag), "jetsLo");
    TH2D* hg2_all = GetTH2D(Form("hg_Jets%s.root",tag), "jetsAll");

    TFile *f = new TFile(Form("jets_8_16%s.root",tag),"recreate");
    
    vector<TH1D*> hi_16n; // negative deta
    vector<TH1D*> lo_16n;
    vector<TH1D*> all_16n;
    vector<TH1D*> rat_16n;

    vector<TH1D*> hi_16p; // positive deta
    vector<TH1D*> lo_16p;
    vector<TH1D*> all_16p;
    vector<TH1D*> rat_16p;

    vector<TH1D*> hi_8;
    vector<TH1D*> lo_8;
    vector<TH1D*> all_8;
    vector<TH1D*> rat_8;

    vector<const char*> hi_title;
    vector<const char*> lo_title;
    vector<const char*> all_title;

    // hi:
    //  kStar
    // lo:
    //  kSquare
    // all:
    //  kCircle

    for (int i=0;i<8;++i){
        hi_16n.push_back(hg2_hi  ->ProjectionX(Form("hi_16n_%i",i),i+1,i+1));
        lo_16n.push_back(hg2_lo  ->ProjectionX(Form("lo_16n_%i",i),i+1,i+1));
        all_16n.push_back(hg2_all->ProjectionX(Form("all_16n_%i",i),i+1,i+1));
        rat_16n.push_back( (TH1D*) hi_16n[i] ->Clone(Form("rat_16n_%i",i)));
        rat_16n[i]->Divide(lo_16n[i]);

        hi_16p.push_back(hg2_hi  ->ProjectionX(Form("hi_16p_%i",i),16-i,16-i));
        lo_16p.push_back(hg2_lo  ->ProjectionX(Form("lo_16p_%i",i),16-i,16-i));
        all_16p.push_back(hg2_all->ProjectionX(Form("all_16p_%i",i),16-i,16-i));
        rat_16p.push_back( (TH1D*) hi_16p[i] ->Clone(Form("rat_16p_%i",i)));
        rat_16p[i]->Divide(lo_16p[i]);

        hi_8.push_back( (TH1D*)  hi_16n[i]->Clone(Form("hi_8_%i",i)));
        hi_8[i]   ->Add( hi_16p[i]);

        lo_8.push_back( (TH1D*)  lo_16n[i]->Clone(Form("hi_8_%i",i)));
        lo_8[i]   ->Add( lo_16p[i]);

        all_8.push_back( (TH1D*)  all_16n[i]->Clone(Form("hi_8_%i",i)));
        all_8[i]   ->Add( all_16p[i]);

        rat_8.push_back( (TH1D*)  hi_8[i]->Clone(Form("rat_8_%i",i)));
        rat_8[i]->Divide( lo_8[i] ); 
    }

    /* TCanvas* c0 = new TCanvas("c0"); */
    /* gStyle->SetPalette(kBird); */
    /* hi_8[0]->Draw("PLC PE"); */
    /* colors.push_back(hi_8[0]->GetLineColor()); */

    /* int top = 6; */
    /* for (int i=1;i<top;++i){ */
    /*     hi_8[i]->Draw("PLC PE same"); */
    /* } */
    /* c0->WaitPrimitive(); */
    /* for (int i=0;i<top;++i){ */
    /*     cout << hi_8[i]->GetLineColor() << endl; */
    /* } */

    int col8[8] {924, 960, 996, 1032, 1068, 1104, 1140, 1176};
    int col4[4] {924, 1009, 1094, 1178};
    /* int col6[6] {924, 975, 1026, 1077, 1128, 1178}; */
    int col6[6] {924, 975, 1026, 1077, 1128, kRed};
    const char* lab[8] {
        "#Delta#phi_{jet to trig}(0-22.5)",
        "#Delta#phi_{jet to trig}(22.5-45)",
        "#Delta#phi_{jet to trig}(45-67.5)",
        "#Delta#phi_{jet to trig}(67.5-90)",
        "#Delta#phi_{jet to trig}(90-112.5)",
        "#Delta#phi_{jet to trig}(112.5-135)",
        "#Delta#phi_{jet to trig}(135-157.5)",
        "#Delta#phi_{jet to trig}(157.5-180)"
    };

    for (int i=0;i<8;++i){
        hi_16n[i]->SetMarkerStyle(kFullStar); 
        lo_16n[i]->SetMarkerStyle(kOpenSquare); 
        all_16n[i]->SetMarkerStyle(kOpenCircle); 
        rat_16n[i]->SetMarkerStyle(kOpenCross);

        /* hi_16n[i]->SetTitle(Form("0-30%%  -%s",lab[i])); */
        /* lo_16n[i]->SetTitle(Form("70-100%% -%s",lab[i])); */
        /* all_16n[i]->SetTitle(Form("All Events,  -%s",lab[i])); */
        /* rat_16n[i]->SetTitle(Form("-%s",lab[i])); */

        hi_16p[i]->SetMarkerStyle(kFullStar); 
        lo_16p[i]->SetMarkerStyle(kOpenSquare); 
        all_16p[i]->SetMarkerStyle(kOpenCircle); 
        rat_16p[i]->SetMarkerStyle(kOpenCross);

        /* hi_16p[i]->SetTitle(Form("0-30%%  +%s",lab[i])); */
        /* lo_16p[i]->SetTitle(Form("70-100%%  +%s",lab[i])); */
        /* all_16p[i]->SetTitle(Form("All Events +%s",lab[i])); */
        /* rat_16p[i]->SetTitle(Form("+%s",lab[i])); */

        hi_8[i]->SetMarkerStyle(kFullStar); 
        lo_8[i]->SetMarkerStyle(kOpenSquare); 
        all_8[i]->SetMarkerStyle(kOpenCircle); 
        rat_8[i]->SetMarkerStyle(kOpenCross);

        /* hi_8[i]->SetTitle(Form("0-30%%  %s",lab[i])); */
        /* lo_8[i]->SetTitle(Form("70-100%%  %s",lab[i])); */
        /* all_8[i]->SetTitle(Form("All Events  %s",lab[i])); */
        /* rat_8[i]->SetTitle(Form("%s",lab[i])); */

        hi_title.push_back(Form("0-30%%  %s"  ,lab[i]));
        lo_title.push_back(Form("70-100%% %s",lab[i]));
        all_title.push_back(Form("All events  %s",lab[i]));

        if (i>1){
            hi_8[i] ->SetMarkerColor(col6[i-2]);
            lo_8[i] ->SetMarkerColor(col6[i-2]);
            all_8[i]->SetMarkerColor(col6[i-2]);
            rat_8[i]->SetMarkerColor(col6[i-2]);

            hi_8[i] ->SetLineColor(col6[i-2]);
            lo_8[i] ->SetLineColor(col6[i-2]);
            all_8[i]->SetLineColor(col6[i-2]);
            rat_8[i]->SetLineColor(col6[i-2]);
        }
    }

    hi_8[2]->GetXaxis()->SetTitle("Jet p_{T}");
    rat_8[2]->GetXaxis()->SetTitle("Jet p_{T}");
    hi_8[2]->GetYaxis()->SetTitle("N_{jets}/N_{trig}");
    rat_8[2]->GetYaxis()->SetTitle("ratio (0-30%)/(70-100%)");

    TCanvas *c = new TCanvas("c");
    c->Divide(1,2,0.1,0);
    c->cd(1);
    gPad->SetLogy();
    TLegend *leg = new TLegend(0.6668419,0.007816,0.9963197,0.994026,NULL,"brNDC");
    bool first=true;
    for (int i=2;i<8;++i){
        if (first) {
            hi_8[i]->SetStats(0);
            hi_8[i]->SetTitle("");
            hi_8[i]->Draw("PE");
        } else {
            hi_8[i]->Draw("PE same");
        }
        lo_8[i]->Draw("same");
        all_8[i]->Draw("same");

        leg->AddEntry(hi_8[i],Form("0-30%%EA, %s",lab[i]));
        if (first) leg->AddEntry(lo_8[i],Form("70-100%%EA, %s",lab[i]));
        if (first) leg->AddEntry(all_8[i],Form("all EA, %s",lab[i]));

        first = false;
    }
    leg->Draw("same");

    c->cd(2);

    leg = new TLegend(0.6659657,0.09725272,0.9980722,0.9944875,NULL,"brNDC");
    first = true;
    for (int i=2;i<8;++i){
        if (first) {
            first = false;
            rat_8[i]->SetStats(0);
            rat_8[i]->SetTitle("");
            rat_8[i]->Draw("PE");
        } else {
            rat_8[i]->Draw("PE same");
        }
        leg->AddEntry(rat_8[i],lab[i]);
    }
    leg->Draw("same");
    TCanvas *c2 = new TCanvas("c2");
    /* c2->Draw(); */
    c2->WaitPrimitive();

    f->Write();
    f->Close();
}
