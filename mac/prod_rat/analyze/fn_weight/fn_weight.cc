root -l <<EOF
    .L ../cc/inc_Tsallis.cc+
    int nbins = 150;

    TFile *fin  = new TFile("fn_weight.root", "update");
    const char* name[6]{"pi","pin","p","pbar","K","Kn"};
    const char* fmt_name[6]{"#pi","#pi^{-}","p","#bar{p}","#Kappa","#Kappa^{-}"};

    /* TNtuple* tup[6]; tup[0] = pi; //tup[1] = pin; tup[2] = p; tup[3] = pbar; tup[4] = K; tup[5]=Kn; */
    TGraphErrors* tge[6];
    TF1*          fit[6];
    TH1D*         abund[6];
    TH1D*         weight[6]; // relative 

    tie(tge[0],fit[0]) = tup_to_fit(pi);
    tie(tge[1],fit[1]) = tup_to_fit(pin);
    tie(tge[2],fit[2]) = tup_to_fit(p);
    tie(tge[3],fit[3]) = tup_to_fit(pbar);
    tie(tge[4],fit[4]) = tup_to_fit(K);
    tie(tge[5],fit[5]) = tup_to_fit(Kn);

    for (auto i{0}; i< 6; ++i){
        tge[i]->SetName(Form("%s_tup",name[i]));
        fit[i]->SetName(Form("%s_f1",name[i]));
        tge[i]->Write();
        fit[i]->Write();

        abund[i] = new TH1D(Form("%s_abund",name[i]),"Abundance weighting;p{T};#frac{d^{2}N}{2#pip_{T}d#etadp_{T}}",
                    nbins,0.,15.);
        TAxis* ax = abund[i] ->GetXaxis();
        for (int j{1}; j<nbins+1; ++j) {
            double val {           fit[i]->Integral( ax->GetBinLowEdge(j), ax->GetBinLowEdge(j+1) ) };
            abund[i]->SetBinContent(j, val);
        }
        abund[i] -> Write();
    }
    TH1D*   summ = (TH1D*) (abund[0])->Clone("sum_abund");
    summ->SetTitle("sum of abundances");
    summ->GetYaxis()->SetTitle("#Sigma_{i}#frac{d^{2}N_{i}}{2#pip_{T}d#etadp_{T}}");
    for (int i{1};i<6;++i) summ->Add(abund[i]);
    summ->Write();
    for (auto i{0}; i< 6; ++i){
        weight[i] = (TH1D*) abund[i]->Clone(Form("%s_weight",name[i]));
        weight[i]->GetYaxis()->SetTitle(Form("#frac{%s}{#Sigma_{i}n_{i}}",fmt_name[i]));
        weight[i]->Divide(summ);
        weight[i]->Write();
    }
    THStack *hs = new THStack("weight_stack","");
    weight[0]->SetFillColor(kBlue);
    weight[1]->SetFillColor(kBlue+1);
    weight[2]->SetFillColor(kRed);
    weight[3]->SetFillColor(kRed+1);
    weight[4]->SetFillColor(kMagenta);
    weight[5]->SetFillColor(kMagenta-2);
    TLegend *leg = new TLegend();
    for (auto i{0}; i< 6; ++i) { hs->Add(weight[i]); leg->AddEntry(weight[i], Form("%s",fmt_name[i])); }
    /* cout << hs->GetXaxis() << endl; */
    hs->Write();


    if (false) {
        TCanvas *c0 = new TCanvas("c0");
        hs->Draw();
        hs->GetXaxis()->SetTitle("p_{T}");
        hs->GetYaxis()->SetTitle("#frac{n_{i}}{#Sigma_{i}n_{i}}");
        leg->Draw("same");
        TCanvas *c1 = new TCanvas("c1");
        c1->WaitPrimitive();
    }
EOF

