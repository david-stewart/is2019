root -l <<EOF
    TFile* top = new TFile("TM_weighted.root","read");
    const char* name[6]{"pi","p","K","pin","pbar","Kn"};
    const char* fancy[6]{"#pi","p","#Kappa","pi^{-}","#bar{p}","#Kappa^{-}"};
    TH2D* hg[6];
    TFile* file[6];

    TCanvas *c = new TCanvas("c");
    c->Divide(3,2,0,0);

    for (int i{0};i<6;++i){
        c->cd(i+1);
        gPad->SetLogz();
        if (i==2 || i == 5) gPad->SetRightMargin(0.13);
        file[i] = new TFile(Form("%s_weight.root",name[i]),"read");
        file[i]->GetObject("TM",hg[i]);
        hg[i]->SetTitle(fancy[i]);
        hg[i]->GetZaxis()->SetRangeUser(1e-10,15);;
        hg[i]->Draw("colz");
    }

    TCanvas *cp = new TCanvas("cp");
    cp->WaitPrimitive();

EOF
