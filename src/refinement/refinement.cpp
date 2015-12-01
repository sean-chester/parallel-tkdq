/**
 * Implementation of the unindexed Refinement algorithm for top-k 
 * dominating queries.
 *
 * @author Sean Chester
 * @date 25 November 2015
 */

#include <iostream>
#include <array>

#include "refinement/refinement.h"

template class Refinement< 2 >;
template class Refinement< 3 >;
template class Refinement< 4 >;
template class Refinement< 5 >;
template class Refinement< 6 >;
template class Refinement< 7 >;
template class Refinement< 8 >;
template class Refinement< 9 >;
template class Refinement< 10 >;


template< uint32_t dims >
void Refinement< dims >
::Init( float** data ) {

	/* Allocate space. */
	data_ = new PTuple< dims >[n_];
	
	/* Copy data from float array into tuple array and record
	 * point ids. */
#pragma omp parallel for
	for ( uint32_t i = 0; i < n_; ++i ) {
		data_[ i ].pid = i;
		data_[ i ].score = 0;
		memcpy( data_[ i ].elems, data[ i ], sizeof( float ) * dims );
	}
}

template< uint32_t dims > uint32_t Refinement< dims > 
::counting_pass( const uint32_t k ) {

	/* Init data structures with all zeroes */
	std::array< uint32_t, ( 1 << dims ) > cell_counts;
	for( uint32_t i = 0; i < ( 1 << dims ); ++i ) {
		cell_counts[ i ] = 0;
		cell_dom_by_counts_[ i ] = 0;
	}
	
	/* Statically build a midpoint to partition a static grid with */
	Tuple< dims > midpoint;
	for( uint32_t i = 0; i < dims; ++i ) { midpoint.elems[ i ] = 0.5; }
	
	/* Assign each point to a grid relative to the midpoint */
	#pragma omp parallel for
	for( uint32_t i = 0; i < n_; ++i ) {
		data_[ i ].partition = DT_bitmap_dvc( data_[ i ], midpoint );
	}
	
	/* Sort the data so that all points in the same grid are adjacent */
	std::__parallel::sort( data_, data_ + n_, std::less< PTuple< dims > >() );
	
	/* Populate population counts for each cell */
	uint32_t start = 0;
	for( uint32_t i = 0; i < n_; ++i ) {
		if( data_[ i ].partition != data_[ start ].partition ) {
			cell_counts[ data_[ start ].partition ] = i - start;
			start = i;
		}
	}
	cell_counts[ data_[ start ].partition ] = n_ - start - 1; //last occupied cell
	const uint32_t last_mask = data_[ start ].partition; // bound on max cell id
	
	/* Sanity check: print out partition counts 
	for( uint32_t i = 0; i <= last_mask; ++i ) { 
		std::cout << i << " " << cell_counts[ i ] << std::endl;
	}
	*/
	
	/* Calculate bounds for each cell, meanwhile maintaining order of lower bounds */
	std::vector< std::pair< uint32_t, uint32_t > > pq;
	//#pragma omp parallel for <-- disabled because 
	// priority queue + cell_dom_by_counts not parallel friendly (NPF)!
	for( uint32_t i = 0; i <= last_mask; ++i ) {
		uint32_t upper_bound = 0, lower_bound = 0;
		for( uint32_t j = i; j <= last_mask; ++j ) { 
			if( ( i & j ) == i ) {
				upper_bound += cell_counts[ j ];
				if( i == 0 && j == ( ( 1 << dims ) - 1 ) ) { // fully dominates
					lower_bound += cell_counts[ j ];
					cell_dom_by_counts_[ j ] += cell_counts[ i ]; //NPF!!!
				}
			}
		}
		grid_cell_bounds_[ i ].first = lower_bound;
		grid_cell_bounds_[ i ].second = upper_bound;
		pq.push_back( std::pair< uint32_t, uint32_t > ( lower_bound, cell_counts[ i ] ) );//NPF!!!
	}
	
	/* Sanity check: print out the bounds that we derived. 
	for( auto it = grid_cell_bounds.begin(); it != grid_cell_bounds.end(); ++it ) {
		std::cout << it->first << " " << it->second << std::endl;
	}
	*/
	
	/* compute minimum lower bound score that may contain TKDQ points. */
	std::make_heap( pq.begin(), pq.end(), std::less< std::pair< uint32_t, uint32_t > >() );
	uint32_t gamma = 0, points_seen = 0;
	while( !pq.empty() && points_seen < k ) {
		
		/* Update gamma threshold with next from the heap. */
		std::pair< uint32_t, uint32_t > &next = pq.front();
		gamma += next.first;
		points_seen += next.second;
		
		/* Pop it off the queue. */
		std::pop_heap( pq.begin(), pq.end(), std::less< std::pair< uint32_t, uint32_t > >() ); 
		pq.pop_back();
	}
	
	/* Determine which partitions can be explicitly pruned froms scores and on gamma. */
	#pragma omp parallel for
	for( uint32_t i = 0; i < ( 1 << dims ); ++i ) {
		if( cell_counts[ i ] == 0 ) { pruned_[ i ] = true; }
		else if( grid_cell_bounds_[ i ].second < gamma ) { pruned_[ i ] = true; }
		else if( cell_dom_by_counts_[ i ] >= k ) { pruned_[ i ] = true; }
		else { pruned_[ i ] = false; }
	}
	
	return gamma;
}


