#!/usr/bin/tcsh

set hadd_004 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/TreeRoots_SL18b/hadd/hadd_file.root # temporary file
set hadd_001 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/TreeRoots/hadd/hadd_file.root          # good file
set nEv = "-1"

#-------------------------------------
# run normAcceptanceTo004
#-------------------------------------
#
# treeRootAnalysis.csh normAcceptanceTo004 004 -1
# treeRootAnalysis.csh normAcceptanceTo004 001 -1
if ( $argv[1] == "normAcceptanceTo004" ) then
    echo $argv[*]

    set c0 = "`pwd`/analysis_out/bbcVz_vzVpd_004_copy.root"
    set prog="normAcceptanceTo004"

    if ( $#argv > 2 ) set nEv = $3
    if ( $argv[2] == 001 ) then
        set f_in = $hadd_001
        set f_out = "normAcceptance_001_vzVpd$4.root"
        set c1 = "`pwd`/analysis_out/bbcVz_slices_vzVpd_001_vzVpd.root"
    else if ( $argv[2] == 004 ) then
        set f_in = $hadd_004
        set f_out = "normAcceptance_004_vzVpd$4.root"
        set c1 = "`pwd`/analysis_out/bbcVz_slices_vzVpd_004_vzVpd.root"
    else 
        echo "Sorry, your input of $argv[*] isn't valid input"
        exit
    endif
    star-submit-template -template sums/sums.xml -entities\
    f_in=$f_in,f_out=$f_out,opts="$prog $f_in:t $f_out $nEv $c0:t $c1:t 5 26 35",pwd=`pwd`,c0=$c0,c1=$c1,tag=$prog
#-------------------------------------
# run nch_in_deciles
#-------------------------------------
# treeRootAnalysis.csh nch_in_deciles 001/004 [nEv] [name_tag]
else if ( $argv[1] == "nch_in_deciles" ) then
    set c0 = "`pwd`/mac/deciles_004.root"
    set c1 = "`pwd`/mac/deciles_004.list"

    if ( ! -e $c0 ) then
        echo "fatal: missing file $c0, which is required to run this script"
        exit
    else if ( ! -e $c1 ) then
        echo "fatal: missing file $c0, which is required to run this script"
        exit
    endif

    echo $argv[*]
    set prog="nch_in_deciles"
    if ( $#argv > 2 ) set nEv = $3
    if ( $argv[2] == 001 ) then
        set f_out = "nch_in_deciles_001$4.root"
        set f_in = $hadd_001
        set which_file = hg_001
    else if ( $argv[2] == 004 ) then
        set f_out = "nch_in_deciles_004$4.root"
        set f_in = $hadd_004
        set which_file = hg_004
    else 
        echo "Sorry, your input of $argv[*] isn't valid input"
        exit
    endif
    star-submit-template -template sums/sums.xml -entities\
    f_in=$f_in,f_out=$f_out,opts="$prog $f_in:t $f_out $nEv $c0:t $c1:t $which_file",pwd=`pwd`,c0=$c0,c1=$c1,tag=$prog
#-------------------------------------
# run TProfile2D
#-------------------------------------
else if ( $argv[1] == "TProfile2D" ) then
    echo $argv[*]
    set prog="make_TProfile2D"
    if ( $#argv > 2 ) set nEv = $3
    if ( $argv[2] == 001 ) then
        set f_out = "TProfile2D_001$4.root"
        set f_in = $hadd_001
    else if ( $argv[2] == 004 ) then
        set f_out = "TProfile2D_004$4.root"
        set f_in = $hadd_004
    else if ( $argv[2] == 206 ) then
        set f_out = "TProfile2D_004$4.root"
        set f_in = $hadd_206
    else 
        echo "Sorry, your input of $argv[*] isn't valid input"
        exit
    endif
    star-submit-template -template sums/sums.xml \
        -entities f_in=$f_in,f_out=$f_out,opts="$prog $f_in:t $f_out $nEv",pwd=`pwd`,c0="",c1="",tag=$prog
#-------------------------------------
# run Fit2D
#-------------------------------------
#  treeRootAnalysis.csh Fit2D analysis_out/bbcVz_slices_vzVpd_004_vzVpd.root pr2 0 fit.root
else if ( $argv[1] == "Fit2D" ) then
    echo "Making fits"
    root -l $argv[2]<<EOF
    .L root_scripts/fit2D.C
    fit2D($argv[3],$argv[4],"$argv[5]")
EOF
#-------------------------------------
# run bbcVz_slices
#-------------------------------------
else if ( $argv[1] == "bbcVz_slices" ) then
    echo $argv[*]
    set prog="bbcVz_slices"
    if ( $#argv > 2 ) set nEv = $3
    set f_out = "${prog}_$argv[2]$4.root" # name like bbcVz_slices_001[optional].rootk

    if ( $argv[2] == 001 ) then
        set f_in = $hadd_001
    else if ( $argv[2] == 004 ) then
        set f_in = $hadd_004
    else 
        echo "Sorry, your input of $argv[*] isn't valid input"
        exit
    endif
    star-submit-template -template sums/sums.xml\
        -entities f_in=$f_in,f_out=$f_out,opts="$prog $f_in:t $f_out $nEv",pwd=`pwd`,c0="",c1="",tag=$prog
#-------------------------------------
# run bbcVz_slices v2
#-------------------------------------
# commands to run:
# tree
#   treeRootAnalysis.csh bbcVz_slices_vzVpd 001 -1 6
#   treeRootAnalysis.csh bbcVz_slices_vzVpd 001 100000 6 short
#   treeRootAnalysis.csh bbcVz_slices_vzVpd 004 -1 6
else if ( $argv[1] == "bbcVz_slices_vzVpd" ) then
    echo $argv[*]
    set prog="bbcVz_slices"
    if ( $#argv > 2 ) set nEv = $3
    set f_out = "${prog}_vzVpd_$argv[2]$5_vzVpd.root" # name like bbcVz_slices_vzVpd_001[optional].root

    if ( $argv[2] == 001 ) then
        set f_in = $hadd_001
    else if ( $argv[2] == 004 ) then
        set f_in = $hadd_004
    else 
        echo "Sorry, your input of $argv[*] isn't valid input"
        exit
    endif
    star-submit-template -template sums/sums.xml\
        -entities f_in=$f_in,f_out=$f_out,opts="$prog $f_in:t $f_out $nEv 6",pwd=`pwd`,c0="",c1="",tag=$prog

else 
    echo "Sorry, your input of $argv[*] isn't valid input"
endif
