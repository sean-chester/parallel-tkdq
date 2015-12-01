#include "bskytree/pivot_selection.h"

#include <cstdio>
#include <cassert>

template class PivotSelection<2>;
template class PivotSelection<3>;
template class PivotSelection<4>;
template class PivotSelection<6>;
template class PivotSelection<8>;
template class PivotSelection<10>;
template class PivotSelection<12>;
template class PivotSelection<13>;
template class PivotSelection<14>;
template class PivotSelection<15>;
template class PivotSelection<16>;
template class PivotSelection<18>;
template class PivotSelection<20>;
template class PivotSelection<22>;
template class PivotSelection<24>;

template void PivotSelection<2>::Execute(vector<Tuple<2> >& dataset);
template void PivotSelection<3>::Execute(vector<Tuple<3> >& dataset);
template void PivotSelection<4>::Execute(vector<Tuple<4> >& dataset);
template void PivotSelection<6>::Execute(vector<Tuple<6> >& dataset);
template void PivotSelection<8>::Execute(vector<Tuple<8> >& dataset);
template void PivotSelection<10>::Execute(vector<Tuple<10> >& dataset);
template void PivotSelection<12>::Execute(vector<Tuple<12> >& dataset);
template void PivotSelection<13>::Execute(vector<Tuple<13> >& dataset);
template void PivotSelection<14>::Execute(vector<Tuple<14> >& dataset);
template void PivotSelection<15>::Execute(vector<Tuple<15> >& dataset);
template void PivotSelection<16>::Execute(vector<Tuple<16> >& dataset);
template void PivotSelection<18>::Execute(vector<Tuple<18> >& dataset);
template void PivotSelection<20>::Execute(vector<Tuple<20> >& dataset);
template void PivotSelection<22>::Execute(vector<Tuple<22> >& dataset);
template void PivotSelection<24>::Execute(vector<Tuple<24> >& dataset);

/*
 * Executes a configured pivot point selection.
 */
template<int DIMS>
void PivotSelection<DIMS>::Execute(vector<Tuple<DIMS> >& dataset) {
  switch (selection_mode_) {
  case PIVOT_MANHATTAN:
    // MaxDom
    ExecuteMaxDom( dataset );
    break;
  case PIVOT_RANDOM:
    ExecuteRandom( dataset );
    break;
  case PIVOT_BALSKY:
    // Balanced (pivot point is a skyline)
    ExecuteBalanced( dataset );
    break;
  default:
    printf( " Warning: not supported pivot selection mode!\n" );
    break;
  }
}

/*
 * "MaxDom" pivot selection technique used in [SkyTree] and
 * as follows described in [BSkyTree] paper:
 *
 *  This selection heuristic selects a pivot point as a
 *  skyline point maximizing the dominance region [SkyTree].
 *  That is, a point is selected with minimum Manhattan sum.
 *  It requires a linear scan on the whole dataset. While this
 *  pivot selection guarantees to maximize the pruning power
 *  for dominance, it does not provide any guarantee on
 *  maximizing incomparability.
 *
 * Our remark: the selected pivot (1) is not necessary a skyline
 * point and (2) does not perform pruning during selection. Both
 * are not true for the original SkyTree proposal [SkyTree].
 */
