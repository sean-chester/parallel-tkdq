/**
 * Implementation of the Partition class
 *
 * @author Sean Chester
 * @date 24 November 2015
 */

#include "partition_based/partition.h"
#include "common/common.h"

/* Template instantiations.*/
template class Partition< 2 >;
template class Partition< 3 >;
template class Partition< 4 >;
template class Partition< 5 >;
template class Partition< 6 >;
template class Partition< 7 >;
template class Partition< 8 >;
template class Partition< 9 >;
template class Partition< 10 >;

template < uint32_t dims >
Partition< dims >
::Partition( const int32_t n ) 
	: upper_bound_score( n - 1 ) {
	
	for( uint32_t d = 0; d < dims; ++d ) {
		lower_bound_coord.elems[ d ] = 0;
		upper_bound_coord.elems[ d ] = 1;
	}
}

template < uint32_t dims >
Partition< dims >
::Partition( Tuple< dims > lower_corner, Tuple< dims > upper_corner
	, const int32_t n ) 
	: upper_bound_score( n - 1 ) {
	
	for( uint32_t d = 0; d < dims; ++d ) {
		lower_bound_coord.elems[ d ] = lower_corner.elems[ d ];
		upper_bound_coord.elems[ d ] = upper_corner.elems[ d ];
	}
}

