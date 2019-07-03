#/bin/bash
root -l<<EOF
    .L JetLoop.C+
    JetLoop a("9up",9,1000);
    a.Loop();
EOF
    # .L JetLoop.C+
    # JetLoop a("",4,1000);
    # a.Loop();


    # JetLoop b("4_8",4,8);
    # b.Loop();

    # JetLoop c("8up",8,1000);
    # b.Loop();
# EOF
