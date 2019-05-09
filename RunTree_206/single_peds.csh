#!/bin/bash
# statusfile=/star/u/djs232/pAu-2015/picoQA/list/2019.02.21_BadTowerStatus_update.list
# infile=/star/data20/reco/production_pAu200_2015/ReversedFullField/P16id.SL18b/2015/140/16140009/st_physics_16140009_raw_1000022.picoDst.root
# list=/star/u/djs232/pAu-2015/picoQA/list

# echo THIS AND $STATUSFILE THAT and $MYLIST
starver SL18h
root4star -q -b ./peds.C\(\)
