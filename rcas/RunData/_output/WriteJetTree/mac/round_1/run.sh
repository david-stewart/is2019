#!/bin/bash
root -l << EOF
    .L sets_hg.cc+
    sets_hg($1,"$2")
EOF
