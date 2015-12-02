
|     ParallelTKDQ, version 1.0      |
|:----------------------------------:|
|        © 2015 Sean Chester         |

------------------------------------


### Table of Contents 

  * [Introduction](#introduction)
  * [Requirements](#requirements)
  * [Installation](#installation)
  * [Documentation](#documentation)
  * [License](#license)
  * [Contact](#contact)
  

------------------------------------
### Introduction
<a name="introduction" ></a>

This ParallelTKDQ software implements several alternative algorithms 
for computing top-k dominating queries with multi-core parallelism. 
The algorithms have been initially described in: 

> S. Chester et al. 2016. "Top-k dominating queries, in parallel, in   
>   memory." Under review for: Proceedings of EDBT, 2 pages.


This software is based on the SkyBench suite, which was developed for
the following paper:

> S. Chester et al. 2015. "Scalable parallelization of skyline  
>   computation for multi-core processors." In: Proceedings of  
>   the 18th IEEE International Conference on Data Engineering  
>		(ICDE 2015), pp. 1083--1094.
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
### License
<a name="license" ></a>

Copyright (c) 2015 Sean Chester

ParallelTKDQ, version 1.0, is distributed freely under the *MIT License*:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


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
