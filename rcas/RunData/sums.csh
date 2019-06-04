#!/usr/bin/tcsh

set hadd_206 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/RunData_SL18b/hadd/hadd_206.root
set hadd_004 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/RunData_SL18b/hadd/hadd_004.root
set hadd_001 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/RunData/hadd/hadd_001.root

# default inputs
set nEv = "-1"
set c0 = ""
set c1 = ""
set prog = $1
echo "Input arguments: $argv[*]"

#-----------------------
# WriteJetTree 
# required: 
#     1:WriteJetTree 2:[001/204/004] 3:jet_r[0.4] 4:opt:nEv 5:opt:name
#
#  sums.csh WriteJetTree 004 4.0 4.0 30 1000 short_test
#  sums.csh WriteJetTree 004 4.0 4.0 30 
#  sums.csh WriteJetTree 206 4.0 4.0 30 
#  sums.csh WriteJetTree 001 4.0 4.0 30 
if ( $prog == runWriteJetTree ) then
    foreach which ( "001" "004" "206" )
        sums.csh WriteJetTree $which 4.0 4.0 30.0 -1 
    end
else if ( $prog == WriteJetTree ) then
    if ( $2 == "001" || $2 == "206" || $2 == "004" ) then
        set f_in = `eval echo \${hadd_$2}`
    else 
        echo "fatal->exit: required [nch_TProfile2D] [001/004/206]"
        exit
    endif

    if ( $# < 4 ) then
        echo "fatal: $prog requires a minimum of 5 input parameters"
        exit
    endif

    set jet_R = $3
    set min_trig = $4
    set max_trig = $5
    if ( $# > 5 ) set nEv = $6
    set f_out = ${prog}_${2}_${jet_R}_Tr${min_trig}_${max_trig}$7.root
    set opts = "$prog $f_in:t $f_out $nEv $jet_R ${min_trig} ${max_trig} $c0:t $c1:t"

    star-submit-template -template sums.xml -entities\
   f_in=$f_in,f_out=$f_out,opts="${opts}",pwd=`pwd`,c0=$c0,c1=$c1,tag=${prog}_$2
#-------------------------------------
else if ( $prog == runDeciles ) then
    foreach CorRaw ( "cor" "raw" )
        foreach which ( "001" "004" "206" )
            foreach pair ( "0 0" "4 30" "8 12" "12 30" )
            sums.csh Deciles ${which} ${CorRaw} ${pair} -1
        end
    end
else if ( $prog == Deciles ) then
# input:  1:Deciles 2:[001||004||206] 3:[orig/corr] 4:min_Et 5:max_Et 6:nEv 7:name_tag
#-------------------------------------
# sums.csh Deciles 004 raw 0 0 -1 
# sums.csh Deciles 001 raw 0 0 -1
# sums.csh Deciles 206 raw 0 0 -1
# sums.csh Deciles 004 cor 0 0 -1 
# sums.csh Deciles 001 cor 0 0 -1
# sums.csh Deciles 206 cor 0 0 -1

# sums.csh Deciles 004 cor  4  0 -1
# sums.csh Deciles 206 cor  4  0 -1
# sums.csh Deciles 206 cor  8 12 -1
# sums.csh Deciles 206 cor 12 30 -1
# sums.csh Deciles 004 raw  4  0 -1
# sums.csh Deciles 206 raw  4  0 -1
# sums.csh Deciles 206 raw  8 12 -1
# sums.csh Deciles 206 raw 12 30 -1
    if ( $2 == "001" || $2 == "206" || $2 == "004" ) then
        set f_in = `eval echo \${hadd_$2}`
    else 
        echo "fatal->exit: required [nch_TProfile2D] [001/004/206]"
        exit
    endif

    if ( $#argv < 6 ) then
        echo "fatal: $prog requires a minimum of 7 input parameters"
        exit
    endif

    set c0 = /star/u/djs232/is2019/rcas/TreeRoots/_output/BBC_dist_004_posRnk.root
    if ( $3 == "raw" ) then
        set hg = "deciles"
        set bbc_corr = 0
        set corr = "bbcCorr"
    else if ( $3 == "cor" ) then
        set hg = "deciles_corr"
        set bbc_corr = 1
        set corr = "bbcRaw"
    else
        echo "Sorry, bad name for histogram, ||$3||"
        exit
    endif


    set loEt = $4
    set hiEt = $5

    if ( $#argv > 5 ) set nEv = $6

    set f_out = "${prog}_${corr}_$2_Et${loEt}_${hiEt}$7.root"

    # set opts="$prog $f_in:t $f_out $nEv $c0:t $hg $loEt $hiEt"
    # echo $opts
    star-submit-template -template sums.xml -entities\
    f_in=$f_in,f_out=$f_out,opts="$prog $f_in:t $f_out $nEv $c0:t $hg $loEt $hiEt ${bbc_corr}",pwd=`pwd`,c0=$c0,c1=$c1,tag=${prog}_$2
#--------------------------------
#       no good options         -
#--------------------------------
else 
    echo "Sorry, your input of ||$argv[*]|| isn't valid input"
endif
