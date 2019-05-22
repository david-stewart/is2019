#!/usr/bin/tcsh

set hadd_206 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/RunData_SL18b/hadd/hadd_500206.root
set hadd_004 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/RunData_SL18b/hadd/hadd_500004.root
set hadd_001 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/RunData/hadd/hadd_500001.root

# default inputs
set nEv = "-1"
set c0 = ""
set c1 = ""
set prog = $1

#-------------------------------------
# run deciles
# input:  1:Deciles 2:[001||004||206] 3:[orig/corr] 4:min_Et 5:max_Et 6:nEv  7:name_tag
#-------------------------------------
# sums.csh Deciles 004 orig 0 0 -1 o
# sums.csh Deciles 004 corr 0 0 -1 c
# sums.csh Deciles 001 orig 0 0 -1 o
# sums.csh Deciles 001 corr 0 0 -1 c
# sums.csh Deciles 206 corr 0 0 -1 c
# sums.csh Deciles 206 corr 0 0 -1 c
if ( $prog == Deciles ) then
    echo "Input arguments: $argv[*]"
    if ( $#argv < 5 ) then
        echo "fatal: $prog requires a minimum of 6 input parameters"
        exit
    endif

    set c0=/star/u/djs232/is2019/rcas/RunData/mac/bbc_dist_500004.root
    if ( $3 == "orig" ) then
        set hg = "o_dist"
    else if ( $3 == "corr" ) then
        set hg = "d_dist"
    else
        echo "Sorry, bad name for histogram, ||$3||"
        exit
    endif

    if ( $#argv > 5 ) set nEv = $6

    set loEt = $4
    set hiEt = $5

    if ( $2 == "001" ) then
        set f_in = $hadd_001
    else if ( $argv[2] == "004" ) then
        set f_in = $hadd_004
    else if ( $argv[2] == "206" ) then
        set f_in = $hadd_206
    else 
        echo "Sorry, your input of ||$argv[*]|| isn't valid input"
        exit
    endif
    set f_out = "${prog}_$2_Et${loEt}_${hiEt}$7.root"

    # set opts="$prog $f_in:t $f_out $nEv $c0:t $hg $loEt $hiEt"
    # echo $opts
    star-submit-template -template sums.xml -entities\
    f_in=$f_in,f_out=$f_out,opts="$prog $f_in:t $f_out $nEv $c0:t $hg $loEt $hiEt",pwd=`pwd`,c0=$c0,c1=$c1,tag=${prog}_$2

#--------------------------------
#       no good options         -
#--------------------------------
else 
    echo "Sorry, your input of ||$argv[*]|| isn't valid input"
endif
