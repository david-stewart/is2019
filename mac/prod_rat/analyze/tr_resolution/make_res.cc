root -l <<EOF
    // note: Prefer MT to TM (because it is what is used in unfolded M to T_projected)
    const char* fancy = ( (strncmp("$3","p",   5) == 0) ? "p"
                       : (strncmp("$3","pbar",5) == 0) ? "#bar{P}"
                       : (strncmp("$3","K"   ,5) == 0) ? "#Kappa"
                       : (strncmp("$3","Kn"  ,5) == 0) ? "#Kappa^{-1}"
                       : (strncmp("$3","pi"  ,5) == 0) ? "#pi"
                       : (strncmp("$3","pin" ,5) == 0) ? "#pi^{-1}"
                       : "");
    /* if (inp0.IsNull()) { */
        /* cout << "Require particle input parameter p_ pbar_ pi_ pin_ etc..." << endl; */
    /* } else { */
        TFile *f_old = new TFile("$1","READ");   
        TFile *f_new = new TFile("r$2.root","RECREATE");
        TH2D* MT;
        f_old->GetObject("MT$2",MT);
        /* MT->Write(); */
        MT->FitSlicesX();
        /* TH1D *amp, *mean, *std; */
        MT->SetName("MT");
        MT->Write();
        MT$2_0->SetName("amplitude");
        MT$2_1->SetName("mean");
        MT$2_2->SetName("sigma");
        MT$2_chi2->SetName("chi2");

        // make the Track Resolution
        TH1D* resRatio = (TH1D*) MT$2_2->Clone("resRatio");
        resRatio->SetTitle(Form("Embedded %s Track p_{T} Resolution",fancy));
        resRatio->GetXaxis()->SetTitle("p_{T,embedded}");
        resRatio->GetYaxis()->SetTitle("#sigma#left(p_{T,matched}#right)/#mu#left(p_{T,matched}#right)");
        resRatio->Divide(MT$2_1);
        /* TCanvas *c = new TCanvas(); */
        /* res->Draw(); */
        /* TCanvas *c0 = new TCanvas("c0"); */
        /* c0->WaitPrimitive(); */

        f_new->Write();
    /* } */

EOF
