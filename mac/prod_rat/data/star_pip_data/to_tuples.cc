root -l<<EOF
    void fill_tup (const char* f_name, 
                   const char* tup_name){
        const char* tup_string = Form("data from %s of https://www.hepdata.net/record/ins709170",f_name);
        TNtuple *tup = new TNtuple(tup_name, tup_string, "pt:dN:err");

        ifstream f;
        f.open(f_name);
        string line;
        
        while (getline(f, line)){
            TString tline{line};
            if (tline.BeginsWith("#") || tline.BeginsWith("$")) continue;
            stringstream ss { line };
            string field;
            int which {0};
            double pt, val, err0, err1;
            bool newval{false};
            while (getline(ss,field,',')){
                stringstream word(field);
                switch (which++) {
                    case 0: word >> pt; newval=true; break;
                    case 3: word >> val; break;
                    case 4: word >> err0; break;
                    case 6: word >> err1; break;
                }
            }
            if (!newval) continue;
            cout << " pt("<<pt<<") val("<<val<<") err0("<<err0<<") err1("<<err1<<")"<<endl;
            double err { TMath::Sqrt(err0*err0+err1*err1) };
            tup->Fill(pt, val, err);
        }
        f.close();
    };

    TFile *file = new TFile("pip_STAR.root","RECREATE");
    fill_tup("Table1.csv"   ,"pi_MB" );
    fill_tup("Table2.csv"   ,"pi_pp"    );
    fill_tup("Table3.csv"   ,"pi_0_20"  );
    fill_tup("Table4.csv"   ,"pi_20_40"  );
    fill_tup("Table5.csv"   ,"pi_40_100"  );

    fill_tup("Table6.csv"   ,"pin_MB"   );
    fill_tup("Table7.csv"   ,"pin_pp"   );
    fill_tup("Table8.csv"   ,"pin_0_20"  );
    fill_tup("Table9.csv"   ,"pin_20_40"  );
    fill_tup("Table10.csv"  ,"pin_40_100"  );

    fill_tup("Table11.csv"  ,"p_MB"   );
    fill_tup("Table12.csv"  ,"p_pp"   );
    fill_tup("Table13.csv"  ,"p_0_20"  );
    fill_tup("Table14.csv"  ,"p_20_40"  );
    fill_tup("Table15.csv"  ,"p_40_100"  );

    fill_tup("Table16.csv"  ,"pbar_MB"   );
    fill_tup("Table17.csv"  ,"pbar_pp"   );
    fill_tup("Table18.csv"  ,"pbar_0_20"  );
    fill_tup("Table19.csv"  ,"pbar_20_40"  );
    fill_tup("Table20.csv"  ,"pbar_40_100"  );

    /* fill_tup("pi_.data"   ,"pim","pi^- abundances,  TABLE VIII of https://journals.aps.org/prc/abstract/10.1103/PhysRevC.88.024906#supplemental"); */
    /* fill_tup("K.data"   ,"K"  ,"Kaon production, TABLE IX of https://journals.aps.org/prc/abstract/10.1103/PhysRevC.88.024906#supplemental"); */
    /* fill_tup("K_.data"  ,"Km" ,"K^- abundances,  TABLE X of https://journals.aps.org/prc/abstract/10.1103/PhysRevC.88.024906#supplemental"); */
    /* fill_tup("p.data"   ,"p"  ,"p   abundances,  TABLE XI of https://journals.aps.org/prc/abstract/10.1103/PhysRevC.88.024906#supplemental"); */
    /* fill_tup("p_.data"   ,"pm"  ,"pbar abundances,  TABLE XII of https://journals.aps.org/prc/abstract/10.1103/PhysRevC.88.024906#supplemental"); */

    file->Write();
EOF
