
|     ParallelTKDQ, version 1.0      |
|:----------------------------------:|
|        © 2015 Sean Chester         |

------------------------------------


### Table of Contents 

  * [Introduction](#introduction)
  * [Requirements](#requirements)
  * [Installation](#installation)
  * [Documentation](#documentation)
  * [Contact](#contact)
  

------------------------------------
### Introduction
<a name="introduction" ></a>

This ParallelTKDQ software implements several alternative algorithms 
for computing top-k dominating queries with multi-core parallelism. 
The algorithms have been initially described in: 

> Sean Chester, Orestis Gkorgkas, and Kjetil Nørvåg. (2016).
>   "Top-k dominating queries, in parallel, in memory."
>   In: _Proceedings of the 19th International Conference on
>   Extending Database Technology (EDBT)_, 698–699.
>   doi: https://doi.org/10.5441/002/edbt.2016.93.


This software is based on [the SkyBench suite](https://github.com/sean-chester/SkyBench),
which was developed for the following paper:

> Sean Chester, Darius Sidlauskas, Ira Assent, and Kenneth S. Bøgh. (2015).
>   "Scalable parallelization of skyline computation for multi-core processors."
>   In: _Proceedings of the 18th IEEE International Conference on Data Engineering
>		(ICDE 2015)_, pp. 1083–1094.
>   doi: http://dx.doi.org/10.1109/ICDE.2015.7113358


------------------------------------
### Requirements
<a name="requirements" ></a>

This software relies on the following packages/libraries:

 * OpenMP for parallelization
 * C++ 11 for newer programming features such as templates and auto types


------------------------------------
### Installation
<a name="installation" ></a>

To generate an executable, simply type `make all` from the root directory
of this package (the same directory in which you found this `README.md` file).
The makefile will generate the executable `bin/ParallelTKDQ`. If you 
encounter difficulties, try first typing `make deepclean` and ensure that
the `bin/` directory exists. You can run the executable from a terminal with 
no command line arguments to get usage instructions. 



------------------------------------
### Documentation
<a name="documentation" ></a>

The code has been documented for `doxygen`. If the `doc/html/` 
directory is empty or stale, you can regenerate the documentation 
by running `doxygen Doxyfile` from within the `doc/` subdirectory.
The `doxygen` settings are included in `doc/Doxyfile` and can be 
freely modified to suit your preferences.


------------------------------------
### Contact
<a name="contact"></a>

This software suite may be updated and so you are encouraged to check  
[ParallelTKDQ on GitHub](#) to ensure 
this is the latest version. Do not hesitate to contact the author 
if you have comments, questions, or bugs to report, or just to let us know 
that you found this software useful and that we should continue releasing 
more.

------------------------------------
