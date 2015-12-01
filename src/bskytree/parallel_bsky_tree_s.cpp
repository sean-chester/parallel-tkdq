/*
 * parallel_bsky_tree_s.cpp
 *
 *  Created on: Jul 6, 2014
 *      Author: dariuss
 */

#include <bskytree/parallel_bskytree_s.h>

#include <cassert>

#include "common/common2.h"
#include "util/timing.h"

template class ParallelBSkyTreeS<2>;
template class ParallelBSkyTreeS<3>;
template class ParallelBSkyTreeS<4>;
template class ParallelBSkyTreeS<6>;
template class ParallelBSkyTreeS<8>;
template class ParallelBSkyTreeS<10>;
template class ParallelBSkyTreeS<12>;
template class ParallelBSkyTreeS<13>;
template class ParallelBSkyTreeS<14>;
template class ParallelBSkyTreeS<15>;
template class ParallelBSkyTreeS<16>;
template class ParallelBSkyTreeS<18>;
template class ParallelBSkyTreeS<20>;
template class ParallelBSkyTreeS<22>;
template class ParallelBSkyTreeS<24>;

template<int DIMS>
void ParallelBSkyTreeS<DIMS>::Init( float** dataset ) {
  data_.reserve( n_ );
  for (uint32_t i = 0; i < n_; i++) {
    Tuple<DIMS> t; // TODO: change to Tuple_S
    t.pid = i;
    memcpy( t.elems, dataset[i], sizeof(float) * DIMS );
    data_.push_back( Tuple_S<DIMS>( t, -1 ) );
  }
}

template<int DIMS>
ParallelBSkyTreeS<DIMS>::~ParallelBSkyTreeS() {
  skyline_.clear();
  data_.clear();
  eqm_.clear();
}

template<int DIMS>
vector<int> ParallelBSkyTreeS<DIMS>::Execute( void ) {

//  initProfiler();
  BSkyTreeS_ALGO();

//  printProfile();

// Add missing points from "equivalence matrix"
  skyline_.insert( skyline_.end(), eqm_.begin(), eqm_.end() );

  return skyline_;
}

template<int DIMS>
void ParallelBSkyTreeS<DIMS>::BSkyTreeS_ALGO() {
  SelectBalanced(); // pivot selection in the data_

  DoPartioning(); // mapping points to binary vectors representing sub-regions

  vector<Tuple_S<DIMS> > &S = data_; // Alias
  volatile bool *const dead = new bool[S.size()];
  memset( const_cast<bool*>(dead), 0, sizeof(bool) * S.size() );
  int head = 1; // always points to the 1st tuple after confirmed heads
  int tail = S.size() - 1; // always points to the last (active) tuple
  while ( head < tail ) {
    int htail = head + BSKYTREE_ACCUM - 1 < tail ? head + BSKYTREE_ACCUM - 1 : tail;
    #pragma omp parallel num_threads(num_threads_)
    { // private constants:
      const uint32_t p_head = head, p_tail = tail, p_htail = htail;
      #pragma omp for nowait
      for (uint32_t th = p_head; th <= p_htail; ++th) { // th -> temporal head
        int cur = p_htail + 1;
        while ( cur <= p_tail ) {
          if ( dead[cur] ) {
            ++cur;
            continue;
          }
          // Check for partial dominance:
          if ( (S[th].partition & S[cur].partition) == S[th].partition
              || (S[th].partition & S[cur].partition) == S[cur].partition ) {

            const int dt_test = DT_dvc( S[th], S[cur] );
            if ( dt_test == DOM_LEFT_ ) {
              dead[cur] = true;
//            S[cur++] = S[tail--]; no compression because of multi-threading
              cur++;
            } else if ( dt_test == DOM_RIGHT_ ) {
              dead[cur] = true;
              S[th] = S[cur];
              cur = p_htail + 1;
//            S[cur] = S[tail--]; no compression because of multi-threading
            } else {
              cur++; // Point-level incomparability
            }
          } else {
            cur++; // Region-level incomparability
          }
        } // with cur we did one pass (till tail)
      } //  (all temporal heads were processed)
    } // END OF PARALLEL

    // Single-thread execution:
    for (uint32_t th = head; th <= htail; ++th) { // th -> temporal head
      int c = th + 1;
      while ( c <= htail ) {
        if ( S[th].pid == S[c].pid ) {
          dead[htail] = true;
          S[c] = S[htail--];
        } else {
          const uint32_t dt_test = DT_dvc( S[th], S[c] );
          if ( dt_test == DOM_LEFT_ ) {
            dead[htail] = true;
            S[c] = S[htail--];
          } else if ( dt_test == DOM_RIGHT_ ) {
            S[th] = S[c];
            dead[htail] = true;
            S[c] = S[htail--];
            c = th + 1;
          } else {
            c++; // two heads are incomparable
          }
        }
      }
    }
    // heads are processed, so update it:
    head = htail + 1;

    // Compress by removing dead tuples:
    int head_dead = head; // will store 1st dead tuple from head & upwards
    int tail_alive = tail; // will store 1st alive tuple from tail & downwards
    while ( head_dead < tail_alive ) {
      while ( !dead[head_dead] && head_dead < tail_alive ) {
        ++head_dead;
      }

      while ( dead[tail_alive] ) {
        --tail_alive;
      }
      if ( tail_alive > head_dead ) {
        dead[head_dead] = false;
        dead[tail_alive] = true;
        S[head_dead++] = S[tail_alive--];
      }
    }
    // Update tail after compression:
    tail = tail_alive;
    while ( dead[tail] )
      --tail;
  } // Skyline computed!

  for (uint32_t i = 0; i <= tail; ++i) {
    skyline_.push_back( S[i].pid );
  }
  delete [] dead;
}

