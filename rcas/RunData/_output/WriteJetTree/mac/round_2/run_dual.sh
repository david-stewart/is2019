#!/bin/bash
for shift in 0 1
do
for x in 0 1 2 3
do
root -l << EOF
    .L dual_set.cc+
    dual_set($x,$shift)
EOF
done
done
