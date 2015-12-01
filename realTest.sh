#!/bin/bash
# -----------------------------------------------------------------------------
# command runner:
function docmd() {
  if [ $# -ne 2 ] ; then echo "do: $1" ; fi
  if( eval "(" $1 ")" )
  then 
    true
  else
	  echo "Command FAILED"
	  exit 1
  fi
}
# -----------------------------------------------------------------------------

# The only variable that is subject to change:
input_dir="workloads/" ;

res_file="./results/real_tests.csv" ;
program="./bin/SkyBench" ;
VERBOSE_FLAG="NVERBOSE"
verbos_opt="" # "-v" <-- use this to print details about each dataset

STARTTIME=$(date +%s)

if [ $# -ne 3 ] ; then 
  echo "USAGE: $0 #num_threads [T|D|P] \"methods\"" ;
  echo "  E.g.: $0 \"1 2 4 8\" T \"bskytree hybrid\")" ;
  echo "  T=report time; D=report dominance tests; P=PAPI measurements." ;
  echo "  Note: with P, only one skyline method in \"methods\" is allowed."
  exit 1 ;
fi

threads="$1" ;
# all algorithms:
algs="bskytree bskytreeS pbskytreeS sfs pskyline psfs qsfs salsa psalsa qsalsa bnl pbnl qbnl hybrid" ;
# single-threaded algorithms:
st_algs="bskytree bskytreeS sfs salsa bnl"
algs="$3" ;
dt="0" ;
papi="0" ;
if [ "$2" = "D" ] ; then
  dt="1";
elif [ "$2" = "P" ]; then
  papi="1";
  num_methods=$(echo "$algs" | wc -w)
  if [ $num_methods -ne 1 ] ; then
    echo "PAPI (P) does not support several skyline method (i.e., \"${algs}\")"
    exit 1
  fi
  num_threads=$(echo "$threads" | wc -w)
  if [ $num_threads -ne 1 ] ; then
    echo "PAPI (P) does not support several thread options (i.e., \"${threads}\")"
    exit 1
  fi
fi

header="dataset"
for a in $algs
do
  if [[ $st_algs == *${a}* ]]
  then
    header="${header} $a" ;
  else
    for t in $threads
    do
      header="${header} ${a}_t${t}" ;
    done
  fi      
done
echo -e "$header" ;
echo -e "$header" > $res_file ;

real_dataset="${input_dir}nba-U-8-17264.csv"
dim="8"
make clean > /dev/null ;
make -j4 all DT=${dt} DIMS=${dim} V=${VERBOSE_FLAG} PAPI=${papi}> /dev/null ;
OUTPUT=$(docmd "$program -f $real_dataset -t \"${threads}\" -s \"${algs}\" ${verbos_opt}" 1) ;
echo -e "NBA$OUTPUT" ;
echo -e "NBA$OUTPUT" >> $res_file ;

real_dataset="${input_dir}house-U-6-127931.csv"
dim="6"
make clean > /dev/null ;
make -j4 all DT=${dt} DIMS=${dim} V=${VERBOSE_FLAG} PAPI=${papi} > /dev/null ;
OUTPUT=$(docmd "$program -f $real_dataset -t \"${threads}\" -s \"${algs}\" ${verbos_opt}" 1) ;
echo -e "HOUSE$OUTPUT";
echo -e "HOUSE$OUTPUT" >> $res_file ;

real_dataset="${input_dir}elv_weather-U-15-566268.csv"
dim="15"
make clean > /dev/null ;
make -j4 all DT=${dt} DIMS=${dim} V=${VERBOSE_FLAG} PAPI=${papi} > /dev/null ;
OUTPUT=$(docmd "$program -f $real_dataset -t \"${threads}\" -s \"${algs}\" ${verbos_opt}" 1) ;
echo -e "WEATHER$OUTPUT";
echo -e "WEATHER$OUTPUT" >> $res_file ;

#real_dataset="${input_dir}airline05_08-U-13-28745461.csv"
#dim="13"
#make clean > /dev/null ;
#make -j4 all DT=${dt} DIMS=${dim} V=${VERBOSE_FLAG} PAPI=${papi} > /dev/null ;
#OUTPUT=$(docmd "$program -f $real_dataset -t \"${threads}\" -s \"${algs}\" ${verbos_opt}" 1) ;
#echo -e "AIRLINE$OUTPUT";
#echo -e "AIRLINE$OUTPUT" >> $res_file ;

ENDTIME=$(date +%s)
echo "DONE [at $(date '+%H:%M:%S'), duration: $(($ENDTIME - $STARTTIME)) s.]"

