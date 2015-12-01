/**
 * Header file to describe definition of Refinement TKDQ solver.
 *
 * @author Sean Chester
 * @date 25 November 2015
 * @see ML Yiu and N Mamoulis. (2009) "Multi-dimensional top-k
 * dominating queries." VLDB Journal 18:695–718.
 */

#ifndef REFINEMENT_H_
#define REFINEMENT_H_

#include <cstdio>
#include <map>
#include <array>
#include <sys/time.h>

#if defined(_OPENMP)
#include <omp.h>
#include <parallel/algorithm>
#else
#include <algorithm>
#define omp_get_thread_num() 0
#define omp_set_num_threads( t ) 0
#endif

#include "common/common.h" 
#include "common/tkdq_solver.h"



typedef std::pair< uint32_t, uint32_t > Bounds; /**< A [lower, upper] bound pair */
typedef std::vector< uint32_t > CandidateSet; /**< List of indexes of candidate points */

/**
 * A class for executing a Refinement algorithm to compute top-k 
 * dominating queries.
 * 
 * @tparam DIMS The number of dimensions in the input dataset.
 */
template< uint32_t dims >
class Refinement: public TKDQ_Solver {

public:
	
	/**
	 * Constructs a new instance of a Refinement TKDQ solver
	 * @post Creates a new Naive TKDQ solver instance.
	 */
  Refinement(uint32_t threads, uint32_t n, float** data ) :
      t_(threads), n_(n) {

    omp_set_num_threads(threads);
    result_.reserve(1024);
    data_ = NULL;
  }

	~Refinement() { }
  
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

  // protected data members:
  uint32_t n_; /**< The number of points in the dataset. */
  const uint32_t t_; /**< The number of threads with which the solution should be obtained. */
  PTuple< dims > *data_; /**< The internal representation of the dataset. */
  std::vector< uint32_t > result_; /**< The vector that will contain the result point ids */

private:

	/**
	 * Conducts the counting pass of the Refinement algorithm.
	 * @param k The number of points that should eventually be output by the 
	 * TKDQ solver.
	 * @return The minimum upper bound, gamma, that a partition must have to possibly 
	 * contain TKDQ points.
	 * @post grid_cell_bounds is populated with upper bounds for each cell i,
	 * corresponding to the cell counts of all other cells partially or fully 
	 * dominated by cell i.
	 */
	uint32_t counting_pass( const uint32_t k );
	
	
	/**
	 * Conducts the filter pass of the Refinement algorithm, using the 
	 * coarse-grained filter (Algorithm 5), adapted for multicore and main memory.
	 * @param gamma A lower-bound threshold; any point with an upper bound less than 
	 * gamma is most certainly *not* a candidate TKDQ point.
	 * @param k The number of points that should eventually be output by the 
	 * TKDQ solver.
	 * @post Modifies the candidates_ set for unpruned grid cells.
	 */ 
	void coarse_grained_filter( const uint32_t gamma, const uint32_t k );
		
	/**
	 * Conducts the final refinement pass (Algorithm 7) of the Refinement 
	 * algorithm in which scores are actually calculated for points.
	 * @post The score attribute is updated for data points that were 
	 * previously denoted as candidates.
	 * @note This is *not* a correct implementation of Algorithm 7 in the sense 
	 * of matching the pseudocode. Algorithm 7 is designed to pick a point p' and  
	 * update the score for every point dominating p'. This is not conducive to 
	 * parallelization; so, I have flipped the logic to instead pick a point p 
	 * and find all the points it dominates, thereby only ever modifying one local 
	 * variable. The impact on efficiency of this is not yet ascertained (there must 
	 * be a reason they designed Algorithm 7 in the fashion they did); however, I 
	 * would expect this implementation here to also make better use of cache. We 
	 * found this (rather than the "correct") approach to be faster in our ICDE and 
	 * PVLDB parallel skyline papers.
	 */
	void refinement_pass();
	
	/**
	 * Returns the number of points that have so far survived as TKDQ candidates.
	 * @return The number of points that have so far survived as TKDQ candidates.
	 */
	uint32_t num_candidates();
	
	/**
	 * Copies the actual result from the internal data structures of the algorithm 
	 * into the standardized vector format that all algorithms must produce (i.e., 
	 * a list of ordered point ids).
	 * @param k The number of points that should be copied into the output vector.
	 * @post Modifies the result_ member variable to know contain the solution that 
	 * should be reported back to a user. Also, resorts the data as if it were 
	 * STuple objects (i.e., by score then point id).
	 */ 
	void prepare_result( const uint32_t k );
	
	// private data members, only for computation of local methods to ease method signatures
	std::array< Bounds, ( 1 << dims ) > grid_cell_bounds_; /**< Lower/upper bounds for each cell */
	std::array< bool, ( 1 << dims ) > pruned_; /**< Indicates which grid cells have been pruned. */
	std::array< CandidateSet, ( 1 << dims ) > candidates_; /**< Candidate points in each cell */
	std::array< uint32_t, ( 1 << dims ) > cell_dom_by_counts_; /**< # points dominating each cell */
};

#endif /* REFINEMENT_H_ */
