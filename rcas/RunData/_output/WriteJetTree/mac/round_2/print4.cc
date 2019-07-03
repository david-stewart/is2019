void print4(int which=3,int which_File=4){
    
    /* int which{3}; */
    TFile* f = new TFile(Form("hg_Jets_%iup.root",which_File),"read");
    TH2D *hi, *lo, *hi_shift, *lo_shift;
    f->GetObject("jetsHi",hi);
    f->GetObject("jetsLo",lo);
    f->GetObject("jetsHi_shift",hi_shift);
    f->GetObject("jetsLo_shift",lo_shift);

    TH1D* phi = hi->ProjectionX("h",which,which);
    TH1D* plo = lo->ProjectionX("l",which,which);
    
    TH1D* phi_shift = hi_shift->ProjectionX("hs",which,which);
    TH1D* plo_shift = lo_shift->ProjectionX("ls",which,which);

    phi->Divide(plo);
    phi->SetMarkerStyle(kOpenCircle);
    phi->SetMarkerColor(kBlack);

    phi_shift->Divide(plo_shift);
    phi_shift->SetMarkerStyle(kFullCircle);
    phi_shift->SetMarkerColor(kRed);
    
    TCanvas *c = new TCanvas("c");
    phi->Draw("PE");
    phi_shift->Draw("PE same");

}
