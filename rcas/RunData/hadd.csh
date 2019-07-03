#!/usr/bin/tcsh

#------------------------------------------------------------------------
# input:  [location from _output] [file_names_to_glob] [output file name]
#------------------------------------------------------------------------

if ( $# < 3 ) then
    echo "fatal: required: [dir from ./_output/] [pre-glob files] [output_file_name]"
else 
    if ( -f $3.flag ) then
        rm ${3}.flag
        star-submit-template -template hadd.xml -entities dir=$1:t,files="${2}_*.root",to_file=$3:t.root
    else
        ls _output/$1:t/${2}_*.root
        touch $3:t.flag
    endif
endif
