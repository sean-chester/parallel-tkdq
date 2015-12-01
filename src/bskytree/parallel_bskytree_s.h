/*
 * parallel_bskytree_s.h
 *
 *  Created on: Jul 6, 2014
 *      Author: darius
 *
 *
 *  Our parallel variant of BSkyTree-S algorithm.
 */

#ifndef PARALLEL_BSKYTREE_S_H_
#define PARALLEL_BSKYTREE_S_H_

#include <map>
#include <vector>

#if defined(_OPENMP)
#include <omp.h>
#include <parallel/algorithm>
#else
#include <algorithm>
#define omp_get_thread_num() 0
#define omp_set_num_threads( t ) 0
#endif

#include <common/skyline_i.h>
#include <bskytree/node.h>

using namespace std;

template<int DIMS>
class ParallelBSkyTreeS: public SkylineI {
public:
  ParallelBSkyTreeS( const uint32_t num_threads, const uint32_t n, const uint32_t d, float** dataset ) :
      num_threads_( num_threads ), n_( n ), d_( d ) {

    omp_set_num_threads( num_threads_ );
    skyline_.reserve( 1024 );
    eqm_.reserve( 1024 );
  }
  virtual ~ParallelBSkyTreeS();

  void Init( float** dataset );
  vector<int> Execute( void );

private:
  void BSkyTreeS_ALGO();
  void DoPartioning();

  // PivotSelection methods
  void SelectBalanced();
  vector<float> SetRangeList( const vector<float>& min_list,
      const vector<float>& max_list );
  float ComputeDistance( const float* value, const vector<float>& min_list,
      const vector<float>& range_list );
  bool EvaluatePoint( const uint32_t pos );

  const uint32_t num_threads_;
  const uint32_t n_;
  const uint32_t d_;
  vector<Tuple_S<DIMS> > data_;

  vector<int> skyline_;
  vector<int> eqm_; // "equivalence matrix"
};

#endif /* PARALLEL_BSKYTREE_S_H_ */
