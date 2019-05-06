root -l <<EOF
    .L e_tree.C
    e_tree f("Kminus_fullSample.root","Kn")
    f.Loop()
    e_tree f("pi_minus_fullSample.root","pin")
    f.Loop()
EOF
