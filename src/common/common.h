/**
 * common.h
 *
 * @date 11 Feb  2014
 * @author dariuss
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <ostream>

static const uint32_t SHIFTS[] = { 1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1
		<< 5, 1 << 6, 1 << 7, 1 << 8, 1 << 9, 1 << 10, 1 << 11, 1 << 12, 1 << 13, 1
		<< 14, 1 << 15, 1 << 16, 1 << 17, 1 << 18, 1 << 19, 1 << 20, 1 << 21, 1
		<< 22, 1 << 23, 1 << 24, 1 << 25, 1 << 26, 1 << 27, 1 << 28, 1 << 29, 1
		<< 30 };

/**
 * Trades two pointer addresses
 */
template < typename T >
void inline swap( T **x, T **y ) { T *tmp = *x; *x = *y; *y = tmp; }

/**
 * A Tuple is vector of float values corresponding to one 
 * data object with a unique id.
 * @tparam DIMS The length of the vector of values (i.e., 
 * number of dimensions in the dataset).
 */
template< uint32_t DIMS > 
struct Tuple {

  float elems[ DIMS ]; /**< The ordered vector of data values */
  uint32_t pid; /**< The unique id for this tuple */
};



/**
 * A score-based tuple is an extension of the Tuple type to 
 * also include a score that is associated with the tuple.
 */
template< uint32_t DIMS >
struct STuple: Tuple< DIMS > {

  float score; // entropy, manhattan sum, or minC

  /**
   * An STuple is considered to be less than another STuple
   * if it has a lower score.
   */
  bool operator<( const STuple &rhs ) const {
    return ( score < rhs.score )
    	|| ( score == rhs.score && this->pid < rhs.pid );
  }

  /**
   * An STuple is considered to be higher than another STuple
   * if it has a higher score.
   */
  bool operator>( const STuple &rhs ) const {
    return score > rhs.score;
  }
  
  /**
	 * Appends this tuple to the output stream in the form: 
	 * [x_1, x_2, ..., x_n ]
	 */
  std::ostream &operator<<( std::ostream &out ) {
  	out << "([ ";
  	for( uint32_t d = 0; d < DIMS; ++d ) {
  		out << this->elems[ d ] << " ";
  	}
  	out << "], " << this->score << ")";
  }
};


/**
 * An extension of the score-based tuple type to also 
 * include a  binary mask indicating a partition to 
 * which the tuple belongs.
 */
template< uint32_t DIMS > 
struct PTuple: STuple < DIMS > {

  uint32_t partition; /**< bit mask: 0 is <= pivot, 1 is > pivot on i'th dimension. */
  
  /**
   * Constructor that builds a new PTuple from an existing Tuple 
   * and a given score and partition mask.
   *
  PTuple< DIMS > (const Tuple< DIMS > t, const uint32_t s, const uint32_t p):
    Tuple< DIMS >( t ), partition( p ) { this->score = s; }
  */

  /**
   * A PTuple is considered to be higher than another PTuple
   * if it has a higher partition, or, failing that, a higher 
   * score.
   */
  bool operator>( const PTuple &rhs ) const {
    if ( partition > rhs.partition ) { return true; }
    else if ( partition == rhs.partition && this->score > rhs.score ) { return true; }
    else if ( partition == rhs.partition && this->score == rhs.score ) { 
    	return this->pid > rhs.pid;
    }
    return false;
  }

  /**
   * A PTuple is considered to be less than another PTuple
   * if it has a lower partition, or, failing that, a lower 
   * score.
   */
  bool operator<( const PTuple &rhs ) const {
    if ( partition < rhs.partition ) { return true; }
    else if ( partition == rhs.partition && this->score < rhs.score ) { return true; }
    else if ( partition == rhs.partition && this->score == rhs.score ) { 
    	return this->pid < rhs.pid;
    }
    return false;
  }

  /**
   * A PTuple is considered equal to another PTuple if they have the 
   * same pid.
   */
  bool operator>=( const PTuple &rhs ) const {
  	if ( this->pid == rhs.pid ) { return true; }
    else if ( partition > rhs.score ) { return true; }
    else if ( partition == rhs.score && this->score >= rhs.score ) { return true; }
    return false;
  }
  
};


/**
 * Appends a Tuple to the output stream in the form: 
 * [x_1, x_2, ..., x_n ]
 */
template < uint32_t DIMS >
std::ostream &operator<<( std::ostream &out, const Tuple< DIMS > &t ) {
	out << "[ ";
	for( uint32_t d = 0; d < DIMS; ++d ) {
		out << t.elems[ d ] << " ";
	}
	out << "]";
	return out;
}


/**
 * Appends an STuple to the output stream in the form: 
 * ([x_1, x_2, ..., x_n ], score)
 */
