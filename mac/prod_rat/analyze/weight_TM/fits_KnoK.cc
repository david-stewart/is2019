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
    /* K->GetYaxis()->SetTitleSize(0.06); */
    /* K->GetYaxis()->SetTitleOffset(0.30); */
    /* K->GetYaxis()->SetLabelSize(0.06); */

    noK->SetMarkerColor(kRed);
    noK->SetLineColor(kRed);
    noK->SetMarkerStyle(kOpenCircle);
    noK->SetMarkerSize(0.3);
    noK->SetStats(0);

    TF1* fK   = new TF1("fK","[0]*exp(-([1]/x)^[2])",0,15);
    fK->SetLineColor(kCyan);
    fK->SetLineWidth(1);
    fK->SetLineStyle(7);
    fK->SetParameters(0.9, 0.10,2.5);
    K->Fit(fK);

    TF1* fnoK = new TF1("fnoK","[0]*exp(-([1]/x)^[2])",0,15);
    fnoK->SetLineColor(kMagenta);
    fnoK->SetLineWidth(1);
    fnoK->SetLineStyle(7);
    fnoK->SetParameters(0.9, 0.10,2.5);
    noK->Fit(fnoK);

    K->Draw("PE");
    fK->Draw("same");
    noK->Draw("PE same");
    fnoK->Draw("same");

    TLegend *t = new TLegend();
    t->AddEntry(K, "(#pi#Kappap)^{#pm} efficiency");
    t->AddEntry(fK,Form("(#pi#Kappap)^{#pm} fit: %.3f(#pm%.2e), %.3f(#pm%.2e), %.3f(#pm%.2e)",
                fK->GetParameter(0),
                fK->GetParError(0),
                fK->GetParameter(1),
                fK->GetParError(1),
                fK->GetParameter(2),
                fK->GetParError(2)));
    t->AddEntry(noK,"(#pip (no #Kappa))^{#pm} efficiency");
    t->AddEntry(fnoK,Form("(no #Kappa)^{#pm} fit: %.3f(#pm%.2e), %.3f(#pm%.2e), %.3f(#pm%.2e)",
                fnoK->GetParameter(0),
                fnoK->GetParError(0),
                fnoK->GetParameter(1),
                fnoK->GetParError(1),
                fnoK->GetParameter(2),
                fnoK->GetParError(2)));
    t->Draw("same");

    /* cout << " " << fK->GetParError(0) << endl; */
    TCanvas *c0 = new TCanvas("c0");
    c0->WaitPrimitive();


EOF
