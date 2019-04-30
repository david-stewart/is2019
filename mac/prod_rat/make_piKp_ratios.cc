root -l piKp_abundances.root<<EOF
    .L cc/TupToErr.cc+
      /* KEY: TNtuple  pbar_f2;1       pbar abundances from fig. 2 with RSS of err */
      /* KEY: TNtuple  p_f2;1  proton abundances from fig. 2 with RSS of err */
      /* KEY: TNtuple  pi_f2;1 pion abundances from fig. 2 with RSS of err */
      /* KEY: TNtuple  anti_p;1    pi- abundances from fig. 2 with RSS of err */
      /* KEY: TNtuple  pbar_f18;1      pbar abundances from fig. 18 with RSS of err */
      /* KEY: TNtuple  p_f18;1 proton abundances from fig. 18 with RSS of err */
      /* KEY: TNtuple  pi_f18;1        pion abundances from fig. 18 with RSS of err */
      /* KEY: TNtuple  anti_pi_f18;1   pi- abundances from fig. 18 with RSS of err */
      /* KEY: TNtuple  K_f18;1 K abundances from fig. 18 with RSS of err */
      /* KEY: TNtuple  anti_K_f18;1    K- abundances from fig. 18 with RSS of err */

    TGraphErrors *d_pbar_f2;
    TGraphErrors *d_p_f2;
    TGraphErrors *d_pi_f2;
    TGraphErrors *d_anti_pi_f2;
    TGraphErrors *d_pbar_f18;
    TGraphErrors *d_p_f18;
    TGraphErrors *d_pi_f18;
    TGraphErrors *d_anti_pi_f18;
    TGraphErrors *d_K_f18;
    TGraphErrors *d_anti_K_f18;

    TF1 *f_pbar_f2;
    TF1 *f_p_f2;
    TF1 *f_pi_f2;
    TF1 *f_anti_pi_f2;
    TF1 *f_pbar_f18;
    TF1 *f_p_f18;
    TF1 *f_pi_f18;
    TF1 *f_anti_pi_f18;
    TF1 *f_K_f18;
    TF1 *f_anti_K_f18;

    
    tie(d_pbar_f2, f_pbar_f2) = tup_to_fit(pbar_f2, EnumParticle::p);
    tie(d_p_f2, f_p_f2) = tup_to_fit(p_f2, EnumParticle::p);
    tie(d_pi_f2, f_pi_f2) = tup_to_fit(pi_f2, EnumParticle::pi);
    tie(d_anti_pi_f2, f_anti_pi_f2) = tup_to_fit(anti_pi_f2, EnumParticle::pi);
    tie(d_pbar_f18, f_pbar_f18) = tup_to_fit(pbar_f18, EnumParticle::p);
    tie(d_p_f18, f_p_f18) = tup_to_fit(p_f18, EnumParticle::p);
    tie(d_pi_f18, f_pi_f18) = tup_to_fit(pi_f18, EnumParticle::pi);
    tie(d_anti_pi_f18, f_anti_pi_f18) = tup_to_fit(anti_pi_f18, EnumParticle::pi);
    tie(d_K_f18, f_K_f18) = tup_to_fit(K_f18, EnumParticle::K);
    tie(d_anti_K_f18, f_anti_K_f18) = tup_to_fit(anti_K_f18, EnumParticle::K);

    /* TMultiGraph* mg = new TMultiGraph(); */
    /* mg->Add(d_pbar_f2); */
    /* mg->Add(d_p_f2); */
    /* mg->Add(d_pi_f2); */
    /* mg->Add(d_anti_pi_f2); */
    /* mg->Add(d_pbar_f18); */
    /* mg->Add(d_p_f18); */
    /* mg->Add(d_pi_f18); */
    /* mg->Add(d_anti_pi_f18); */
    /* mg->Add(d_K_f18); */
    /* mg->Add(d_anti_K_f18); */

    /* double minY { mg->GetMinimum() }; */
    /* double maxY { mg->GetMaximum() }; */

    //plot the fits and data
    /* cout << minX<<":"<<maxX<<"  " << minY<<":"<<maxY<<endl; */

    TCanvas *c = new TCanvas("c");

    /*
     | 10% | 33% | 33% | 33% |
     |0.0  |0.1  |0.43 |0.76 |0.99|

     other way:

     |10% | 45% |45% |
     |0.0 |0.1  |0.45 |100|
    */

    double xpad[5] = {0., .10, .4, .7, 1.};
    double ypad[4] = {0, 0.10, 0.55, 1.0};

    xpad[1] = xpad[1]*(xpad[4]-xpad[0]) / (xpad[2]-xpad[0]); // fix ratio of xpad[1] (overall) to ratio of the pad itself.
    ypad[1] = ypad[1]*(ypad[3]-ypad[0]) / (ypad[2]-ypad[0]); // fix ratio of xpad[1] (overall) to ratio of the pad itself.

    /* c->cd(); */
    TPad *pad_pi = new TPad("pad_pi","pad_pi",   xpad[0], ypad[2], xpad[2], ypad[3]);
    pad_pi->SetMargin(xpad[1],0,0,0);
    pad_pi->SetLogy();
    pad_pi->Draw();

    /* c->cd(); */
    TPad *pad_api = new TPad("pad_api","pad_api",xpad[0], ypad[0], xpad[2], ypad[2]);
    pad_api->SetMargin(xpad[1],0,ypad[1],0);
    pad_api->SetLogy();
    pad_api->Draw();
    
    TPad *pad_p = new TPad("pad_p","pad_p",xpad[2], ypad[2], xpad[3], ypad[3]);
    pad_p->SetMargin(0,0,0,0);
    pad_p->SetLogy();
    pad_p->Draw();

    TPad *pad_ap = new TPad("pad_ap","pad_ap",xpad[2], ypad[0], xpad[3], ypad[2]);
    pad_ap->SetMargin(0,0,ypad[1],0);
    pad_ap->SetLogy();
    pad_ap->Draw();

    TPad *pad_K = new TPad("pad_K","pad_K",xpad[3], ypad[2], xpad[4], ypad[3]);
    pad_K->SetMargin(0,0,0,0);
    pad_K->SetLogy();
    pad_K->Draw();

    TPad *pad_aK = new TPad("pad_aK","pad_aK",xpad[3], ypad[0], xpad[4], ypad[2]);
    pad_aK->SetMargin(0,0,ypad[1],0);
    pad_aK->SetLogy();
    pad_aK->Draw();

    double titleSizeLong(0.05);
    double titleSizeShort{titleSizeLong * (xpad[2]-xpad[0]) / (xpad[3]-xpad[2]) };

    double min_mg(2e-9);

    .L cc/myTools.cc+

    /* c->cd(); */
    int fontsize{17};
    int fontlabel{43};
    pad_pi->cd();
    TMultiGraph* mg_pi = new TMultiGraph();
    mg_pi->SetTitle("#pi");
    mySetTitle(mg_pi->GetYaxis(),"#frac{d^{2}N}{2#pip_{T}d#etadp_{T}}", 3.3, 17);
    /* mg_pi->GetYaxis()->SetTitle( "#frac{d^{2}N}{2#pip_{T}d#etadp_{T}}"); */
    /* mg_pi->GetYaxis()->SetLabelFont(fontlabel); */
    /* mg_pi->GetYaxis()->SetLabelSize(fontsize); */
    /* mg_pi->GetYaxis()->SetTitleFont(fontlabel); */
    /* mg_pi->GetYaxis()->SetTitleSize(fontsize); */
    /* mg_pi->GetYaxis()->SetTitleOffset(3.3); */
    d_pi_f2->SetMarkerColor(kBlue);
    mg_pi->Add(d_pi_f2,"lpe");
    mg_pi->Add(d_pi_f18,"lpe");
    mg_pi->GetXaxis()->SetLimits(0,10);
    mg_pi->SetMinimum(min_mg);
    mg_pi->SetMaximum(10.);
    mg_pi->SetTitle("#pi");
    mg_pi->Draw("same");


    pad_api->cd();
    TMultiGraph* mg_api = new TMultiGraph();
    mg_api->SetTitle("#pi^{-}");
    mySetTitle(mg_api->GetYaxis(),"#frac{d^{2}N}{2#pip_{T}d#etadp_{T}}", 3.3, 17);
    /* mg_api->GetYaxis()->SetTitle("#frac{d^{2}N}{2#pip_{T}d#etadp_{T}}"); */
    d_pi_f2->SetMarkerColor(kBlue);
    mySetTitle(mg_api->GetXaxis(), "p_{T}",2.1,17);

    d_anti_pi_f2->SetMarkerColor(kBlue);
    mg_api->Add(d_anti_pi_f2,"lpe");
    mg_api->Add(d_anti_pi_f18,"lpe");
    mg_api->GetXaxis()->SetLimits(0,10);
    mg_api->SetMinimum(min_mg);
    mg_api->SetMaximum(10.);
    mg_api->Draw("same");

    pad_p->cd();
    TMultiGraph* mg_p = new TMultiGraph();
    mg_p->SetTitle("p");
    mg_p->GetYaxis()->SetTitle("#frac{d^{2}N}{2#pip_{T}d#etadp_{T}}");
    d_p_f2->SetMarkerColor(kBlue);
    mg_p->Add(d_p_f2,"lpe");
    mg_p->Add(d_p_f18,"lpe");
    mg_p->GetXaxis()->SetLimits(0,10);
    mg_p->SetMinimum(min_mg);
    mg_p->SetMaximum(10.);
    mg_p->Draw("same");

    pad_ap->cd();
    TMultiGraph* mg_ap = new TMultiGraph();
    mg_ap->SetTitle("#bar{p}");
    mySetTitle(mg_ap->GetXaxis(), "p_{T}",2.1,17);
    d_pbar_f2->SetMarkerColor(kBlue);
    mg_ap->Add(d_pbar_f2,"lpe");
    mg_ap->Add(d_pbar_f18,"lpe");
    mg_ap->GetXaxis()->SetLimits(0,10);
    mg_ap->SetMinimum(min_mg);
    mg_ap->SetMaximum(10.);
    mg_ap->Draw("same");

    pad_K->cd();
    TMultiGraph* mg_K = new TMultiGraph();
    mg_K->SetTitle("#Kappa");
    mg_K->Add(d_K_f18,"lpe");
    mg_K->GetXaxis()->SetLimits(0,10);
    mg_K->SetMinimum(min_mg);
    mg_K->SetMaximum(10.);
    mg_K->Draw("same");

    pad_aK->cd();
    TMultiGraph* mg_aK = new TMultiGraph();
    mg_aK->SetTitle("#Kappa^{-}");
    mySetTitle(mg_aK->GetXaxis(), "p_{T}",2.1,17);
    /* d_p_f2->SetMarkerColor(kBlue); */
    /* mg_K->Add(d_K_f2,"lpe"); */
    mg_aK->Add(d_anti_K_f18,"lpe");
    mg_aK->GetXaxis()->SetLimits(0,10);
    mg_aK->SetMinimum(1e-10);
    mg_aK->SetMaximum(10.);
    mg_aK->Draw("same");

    

    TCanvas *c1 = new TCanvas("c1");
    c1->WaitPrimitive();

EOF
