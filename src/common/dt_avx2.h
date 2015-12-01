/**
 * @file
 *
 *  Created on: Dec 2, 2014
 *      Author: sidlausk
 *
 *  Templated (v2) dominance tests using AVX/SSE instructions.
 */

#ifndef DT_AVX2_H_
#define DT_AVX2_H_

#include <immintrin.h>  // AVX
#include <iostream>

template< uint32_t DIMS >
inline uint32_t DT_bitmap_dvc(const Tuple<DIMS> &cur, const Tuple<DIMS> &sky) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count, 1 );
#endif

  uint32_t lattice = 0;
  uint32_t dim = 0;

  if ( DIMS >= 8) {
    const Tuple<DIMS> __attribute__ ((aligned(32))) cur_value = cur;
    const Tuple<DIMS> __attribute__ ((aligned(32))) sky_value = sky;

    for (; dim + 8 <= DIMS; dim += 8) {
      __m256 p_ymm = _mm256_load_ps(cur_value.elems + dim);
      __m256 sky_ymm = _mm256_load_ps(sky_value.elems + dim);
      __m256 comp_le = _mm256_cmp_ps(sky_ymm, p_ymm, 2);
      uint32_t le_mask = _mm256_movemask_ps(comp_le);
      lattice = lattice | (le_mask << dim);
    }

    for (; dim + 4 <= DIMS; dim += 4) {
      __m128 p_xmm = _mm_load_ps(cur_value.elems + dim);
      __m128 sky_xmm = _mm_load_ps(sky_value.elems + dim);
      __m128 le128 = _mm_cmp_ps(sky_xmm, p_xmm, 2);
      uint32_t le_mask = _mm_movemask_ps(le128);
      lattice = lattice | (le_mask << dim);
    }

    for (; dim < DIMS; ++dim)
      if (sky.elems[dim] <= cur.elems[dim])
        lattice |= SHIFTS[dim];

  } else if (DIMS >= 4) {
    const Tuple<DIMS> __attribute__ ((aligned(32))) cur_value = cur;
    const Tuple<DIMS> __attribute__ ((aligned(32))) sky_value = sky;

    for (; dim + 4 <= DIMS; dim += 4) {
      __m128 p_xmm = _mm_load_ps(cur_value.elems + dim);
      __m128 sky_xmm = _mm_load_ps(sky_value.elems + dim);
      __m128 comp_le = _mm_cmp_ps(sky_xmm, p_xmm, 2);
      uint32_t le_mask = _mm_movemask_ps(comp_le);
      lattice = lattice | (le_mask << dim);
    }

    for (; dim < DIMS; ++dim)
      if (sky.elems[dim] <= cur.elems[dim])
        lattice |= SHIFTS[dim];

  } else {

    for (; dim < DIMS; ++dim)
      if (sky.elems[dim] <= cur.elems[dim])
        lattice |= SHIFTS[dim];
  }

#if COUNT_DT==1
  if ( lattice == ((1<<DIMS) - 1) )
  __sync_fetch_and_add( &dt_count_dom, 1 );
  else
  __sync_fetch_and_add( &dt_count_incomp, 1 );
#endif

  return lattice;
}

template<int DIMS>
inline uint32_t DT_bitmap(const Tuple<DIMS> cur, const Tuple<DIMS> sky) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count, 1 );
#endif
  uint32_t lattice = 0;
  uint32_t dim = 0;

  if (DIMS >= 8) {
    const Tuple<DIMS> __attribute__ ((aligned(32))) cur_value = cur;
    const Tuple<DIMS> __attribute__ ((aligned(32))) sky_value = sky;

    for (; dim + 8 <= DIMS; dim += 8) {
      __m256 p_ymm = _mm256_load_ps(cur_value.elems + dim);
      __m256 sky_ymm = _mm256_load_ps(sky_value.elems + dim);
      __m256 comp_lt = _mm256_cmp_ps(sky_ymm, p_ymm, 1);
      uint32_t lt_mask = _mm256_movemask_ps(comp_lt);
      lattice = lattice | (lt_mask << dim);
    }

    for (; dim + 4 <= DIMS; dim += 4) {
      __m128 p_xmm = _mm_load_ps(cur_value.elems + dim);
      __m128 sky_xmm = _mm_load_ps(sky_value.elems + dim);
      __m128 comp_lt = _mm_cmp_ps(sky_xmm, p_xmm, 1);
      uint32_t lt_mask = _mm_movemask_ps(comp_lt);
      lattice = lattice | (lt_mask << dim);
    }

    for (; dim < DIMS; dim++)
      if (sky.elems[dim] < cur.elems[dim])
        lattice |= SHIFTS[dim];

  } else if (DIMS >= 4) {
    const Tuple<DIMS> __attribute__ ((aligned(32))) cur_value = cur;
    const Tuple<DIMS> __attribute__ ((aligned(32))) sky_value = sky;

    for (; dim + 4 <= DIMS; dim += 4) {
      __m128 p_xmm = _mm_load_ps(cur_value.elems + dim);
      __m128 sky_xmm = _mm_load_ps(sky_value.elems + dim);
      __m128 comp_lt = _mm_cmp_ps(sky_xmm, p_xmm, 1);
      uint32_t lt_mask = _mm_movemask_ps(comp_lt);
      lattice = lattice | (lt_mask << dim);
    }

    for (; dim < DIMS; dim++)
      if (sky.elems[dim] < cur.elems[dim])
        lattice |= SHIFTS[dim];

  } else {
    for (dim = 0; dim < DIMS; dim++)
      if (sky.elems[dim] < cur.elems[dim])
        lattice |= SHIFTS[dim];
  }

