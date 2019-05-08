# if [ -e TM.root ]; then rm TM.root; fi
# for part in T MT TM 
# do
#     rootcp ../emb_data/TM.root:$part* TM.root
#     rootcp ../emb_data/TM.root:A_$part* TM.root
#     rootcp ../emb_data/TM.root:B_$part* TM.root
# done
# rootcp ../fn_weight.root:*_abund TM.root

if [ -e TM_weighted.root ]; then rm TM_weighted.root; fi

for part in pi pin p pbar K Kn
do
    # echo a1
    echo working on $part
    ./run.cc $part
    ./run.cc $part A_
    ./run.cc $part B_
    # echo a2
    for type in T M TM MT naif
    do
        # echo a3 $type
        from="$type"_"$part"
        rootcp TM_weighted.root:$from   tmp_$part.root:$type
        rootcp TM_weighted.root:A_$from tmp_$part.root:A_$type
        rootcp TM_weighted.root:B_$from tmp_$part.root:B_$type
        # echo a4 $type
    done
done
# echo a5
hadd tmp_hadd.root tmp*.root 
rootmv tmp_hadd.root:* TM_weighted.root

# rm tmp*
