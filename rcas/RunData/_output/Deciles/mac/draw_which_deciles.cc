void myDrawLines(TAxis* edges, double min_val, double max_val) {
    for (int i=2;i<11;++i){
        TLine* line = new TLine(edges->GetBinLowEdge(i),min_val,
                                edges->GetBinLowEdge(i),max_val);
        /* line->SetLineColor(kBlue-10); */
        line->SetLineColor(kGray+2);
        line->SetLineStyle(2);
        line->SetLineWidth(1);
        line->Draw("same");
    }
};

void draw_which_deciles(){
    TFile *f = new TFile("../004_7-24k_nchRaw.root","read");
    TH2D* hg2, *dec;
    f->GetObject("h2_bbcDist_C", hg2);

    TH1D* distr = hg2->ProjectionX();
    distr->Rebin(100);
    /* TCanvas *c = new TCanvas("c"); */
    TCanvas *c = new TCanvas("c", "",10,45,1518,400);
    c->SetLogy();

    distr->Scale(1./distr->Integral("width"));
    distr->GetXaxis()->SetTitle("Event Activity_{BBC}");
    distr->GetXaxis()->SetTitle("Event Activity_{BBC}");
    distr->GetXaxis()->SetRange(1,105000);
    distr->GetXaxis()->SetLabelFont(42);
    distr->GetXaxis()->SetLabelSize(0.07);
    distr->GetXaxis()->SetTitleSize(0.07);
    distr->GetXaxis()->SetTitleOffset(1.02);
    distr->GetXaxis()->SetTitleFont(42);
    distr->GetYaxis()->SetTitle("#frac{1}{N_{ev}}  #frac{dN_{ev}}{dEA_{BBC}}");
    distr->GetYaxis()->SetLabelFont(42);
    distr->GetYaxis()->SetLabelOffset(0.007);
    distr->GetYaxis()->SetLabelSize(0.06);
    distr->GetYaxis()->SetTitleSize(0.07);
    distr->GetYaxis()->SetTitleOffset(0.5);
    distr->GetYaxis()->SetTitleFont(42);
    distr->GetZaxis()->SetLabelFont(42);
    distr->GetZaxis()->SetLabelSize(0.035);
    distr->GetZaxis()->SetTitleSize(0.035);
    distr->GetZaxis()->SetTitleOffset(1);
    distr->GetZaxis()->SetTitleFont(42);
    distr->GetYaxis()->SetRangeUser(9e-10,5e-5);
    distr->SetStats(0);
    distr->Draw();

    f->GetObject("h2_bbcDec_C", dec);
    TAxis *edges = dec->ProjectionX()->GetXaxis();
    myDrawLines(edges, 9e-10,5e-5);

}
