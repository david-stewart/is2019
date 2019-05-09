root -l <<EOF
    // input: 
    // $1  :: particle name: pi pin p pbar K Kn
    // $2  :: group designator: A_ B_ or "" (nothing)
    TString inp0{"$1"};
    switch (
    const char* fancy[6]{"#pi","p","#Kappa","pi^{-}","#bar{p}","#Kappa^{-}"};
    if ("$2" == "p"
    if (!inp0.IsNull()) {
        .L ../cc/inc_Tsallis.cc+
        /* cout << " yes " << "$1" << endl; */
        TFile* f_old   = new TFile("../emb_data/TM.root", "read");
        TFile* f_abund = new TFile("../fn_weight/fn_weight.root", "read");
        TFile* f_new   = new TFile("TM_weighted.root", "update");
        TH1D* _T;
        TH1D* _abund;
        TH2D* _TM;
        TH2D* _MT;

        f_old->cd();
        
        f_old->GetObject(Form("%sT_%s","$2","$1"), _T);
        f_old->GetObject(Form("%sTM_%s","$2","$1"),_TM);
        f_old->GetObject(Form("%sMT_%s","$2","$1"),_MT);

        f_abund->cd();
        f_abund->GetObject("$1_abund",_abund);

        f_new->cd();
        weight_TH1(_T, _abund);
        /* _T->Sumw2(); */
        TProfile* old_M = _MT->ProfileX(Form("old_%sM_%s","$2","$1"));
        xWeight_TH2(_TM, _abund);
        yWeight_TH2(_MT, _abund);
        TH1D* _M = _MT  ->ProjectionX(Form("%sM_%s","$2","$1"));

        /* TProfile* rat = _MT->ProfileX(Form("rat_%sM_%s","$2","$1")); */
        TProfile* rat = _MT->ProfileX("rat");
        TH1D* num = rat->ProjectionX(Form("rat_%sM_%s","$2","$1"));
        TH1D* den = old_M->ProjectionX();
        num->Divide(den);
        //Get the ratio with weighted to the prior
        
        TH1D* naif_eff = (TH1D*) _M->Clone(Form("%snaif_%s","$2","$1"));
        naif_eff->GetYaxis()->SetTitle("$1 recon efficiency, w/ assumed prior");
        /* naif_eff->Sumw2(); */  
        naif_eff->Divide(_T);

        naif_eff->Write();
        _abund->Write();
        _T->Write();
        _TM->Write();
        _MT->Write();
        _M->Write();
        num->Write();

        /* if (!gDirectory->GetListOfKeys()->Contains("T")){ */
        /*     TH1D* T = (TH1D*) _T->Clone("T"); */
        /*     T->SetTitle("All particles, MC truth"); */
        /*     T->Write(); */

        /*     TH1D* M = (TH1D*) _M->Clone("M"); */
        /*     M->SetTitle("All particles, Matched tracks"); */
        /*     M->Write(); */

        /*     /1* cout << " || name T $1 " << T->GetEntries() << endl; *1/ */
        /*     /1* cout << " || name M $1 " << M->GetEntries() << endl; *1/ */

        /*     TH2D* TM = (TH2D*) _TM->Clone("TM"); */
        /*     TM->SetTitle("All particles TM"); */
        /*     TM->Write(); */

        /*     TH2D* MT = (TH2D*) _MT->Clone("MT"); */
        /*     MT->SetTitle("All particles MT"); */

        /*     T ->Write(); */
        /*     M->Write( ); */
        /*     MT->Write(); */
        /*     TM->Write(); */
        /* } else { */
        /*     T->Add(_T); */
        /*     M->Add(_M); */
        /*     TM->Add(_TM); */
        /*     MT->Add(_MT); */
        /*     /1* cout << " :: name T $1 + " << _T->GetEntries() << " = " << T->GetEntries() << endl; *1/ */
        /*     /1* cout << " :: name M $1 + " << _M->GetEntries() << " = " <<  M->GetEntries() << endl; *1/ */
        /*     f_new->Write("",TObject::kOverwrite); */
        /* } */
    } 
EOF
