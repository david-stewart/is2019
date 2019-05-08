root -l<<EOF

    void fill_tup (const char* f_name, 
                   const char* tup_name,
                   const char* tup_string) {
        TNtuple *tup0_20 = new TNtuple(Form("%s_0_20",tup_name), tup_string, "pt:dN:err");
        TNtuple *tup20_40 = new TNtuple(Form("%s_20_40",tup_name), tup_string, "pt:dN:err");
        TNtuple *tup40_60 = new TNtuple(Form("%s_40_60",tup_name), tup_string, "pt:dN:err");
        TNtuple *tup60_88 = new TNtuple(Form("%s_60_88",tup_name), tup_string, "pt:dN:err");
        TNtuple *tup0_100 = new TNtuple(Form("%s_0_100",tup_name), tup_string, "pt:dN:err");

        ifstream f;
        f.open(f_name);
        string line;
        while (getline(f, line)){
            istringstream iss { line };
            double pt, val0, val20, val40, val60, valmb;
            double     err0, err20, err40, err60, errmb;
            iss >> pt 
                >> val0  >> err0
                >> val20 >> err20
                >> val40 >> err40
                >> val60 >> err60
                >> valmb >> errmb;
            tup0_20->Fill(pt, val0, err0);
            tup20_40->Fill(pt, val20, err20);
            tup40_60->Fill(pt, val40, err40);
            tup60_88->Fill(pt, val60, err60);
            tup0_100->Fill(pt, valmb, errmb);
        }
        f.close();
    };

    TFile *file = new TFile("piKp_phenix.root","RECREATE");
    fill_tup("pi.data"   ,"pi"  ,"pi abundances,  TABLE VII of https://journals.aps.org/prc/abstract/10.1103/PhysRevC.88.024906#supplemental");
    fill_tup("pi_.data"   ,"pin","pi^- abundances,  TABLE VIII of https://journals.aps.org/prc/abstract/10.1103/PhysRevC.88.024906#supplemental");
    fill_tup("K.data"   ,"K"  ,"Kaon production, TABLE IX of https://journals.aps.org/prc/abstract/10.1103/PhysRevC.88.024906#supplemental");
    fill_tup("K_.data"  ,"Kn" ,"K^- abundances,  TABLE X of https://journals.aps.org/prc/abstract/10.1103/PhysRevC.88.024906#supplemental");
    fill_tup("p.data"   ,"p"  ,"p   abundances,  TABLE XI of https://journals.aps.org/prc/abstract/10.1103/PhysRevC.88.024906#supplemental");
    fill_tup("p_.data"   ,"pbar"  ,"pbar abundances,  TABLE XII of https://journals.aps.org/prc/abstract/10.1103/PhysRevC.88.024906#supplemental");

    file->Write();
EOF
