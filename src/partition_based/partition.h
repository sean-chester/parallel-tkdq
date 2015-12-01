/**
 * Definition of a Partition for the PartitionBased TKDQ solver.
 *
 * @author Sean Chester
 * @date 24 November 2015
 * @see PartitionBased
 */

#ifndef PARTITION_H_
#define PARTITION_H_

#include "common/common.h"
#include <vector>

template < uint32_t dims >
class Partition {

public:

	/**
	 * Creates a new Part object with nothing specified.
	 * @post Constructs a new, empty Partition object.
	 */
	Partition( ) {}

	/**
	 * Creates a new Partition object that has a specified upper 
	 * bound score, n, and covers the entire space [0,..., 0] 
	 * to [1,..., 1].
	 * @param n The number of points in the partition (and, thus, 
	 * one larger than the maximum dominance score a point can 
	 * obtain).
	 * @post Constructs a new Partition object corresponding to the 
	 * entire data space.
	 */
	Partition( const int32_t n );

	/**
	 * Creates a new Partition object that has a specified upper 
	 * bound score, n, and covers a given hyper-rectangular space.
	 * @param lower_corner The minimum coordinate of the hyper-rectangle 
	 * for each dimension
	 * @param upper_corner The maximum coordinate of the hyper-rectangle 
	 * for each dimension.
	 * @param n The number of points in the partition (and, thus, 
	 * one larger than the maximum dominance score a point can 
	 * obtain).
	 * @post Constructs a new Partition object corresponding to the 
	 * given data space.
	 */
	Partition( Tuple< dims > lower_corner, Tuple< dims > upper_corner, const int32_t n );
  
  /**
   * Appends Partition p to the output stream out.
   * @param out The output stream to which the Partition should be appended
   * @param p The Partition that should be appended to the output stream
   * @return The output stream, out, modified to have p appended to it
   * @post The output stream, out, is modified to have p appended to it
   */
  friend std::ostream& operator<<( std::ostream &out, const Partition< dims > &p ) { 
  	out << "(" << p.upper_bound_score << ") ";
		out << p.lower_bound_coord << "<->" << p.upper_bound_coord;
		out << ":";
		for( uint32_t i = 0; i < p.points.size(); ++i ) {
			out << " " << p.points[ i ];
		}
		return out;
	}  
	
	
	
	//data members -- currently public
	Tuple< dims > lower_bound_coord;
	Tuple< dims > upper_bound_coord;
	std::vector< uint32_t > points;
	int32_t upper_bound_score;
};

#endif /* PARTITION_H_ */
