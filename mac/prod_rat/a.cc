gSystem->Load("cc/FillTGraph_cc.so");
void a() {
    TFile *f = new TFile("piKp_abundances.root","READ");
    TNtuple* k;
    f->GetObject("K_f18",k);
/* root -l piKp_abundances.root<<EOF */
    /* .L cc/FillTGraph.cc+ */
    TCanvas *c = new TCanvas("c");
    auto g = FillTGraph(k);
    g->Draw();
    TCanvas *d = new TCanvas("d");
    d->WaitPrimitive();
}