template<int DIMS>
void PivotSelection<DIMS>::ExecuteMaxDom(vector<Tuple<DIMS> >& dataset) {

  if ( pruning_on_ ) { // PRUNING ON
    const uint32_t head = 0;
    uint32_t tail = dataset.size() - 1, cur_pos = 1;
    float* hvalue = dataset[head].elems;
    float min_sum = ComputeManhattanSum( hvalue );

    while ( cur_pos <= tail ) {
      float* cvalue = dataset[cur_pos].elems;
      const uint32_t dtest = DominanceTest( dataset[head], dataset[cur_pos] );
      if ( dtest == DOM_LEFT_ ) {
        dataset[cur_pos] = dataset[tail];
        dataset.pop_back();
        tail--;
      } else if ( dtest == DOM_RIGHT_ ) {
        dataset[head] = dataset[cur_pos];
        dataset[cur_pos] = dataset[tail];
        dataset.pop_back();
        tail--;

        hvalue = dataset[head].elems;
        min_sum = ComputeManhattanSum( hvalue );
      } else {
        assert( dtest == DOM_INCOMP_ );
        float cur_dist = ComputeManhattanSum( cvalue );

        if ( cur_dist < min_sum ) {
          if ( EvaluatePoint( cur_pos, dataset ) ) {
            std::swap( dataset[head], dataset[cur_pos] );

            hvalue = dataset[head].elems;
            min_sum = cur_dist;
            cur_pos++;
          } else {
            dataset[cur_pos] = dataset[tail];
            dataset.pop_back();
            tail--;
          }
        } else
          cur_pos++;
      }
    }

  } else { // PRUNING OFF
    uint32_t min_pos = 0;
    float min_sum = ComputeManhattanSum( dataset[0].elems );

    const uint32_t n = dataset.size();
    for (uint32_t cur_pos = 1; cur_pos < n; cur_pos++) {
      const float sum = ComputeManhattanSum( dataset[cur_pos].elems );
      if ( sum < min_sum ) {
        min_sum = sum;
        min_pos = cur_pos;
      }
    }
    std::swap( dataset[0], dataset[min_pos] );
  }
}

/*
 * Chooses a random skyline point as a pivot. Guaranteed to
 * be a skyline point by transitivity.
 *
 * In addition to that, removes points from dataset that are
 * dominated by the (current) pivot point.
 */
template<int DIMS>
void PivotSelection<DIMS>::ExecuteRandom(vector<Tuple<DIMS> >& dataset) {

  if ( pruning_on_ ) { // PRUNING ON
    uint32_t n = dataset.size();
    srand( 13 );
    uint32_t rand_pos = rand() % n;
    std::swap( dataset[0], dataset[rand_pos] );
    for (uint32_t i = 0; i < n; ++i) {
      const uint32_t dtest = DominanceTest( dataset[0], dataset[i] );
      if ( dtest == DOM_LEFT_ ) {
        std::swap( dataset[i--], dataset[--n] );
        dataset.pop_back();
      }
      else if ( dtest == DOM_RIGHT_ ) {
        std::swap( dataset[0], dataset[i] );
        std::swap( dataset[i--], dataset[--n] );
        dataset.pop_back();
      }
    }
  } else { // PRUNING OFF
    const uint32_t n = dataset.size();
    srand( 13 );
    uint32_t rand_pos = rand() % n;
    std::swap( dataset[0], dataset[rand_pos] );
    for (uint32_t i = 0; i < n; ++i) {
      if ( DominatedLeft( dataset[0], dataset[i] ) )
        std::swap( dataset[0], dataset[i] );
    }
  }
}

/*
 * Chooses a pivot based on minimum range. The chosen pivot
 * is a skyline point.
 *
 * In addition to that, removes points from dataset that are
 * dominated by the (current) pivot point.
 */
