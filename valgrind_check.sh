#!/bin/sh

make clean

make -j4 dbg DIMS=8 > /dev/null ;

valgrind -v --leak-check=full --show-reachable=yes \
    ./bin/SkyBench -f workloads/nba-U-8-17264.csv -t 1 -v -s "$1"
