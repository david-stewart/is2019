root -l<<EOF
    .L e_tree.C

    e_tree f0("input/AntiProton_fullSample.root","pbar");
    f0->Loop();

    e_tree f1("input/Kminus_fullSample.root","Kn");
    f1->Loop();

    e_tree f2("input/Kplus_fullSample.root","K");
    f2->Loop();

    e_tree f3("input/Proton_fullSample.root","p");
    f3->Loop();

    e_tree f4("input/pi_minus_fullSample.root","pin");
    f4->Loop();

    e_tree f5("input/pi_plus_fullSample.root","pi");
    f5->Loop();

    mv *.root matrices
EOF
