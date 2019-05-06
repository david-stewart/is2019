root -l $1<<EOF
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
   TLegend *leg = new TLegend(0.3808511,0.08857143,0.9446809,0.2285714,NULL,"brNDC");

    bbc20_E->SetMarkerColor(kBlue);
    bbc20_E->SetLineColor(kBlue);
    bbc20_E->SetMarkerStyle(kOpenSquare);
    bbc20_E->GetYaxis()->SetTitle("MC to Matched Reconstructed Ratio");
    bbc20_E->GetYaxis()->SetRangeUser(0.05,1.0);
    bbc20_E->SetTitle("$2");
    bbc20_E->GetXaxis()->SetTitle("");
    bbc20_E->GetXaxis()->SetRangeUser(0,15.);
    bbc20_E->GetYaxis()->SetTitle("Ratio: Embedded/Reconstructed tracks");
    bbc20_E->GetYaxis()->SetTitleSize(0.05);
    bbc20_E->GetYaxis()->SetTitleOffset(0.69);

    bbc20_E->Draw("PE");
    leg->AddEntry(bbc20_E,"Events with BBC_{ES}<20%");

    bbc80_E->SetMarkerColor(kRed);
    bbc80_E->SetLineColor(kRed);
    bbc80_E->SetMarkerStyle(kFullCircle);
    bbc80_E->Draw("PEsame");
    leg->AddEntry(bbc80_E,"Events with BBC_{ES}>80%");
    leg->Draw("same");

    pad2->cd();


    pad2->cd();
    TH1D* rat = (TH1D*) bbc80_E->Clone("ratio");
    rat->Sumw2();
    rat->GetXaxis()->SetRangeUser(0,15.);
    rat->GetYaxis()->SetRangeUser(0.6,1.8);
    rat->Divide(bbc20_E);
    rat->SetTitle("");
    rat->GetYaxis()->SetTitle("(BBC>80%)/(BBC<20%) (red/blue)");
    rat->GetXaxis()->SetTitle("Embedded Track p_{T}");
    rat->SetMarkerColor(kBlack);
    rat->SetLineColor(kBlack);
    rat->SetMarkerStyle(kFullDiamond);
    rat->GetYaxis()->SetTitleSize(0.05);
    rat->GetYaxis()->SetTitleOffset(.69);
    rat->GetXaxis()->SetTitleSize(0.05);
    rat->GetXaxis()->SetTitleOffset(1.04);
    rat->Draw("PE");

    /* TCanvas* p_canv = new TCanvas("pause"); */
    /* p_canv->WaitPrimitive(); */

    c->SaveAs("pdf/$2.pdf");

