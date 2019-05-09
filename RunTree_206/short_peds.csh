#!/bin/csh
starver SL18b
echo "STAR_VERSION: $STAR_VERSION"
root4star -q -b -l ./peds.C\(-1,\"/star/u/djs232/pAu-2015/picoQA/list/16.list\"\)
