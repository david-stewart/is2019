#!/usr/bin/tcsh

set hadd_004 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/TreeRoots_SL18b/v1/hadd_file.root # temporary file
set hadd_001 = /gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/TreeRoots/hadd_file.root          # good file

# root4star -q -b -l ./treeRootAnalysis.C\(\"$hadd_001\ 1000\ test.root\",\"make_TProfile2D\"\)

if ( $argv[1] == "TProfile2D" ) then
    if ( $argv[2] == "001" ) then
        star-submit-template -template sums/sums.xml -entities pwd=`pwd`,cp_file=$hadd_001,in_file=hadd_file.root,out_file=TProfile2D_001.root
    else if ( $argv[2] == 004 ) then
        star-submit-template -template sums/sums.xml -entities pwd=`pwd`,cp_file=$hadd_004,in_file=hadd_file.root,out_file=TProfile2D_004.root
    else 
        echo "Sorry, your input of $argv[*] isn't valid input"
    endif
endif


# set larg = $argv[1]
# # echo a0
# if ( $#argv > 1 ) then
#     foreach x ( $argv[2-$#] )
#         set larg = "$larg\ $x"
#     end
# endif
# echo "Larg: $larg "

# echo "First '$argv[1]' second '$argv[3]' and all: '$argv[*]'"
# echo "Last: $argv[$#]"

# set lib=${2}

# if ($# == 1) then
# 	set n_runs=$1
# else
# 	set n_runs=1000
# endif

# switch (${lib})
# case "SL18b":
#     set full_in_file="/gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/TreeRoots_SL18b/hadd_file.root"
# 	set loc_in_file=$full_in_file:t
# 	set out_file="myFits_SL18b.root"
# 	breaksw
# case "HOTEL":
#     set full_in_file="/gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/TreeRoots/hadd_file.root"
# 	set loc_in_file=$full_in_file:t
# 	set out_file="myFit_HOTEL.root"
# 	breaksw
# default:
# 	echo Running short local job
#     root4star -q -b -l ./myFit.C\(\"_TreeRoots_haddfile.root\",\"p2_HOTEL.root\",${n_runs}\)
# 	exit
# endsw
# # echo -entities full_in_file=${full_in_file},loc_in_file=${loc_in_file},out_file=${outfile},pwd=`pwd`
# star-submit-template -template _sums.xml\
#  -entities full_in_file=${full_in_file},loc_in_file=${loc_in_file},out_file=${out_file},pwd=`pwd`,n_runs=${n_runs}
	
# # root4star -q -b -l ./myFit.C\(\"hadd_file_HOTEL.root\",\"p2_HOTEL.root\",100000\)
# # root4star -q -b -l ./myFit.C\(\"hadd_file_HOTEL.root\",\"p2_HOTEL.root\",100000\)
# # root4star -q -b -l ./myFit.C\(\"test_in.root\",\"test_out.root\"\)
