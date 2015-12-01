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
  echo "USAGE: ./dimTest #num_threads \"methods\" (e.g., ./dimTest 8 \"bskytree hybrid\")" ; 
  exit 1 ;
fi
threads="$1" ;
#algs="bskytree sfs pskyline psfs qsfs salsa psalsa qsalsa bnl pbnl qbnl hybrid" ;
algs="$2" ;

# The only variable that is subject to change:
input_dir="../workloads/" ;

dims="6 8 10 12 14 16"
card="1000000" ;
dists="c i a" ; # "core inde anti"
dist_labels="Correlated Independant Anticorrelated" ;
labels_arr=($dist_labels) ;
output_dir="./results" ;
program="./bin/SkyBench" ;
file_prefix="trans_" ;
transl=-0.25

i=0;
for dist in $dists
do
  res_file="${output_dir}/${file_prefix}${transl}-dim-${dist}-${card}.csv" ;
  echo "DISTRIBUTION: ${labels_arr[${i}]} ($dist)" ;
  echo "Output file: $res_file"
  outline="dim $algs"

#  for t in $threads
#  do
#    outline="${outline} HPSkycube_t${t}" ;
#  done

  echo -e "$outline" ;
  echo -e "$outline" > $res_file ;

  for dim in $dims
  do
      # Compile for each number of dimensions
      make clean > /dev/null ;
      make -j4 all DIMS=${dim} V=NVERBOSE > /dev/null ;
      
      datafile="${input_dir}${file_prefix}${transl}-${dist}-${dim}-${card}.csv" ;
      #echo "RUNNING: $program $datafile $threads" ;
      OUTPUT=$(docmd "$program -f $datafile -t ${threads} -s \"${algs}\"" 0) ;
      outline="${dim} $OUTPUT" ;
      echo -e "$outline";
      echo -e "$outline" >> $res_file ;
	    # echo $outfile ;
  done
  i=`expr $i + 1` ;
  echo "" ;
done

