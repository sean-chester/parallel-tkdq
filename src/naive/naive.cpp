/**
 * Implementation of our naive parallel top-k dominating query algorithm.
 *
 * @author Sean Chester
 * @date 11 November 2015
 */

#include <iostream>

#include "naive.h"

template class Naive< 2 >;
template class Naive< 3 >;
template class Naive< 4 >;
template class Naive< 5 >;
template class Naive< 6 >;
template class Naive< 7 >;
template class Naive< 8 >;
template class Naive< 9 >;
template class Naive< 10 >;


template< uint32_t dims >
void Naive< dims >::Init( float** data ) {

	/* Allocate space. */
	data_ = new STuple< dims >[n_];
	
	/* Copy data from float array into tuple array and record
	 * point ids. */
#pragma omp parallel for
	for ( uint32_t i = 0; i < n_; ++i ) {
		data_[ i ].pid = i;
		data_[ i ].score = 0;
		memcpy( data_[ i ].elems, data[ i ], sizeof( float ) * dims );
	}
}


template< uint32_t dims >
std::vector< uint32_t > Naive< dims >::Execute( const uint32_t k ) {
	
	/* First, calculate Manhattan norm for every point. */
#pragma omp parallel for
	for( uint32_t i = 0; i < n_; ++i ) {
		for( uint32_t d = 0; d < dims; ++d ) {
			data_[ i ].score += data_[ i ].elems[ d ];
		}
	}
	
	/* Next, sort the data by descending manhattan norm. */
	std::sort( data_, data_ + n_ );
	

	/* Then, compute the top-k dominating score for every point. */\
#pragma omp parallel for schedule( dynamic, 128 )
	for( uint32_t i = 0; i < n_; ++i ) {
		data_[ i ].score = 0;
		uint32_t num_dominated_by = 0;
		for( uint32_t j = 0; j < i; ++j ) {
			if( DominateLeft< dims >( data_[ j ], data_[ i ] ) ) {
				if( ++num_dominated_by >= k ) { break; }
			}
		}
		if( num_dominated_by < k ) {
			for( uint32_t j = i + 1; j < n_; ++j ) {
				if( DominateLeft< dims >( data_[ i ], data_[ j ] ) ) { ++data_[ i ].score; }
			}
		} 
	}

	/* Re-sort the data, this time by top-k dominating score. */
	std::sort( data_, data_ + n_, std::greater< STuple< dims > >() );
	
	/* Copy the top-k points from the list into the output array and return it. */
	for( uint32_t i = 0; i < k; ++i ) {
		result_.push_back( data_[ i ].pid );
	}
	return result_;
}