template< uint32_t dims > void Refinement< dims >
::prepare_result( const uint32_t k ) {
	
	/* Sort all the data points by the dominance score (pruned ones have score = 0 ) */
	std::__parallel::sort( data_, data_ + n_, std::greater< STuple< dims > >() );
	
	/* Then copy k first points into result vector */
	for( uint32_t i = 0; i < k; ++i ) {
		result_.push_back( data_[ i ].pid );
		//std::cout << i << " " << data_[ i ] << std::endl;
	}
}


// Implementation mostly correct. 
// Bit about partial dominance => irrelevance doesn't apply 'til 
// filter pass is implemented so isn't here yet.
// also, not doing new checks with gamma (Line 3), 'though I think that 
// will be reflected by pruned_ anyway, since gamma doesn't get updated 
// in Algorithm 7.
// Also, haven't implemented Line 6 since this seems to be related to 
// the irrelevance bit.
template< uint32_t dims > void Refinement< dims >
::refinement_pass( ) {
	
	/* Create flattened list of candidates for better parallel workload balance */
	std::vector< uint32_t > flat_candidates;
	for( auto it = candidates_.begin(); it != candidates_.end(); ++it ) {
		flat_candidates.insert( flat_candidates.end(), it->begin(), it->end() );
	}
	
	/* Iterate every candidate point, computing its score */
	#pragma omp parallel for schedule ( dynamic, 16 )
	for( uint32_t i = 0; i < flat_candidates.size(); ++i ) { // Line 12 (sort of)
		const uint32_t index = flat_candidates[ i ];
		const uint32_t my_partition = data_[ index ].partition;
		
		/* Iterate dataset to calculate the score of this candidate */
		// To be more fair, we should iterate partitions a la my ICDE/PVLDB papers
		// I.e., build a map from partition to start index so we can skip entire 
		// partitions at once. But I don't think this is a hot spot of the code.
		for( uint32_t other = 0; other < n_; ++other ) {
			const uint32_t your_partition = data_ [ other ].partition;
		
			/* Skip the point if there is a reason to */
			if( pruned_[ your_partition ] ) { continue; } // Line 3,9 (sort of)
			if( ( my_partition & your_partition ) == my_partition ) { // Line 11
				if( DominateLeft< dims >( data_[ index ], data_[ other ] ) ) { // Line 12 (sort of)
					++data_[ index ].score; // Line 13 (sort of)
				}
			}
		}
	}
}


template< uint32_t dims > void Refinement< dims >
::coarse_grained_filter( const uint32_t gamma, const uint32_t k ) {
	
	/* Create an array of counts for how many times each point has been dominated */
	std::vector< uint32_t > dom_counts;
	dom_counts.reserve( n_ );
	
	/* Iterate all points to determine if they should be candidates */
	//#pragma omp parallel for //<--note! means points against which p are compared is non-deterministic!
	for( uint32_t i = 0; i < n_; ++i ) { // Line 4
		const uint32_t p = data_[ i ].partition; // Line 5
		const uint32_t lower_bound = grid_cell_bounds_[ p ].first; // Line 6
		const uint32_t upper_bound = grid_cell_bounds_[ p ].second; // Line 6
		dom_counts[ i ] = cell_dom_by_counts_[ p ]; // Line 6
		bool done_processing = false;
		for( uint32_t other_p = 0; other_p < p && !done_processing; ++other_p ) { // Line 12
			if( ( p & other_p ) == other_p ) { // Line 12
				for( auto it = candidates_[ other_p ].begin(); 
					it != candidates_[ other_p ].end(); ++it ) { // Line 8
					
					if( DominateLeft< dims >( data_[ *it ], data_[ i ] ) ) { // Line 8
						if( ++dom_counts[ i ] >= k ) { // Line 9, 10
							done_processing = true; break; //Line 11
						}
					}
				}
			}
		}
		// Could skip Lines 12-16 because not very parallel or cache friendly.
		// But this means that points will not be visited later so less pruning.
		for( uint32_t other_p = p; other_p < ( 1 << dims ) && !done_processing; ++other_p ) { // Line 7
			if( ( p & other_p ) == p ) { // Line 7
				auto it = candidates_[ other_p ].begin(); 
				while( it != candidates_[ other_p ].end() ) { // Line 13
					
					if( DominateLeft< dims >( data_[ i ], data_[ *it ] ) ) { // Line 13
						if( ++dom_counts[ *it ] >= k ) { // Line 14, 15
							it = candidates_[ other_p ].erase( it ); //Line 16
						}
						else { ++it; }
					}else { ++it; }
				}
			}
		}
		if( !done_processing && upper_bound >= gamma ) { // Line 17
			candidates_[ p ].push_back( i ); // Line 18
		}
	}
}


template< uint32_t dims > uint32_t Refinement< dims >
::num_candidates() {
	uint32_t count = 0;
	for( uint32_t i = 0; i < ( 1 << dims ); ++i ) {
		count += candidates_[ i ].size();
	}
	return count;
}


template< uint32_t dims >
std::vector< uint32_t > Refinement< dims >
::Execute( const uint32_t k ) {
	
	/* First, conduct counting pass. */
	uint32_t gamma = counting_pass( k );
	
	/* Next, conduct the filter pass. */
	coarse_grained_filter( gamma, k );
	//std::cout << "Num candidates = " << num_candidates() << std::endl;


	/* Finally, conduct the refinement pass (Algorithm 7). */
	refinement_pass();
	
	
	/* Copy the top-k points into the output array and return it. */
	prepare_result( k );
	return result_;
}
