#!/usr/bin/tcsh
set infile=/star/u/djs232/is2019/rcas/RunData/mac/bbc_dist_500004.root
set hadd_004 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/TreeRoots_SL18b/hadd/hadd_file.root # temporary file
set hadd_001 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/TreeRoots/hadd/hadd_file.root          # good file
set c0 = "`pwd`/analysis_out/bbcVz_vzVpd_004_copy.root"
set c1 = "`pwd`/analysis_out/bbcVz_slices_vzVpd_001_vzVpd.root"
# TreeRootsAnalysis* myAnalysis = new TreeRootsAnalysis("Deciles ${hadd_004} deciles.root -1 ${infile} bbc_dist 0 0");

root4star -b -l<<EOF
    gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
    gSystem->Load("TreeRootsAnalysis");
    TreeRootsAnalysis* myAnalysis = new TreeRootsAnalysis("normAcceptanceTo004 ${hadd_001} nA.root 1000 $c0 $c1 5 26 35");
EOF
