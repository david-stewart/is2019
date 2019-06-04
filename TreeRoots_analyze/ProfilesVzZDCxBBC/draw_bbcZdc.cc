root -l bbc_zdcX_001vs004.root<<EOF
    TCanvas *c = new TCanvas("c");  
    zdcX_500004->GetYaxis()->SetTitle("BBC ADC East Signal (PMTs 1-16)");
    zdcX_500004->GetXaxis()->SetTitle("ZDCx");
    zdcX_500004->SetStats(0);

    zdcX_500001->SetMarkerStyle(kFullSquare);
    zdcX_500001->SetMarkerColor(kRed);

    zdcX_500004->Draw("PE");
    zdcX_500001->Draw("PE same");

    TLegend *tl = new TLegend();
    tl->AddEntry(zdcX_500004, "VPD-novtx");
    tl->AddEntry(zdcX_500001, "VPDMB-5-ssd");
    tl->Draw("same");



    TCanvas *c0 = new TCanvas("c0");  
    c0->WaitPrimitive();
EOF