template < uint32_t DIMS >
std::ostream &operator<<( std::ostream &out, const STuple< DIMS > &t ) {
	out << "([ ";
	for( uint32_t d = 0; d < DIMS; ++d ) {
		out << t.elems[ d ] << " ";
	}
	out << "], " << t.score << ")";
	return out;
}


/**
 * Appends a PTuple to the output stream in the form: 
 * ([x_1, x_2, ..., x_n ], score, partition)
 */
template < uint32_t DIMS >
std::ostream &operator<<( std::ostream &out, const PTuple< DIMS > &t ) {
	out << "([ ";
	for( uint32_t d = 0; d < DIMS; ++d ) {
		out << t.elems[ d ] << " ";
	}
	out << "], " << t.score << ", " << t.partition << ")";
	return out;
}

extern uint64_t dt_count;
extern uint64_t dt_count_dom;
extern uint64_t dt_count_incomp;

// returns the maximum attribute value
template< uint32_t NUM_DIMS >
inline float get_max(const STuple< NUM_DIMS > &p) {
  float maxc = p.elems[0];
  for (uint32_t d = 1; d < NUM_DIMS; d++) {
    maxc = std::max( maxc, p.elems[d] );
  }
  return maxc;
}



#if __AVX__

#include "common/dt_avx2.h"

#else

/**
 * 2-way dominance test with NO assumption for distinct value condition.
 */
template < uint32_t NUM_DIMS >
inline int DominanceTest(const Tuple< NUM_DIMS > &t1, const Tuple< NUM_DIMS > &t2) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count, 1 );
#endif
  bool t1_better = false, t2_better = false;

  for (uint32_t i = 0; i < NUM_DIMS; i++) {
    if ( t1.elems[i] < t2.elems[i] )
      t1_better = true;
    else if ( t1.elems[i] > t2.elems[i] )
      t2_better = true;

    if ( t1_better && t2_better )
      return DOM_INCOMP;
  }
  if ( !t1_better && t2_better )
    return DOM_RIGHT;
  if ( !t2_better && t1_better )
    return DOM_LEFT;

//    if ( !t1_better && !t2_better )
//      return DOM_INCOMP; //equal
  return DOM_INCOMP;
}

/**
 * Dominance test returning result as a bitmap.
 * This is an original version (assuming distinct value
 * condition) used in in BSkyTree.
 *
 * In BSkyTree, it is by far the most frequent dominance test.
 */
template < uint32_t NUM_DIMS > 
inline uint32_t DT_bitmap_dvc(const Tuple< NUM_DIMS > &cur_value, const Tuple< NUM_DIMS > &sky_value) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count, 1 );
#endif

  uint32_t lattice = 0;
  for (uint32_t dim = 0; dim < NUM_DIMS; dim++)
    if ( sky_value.elems[dim] <= cur_value.elems[dim] )
      lattice |= SHIFTS[dim];

#if COUNT_DT==1
  if ( lattice == ALL_ONES)
    __sync_fetch_and_add( &dt_count_dom, 1 );
  else
    __sync_fetch_and_add( &dt_count_incomp, 1 );
#endif 
  return lattice;
}

/**
 * The same as above, but no assuming DVC.
 *
 * Note: is not called so frequently as DT_bitmap_dvc in BSkyTree,
 * so performance gain is negligible.
 */
template < uint32_t NUM_DIMS >
inline uint32_t DT_bitmap(const Tuple< NUM_DIMS > &cur_value, const Tuple< NUM_DIMS > &sky_value) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count, 1 );
#endif

  uint32_t lattice = 0;
  for (uint32_t dim = 0; dim < NUM_DIMS; dim++)
    if ( sky_value.elems[dim] < cur_value.elems[dim] )
      lattice |= SHIFTS[dim];

#if COUNT_DT==1
  if ( lattice == ALL_ONES)
    __sync_fetch_and_add( &dt_count_dom, 1 );
  else
    __sync_fetch_and_add( &dt_count_incomp, 1 );
#endif 
  return lattice;
}


/**
 * One-way (optimized) dominance test.
 * No assumption for distinct value condition.
 */
template < uint32_t NUM_DIMS >
inline bool DominateLeft(const Tuple< NUM_DIMS > &t1, const Tuple< NUM_DIMS > &t2) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count, 1 );
#endif
  uint32_t i;
  for (i = 0; i < NUM_DIMS && t1.elems[i] <= t2.elems[i]; ++i)
    ;
  if ( i < NUM_DIMS ) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count_incomp, 1 );
#endif
    return false; // Points are incomparable.
  }

  for (i = 0; i < NUM_DIMS; ++i) {
    if ( t1.elems[i] < t2.elems[i] ) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count_dom, 1 );
#endif
      return true; // t1 dominates t2
    }
  }
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count_incomp, 1 );
#endif
  return false; // Points are equal.
}

/**
 * Dominance test assuming distinct value condition.
 * DominateLeft(x, x) returns 1.
 */
