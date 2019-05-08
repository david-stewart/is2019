root -l TM_weighted.root<<EOF

    TCanvas *c = new TCanvas("c");
    c->Divide(3,2,0,0);
    c->cd(1);
    gPad->SetLogz(); 
    TM_pi->GetZaxis()->SetRangeUser(1e-10,15);;
    TM_pi->SetTitle("#pi");
    TM_pi->Draw("colz");

    c->cd(2)
    gPad->SetLogz(); 
    TM_p->GetZaxis()->SetRangeUser(1e-10,15);
    TM_p->SetTitle("p");
    TM_p->Draw("colz");

    c->cd(3)
    gPad->SetLogz(); 
    TM_K->GetZaxis()->SetRangeUser(1e-10,15);
    TM_K->SetTitle("#Kappa");
    TM_K->Draw("colz");

    c->cd(4)
    gPad->SetLogz(); 
    TM_pin->GetZaxis()->SetRangeUser(1e-10,15);
    TM_pin->SetTitle("#pi^{-}");
    TM_pin->Draw("colz");

    c->cd(5)
    gPad->SetLogz(); 
    TM_pbar->GetZaxis()->SetRangeUser(1e-10,15);
    TM_pbar->SetTitle("#bar{p}^{-}");
    TM_pbar->Draw("colz");

    c->cd(6)
    gPad->SetLogz(); 
    TM_Kn->GetZaxis()->SetRangeUser(1e-10,15);
    TM_Kn->SetTitle("#Kappa^{-}");
    TM_Kn->Draw("colz");


    TCanvas *cp = new TCanvas("cp");
    cp->WaitPrimitive();

EOF