#if COUNT_DT==1
  if ( lattice == ((1<<DIMS) - 1) )
  __sync_fetch_and_add( &dt_count_dom, 1 );
  else
  __sync_fetch_and_add( &dt_count_incomp, 1 );
#endif

  return lattice;
}

/**
 * 2-way dominance test with NO assumption for distinct value condition.
 *
template<int DIMS>
inline int DominanceTest(const Tuple<DIMS> &left, const Tuple<DIMS> &right) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count, 1 );
#endif
  uint32_t dim = 0, left_better = 0, right_better = 0;

  if (DIMS >= 8) {
    const Tuple<DIMS> __attribute__ ((aligned(32))) right_value = right;
    const Tuple<DIMS> __attribute__ ((aligned(32))) left_value = left;

    for (; dim + 8 <= DIMS; dim += 8) {
      __m256 right_ymm = _mm256_load_ps(right_value.elems + dim);
      __m256 left_ymm = _mm256_load_ps(left_value.elems + dim);
      if (!left_better) {
        __m256 comp_lt = _mm256_cmp_ps(left_ymm, right_ymm, 1);
        uint32_t lt_mask = _mm256_movemask_ps(comp_lt);
        left_better = lt_mask & (SHIFTS_[8] - 1);
      }
      if (!right_better) {
        __m256 comp_lt = _mm256_cmp_ps(right_ymm, left_ymm, 1);
        uint32_t lt_mask = _mm256_movemask_ps(comp_lt);
        right_better = lt_mask & (SHIFTS_[8] - 1);
      }
      if (left_better && right_better)
        return DOM_INCOMP_;
    }

    for (; dim + 4 <= DIMS; dim += 4) {
      __m128 right_ymm = _mm_load_ps(right_value.elems + dim);
      __m128 left_ymm = _mm_load_ps(left_value.elems + dim);
      if (!left_better) {
        __m128 comp_lt = _mm_cmp_ps(left_ymm, right_ymm, 1);
        uint32_t lt_mask = _mm_movemask_ps(comp_lt);
        left_better = lt_mask & (SHIFTS_[4] - 1);
      }
      if (!right_better) {
        __m128 comp_lt = _mm_cmp_ps(right_ymm, left_ymm, 1);
        uint32_t lt_mask = _mm_movemask_ps(comp_lt);
        right_better = lt_mask & (SHIFTS_[4] - 1);
      }
      if (left_better && right_better)
        return DOM_INCOMP_;
    }

    for (; dim < DIMS; dim++) {
      if (!right_better && right.elems[dim] < left.elems[dim])
        right_better = 1;
      if (!left_better && left.elems[dim] < right.elems[dim])
        left_better = 1;
      if (left_better && right_better)
        return DOM_INCOMP_;
    }

  } else if (DIMS >= 4) {
    const Tuple<DIMS> __attribute__ ((aligned(32))) right_value = right;
    const Tuple<DIMS> __attribute__ ((aligned(32))) left_value = left;

    for (; dim + 4 <= DIMS; dim += 4) {
      __m128 right_ymm = _mm_load_ps(right_value.elems + dim);
      __m128 left_ymm = _mm_load_ps(left_value.elems + dim);
      if (!left_better) {
        __m128 comp_lt = _mm_cmp_ps(left_ymm, right_ymm, 1);
        uint32_t lt_mask = _mm_movemask_ps(comp_lt);
        left_better = lt_mask & (SHIFTS_[4] - 1);
      }
      if (!right_better) {
        __m128 comp_lt = _mm_cmp_ps(right_ymm, left_ymm, 1);
        uint32_t lt_mask = _mm_movemask_ps(comp_lt);
        right_better = lt_mask & (SHIFTS_[4] - 1);
      }
      if (left_better && right_better)
        return DOM_INCOMP_;
    }

    for (; dim < DIMS; dim++) {
      if (!right_better && right.elems[dim] < left.elems[dim])
        right_better = 1;
      if (!left_better && left.elems[dim] < right.elems[dim])
        left_better = 1;
      if (left_better && right_better)
        return DOM_INCOMP_;
    }

  } else {
    for (dim = 0; dim < DIMS; dim++) {
      if (!right_better)
        right_better = (right.elems[dim] < left.elems[dim]);
      if (!left_better)
        left_better = (left.elems[dim] < right.elems[dim]);
      if (left_better && right_better)
        return DOM_INCOMP_;
    }
  }

  if (left_better && !right_better)
    return DOM_LEFT_;
  else if (right_better && !left_better)
    return DOM_RIGHT_;
  else
    return DOM_INCOMP_; //equal.
}
*/
/**
 * One-way (optimized) dominance test.
 * No assumption for distinct value condition.
 */
