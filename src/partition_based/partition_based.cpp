/**
 * Implementation of our proposed partition-based parallel top-k 
 * dominating query algorithm.
 *
 * @author Sean Chester
 * @date 12 November 2015
 */

#include <cstring>
#include <cassert>
#include <iterator>
#include <iostream>

#include "partition_based/partition_based.h"
#include "partition_based/partition.h"
#include "common/common.h"

#include <vector>
#include <unordered_map>

/* Template instantiations.*/
template class PartitionBased< 2 >;
template class PartitionBased< 3 >;
template class PartitionBased< 4 >;
template class PartitionBased< 5 >;
template class PartitionBased< 6 >;
template class PartitionBased< 7 >;
template class PartitionBased< 8 >;
template class PartitionBased< 9 >; 
template class PartitionBased< 10 >;



const auto& maxAnswer = std::greater< answer >(); /**< Alias for heap's comparator */

template< uint32_t dims >
void PartitionBased< dims >
::Init( float** data ) {

	data_ = new PTuple< dims >[ n_ ];
	
#pragma omp parallel for
	for ( uint32_t i = 0; i < n_; ++i ) {
		data_[ i ].pid = i;
		memcpy( data_[ i ].elems, data[ i ], sizeof( float ) * dims );
		data_[ i ].score = 0;
		data_[ i ].partition = 0;
	}
}

template < uint32_t dims >
void inline generate_coordinates( Partition< dims > &original, Partition< dims > &result, 
	const uint32_t bitmask, Tuple< dims > &pivot ) {
	
	for( uint32_t d = 0; d < dims; ++d ) {
		if( original.lower_bound_coord.elems[ d ] < pivot.elems[ d ] 
			&& pivot.elems[ d ] < original.upper_bound_coord.elems[ d ]  ) { // pivot overlaps range
			
			if( bitmask & SHIFTS[ d ] ) {
				result.lower_bound_coord.elems[ d ] = pivot.elems[ d ];
				result.upper_bound_coord.elems[ d ] = original.upper_bound_coord.elems[ d ];
			}
			else {
				result.lower_bound_coord.elems[ d ] = original.lower_bound_coord.elems[ d ];
				result.upper_bound_coord.elems[ d ] = pivot.elems[ d ];
			}
		}
		else { // don't need to split this dimension.
			result.lower_bound_coord.elems[ d ] = original.lower_bound_coord.elems[ d ];
			result.upper_bound_coord.elems[ d ] = original.upper_bound_coord.elems[ d ];
		}
	}
}


template< uint32_t dims > uint32_t inline PartitionBased< dims >
::select_pivot( Partitioning< dims > *partitions, std::vector< uint32_t > *choices ) {
	
	/* Create array of results for parallel reduction */
	std::vector< uint32_t > min_id( t_, -1 );
	
	/* First find partition with the most points in it */
	for( uint32_t i = 0; i < choices->size(); ++i ) {
		auto choice = choices->begin() + i;
		auto next = partitions->begin() + *choice;
		if( next->points[ 0 ] < min_id[ omp_get_thread_num() ] ) {
			min_id[ omp_get_thread_num() ] = next->points[ 0 ];
		}
	}
	
	/* Then return the point in the partition with the largest dominance area */
	for( uint32_t i = 1; i < t_; ++i ) {
		if( min_id[ i ] < min_id[ 0 ] ) { min_id[ 0 ] = min_id[ i ]; }
	}
	return min_id[ 0 ];
}


template< uint32_t dims > void inline PartitionBased< dims >
::sort_by_volume() {
	
	/* First calculate the volume for every point in parallel. */
#pragma omp parallel for
	for( uint32_t i = 0; i < n_; ++i ) {
		float volume = 1;
		for( uint32_t d = 0; d < dims; ++d ) {
			volume *= ( 1 - data_[ i ].elems[ d ] );
		}
		data_[ i ].score = volume;
	}
	
	/* Then use built-in parallel sort to sort points by volume */
	std::__parallel::sort( data_, data_ + n_, std::greater< PTuple< dims > >() );
}


template< uint32_t dims > void inline PartitionBased< dims >
::copy_result( std::vector< answer > &q, const uint32_t k) {
	
	for( uint32_t i = 0; i < k; ++i ) {
		
		//Move next lowest scoring k'th highest point to solution vector
		result_.push_back( data_[ q.front().second ].pid );
		
		//Then remove it from the heap
		std::pop_heap( q.begin(), q.end(), maxAnswer );
		q.pop_back();
	}
	std::reverse( result_.begin(), result_.end() ); // because q was sorted in opposite order
}


