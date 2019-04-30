root -l<<EOF
    TH1D* pi;

    TH1D* K;
    TH1D* antiK;
    TH1D* p;
    TH1D* pbar;
    TH1D* antipi;

    TFile *f;

    f = new TFile("pi.root","READ");
    f->GetObject("all_E",pi);

    f = new TFile("K.root","READ");
    f->GetObject("all_E",K);

    f = new TFile("anti_K.root","READ");
    f->GetObject("all_E",antiK);

    f = new TFile("p.root","READ");
    f->GetObject("all_E",p);

    f = new TFile("pbar.root","READ");
    f->GetObject("all_E",pbar);

    f = new TFile("anti_pi.root","READ");
    f->GetObject("all_E",antipi);

    float s{0.5};

    K->SetMarkerStyle(kOpenSquare);
    K->SetMarkerColor(kBlue);
    K->SetLineColor(kBlue);
    K->SetMarkerSize(s);

    antiK->SetMarkerStyle(kOpenSquare);
    antiK->SetMarkerColor(kRed);
    antiK->SetLineColor(kRed);
    antiK->SetMarkerSize(s);

    p->SetMarkerStyle(kFullCircle);
    p->SetMarkerColor(kCyan+2);
    p->SetLineColor(kCyan+2);
    p->SetMarkerSize(s);

    pbar->SetMarkerStyle(kOpenCircle);
    pbar->SetMarkerColor(kMagenta);
    pbar->SetLineColor(kMagenta);
    pbar->SetMarkerSize(s);

    antipi->SetMarkerStyle(kOpenStar);
    antipi->SetMarkerColor(kBlack);
    antipi->SetLineColor(kBlack);
    antipi->SetMarkerSize(s);

    pi->SetMarkerStyle(kFullStar);
    pi->SetMarkerColor(kBlack);
    pi->SetLineColor(kBlack);
    pi->SetMarkerSize(s);

    
    gStyle->SetOptStat(0);

    double w = 470;
    double h = 700;
    TCanvas *c = new TCanvas("c1", "canvas", w, h);
    c->SetWindowSize(w + (w - c->GetWw()), h + (h - c->GetWh()));
    c->cd();
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.50, 1, 1.0);
    pad1->SetBottomMargin(0); // Upper and lower plot are joined
    pad1->SetLeftMargin(0.11); // Upper and lower plot are joined
    pad1->SetRightMargin(0.01); // Upper and lower plot are joined
    pad1->Draw();             // Draw the upper pad: pad1

    c->cd();
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0.0, 1, 0.50);
    pad2->SetBottomMargin(0.25); // Upper and lower plot are joined
    pad2->SetLeftMargin(0.11); // Upper and lower plot are joined
    pad2->SetRightMargin(0.01); // Upper and lower plot are joined
    pad2->SetTopMargin(0); // Upper and lower plot are joined
    pad2->Draw();             // Draw the upper pad: pad1

   /* TLegend *leg = new TLegend(0.2180451,0.1129032,0.6942356,0.5451613,NULL,"brNDC"); */
   pad1->cd();
   /* TLegend *leg = new TLegend();//0.2180451,0.1129032,0.6942356,0.5451613,NULL,"brNDC"); */
   TLegend *leg = new TLegend(0.3808511,0.08857143,0.9446809,0.66,NULL,"brNDC");

    /* pi->SetMarkerColor(kBlue); */
    /* pi->SetLineColor(kBlue); */
    /* pi->SetMarkerStyle(kOpenSquare); */
    pi->GetYaxis()->SetTitle("MC to Matched Reconstructed Ratio");
    pi->GetYaxis()->SetRangeUser(0.05,1.0);
    pi->SetTitle("");
    pi->GetXaxis()->SetTitle("");
    pi->GetXaxis()->SetRangeUser(0,15.);
    pi->GetYaxis()->SetTitle("Ratio: Embedded/Reconstructed tracks");
    pi->GetYaxis()->SetTitleSize(0.05);
    pi->GetYaxis()->SetTitleOffset(0.69);

    pi->Draw("PE");
    antipi->Draw("PEsame");
    p->Draw("PEsame");
    pbar->Draw("PEsame");
    K->Draw("PEsame");
    antiK->Draw("PEsame");

    leg->AddEntry(pi,"Track efficiency #pi^{+}");
    leg->AddEntry(antipi,"Track efficiency #pi^{-}");
    leg->AddEntry(p,"Track efficiency p");
    leg->AddEntry(pbar,"Track efficiency #bar{p}");
    leg->AddEntry(K,"Track efficiency #kappa^{+}");
    leg->AddEntry(antiK,"Track efficiency #kappa^{-}");

    /* other->SetMarkerColor(kRed); */
    /* other->SetLineColor(kRed); */
    /* other->SetMarkerStyle(kFullCircle); */
    /* other->Draw("PEsame"); */
    /* leg->AddEntry(other,"Track efficiency $2"); */
    leg->Draw("same");

    pad2->cd();
    pad2->cd();

    TH1D* K_rat      = (TH1D*) K->Clone("K_rat");
    TH1D* antiK_rat  = (TH1D*) antiK->Clone("antiK_rat");
    TH1D* p_rat      = (TH1D*) p->Clone("p_rat");
    TH1D* pbar_rat   = (TH1D*) pbar->Clone("pbar_rat");
    TH1D* antipi_rat = (TH1D*) antipi->Clone("antipi_rat");

    K_rat      ->Sumw2();
    antiK_rat  ->Sumw2();
    p_rat      ->Sumw2();
    pbar_rat   ->Sumw2();
    antipi_rat ->Sumw2();

    K_rat      ->Divide(pi);
    antiK_rat  ->Divide(pi);
    p_rat      ->Divide(pi);
    pbar_rat   ->Divide(pi);
    antipi_rat ->Divide(pi);

    /* rat->Sumw2(); */
    /* rat->GetXaxis()->SetRangeUser(0,15.); */
    p_rat->GetXaxis()->SetRangeUser(0.,15.);
    p_rat->Divide(pi);
    p_rat->SetTitle("");
    p_rat->GetYaxis()->SetTitle("Ratios to #pi spectra");
    p_rat->GetXaxis()->SetTitle("Embedded Track p_{T}");
    /* p_rat->SetMarkerColor(kBlack); */
    /* p_rat->SetLineColor(kBlack); */
    /* p_rat->SetMarkerStyle(kFullDiamond); */
    p_rat->GetYaxis()->SetTitleSize(0.05);
    p_rat->GetYaxis()->SetTitleOffset(.69);
    p_rat->GetXaxis()->SetTitleSize(0.05);
    p_rat->GetXaxis()->SetTitleOffset(1.04);
    p_rat->Draw("PE");

    antipi_rat ->Draw("PEsame");
    K_rat      ->Draw("PEsame");
    antiK_rat  ->Draw("PEsame");
    p_rat      ->Draw("PEsame");
    pbar_rat   ->Draw("PEsame");

    TLegend *leg2 = new TLegend(0.6744681,0.3028571,0.887234,0.6914286,NULL,"brNDC");
    
    leg2->AddEntry(antipi_rat,"#pi^{-}/#pi^{+}");
    leg2->AddEntry(p_rat,     "p/#pi^{+}");
    leg2->AddEntry(pbar_rat,  "#bar{p}/#pi^{+}");
    leg2->AddEntry(K_rat,     "#kappa^{+}/#pi^{+}");
    leg2->AddEntry(antiK_rat, "#kappa^{-}/#pi^{+}");
    leg2->Draw("same");




    TCanvas* p_canv = new TCanvas("pause");
    p_canv->WaitPrimitive();

    /* c->SaveAs("pdf/$2.pdf"); */

