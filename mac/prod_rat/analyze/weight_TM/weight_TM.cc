root -l <<EOF
    // input: 
    // $1  :: system -- which particle system to use
    // $2  :: particle name: pi pin p pbar K Kn
    // $3  :: group designator: A_ B_ or "" (nothing)
    TString inp0{"$2"};
    if (!inp0.IsNull()) {
        .L ../cc/inc_Tsallis.cc+

        TFile* f_old   = new TFile("../emb_data/TM.root", "read");
        TFile* f_abund = new TFile("../fn_weight/fn_weight_$1.root", "read");
        TFile* f_new   = new TFile("$3$2_weight_$1.root", "update");
        TH1D* T;
        TH1D* abund;
        TH2D* TM;
        TH2D* MT;

        f_old->cd();
        
        f_old->GetObject(Form("%sT_%s","$3","$2"), T);
        f_old->GetObject(Form("%sTM_%s","$3","$2"),TM);
        f_old->GetObject(Form("%sMT_%s","$3","$2"),MT);

        f_abund->cd();
        f_abund->GetObject("$2_abund",abund);

        f_new->cd();
        weight_TH1(T, abund);
        TH1D* den = MT->ProfileX()->ProjectionX("$3pre_weighting");
        xWeight_TH2(TM, abund); // function from ../cc/inc_Tsallis.cc
        yWeight_TH2(MT, abund); // function from ../cc/inc_Tsallis.cc
        TH1D* M = MT  ->ProjectionX("$3M");

        TH1D* num = MT->ProfileX()->ProjectionX("$3ratio_M");
        num->SetTitle("Ratio of Profile to M, w/prior : w/o prior");
        num->Divide(den);
        
        TH1D* naif_eff = (TH1D*) M->Clone("$3naif_eff");
        naif_eff->GetYaxis()->SetTitle("$2 recon efficiency, w/ assumed prior");
        naif_eff->Divide(T);

        naif_eff->Write();
        abund->SetName("$3abund");
        T->SetName ("$3T");
        TM->SetName("$3TM");
        MT->SetName("$3MT");

        abund->Write();
        T->Write();
        TM->Write();
        MT->Write();
        M->Write();
        num->Write();
    } 
EOF