template<int DIMS>
inline bool DominateLeft(const Tuple<DIMS> &left, const Tuple<DIMS> &right) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count, 1 );
#endif
  uint32_t dim = 0;

  if (DIMS >= 8) {
    const Tuple<DIMS> __attribute__ ((aligned(32))) right_value = right;
    const Tuple<DIMS> __attribute__ ((aligned(32))) left_value = left;

    for (; dim + 8 <= DIMS; dim += 8) {
      __m256 right_ymm = _mm256_load_ps(right_value.elems + dim);
      __m256 left_ymm = _mm256_load_ps(left_value.elems + dim);
      __m256 comp_lt = _mm256_cmp_ps(left_ymm, right_ymm, 2);
      uint32_t lt_mask = _mm256_movemask_ps(comp_lt);
      if (lt_mask != 255)
        return false;
    }

    for (; dim + 4 <= DIMS; dim += 4) {
      __m128 right_xmm = _mm_load_ps(right_value.elems + dim);
      __m128 left_xmm = _mm_load_ps(left_value.elems + dim);
      __m128 comp_lt = _mm_cmp_ps(left_xmm, right_xmm, 2);
      uint32_t lt_mask = _mm_movemask_ps(comp_lt);
      if (lt_mask != 15)
        return false;
    }

    for (; dim < DIMS; dim++)
      if (right.elems[dim] < left.elems[dim])
        return false;

  } else if (DIMS >= 4) {
    const Tuple<DIMS> __attribute__ ((aligned(32))) right_value = right;
    const Tuple<DIMS> __attribute__ ((aligned(32))) left_value = left;

    for (; dim + 4 <= DIMS; dim += 4) {
      __m128 right_xmm = _mm_load_ps(right_value.elems + dim);
      __m128 left_xmm = _mm_load_ps(left_value.elems + dim);
      __m128 comp_lt = _mm_cmp_ps(left_xmm, right_xmm, 2);
      uint32_t lt_mask = _mm_movemask_ps(comp_lt);
      if (lt_mask != 15)
        return false;
    }

    for (; dim < DIMS; dim++)
      if (right.elems[dim] < left.elems[dim])
        return false;

  } else {
    for (dim = 0; dim < DIMS; dim++)
      if (right.elems[dim] < left.elems[dim])
        return false;
  }

  //test equality.
  for (dim = 0; dim < DIMS; dim++)
    if (right.elems[dim] != left.elems[dim])
      return true;

  return false; //points are equal.
}

/**
 * One-way (optimized) dominance test.
 * With distinct value condition assumption.
 */
template< uint32_t DIMS>
inline bool DominateLeftDVC(const Tuple<DIMS> &left, const Tuple<DIMS> &right) {
#if COUNT_DT==1
  __sync_fetch_and_add( &dt_count, 1 );
#endif
  uint32_t dim = 0;

  if (DIMS >= 8) {
    const Tuple<DIMS> __attribute__ ((aligned(32))) right_value = right;
    const Tuple<DIMS> __attribute__ ((aligned(32))) left_value = left;

    for (; dim + 8 <= DIMS; dim += 8) {
      __m256 right_ymm = _mm256_load_ps(right_value.elems + dim);
      __m256 left_ymm = _mm256_load_ps(left_value.elems + dim);
      __m256 comp_lt = _mm256_cmp_ps(left_ymm, right_ymm, 2);
      uint32_t lt_mask = _mm256_movemask_ps(comp_lt);
      if (lt_mask != 255)
        return false;
    }

    for (; dim + 4 <= DIMS; dim += 4) {
      __m128 right_xmm = _mm_load_ps(right_value.elems + dim);
      __m128 left_xmm = _mm_load_ps(left_value.elems + dim);
      __m128 comp_lt = _mm_cmp_ps(left_xmm, right_xmm, 2);
      uint32_t lt_mask = _mm_movemask_ps(comp_lt);
      if (lt_mask != 15)
        return false;
    }

    for (; dim < DIMS; dim++)
      if (right.elems[dim] < left.elems[dim])
        return false;

  } else if (DIMS >= 4) {
    const Tuple<DIMS> __attribute__ ((aligned(32))) right_value = right;
    const Tuple<DIMS> __attribute__ ((aligned(32))) left_value = left;

    for (; dim + 4 <= DIMS; dim += 4) {
      __m128 right_xmm = _mm_load_ps(right_value.elems + dim);
      __m128 left_xmm = _mm_load_ps(left_value.elems + dim);
      __m128 comp_lt = _mm_cmp_ps(left_xmm, right_xmm, 2);
      uint32_t lt_mask = _mm_movemask_ps(comp_lt);
      if (lt_mask != 15)
        return false;
    }

    for (; dim < DIMS; dim++)
      if (right.elems[dim] < left.elems[dim])
        return false;

  } else {
    for (dim = 0; dim < DIMS; dim++)
      if (right.elems[dim] < left.elems[dim])
        return false;
  }

  return true;
}


#endif /* DT_AVX2_H_ */
