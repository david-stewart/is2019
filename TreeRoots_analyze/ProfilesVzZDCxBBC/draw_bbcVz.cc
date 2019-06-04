root -l bbc_vz_001vs004.root<<EOF
    TCanvas *c = new TCanvas("c");  
    bbcE_Vz_500004->GetYaxis()->SetTitle("BBC ADC East Signal");
    bbcE_Vz_500004->GetXaxis()->SetTitle("Z_{vertex}");
    bbcE_Vz_500004->SetStats(0);

    bbcES_Vz_500004->SetMarkerStyle(kOpenSquare);
    bbcES_Vz_500004->SetMarkerColor(kRed);

    bbcE_Vz_500004->SetMarkerStyle(kFullSquare);
    bbcE_Vz_500004->SetMarkerColor(kRed);

    bbcES_Vz_500001->SetMarkerStyle(kOpenCircle);
    bbcES_Vz_500001->SetMarkerColor(kBlue);

    bbcE_Vz_500004->Draw("PE");
    bbcES_Vz_500004->Draw("PE same");
    bbcES_Vz_500001->Draw("PE same");

    TLegend *tl = new TLegend();
    tl->AddEntry(bbcES_Vz_500004, "VPDMB-novtx: (PMTs 1-16)");
    tl->AddEntry(bbcE_Vz_500004,  "VPDMB-novtx: (PMTs 1-24)");
    tl->AddEntry(bbcES_Vz_500001, "VPDMB-d-ssh, 500001: (PMTs 1-16)");
    tl->Draw("same");



    TCanvas *c0 = new TCanvas("c0");  
    c0->WaitPrimitive();
EOF
