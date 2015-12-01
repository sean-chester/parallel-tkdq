/**
 * Header file to describe definition of Naive class.
 *
 * @author Sean Chester
 * @date 11 November 2015
 */

#ifndef NAIVE_H_
#define NAIVE_H_

#include <cstdio>
#include <map>
#include <sys/time.h>

#if defined(_OPENMP)
#include <omp.h>
#include <parallel/algorithm>
#else
#include <algorithm>
#define omp_get_thread_num() 0
#define omp_set_num_threads( t ) 0
#endif

#include "common/common.h" //was common2.h
#include "common/tkdq_solver.h"
//#include "util/papi_counting.h"


/**
 * A class for executing our Naive algorithm to compute top-k dominating queries.
 * 
 * @tparam DIMS The number of dimensions in the input dataset.
 */
template< uint32_t DIMS >
class Naive: public TKDQ_Solver {

public:
	
	/**
	 * Constructs a new instance of a Naive TKDQ solver
	 * @post Creates a new Naive TKDQ solver instance.
	 */
  Naive(uint32_t threads, uint32_t n, float** data ) :
      t_(threads), n_(n) {

    omp_set_num_threads(threads);
    result_.reserve(1024);
    data_ = NULL;
  }

	/**
	 * Destructor for the Naive TKDQ solver
	 * @post Destroys the Naive TKDQ solver instance.
	 */
  ~Naive() { }
  
  /**
   * Initializes the TKDQ solver with a new dataset.
   * @param data The flat array of floats corresponding to the 
   * input data.
   * @post Populates the internal data structures of this Naive 
   * TKDQ solver to reflect the dataset provided.
   */
  void Init( float** data );

	std::vector< uint32_t > Execute( const uint32_t k );


protected:

  // Data members:
  uint32_t n_; /**< The number of points in the dataset. */
  const uint32_t t_; /**< The number of threads with which the solution should be obtained. */
  STuple<DIMS> *data_; /**< The internal representation of the dataset. */
  std::vector< uint32_t > result_; /**< The vector that will contain the result point ids */

};

#endif /* NAIVE_H_ */