template < uint32_t NUM_DIMS >
inline int DominateLeftDVC(const Tuple< NUM_DIMS > &t1, const Tuple< NUM_DIMS > &t2) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count, 1 );
#endif
  for (uint32_t i = 0; i < NUM_DIMS; i++) {
    if ( t1.elems[i] > t2.elems[i] ) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count_incomp, 1 );
#endif
      return 0;
    }
  }
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count_dom, 1 );
#endif
  return 1;
}

#endif

/**
 * Dominance test that computes a bitmap.
 * Produces side-effects in cur_value: sets score 
 * and partition.
 */
template < uint32_t NUM_DIMS >
inline void DT_bitmap_withsum( PTuple< NUM_DIMS > &cur_value, const PTuple< NUM_DIMS > &sky_value) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count, 1 );
#endif
	
	const uint32_t all_ones = ( 1 << NUM_DIMS ) - 1;

  uint32_t partition = 0;
  cur_value.score = 0;
  for (uint32_t d = 0; d < NUM_DIMS; d++) {
    if ( sky_value.elems[d] < cur_value.elems[d] )
      partition |= SHIFTS[d];
    cur_value.score += cur_value.elems[d];
  }
#if COUNT_DT==1
  if ( partition == all_ones )
    __sync_fetch_and_add( &dt_count_dom, 1 );
  else
    __sync_fetch_and_add( &dt_count_incomp, 1 );
#endif
  if ( partition == all_ones ) {
    cur_value.markPruned();
  } else {
    cur_value.setPartition( partition );
  }
}

/**
 * Dominance test assuming distinct value condition.
 *
template < uint32_t NUM_DIMS >
inline int DT_dvc(const Tuple< NUM_DIMS > &t1, const Tuple< NUM_DIMS > &t2) {
  bool t1_better = false, t2_better = false;
  for (uint32_t d = 0; d < NUM_DIMS; d++) {
//    if ( t1.elems[d] < t2.elems[d] )
//      t1_better = true;
//    else if ( t1.elems[d] > t2.elems[d] )
//      t2_better = true;
    t1_better = t1.elems[d] < t2.elems[d] || t1_better;
    t2_better = t1.elems[d] > t2.elems[d] || t2_better;

    if ( t1_better && t2_better ) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count_incomp, 1 );
#endif
      return DOM_INCOMP;
    }
  }

  if ( !t1_better && t2_better ) {
    return DOM_RIGHT;
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count_dom, 1 );
#endif
  }
  if ( !t2_better && t1_better ) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count_dom, 1 );
#endif
    return DOM_LEFT;
  }

  //    if ( !t1_better && !t2_better )
  //      return DOM_UNCOMP; //equal
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count_incomp, 1 );
#endif
  assert( false );
  return DOM_INCOMP;
}
*/

/**
 * Returns true if left tuple is dominated.
 *
 * Used in EvaluatePoint. Assumes DVC. Identical to DominateRightDVC().
 */
template < uint32_t NUM_DIMS >
inline bool DominatedLeft(const Tuple< NUM_DIMS > &t1, const Tuple< NUM_DIMS > &t2) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count, 1 );
#endif

  for (uint32_t d = 0; d < NUM_DIMS; d++) {
    if ( t1.elems[d] < t2.elems[d] ) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count_incomp, 1 );
#endif
      return false;
    }
  }
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count_dom, 1 );
#endif
  return true;
}

/**
 * Dominance test assuming distinct value condition.
 * DominateLeft(x, x) returns 1.
 */
template < uint32_t NUM_DIMS >
inline int DominateRightDVC(const Tuple< NUM_DIMS > &t1, const Tuple< NUM_DIMS > &t2) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count, 1 );
#endif
  for (uint32_t i = 0; i < NUM_DIMS; i++) {
    if ( t1.elems[i] < t2.elems[i] ) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count_incomp, 1 );
#endif
      return 0;
    }
  }
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count_dom, 1 );
#endif
  return 1;
}

/**
 * Test for equality.
 */
template< uint32_t NUM_DIMS >
inline bool EqualityTest(const Tuple< NUM_DIMS > &t1, const Tuple< NUM_DIMS > &t2) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count, 1 );
#endif
  bool eq = true;
  for (uint32_t d = 0; d < NUM_DIMS; d++)
    if ( t1.elems[d] != t2.elems[d] ) {
      eq = false;
      break;
    }
  return eq;
}

template < uint32_t NUM_DIMS >
inline float calc_norm_range ( const Tuple< NUM_DIMS > &t, const float *mins, const float *ranges ) {
  float min, max;
  min = max = ( t.elems[0] - mins[0] ) / ranges[0];
  for (uint32_t j = 1; j < NUM_DIMS; j++) {
    const float v_norm = ( t.elems[j] - mins[j] ) / ranges[j];
    if ( min > v_norm )
      min = v_norm;
    else if ( max < v_norm )
      max = v_norm;
  }
  return max - min;
}

#endif /* COMMON_H_ */
