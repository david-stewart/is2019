root -l StarPhenix_piKp_comp.root<<EOF
    .L ../cc/inc_Tsallis.cc+
    auto p_ph = ctup(p_phenix,       kFullCircle, kBlue, 1);
    auto pbar_ph = ctup(pbar_phenix, kFullCircle, kRed,  1);

    auto p_st    = ctup(p_star,    kFullStar, kBlue, 1);
    auto pbar_st = ctup(pbar_star, kFullStar, kRed, 1);

    auto pi_ph = ctup(pi_phenix,  kOpenCircle, kBlue, 3);
    auto pin_ph = ctup(pin_phenix, kOpenCircle, kRed, 3);

    auto pi_st = ctup(pi_star,   kOpenStar, kBlue, 3);
    auto pin_st = ctup(pin_star, kOpenStar, kRed, 3);

    auto K_ph  = ctup(K_phenix,   kOpenSquare, kCyan+2,3);
    auto Kn_ph = ctup(Kn_phenix, kOpenSquare, kCyan-2,3);

    auto Kshort = ctup(Kshort_star,  kFullFourTrianglesX, kBlack, 7);
    auto pi_pp = ctup(star_pi_pp,   kOpenCrossX, kGreen+3, 7);
    auto pin_pp = ctup(star_pin_pp,  kFullCrossX, kGreen+3, 7);
    auto p_pp = ctup(star_p_pp,    kOpenTriangleDown, kMagenta+1, 7);
    auto pbar_pp = ctup(star_pbar_pp, kFullTriangleDown, kMagenta+1, 7);


    TCanvas *c = new TCanvas("c");
    c->SetLogy();

    p_st->SetTitle("d+Au 200 GeV MB(STAR) or 0-100%(PHENIX) #pi#Kappap data");
    p_st->SetMaximum(15.);
    p_st->GetYaxis()->SetTitle("#frac{d^{2}N}{2#pip_{T}d#etadp_{T}}");
    p_st->GetXaxis()->SetTitle("p_{T}");
    p_st->GetXaxis()->SetRangeUser(0,15.);
    p_st->Draw("APE");
    pbar_st->Draw("PE same");

    p_ph->Draw("PE same");
    pbar_ph->Draw("PE same");


    pi_st->Draw("PE same");
    pin_st->Draw("PE same");

    pi_ph->Draw("PE same");
    pin_ph->Draw("PE same");

    K_ph->Draw("PE same");
    Kn_ph->Draw("PE same");

    Kshort->Draw("PE same");
    pi_pp->Draw("PE same");
    pin_pp->Draw("PE same");
    p_pp->Draw("PE same");
    pbar_pp->Draw("PE same");
    
    TLegend* leg = new TLegend();
    leg->AddEntry(p_st,"p STAR");
    leg->AddEntry(p_ph,"p Phenix");
    leg->AddEntry(pbar_st,"#bar{p} STAR");
    leg->AddEntry(pbar_ph,"#bar{p} Phenix");
    leg->AddEntry(pi_st,"#pi STAR");
    leg->AddEntry(pi_ph,"#pi Phenix");
    leg->AddEntry(pin_st,"#pi^{-} STAR");
    leg->AddEntry(pin_ph,"#pi^{-} Phenix");
    leg->AddEntry(K_ph,"#Kappa Phenix");
    leg->AddEntry(Kn_ph,"#Kappa^{-} Phenix");

    leg->AddEntry(Kshort,"#Kappa_{short} pp STAR");
    leg->AddEntry(pi_pp,"#pi^{+} pp STAR");
    leg->AddEntry(pin_pp,"#pi^{-} pp STAR");
    leg->AddEntry(p_pp,"p pp STAR");
    leg->AddEntry(pbar_pp,"#bar{p} pp STAR");
    leg->Draw("same");

    TCanvas *c0 = new TCanvas("c0");
    c0->WaitPrimitive();



EOF
