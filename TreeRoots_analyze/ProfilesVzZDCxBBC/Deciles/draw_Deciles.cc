root -l HOTEL.root<<EOF

    TCanvas *c = new TCanvas("c");  
    c->SetLogy();
    h_quant_corr->GetXaxis()->SetTitle("BBC ADC East Signal (PMTs 1-24)");
    h_quant_corr->GetYaxis()->SetTitle("N_{events}/N_{total}");
    /* h_quant->SetMarkerStyle(kNone); */
    /* h_quant->SetMarkerColor(kRed); */
    double ymax = 0.4e-3;
    /* h_quant->GetYaxis()->SetRangeUser(0,ymax); */
    h_quant->SetMarkerStyle(1);
    h_quant->SetLineColor  (kBlack);
    h_quant->SetMarkerColor(kBlack);

    h_quant_corr->SetMarkerStyle(1);
    h_quant_corr->SetLineColor  (kRed);
    h_quant_corr->SetMarkerColor(kRed);
    h_quant_corr->DrawNormalized("PE");
    h_quant->DrawNormalized("PE same");

    // draw the vertical lines on quantiles
    double x[9] = {0.1,.2,3,.4,.5,.6,.7,.8,.9};
    double x_corr[9] = {0.1,.2,3,.4,.5,.6,.7,.8,.9};
    double q[9];
    double q_corr[9];
    h_quant->GetQuantiles(9,q,x);
    h_quant_corr->GetQuantiles(9,q_corr,x_corr);

    TLine* l[9];
    TLine* l_corr[9];
    for (int i = 0; i<9;++i){
        l[i] = new TLine(q[i],0,q[i],ymax);
        l[i]->SetLineColor(kBlack);
        l[i]->SetLineStyle(2);
        l[i]->Draw("same");

        l_corr[i] = new TLine(q_corr[i],0,q_corr[i],ymax);
        l_corr[i]->SetLineColor(kRed);
        l_corr[i]->SetLineStyle(2);
        l_corr[i]->Draw("same");
    }

    TLegend *tl = new TLegend();
    tl->AddEntry(h_quant,         "BBC_E Distribution");
    tl->AddEntry(h_quant_corr,    "BBC_E Distribution corrected for Vz and ZDCx");
    tl->Draw("same");

    TCanvas *c0 = new TCanvas("c0");  
    c0->WaitPrimitive();
EOF
