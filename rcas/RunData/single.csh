#!/usr/bin/tcsh
# RunDataAnalysis* a = new RunDataAnalysis("Deciles",-1,"test.list","out.root","0 004__posRnk.root deciles 12 30")
# RunDataAnalysis* a = new RunDataAnalysis("Deciles",100,"test.list","out.root","7000 24000 0.1")
    # RunDataAnalysis* a = new RunDataAnalysis("Deciles",30000,"test.list","out.root","7000 25000 11. 1",0)
set testlist = `pwd`/input/sched5B1F16A5B6A1640FC327B8B35F131DB3_31.list
root -l<<EOF
    gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
    gSystem -> Load("${FASTJET}/lib/libfastjet");
    gSystem -> Load("${FASTJET}/lib/libfastjettools");
    gSystem->Load("RunDataAnalysis");
    RunDataAnalysis* a = new RunDataAnalysis("WriteJetTree",30000,"test.list","out.root","4 1000 1",0)
EOF
