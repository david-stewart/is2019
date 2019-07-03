#!/bin/bash
root -l <<EOF
    .L ../pt_unfold.cc+
    pt_unfold("7k24k_opt3")
EOF
