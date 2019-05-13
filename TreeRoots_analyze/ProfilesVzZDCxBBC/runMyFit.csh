#!/usr/bin/csh

set lib=${2}

if ($# == 1) then
	set n_runs=$1
else
	set n_runs=1000
endif

switch (${lib})
case "SL18b":
    set full_in_file="/gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/TreeRoots_SL18b/hadd_file.root"
	set loc_in_file=$full_in_file:t
	set out_file="myFits_SL18b.root"
	breaksw
case "HOTEL":
    set full_in_file="/gpfs/mnt/gpfs01/star/pwg/dstewart/outroot/is2019/TreeRoots/hadd_file.root"
	set loc_in_file=$full_in_file:t
	set out_file="myFit_HOTEL.root"
	breaksw
default:
	echo Running short local job
    root4star -q -b -l ./myFit.C\(\"hadd_file_HOTEL.root\",\"p2_HOTEL.root\",${n_runs}\)
	exit
endsw
# echo -entities full_in_file=${full_in_file},loc_in_file=${loc_in_file},out_file=${outfile},pwd=`pwd`
star-submit-template -template _sums.xml\
 -entities full_in_file=${full_in_file},loc_in_file=${loc_in_file},out_file=${out_file},pwd=`pwd`,n_runs=${n_runs}
	
# root4star -q -b -l ./myFit.C\(\"hadd_file_HOTEL.root\",\"p2_HOTEL.root\",100000\)
# root4star -q -b -l ./myFit.C\(\"hadd_file_HOTEL.root\",\"p2_HOTEL.root\",100000\)
# root4star -q -b -l ./myFit.C\(\"test_in.root\",\"test_out.root\"\)
