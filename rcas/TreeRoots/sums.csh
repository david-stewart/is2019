#!/usr/bin/tcsh

set hadd_004 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/TreeRoots_SL18b/hadd/hadd_004.root # good
set hadd_206 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/TreeRoots_SL18b/hadd/hadd_206.root # good
set hadd_001 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/TreeRoots/hadd/hadd_001.root       # only for |vz|<10
set nEv = "-1"
set c0 = ""
set c1 = ""

##################################################
# nch_TProfile2D
# sums.csh nch_TProfile2D 001 -1 1 0 0
# sums.csh nch_TProfile2D 206 -1 1 0 0
# sums.csh nch_TProfile2D 004 -1 1 0 0
# sums.csh nch_TProfile2D 001 -1 0 0 0
# sums.csh nch_TProfile2D 206 -1 0 0 0
# sums.csh nch_TProfile2D 004 -1 0 0 0

# sums.csh nch_TProfile2D 001 -1 1 1 0
# sums.csh nch_TProfile2D 206 -1 1 1 0
# sums.csh nch_TProfile2D 004 -1 1 1 0
# sums.csh nch_TProfile2D 001 -1 0 1 0
# sums.csh nch_TProfile2D 206 -1 0 1 0
# sums.csh nch_TProfile2D 004 -1 0 1 0

# sums.csh nch_TProfile2D 001 -1 1 0 1
# sums.csh nch_TProfile2D 206 -1 1 0 1
# sums.csh nch_TProfile2D 004 -1 1 0 1
# sums.csh nch_TProfile2D 001 -1 0 0 1
# sums.csh nch_TProfile2D 206 -1 0 0 1
# sums.csh nch_TProfile2D 004 -1 0 0 1

# sums.csh nch_TProfile2D 001 -1 1 1 1
# sums.csh nch_TProfile2D 206 -1 1 1 1
# sums.csh nch_TProfile2D 004 -1 1 1 1
# sums.csh nch_TProfile2D 001 -1 0 1 1
# sums.csh nch_TProfile2D 206 -1 0 1 1
# sums.csh nch_TProfile2D 004 -1 0 1 1
##################################################
if ( $1 == "drive_nch_TProfiles2D" ) then
    foreach set ( "001" "206" "004" )
        foreach rnk ( 0 1 )
            foreach corr ( 0 1 )
                foreach gt4GeV ( 0 1 )
                    echo "./sums.csh nch_TProfile2D $set -1 $rnk $corr $gt4GeV"
                    ./sums.csh nch_TProfile2D $set -1 $rnk $corr $gt4GeV
                end
            end
        end
    end
else if ( $1 == "in_vz6" ) then
    foreach set ( "001" "206" "004" )
        foreach rnk ( 1 )
            foreach corr ( 1 )
                foreach gt4GeV ( 0 1 )
                    echo "./sums.csh nch_TProfile2D $set -1 $rnk $corr $gt4GeV corr"
                    ./sums.csh nch_TProfile2D $set -1 $rnk $corr $gt4GeV corr
                end
            end
        end
    end

else if ( $1 == "nch_TProfile2D" ) then
    if ( $2 == "001" || $2 == "206" || $2 == "004" ) then
        set f_in = `eval echo \${hadd_$2}`
    else 
        echo "fatal->exit: required [nch_TProfile2D] [001/004/206]"
        exit
    endif
    if ( $# > 2 ) set nEv = $3

    if ( $4 == 1 ) then
        set rnk = "_posRnk"
    else 
        set rnk = "_negRnk"
    endif

    if ( $5 == 1 ) then
        set corr = "_bbcCor"
    else 
        set corr = "_bbcRaw"
    endif

    if ( $6 == 1 ) then
        set gt4GeV = "_gt4GeV_"
    else
        set gt4GeV = ""
    endif

    set f_out = nch_TProfile2D_$2${rnk}${gt4GeV}${corr}$7.root
    set prog = $1
    star-submit-template -template sums.xml -entities\
        f_in=$f_in,f_out=$f_out,opts="$prog $f_in:t $f_out $nEv $4 $5 $6 $c0:t $c1:t",pwd=`pwd`,c0=$c0,c1=$c1,tag=$prog$2

    #--------------------------------
    # TOF_dist:
    # need file type and nEv and name
    # sums.csh TOF_dist 206 -1 lt10
    # sums.csh TOF_dist 001 -1 lt10
    # sums.csh TOF_dist 004 -1 lt10
    # sums.csh TOF_dist 004 1000000 test
    #--------------------------------
else if ( $1 == "TOF_dist" ) then
    set prog = $1
    if ( $2 == "001" || $2 == "206" || $2 == "004" ) then
        set f_in = `eval echo \${hadd_$2}`
    else 
        echo "fatal->exit: required [nch_TProfile2D] [001/004/206]"
        exit
    endif
    if ( $# > 2 ) set nEv = $3
    set f_out = TOF_dist_$2$4.root
    set prog = $1
    star-submit-template -template sums.xml -entities\
        f_in=$f_in,f_out=$f_out,opts="$prog $f_in:t $f_out $nEv $4 $5 $6 $c0:t $c1:t",pwd=`pwd`,c0=$c0,c1=$c1,tag=$prog$2
else if ( $1 == "LumiProfiles" ) then
    # sums.csh LumiProfiles 0
    # sums.csh LumiProfiles 1
    if ( $# < 1 ) then
        echo "fatal->exit: required LumiProfiles [0/1]"
        exit
    endif
    set prog=$1
    set f_in=$hadd_004
    set f_out="LumiPr004_pRank_$2.root"
    star-submit-template -template sums.xml -entities\
    f_in=$f_in,f_out=$f_out,opts="$prog $f_in:t $f_out $nEv $2 $c0:t $c1:t",pwd=`pwd`,c0=$c0,c1=$c1,tag=$prog$2
else if ( $1 == "drive_BBC_dist" ) then
    foreach set ( "001" "206" "004" )
        foreach rnk ( 0 1 )
            echo "./sums.csh BBC_dist ${set} ${rnk}"
            ./sums.csh BBC_dist ${set} ${rnk}
        end
    end
else if ( $1 == "BBC_dist" ) then
    if ( $# < 3 ) then
        echo "fatal->exit: required BBC_dist [0/1]"
        exit
    endif
    if ( $2 == "001" || $2 == "206" || $2 == "004" ) then
        set f_in = `eval echo \${hadd_$2}`
    else 
        echo "fatal->exit: required [nch_TProfile2D] [001/004/206]"
        exit
    endif

    if ( $3 == 1 ) then
        set rnk = "_posRnk"
    else 
        set rnk = "_negRnk"
    endif
    set prog=$1
    set which=$2
    set f_out="${prog}_${which}${rnk}$4.root"

    star-submit-template -template sums.xml -entities\
    f_in=$f_in,f_out=$f_out,opts="$prog $f_in:t $f_out $nEv $3 $c0:t $c1:t",pwd=`pwd`,c0=$c0,c1=$c1,tag=$prog$2
else if ( $1 == "Nch" ) then
    set prog=$1
    set f_in=$hadd_004
    set f_out="$1_004.root"
    star-submit-template -template sums.xml -entities\
    f_in=$f_in,f_out=$f_out,opts="$prog $f_in:t $f_out $nEv $2 $c0:t $c1:t",pwd=`pwd`,c0=$c0,c1=$c1,tag=$prog$2
else
    echo "Sorry, your input of $* isn't valid input"
endif