/*
 * Partitions the data using the pivot point (data_[0]) by
 * assigning partition bitmap to each tuple. Also, removes
 * the points that are pruned (ALL_ONES partition).
 */
template<int DIMS>
void ParallelBSkyTreeS<DIMS>::DoPartioning() {
  const uint32_t pruned = SHIFTS_[DIMS] - 1;
  const Tuple<DIMS> &pivot = data_[0];
  for (uint32_t i = 1; i < data_.size(); ++i) {
    if ( EqualityTest( pivot, data_[i] ) ) {
      eqm_.push_back( data_[i].pid );
      data_[i] = data_.back();
      data_.pop_back();
      continue;
    }
    const uint32_t lattice = DT_bitmap_dvc( data_[i], pivot );
    if ( lattice < pruned ) {
      assert( !DominateLeft( pivot, data_[i] ) );
      data_[i].partition = lattice;
    } else {
      data_[i] = data_.back();
      data_.pop_back();
    }
  }
}

/*
 * Chooses a pivot based on minimum range. The chosen pivot
 * is a skyline point.
 *
 * In addition to that, removes points from data_ that are
 * dominated by the (current) pivot point.
 *
 * The pivot point is stored in data_[0].
 */
template<int DIMS>
void ParallelBSkyTreeS<DIMS>::SelectBalanced() {
  const vector<float> min_list( DIMS, 0.0 );
  const vector<float> max_list( DIMS, 1.0 );

  const uint32_t head = 0;
  uint32_t tail = data_.size() - 1, cur_pos = 1;
  float* hvalue = data_[head].elems;

  vector<float> range_list = SetRangeList( min_list, max_list );
  float min_dist = ComputeDistance( hvalue, min_list, range_list );

  while ( cur_pos <= tail ) {
    float* cvalue = data_[cur_pos].elems;

    const uint32_t dtest = DominanceTest( data_[head], data_[cur_pos] );
    if ( dtest == DOM_LEFT_ ) {
      data_[cur_pos] = data_[tail];
      data_.pop_back();
      tail--;
    } else if ( dtest == DOM_RIGHT_ ) {
      data_[head] = data_[cur_pos];
      data_[cur_pos] = data_[tail];
      data_.pop_back();
      tail--;

      hvalue = data_[head].elems;
      min_dist = ComputeDistance( hvalue, min_list, range_list );
      cur_pos = 1; // THIS IS THE SAME BUG AS IN QSkyCube: cur_pos is not reseted
    } else {
      assert( dtest == DOM_INCOMP_ );
      float cur_dist = ComputeDistance( cvalue, min_list, range_list );

      if ( cur_dist < min_dist ) {
        if ( EvaluatePoint( cur_pos ) ) {
          std::swap( data_[head], data_[cur_pos] );

          hvalue = data_[head].elems;
          min_dist = cur_dist;
          cur_pos++;
        } else {
          data_[cur_pos] = data_[tail];
          data_.pop_back();
          tail--;
        }
      } else
        cur_pos++;
    }
  }
}

template<int DIMS>
vector<float> ParallelBSkyTreeS<DIMS>::SetRangeList( const vector<float>& min_list,
    const vector<float>& max_list ) {
  vector<float> range_list( DIMS, 0 );
  for (uint32_t d = 0; d < DIMS; d++)
    range_list[d] = max_list[d] - min_list[d];

  return range_list;
}

/*
 * Note that here we do not need to do normalization (we assume
 * the data is pre-normalized). Though, we do (TODO!)
 */
template<int DIMS>
float ParallelBSkyTreeS<DIMS>::ComputeDistance( const float* value,
    const vector<float>& min_list, const vector<float>& range_list ) {
  float max_d, min_d;

  max_d = min_d = (value[0] - min_list[0]) / range_list[0];
  for (uint32_t d = 1; d < DIMS; d++) {
    float norm_value = (value[d] - min_list[d]) / range_list[d];
    if ( min_d > norm_value )
      min_d = norm_value;
    else if ( max_d < norm_value )
      max_d = norm_value;
  }

  return max_d - min_d;
}

/*
 * Checks if the point dataset[pos] is not dominated by any of points
 * before pos (dataset[0..pos-1]).
 *
 * Note that here we can remove additionally dominated points, but the
 * code does not do it (the paper suggests, though).
 */
template<int DIMS>
bool ParallelBSkyTreeS<DIMS>::EvaluatePoint( const uint32_t pos ) {
  const Tuple<DIMS> &cur_tuple = data_[pos];
  for (uint32_t i = 0; i < pos; ++i) {
    const Tuple<DIMS> &prev_value = data_[i];
    if ( DominatedLeft( cur_tuple, prev_value ) )
      return false;
  }

  return true;
}
