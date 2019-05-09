if ! [ -e KnoK.root ]
then 
    rootcp TM_weighted.root:naif_eff KnoK.root:K
    rootcp pip_weighted.root:naif_eff KnoK.root:noK
fi

root -l KnoK.root<<EOF
    K->SetTitle("efficiency with #pi^{#pm}, #Kappa^{#pm}, and p/#bar{p}");
    noK->SetTitle("efficiency without #Kappa^{#pm}");

    K->SetMarkerColor(kBlue);
    K->SetLineColor(kBlue);
    K->SetMarkerStyle(kOpenSquare);
    K->SetMarkerSize(0.3);
    K->GetYaxis()->SetRangeUser(0,2.);
    K->SetTitle("");
    K->SetStats(0);
    K->GetYaxis()->SetTitleSize(0.06);
    K->GetYaxis()->SetTitleOffset(0.30);
    K->GetYaxis()->SetLabelSize(0.06);

    noK->SetMarkerColor(kRed);
    noK->SetLineColor(kRed);
    noK->SetMarkerStyle(kOpenCircle);
    noK->SetMarkerSize(0.3);
    noK->SetStats(0);

    TCanvas *c = new TCanvas("c");
    c->Divide(1,2,0,0);

    c->cd(1);
    K->Draw("PE");
    noK->Draw("PE same");
    TLegend *tl = new TLegend();
    tl->AddEntry(K,"eff. w/#Kappa's included");
    tl->AddEntry(noK,"eff. w/o #Kappa's included");
    tl->Draw("same");


    c->cd(2);
    TH1D* rat = (TH1D*) K->Clone("ratio");
    rat->Divide(noK);
    rat->SetStats(0);
    rat->GetYaxis()->SetTitle("Efficiency ratio of (w/#Kappa's)/(w/o #Kappa's)");
    rat->SetMarkerColor(kBlack);
    rat->SetLineColor(kBlack);
    rat->SetMarkerStyle(kOpenCircle);
    rat->SetMarkerSize(0.3);
    rat->GetYaxis()->SetTitleSize(0.06);
    rat->GetYaxis()->SetTitleOffset(0.30);
    rat->GetYaxis()->SetLabelSize(0.06);
    rat->Draw("PE");



    TCanvas *c0 = new TCanvas("c0");
    c0->WaitPrimitive();

EOF
