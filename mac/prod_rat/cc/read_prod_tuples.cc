root -l<<EOF

    void fill_tup (const char* f_name, 
                   const char* tup_name,
                   const char* tup_string) {
        TNtuple *tup = new TNtuple(tup_name, tup_string, "pt:dN:err");
        ifstream f;
        f.open(f_name);
        string line;
        while (getline(f, line)){
            cout << "line: " << line << endl;
            istringstream iss { line };
            double pt, val, err;
            iss >> pt >> val >> err;
            tup->Fill(pt, val, err);
        }
        f.close();
    };

    void fill_tup2 (const char* f_name, 
                    const char* tup_name,
                    const char* tup_string) {
        TNtuple *tup = new TNtuple(tup_name, tup_string, "pt:dN:err");
        ifstream f;
        f.open(f_name);
        string line;
        while (getline(f, line)){
            istringstream iss { line };
            double pt, val, err0, err1;
            iss >> pt >> val >> err0 >> err1;
            double err { TMath::Sqrt(err0*err0+err1*err1) };
            tup->Fill(pt, val, err);
        }
        f.close();
    };


    TFile *file = new TFile("piKp_abundances.root","RECREATE");
    fill_tup2("fig2_data/pbar.txt"   ,"pbar_f2"   ,"pbar abundances from fig. 2 with RSS of err");
    fill_tup2("fig2_data/p.txt"      ,"p_f2"      ,"proton abundances from fig. 2 with RSS of err");
    fill_tup2("fig2_data/pi.txt"     ,"pi_f2"     ,"pion abundances from fig. 2 with RSS of err");
    fill_tup2("fig2_data/anti_pi.txt","anti_pi_f2","pi- abundances from fig. 2 with RSS of err");

    fill_tup("fig18_data/pbar.txt"   ,"pbar_f18"   ,"pbar abundances from fig. 18 with RSS of err");
    fill_tup("fig18_data/p.txt"      ,"p_f18"      ,"proton abundances from fig. 18 with RSS of err");
    fill_tup("fig18_data/pi.txt"     ,"pi_f18"     ,"pion abundances from fig. 18 with RSS of err");
    fill_tup("fig18_data/anti_pi.txt","anti_pi_f18","pi- abundances from fig. 18 with RSS of err");
    fill_tup("fig18_data/K.txt"      ,"K_f18"      ,"K abundances from fig. 18 with RSS of err");
    fill_tup("fig18_data/anti_K.txt" ,"anti_K_f18" ,"K- abundances from fig. 18 with RSS of err");

    file->Write();
EOF
