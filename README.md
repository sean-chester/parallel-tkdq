This is SkyBench, our experiments suite for testing the performance of skyline
algorithms on large-scale, real-valued workloads. Please consult the licensing
shipped with this code, and, if using it for academic purposes, cite the 
following paper:

@InProceedings{mcsky,
 author = {Chester, Sean and {\v S}idlauskas, Darius and Assent, Ira and B{\o}gh, Kenneth S},
 title = {Scalable parallelization of skyline computation for multi-core processors},
 booktitle = {Proceedings of the 18th IEEE International Conference on Data Engineering (ICDE 2015)},
 year = {2015},
 pages = {},
 publisher = {IEEE},
}

QUICK-START:
============
To run, the code needs to be compiled with the given number of dimensions*.
For example, to compute the skyline of the 8-dimensional NBA data set located
in workloads/nba-U-8-17264.csv, do:

$ make all DIMS=8
$ ./bin/SkyBench -f workloads/nba-U-8-17264.csv -v

By default, it will compute skyline with all algorithms. Running ./bin/SkyBench
without parametres will provide more details about the supported options.

You can make use of the provided shell script (/script/runExp.sh) that does all of
the above automatically. For details, execute:
$ ./script/runExp.sh

To reproduce the experiment with real datasets (Table II in the paper), do (assuming
a 16-core machine):
$ ./scripts/realTest.sh 16 T "bskytree pbskytree pskyline qflow hybrid"

* For performance reasons, few previous skyline algorithms that we obtained from
  authors were compiling their code for a specific number of dimensions. For a fair
  comparison, we developed our algorithms following the same approach. Nevertheless,
  we have also tried a prototype without this peculiarity (using C++ templates) and
  were able to achieve very similar performance too.

DATASETS:
=========
We include two classic skyline datasets (NBA and HOUSE):
workloads/nba-U-8-17264.csv
workloads/house-U-6-127931.csv

And WEATHER dataset that we obtained from http://www.cru.uea.ac.uk/cru/data/hrg/tmc:
workloads/elv_weather-U-15-566268.csv