template< uint32_t dims > std::vector< uint32_t > PartitionBased< dims >
::Execute( const uint32_t k ) {
	
	/* Create a partitioning and a double buffer copy */
	Partitioning< dims > *occupied_partitions = new Partitioning< dims >(),
		*occupied_partitions_db = new Partitioning< dims >();
		
	std::vector< uint32_t > active_partitions; //indexes for partitions that may have sol'ns
	const uint32_t dominated_mask = ( 1 << dims ) - 1; //all relevant bits are set.
		
	/* Initialise priority queue with dummy answers that have no score. */
	std::vector< answer > q;
	for( uint32_t i = 0; i < k; ++i ) { 
		q.push_back( std::pair< uint32_t, uint32_t > ( 0, i ) );
	}
	std::make_heap( q.begin(), q.end(), maxAnswer );
	
	/* Push every point into original partition (entire data space) */
	occupied_partitions->push_back( Partition< dims >( n_ ) );
	for( uint32_t i = 0; i < n_; ++i ) {
		occupied_partitions->back().points.push_back( i );
	}
	active_partitions.push_back( 0 ); // make this partition is active
	
	
	/* First, sort by volume of dominance area. */
	sort_by_volume();
	
	/* print out sorted data for testing. 
	for( uint32_t i = 0; i < n_; ++i ) {
		std::cout << data_[ i ] << std::endl;
	}*/

	
	/* Iterate algorithm while some partitions still have sufficiently large upper bounds */
	while( !active_partitions.empty() ) { 
		
		/* Sanity check -- which partitions are still active 
		std::cout << std::endl;
		for( auto it = active_partitions.begin(); it != active_partitions.end(); ++it ) {
			std::cout << " active " << *it <<std::endl;
		}
		*/
	
		/* Select pivot and use it to partition dataset */
		const uint32_t pivot = select_pivot( occupied_partitions, &active_partitions );
		uint32_t pivot_score = 0;
		active_partitions.clear();
		
		#pragma omp parallel for
		for( uint32_t i = 1; i < n_; ++i ) {
			data_[ i ].partition = DT_bitmap_dvc( data_[ i ], data_[ pivot ] );
		}
		
		/* Sanity check -- which pivot did we choose? 
		std::cout << pivot << data_[ pivot ] << std::endl;
		*/
		
		/* Sub-partition our partitioning based on newly generated bitmasks */
		swap( &occupied_partitions, &occupied_partitions_db );
		Partitioning< dims > occ_parts_per_thread[ t_ ]; // array for parallel reduction
		#pragma omp parallel for reduction( +: pivot_score )
		for( uint32_t i = 0; i < occupied_partitions_db->size(); ++i ) {
		
			/* Grab the next partition to split */
			Partition< dims > &toBeSplit = occupied_partitions_db->at( i );
			const uint32_t prev_score = toBeSplit.upper_bound_score;
			
			/* Conduct the actual sub-partitioning */
			std::unordered_map< uint32_t, Partition< dims > > subpartitions;
			for( auto it = toBeSplit.points.begin(); it != toBeSplit.points.end(); ++it ) {
				if( *it == pivot ) { continue; }
				const uint32_t p = data_[ *it ].partition;
				if( subpartitions.count( p ) == 0 ) {
					
					subpartitions[ p ] = Partition< dims >( 0 );
					generate_coordinates( toBeSplit, subpartitions[ p ], p, data_[ pivot ] );
				}
				subpartitions[ p ].points.push_back( *it );
			}
			
			/* Increment pivot's score if there is a relevant subpartition. */
			if( subpartitions.count( dominated_mask ) > 0 ) {
				pivot_score += subpartitions[ dominated_mask ].points.size();
			}
			
			/* Copy all the subpartitions into our parallel-reduction array */
			for( auto it = subpartitions.begin(); it != subpartitions.end(); ++it ) {
				occ_parts_per_thread[ omp_get_thread_num() ].push_back( it->second );
			}
		}
		occupied_partitions_db->clear(); // these have all been sub-partitioned now.
		
		/* Complete parallel reduction */
		for( uint32_t i = 0; i < t_; ++i ) {
			if( occ_parts_per_thread[ i ].size() == 0 ) { continue; }
			occupied_partitions->insert( occupied_partitions->end(), 
				occ_parts_per_thread[ i ].begin(), occ_parts_per_thread[ i ].end() );
		}
		
		/* Check if this pivot qualifies for our current top k solution */
		if( pivot_score > q.front().first ) {
			
			// Remove k'th best
			std::pop_heap( q.begin(), q.end(), maxAnswer );
			q.pop_back();
			
			// Add new one to heap
			q.push_back( answer ( pivot_score, pivot ) );
			std::push_heap( q.begin(), q.end(), maxAnswer );
		}
	
		/* Update the upper bound scores for every partition and determine which are active */
		std::vector< uint32_t > ap_reduction[ t_ ]; //for parallel reduction
		
		#pragma omp parallel for
		for( uint32_t i = 0; i < occupied_partitions->size(); ++i ) {
		
			// Add counts for any other partition that this one can (partially or not) dominate 
			for( auto it = occupied_partitions->begin(); it != occupied_partitions->end(); ++it ) { 
				if( DominateLeftDVC( occupied_partitions->at( i ).lower_bound_coord, it->lower_bound_coord ) ) { 
					occupied_partitions->at( i ).upper_bound_score += it->points.size();
				}
			}
			if( occupied_partitions->at( i ).upper_bound_score > q.front().first ) {
				ap_reduction[ omp_get_thread_num() ].push_back( i );
			}
		}
		
		/* Complete parallel reduction of active_partitions. */
		for( uint32_t i = 0; i < t_; ++i ) {
			active_partitions.insert( active_partitions.end(), 
				ap_reduction[ i ].begin(), ap_reduction[ i ].end() );
		}
	
		/* Print for sanity check. What do our new partitions look like now? 
		for( auto it = occupied_partitions->begin(); it != occupied_partitions->end(); ++it ) { 
			std::cout << "Threshold: " <<  q.front().first << *it << std::endl;
		}
		*/
	
	}// ----------------------- END OF MAIN LOOP  ------------------------- //
	
	/* Copy result from priority queue into actual result vector */
	copy_result( q, k );
	
	/* Echo solution to stdout
	for( auto it = result_.begin(); it != result_.end(); ++it ) {
		for( uint32_t i = 0; i < n_; ++i ) {
			if( data_[ i ].pid == *it ) {
				std::cout << data_[ i ] << std::endl;
				break;
			}
		}
	}
	 */
	
	
	/* Clean up and return */
	delete occupied_partitions, occupied_partitions_db;
	return result_;
}
