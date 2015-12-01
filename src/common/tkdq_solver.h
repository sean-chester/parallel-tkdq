/**
 * General interface for implementations of parallel algorithms 
 * that solve top-k dominating queries
 *
 * @date 11 Nov 2015
 * @author Sean Chester
 */

#ifndef TKDQ_SOLVER_H_
#define TKDQ_SOLVER_H_

#include <stdint.h>
#include <vector>

/**
 * An abstract class defining the basic behaviour 
 * of an algorithm that can solve top-k dominating 
 * queries.
 */
class TKDQ_Solver{

public:

  TKDQ_Solver() { } /**< Instantiates a new instance of a TKDQ solver */
  virtual ~TKDQ_Solver() { } /**< Destroys this instance of a TKDQ solver */

	/**
	 * Initializes the TKDQ solver with a new input dataset.
	 * @param data A flat array of floats corresponding to the input data 
	 * over which the top-k dominating query should be answered.
	 * @note Operations in this function should be restricted to those that 
	 * are unfair to time; this is meant to provide a fair starting point for 
	 * all algorithms, even if they use alternate data structures.
	 */
  virtual void Init( float** data ) = 0;
  
  /**
   * Executes a top-k dominating query on the dataset with which the TKDQ 
   * solver was initialised. 
   * @param k The number of best points that should be returned.
   * @return An ordered list of point ids corresponding to the query result. 
	 * The first point should be the point that dominates the most other data 
	 * points in the input set and the subsequent points should be the next 
	 * highest scoring data points in descending order of score. Ties should 
	 * be broken by point id in order to ensure comparability across TKDQ 
	 * solvers.
   * @post Another invocation of Init() may be necessary in order to invoke 
   * this method again. This method is free to corrupt internal data 
   * structures.
   * @note The execution of this method is what will be timed; so, all 
   * relevant computation should take place within an invocation of this.
   */
  virtual std::vector< uint32_t > Execute( const uint32_t k ) = 0;

};

#endif /* TKDQ_SOLVER_H_ */