template<int DIMS>
void PivotSelection<DIMS>::ExecuteBalanced(vector<Tuple<DIMS> >& dataset) {

  if ( pruning_on_ ) { // PRUNING ON
    const uint32_t head = 0;
    uint32_t tail = dataset.size() - 1, cur_pos = 1;
    float* hvalue = dataset[head].elems;

    vector<float> range_list = SetRangeList( min_list_, max_list_ );
    float min_dist = ComputeDistance( hvalue, min_list_, range_list );

    while ( cur_pos <= tail ) {
      float* cvalue = dataset[cur_pos].elems;

      const uint32_t dtest = DominanceTest( dataset[head], dataset[cur_pos] );
      if ( dtest == DOM_LEFT_ ) {
        dataset[cur_pos] = dataset[tail];
        dataset.pop_back();
        tail--;
      } else if ( dtest == DOM_RIGHT_ ) {
        dataset[head] = dataset[cur_pos];
        dataset[cur_pos] = dataset[tail];
        dataset.pop_back();
        tail--;

        hvalue = dataset[head].elems;
        min_dist = ComputeDistance( hvalue, min_list_, range_list );
        cur_pos = 1; // THIS IS THE SAME BUG AS IN QSkyCube: cur_pos is not reseted
      } else {
        assert( dtest == DOM_INCOMP_ );
        float cur_dist = ComputeDistance( cvalue, min_list_, range_list );

        if ( cur_dist < min_dist ) {
          if ( EvaluatePoint( cur_pos, dataset ) ) {
            std::swap( dataset[head], dataset[cur_pos] );

            hvalue = dataset[head].elems;
            min_dist = cur_dist;
            cur_pos++;
          } else {
            dataset[cur_pos] = dataset[tail];
            dataset.pop_back();
            tail--;
          }
        } else
          cur_pos++;
      }
    }
  } else { // PRUNING OFF
    vector<float> range_list = SetRangeList( min_list_, max_list_ );
    const uint32_t n = dataset.size();
    float min_dist = ComputeDistance( dataset[0].elems, min_list_, range_list );
    for (uint32_t pos = 1; pos < n; ++pos) {
      const float cur_dist = ComputeDistance( dataset[pos].elems, min_list_,
          range_list );
      if ( cur_dist < min_dist ) {
        if ( EvaluatePoint( pos, dataset ) ) {
          std::swap( dataset[0], dataset[pos] );
          min_dist = cur_dist;
        }
      }
    }
  }
}

template<int DIMS>
vector<float> PivotSelection<DIMS>::SetRangeList(const vector<float>& min_list,
    const vector<float>& max_list) {
  vector<float> range_list( NUM_DIMS, 0 );
  for (uint32_t d = 0; d < NUM_DIMS; d++)
    range_list[d] = max_list[d] - min_list[d];

  return range_list;
}

/*
 * Note that here normalization must be done (even though we assume
 * the data is pre-normalized) because it spreads the values within
 * each (recursed) partition (where all values are within a range).
 */
template<int DIMS>
float PivotSelection<DIMS>::ComputeDistance(const float* value,
    const vector<float>& min_list, const vector<float>& range_list) {
  float max_d, min_d;

  max_d = min_d = (value[0] - min_list[0]) / range_list[0];
  for (uint32_t d = 1; d < NUM_DIMS; d++) {
    float norm_value = (value[d] - min_list[d]) / range_list[d];
    if ( min_d > norm_value )
      min_d = norm_value;
    else if ( max_d < norm_value )
      max_d = norm_value;
  }

  return max_d - min_d;
}

/*
 * Computes Manhattan sum for a given point.
 * (The smaller the Manhattan sum, the bigger dominance
 * region the point has.)
 */
template<int DIMS>
float PivotSelection<DIMS>::ComputeManhattanSum(const float* value) {
  float sum = value[0];
  for (uint32_t d = 1; d < NUM_DIMS; d++)
    sum += value[d];

  return sum;
}

template<int DIMS>
float PivotSelection<DIMS>::ComputeCosine(const float* value,
    const vector<float>& min_list, const vector<float>& range_list) {
  float sum = 0.0, deno = 0.0;
  for (uint32_t d = 0; d < NUM_DIMS; d++) {
    float sum_value = (value[d] - min_list[d]) / range_list[d];

    sum += sum_value;
    deno += pow( sum_value, 2 );
  }

  return -sum / sqrt( deno );
}

/*
 * Checks if the point dataset[pos] is not dominated by any of points
 * before pos (dataset[0..pos-1]).
 *
 * Note that here we can remove additionally dominated points, but the
 * code does not do it (the paper suggests, though).
 */
template<int DIMS>
bool PivotSelection<DIMS>::EvaluatePoint(const uint32_t pos, vector<Tuple<DIMS> >& dataset) {
  const Tuple<DIMS> &cur_tuple = dataset[pos];
  for (uint32_t i = 0; i < pos; ++i) {
    const Tuple<DIMS> &prev_value = dataset[i];
    if ( DominatedLeft( cur_tuple, prev_value ) )
      return false;
  }

  return true;
}
