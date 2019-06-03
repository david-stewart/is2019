#!/usr/bin/tcsh
# echo "Sorry, your input of ||$argv[*]|| isn't valid input"
# echo "There are |$#argv| events with t$2h"


set c0 = /star/u/djs232/is2019/rcas/TreeRoots/_output/BBC_dist_004_posRnk.root
set hadd_004 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/RunData_SL18b/hadd/hadd_500004.root
set hadd_206 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/RunData_SL18b/hadd/hadd_500206.root
set hadd_spec = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/RunData_SL18b/206/41CA9473522775DF57F36DE661D0B6E1_374.root
root4star -b -l<<EOF
    gROOT   -> LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
    gSystem -> Load("$FASTJET/lib/libfastjet");
    gSystem -> Load("$FASTJET/lib/libfastjettools");
    gSystem -> Load("RunDataAnalysis");
    RunDataAnalysis* myAnalysis = new RunDataAnalysis("Deciles $hadd_004 out.root 100 ${c0} deciles  0 0 0 ");
EOF
