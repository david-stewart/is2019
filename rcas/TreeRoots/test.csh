#!/usr/bin/tcsh
# set hadd_004 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/TreeRoots_SL18b/hadd/hadd_file.root # temporary file

# not, only good for |vz|<10
set hadd_004 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/TreeRoots_SL18b/hadd/hadd_004.root # good
set hadd_206 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/TreeRoots_SL18b/hadd/hadd_206.root # good
set hadd_001 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/TreeRoots/hadd/hadd_001.root       # only for |vz|<10
set c0 = "`pwd`/analysis_out/bbcVz_vzVpd_004_copy.root"
set c1 = "`pwd`/analysis_out/bbcVz_slices_vzVpd_001_vzVpd.root"
# TreeRootsAnalysis* myAnalysis = new TreeRootsAnalysis("Deciles ${hadd_004} deciles.root -1 ${infile} bbc_dist 0 0");
# TreeRootsAnalysis* myAnalysis = new TreeRootsAnalysis("TOF_dist ${hadd_004} test.root 1000000");

root4star -b -l<<EOF
    gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
    gSystem->Load("TreeRootsAnalysis");
    TreeRootsAnalysis* myAnalysis = new TreeRootsAnalysis("nch_TProfile2D ${hadd_004} test.root 10000 1 1 0 test");
EOF
