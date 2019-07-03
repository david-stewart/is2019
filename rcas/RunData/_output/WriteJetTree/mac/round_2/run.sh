#!/bin/bash
root -l << EOF
    .L dual_set.cc+
    dual_set($1,0,0)
EOF
