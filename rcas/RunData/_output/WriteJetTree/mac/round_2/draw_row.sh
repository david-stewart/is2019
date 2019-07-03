root -l hg_Jets_8up.root<<EOF
    TCanvas*c = new TCanvas("c");
    c->SetLogy();
    rho_lo->GetYaxis()->SetTitle("#frac{1}{#SigmaN_{ev}} #frac{dN_{ev}}{d#rho}");
    rho_lo->Draw("PE");
    rho_hi->Draw("PE same");
    TLegend *leg = new TLegend();
    leg->AddEntry(rho_lo, "EA_{70-90%}");
    leg->AddEntry(rho_hi, "EA_{0-30%}");
    leg->Draw("same");
    TCanvas*c0 = new TCanvas("c0");
    c0->WaitPrimitive();
EOF
