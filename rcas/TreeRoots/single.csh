#!/usr/bin/tcsh
set prog = make_TProfile2D
set a = "in_test.root"
set c = -1
set b = "out_test__.root"
set opts = "$prog $a $b $c"
echo "|$opts|"
# ./_treeRootAnalysis.csh make_TProfile2D $opts
root4star -b -l<<EOF
    .X treeRootAnalysis.C("$opts")
EOF

# EOF./treeRootAnalysis.C\(\"make_TProfile2D\",\"$a\ $b\ $c\"\)
# root4star -q -b -l ./treeRootAnalysis.C\(\"make_TProfile2D\",\"$opts\"\)
