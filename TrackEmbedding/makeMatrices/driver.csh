root -l<<EOF
    .L e_tree.C

    e_tree f0("AntiProton_fullSample.root","pbar");
    f0->Loop();

    e_tree f1("Kminus_fullSample.root","Km");
    f1->Loop();

    e_tree f2("Kplus_fullSample.root","K");
    f2->Loop();

    e_tree f3("Proton_fullSample.root","p");
    f3->Loop();

    e_tree f4("pi_minus_fullSample.root","pin");
    f4->Loop();

    e_tree f5("pi_plus_fullSample.root","pi");
    f5->Loop();
EOF
