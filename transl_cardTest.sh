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

if [ $# -ne 2 ] ; then 
  echo "USAGE: ./cardTest #num_threads \"methods\" (e.g., ./cardTest 8 \"bskytree hybrid\")" ; 
  exit 1 ;
fi
#algos="bskytree sfs pskyline psfs qsfs salsa psalsa qsalsa bnl pbnl qbnl hybrid" ;
threads="$1" ;
algs="$2"

# The only variable that is subject to change:
input_dir="../workloads/" ;

dim="12"
cards="250000 500000 1000000 2000000 4000000" ;
dists="c i a" ; # "core inde anti"
dist_labels="Correlated Independant Anticorrelated" ;
labels_arr=($dist_labels) ;
output_dir="./results" ;
program="./bin/SkyBench" ;
file_prefix="trans_" ;
transl=-0.25

make clean > /dev/null ;
make -j4 all DIMS=${dim} V=NVERBOSE > /dev/null ;

i=0;
for dist in $dists
do
  res_file="${output_dir}/${file_prefix}${transl}-card-${dist}-${dim}.csv" ;
  echo "DISTRIBUTION: ${labels_arr[${i}]} ($dist)" ;
  echo "Output file: $res_file"

  outline="card $algs"

#  for t in $threads
#  do
#    outline="${outline} HPSkycube_t${t}" ;
#  done

  echo -e "$outline" ;
  echo -e "$outline" > $res_file ;
  for card in $cards
  do
    datafile="${input_dir}${file_prefix}${transl}-${dist}-${dim}-${card}.csv" ;
    OUTPUT=$(docmd "$program -f $datafile -t ${threads} -s \"${algs}\"" 0) ;
    outline="${card} $OUTPUT" ;
    echo -e "$outline";
    echo -e "$outline" >> $res_file ;
  done
  i=`expr $i + 1` ;
  echo "" ;
done

