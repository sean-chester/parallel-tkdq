/**
 * Header file to describe definition of partition-based parallel
 * TKDQ solver.
 *
 * @author Sean Chester
 * @date 12 November 2015
 */

#ifndef PARTITION_BASED_H_
#define PARTITION_BASED_H_


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
#include "partition_based/partition.h"


#include <vector>


typedef std::pair< uint32_t, uint32_t > answer; /**< A (# points dominated, point id) pair */
template< uint32_t dims >
using Partitioning = std::vector< Partition< dims > >; /**< A set of non-empty partitions */

/**
 * A class for executing our Naive algorithm to compute top-k dominating queries.
 * 
 * @tparam DIMS The number of dimensions in the input dataset.
 */
template< uint32_t dims >
class PartitionBased: public TKDQ_Solver {

public:
	
	PartitionBased(uint32_t threads, uint32_t n, float** data ) :
      t_(threads), n_(n) {

    omp_set_num_threads( threads );
    result_.reserve(1024);
    data_ = NULL;
  }

	~PartitionBased() { }
  void Init( float** data );
	std::vector< uint32_t > Execute( const uint32_t k );


protected:

  // Data members:
  uint32_t n_; /**< The number of points in the dataset. */
  const uint32_t t_; /**< The number of threads with which the solution should be obtained. */
  PTuple< dims >* data_; /**< The internal representation of the dataset. */
  std::vector< uint32_t > result_; /**< The vector that will contain the result point ids */

private:
	
	/**
	 * Sorts the underlying data by the volume of their dominance area
	 * @post The dataset is re-ordered.
	 */ 
	void inline sort_by_volume();
	
	/**
	 * Chooses the next point that should be used as a pivot, based on the point 
	 * with the largest dominance area of those in still active partitions.
	 * @param partitions The set of all non-empty partitions from which to select a 
	 * pivot point.
	 * @param choices The indexes of partitions that are still active
	 * @return The index of the point that should next be used as a pivot.
	 */
	uint32_t inline 
	select_pivot( Partitioning< dims > *partitions, std::vector< uint32_t > *choices );

	
	/**
	 * Copies the result from a heap q into the local result vector
	 * @param q The heap of top k dominating points computed by Execute()
	 * @param k The number of points in q
	 * @post The internal representation of the result in this PartitionBased 
	 * object is updated to reflect the result computed in q.
	 */
	void inline copy_result( std::vector< answer > &q, const uint32_t k);
};

#endif /* PARTITION_BASED_H_ */
