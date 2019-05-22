#!/usr/bin/tcsh
# echo "Sorry, your input of ||$argv[*]|| isn't valid input"
# echo "There are |$#argv| events with t$2h"
set c0=/star/u/djs232/is2019/rcas/RunData/mac/bbc_dist_500004.root
set hadd_004 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/RunData_SL18b/hadd/hadd_500004.root
root4star -b -l<<EOF
    gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
    gSystem->Load("RunDataAnalysis");
    RunDataAnalysis* myAnalysis = new RunDataAnalysis("Deciles $hadd_004 out.root -1 ${c0} o_dist 0 0");
EOF
