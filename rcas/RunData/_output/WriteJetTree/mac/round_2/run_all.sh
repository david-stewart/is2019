#!/bin/bash
for x in 0 1 2 3 
do
root -l << EOF
    .L sets_hg.cc+
    sets_hg($x)
EOF
done
