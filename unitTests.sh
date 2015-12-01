#!/bin/bash

FAIL=0 ;

data_dir="workloads"
threads="8" ;
program="./bin/SkyBench" ;
algs="bskytree sfs pskyline psfs qsfs salsa psalsa qsalsa bnl pbnl qbnl hybrid treefree"

if [ $# -eq 1 ] ; then
	algs="$1";
fi

echo "Alg: $algs" ;
echo "threads: $threads"

# Test with HOUSE dataset
dataset="$data_dir/house-U-6-127931.csv"
echo -n "-> make -j8 all DIMS=6.. " ;
make clean > /dev/null ;
make -j8 all DIMS=6 > /dev/null ; # IMPORTANT TO COMPILE WITH THE RIGHT #DIMENSIONS!
echo " done" ;
echo -n "-> -f $dataset.. "
$program -f $dataset -t $threads -v -s "$algs" > /dev/null ;
if [ $? -eq 0 ]; then
    echo " done" ;
else
    echo "FAIL" ;
    FAIL=1 ;
fi

# Test with NBA dataset
dataset="$data_dir/nba-U-8-17264.csv"
echo -n "-> make -j8 all DIMS=8.. " ;
make clean > /dev/null ;
make -j8 all DIMS=8 > /dev/null ; # IMPORTANT TO COMPILE WITH THE RIGHT #DIMENSIONS!
echo " done" ;
echo -n "-> -f $dataset.. "
$program -f $dataset -t $threads -v -s "$algs" > /dev/null ;
if [ $? -eq 0 ]; then
    echo " done" ;
else
    echo "FAIL" ;
    FAIL=1 ;
fi

# Test with weather dataset
dataset="$data_dir/elv_weather-U-15-1000.csv"
echo -n "-> make -j8 all DIMS=15.. " ;
make clean > /dev/null ;
make -j8 all DIMS=15 > /dev/null ; # IMPORTANT TO COMPILE WITH THE RIGHT #DIMENSIONS!
echo " done"
echo -n "-> -f $dataset.. "
$program -f $dataset -t $threads -v -s "$algs" > /dev/null ;
if [ $? -eq 0 ]; then
    echo "done" ;
else
    echo "FAIL"
    FAIL=1 ;
fi

# Test with airline dataset
echo -n "-> make -j8 all DIMS=13.. " ;
make clean > /dev/null ;
make -j8 all DIMS=13 > /dev/null ; # IMPORTANT TO COMPILE WITH THE RIGHT #DIMENSIONS!
echo "done"

dataset="$data_dir/airline-U-13-10000.csv"
echo -n "-> -f $dataset.. "
$program -f $dataset -t $threads -v -s "$algs" > /dev/null ;

if [ $? -eq 0 ]; then
    echo "done" ;
else
    echo "FAIL"
    FAIL=1 ;
fi

# Test with all equal tuples
echo -n "-> make -j8 all DIMS=3.. " ;
make clean > /dev/null ;
make -j8 all DIMS=3 > /dev/null ; # IMPORTANT TO COMPILE WITH THE RIGHT #DIMENSIONS!
echo "done"

dataset="$data_dir/airline05_08-3-1000.csv"
echo -n "-> -f $dataset.. "
$program -f $dataset -t $threads -v -s "$algs" > /dev/null ;
if [ $? -eq 0 ]; then
    echo "done" ;
else
    echo "FAIL"
    FAIL=1 ;
fi

# Test bigger real dataset
echo -n "-> make -j8 all DIMS=4.. " ;
make clean > /dev/null ;
make -j8 all DIMS=4 > /dev/null ; # IMPORTANT TO COMPILE WITH THE RIGHT #DIMENSIONS!
echo "done"

dataset="$data_dir/elv_weather-U-4-566268.csv"
echo -n "-> -f $dataset.. "
$program -f $dataset -t $threads -v -s "$algs" > /dev/null ;
if [ $? -eq 0 ]; then
    echo "done" ;
else
    echo "FAIL"
    FAIL=1 ;
fi
