root -l <<EOF
    // input: 
    // $1  :: particle name: pi pin p pbar K Kn
    // $2  :: group designator: A_ B_ or "" (nothing)
    TString inp0{"$1"};
    if (!inp0.IsNull()) {
        .L ../cc/inc_Tsallis.cc+

        TFile* f_old   = new TFile("../emb_data/TM.root", "read");
        TFile* f_abund = new TFile("../fn_weight/fn_weight.root", "read");
        TFile* f_new   = new TFile("$2$1_weight.root", "update");
        TH1D* T;
        TH1D* abund;
        TH2D* TM;
        TH2D* MT;

        f_old->cd();
        
        f_old->GetObject(Form("%sT_%s","$2","$1"), T);
        f_old->GetObject(Form("%sTM_%s","$2","$1"),TM);
        f_old->GetObject(Form("%sMT_%s","$2","$1"),MT);

        f_abund->cd();
        f_abund->GetObject("$1_abund",abund);

        f_new->cd();
        weight_TH1(T, abund);
        TH1D* den = MT->ProfileX()->ProjectionX("$2pre_weighting");
        xWeight_TH2(TM, abund); // function from ../cc/inc_Tsallis.cc
        yWeight_TH2(MT, abund); // function from ../cc/inc_Tsallis.cc
        TH1D* M = MT  ->ProjectionX("$2M");

        TH1D* num = MT->ProfileX()->ProjectionX("$2ratio_M");
        num->SetTitle("Ratio of Profile to M, w/prior : w/o prior");
        num->Divide(den);
        
        TH1D* naif_eff = (TH1D*) M->Clone("$2naif_eff");
        naif_eff->GetYaxis()->SetTitle("$1 recon efficiency, w/ assumed prior");
        naif_eff->Divide(T);

        naif_eff->Write();
        abund->SetName("$2abund");
        T->SetName ("$2T");
        TM->SetName("$2TM");
        MT->SetName("$2MT");

        abund->Write();
        T->Write();
        TM->Write();
        MT->Write();
        M->Write();
        num->Write();
    } 
EOF
