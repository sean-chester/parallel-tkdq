/**
 * @file papi_counting.h
 * Library for updating/accessing standard PAPI counters
 * @author Sean Chester (schester@cs.au.dk)
 * @version 2.1
 * @remark This is a major revision of the PAPI library built by
 * Kenneth S. B&oslash;gh, Matthias Rav, and Manuel R. Ciosici.
 * @remark This version is a minor update to calculate relevant PAPI counters on an
 * Intel(R) Xeon(R) CPU E5-2683 v3, running the SkyBench suite.
 * @date 16 July 2015
 * @note The first time you use this library on a new architecture, it may be
 * worth running \c papi_avail to ensure that the hardware counters set up in this library
 * are actually available on the machine that you are using.
 *
 * This library contains a number of classes for tracking
 * hardware counters. A base class defines the general
 * behaviour, and each sub class defines the specific hardware
 * counters that are tracked. To use the library, ensure that:
 * <pre><code>
 *  PAPI_library_init( PAPI_VER_CURRENT );
 *	if( PAPI_thread_init( pthread_self ) != PAPI_OK ) {
 *	    //handle PAPI initialization error.
 *	}
 * </code></pre>
 * is called somewhere prior to the use of the counters, with PAPI_thread_init called 
 * from each thread. Also,
 * allocate an array of counters for each thread/core on the machine,
 * and start/stop the counters inside a <code>\#pragma omp parallel</code> block (so
 * that each counter in the array is started by its corresponding thread).
 * For example:
 * <pre><code>
 *  papi_base< 5 > counters[ omp_get_num_threads() ];
 *
 * \#pragma omp parallel
 * 	{
 * 		counters[ omp_get_thread_num() ].start();
 * 		counters[ omp_get_thread_num() ].stop();
 * 	}
 * </code></pre>
 * This base class defines print methods that can be used to extract human-readable
 * headers and to echo the values of the counters to an output stream.
 */

#include <iostream>

#include "papi.h"


#ifndef PAPI_COUNTING_H
#define PAPI_COUNTING_H


#define	PAPI_MODE_OFF	0 /**< Indicates not to use PAPI counters. */
#define	PAPI_MODE_CACHE	1 /**< Indicates to measure L2 and L3 cache performance. */
#define	PAPI_MODE_BRANCH	2 /**< Indicates to measure branch prediction performance. */
#define	PAPI_MODE_THROUGHPUT	3 /**< Indicates to measure throughput as cycles per instruction. */


/**
 * The timer struct originally used by Kenneth, Matthias, and
 * Manuel, but no longer supported.
 * @deprecated
 * This is the profiler that was used in the original PAPI library. It is still
 * fully functional, but unsupported. Profiler is the current suggested alternative.
 * @see Profiler
 * A more powerful profiler that is suggested for use instead.
 */
class oldtimer {
public:

	/**
	 * Constructs a new oldtimer class instance.
	 */
	oldtimer() {}

	/**
	 * Destructs the oldtimer instance.
	 */
	~oldtimer() {}

	/**
	 * Starts the internal clock of this profiler.
	 */
	void start() {
		clock_gettime(CLOCK_MONOTONIC, &time_start);
	}

	/**
	 * Halts the internal clock of this profiler.
	 */
	void stop() {
		clock_gettime(CLOCK_MONOTONIC, &time_end);
	}

	/**
	 * Reports the time elapsed between the most recent stop() invocation and the
	 * most recent start() invocation.
	 * @return The elapsed time between start() and stop() invocations.
	 */
	double elapsed() {
		return (time_end.tv_sec - time_start.tv_sec)
				+ (time_end.tv_nsec	- time_start.tv_nsec) * 1e-9;
	}

private:
	timespec time_start; /**< The time at which this timer's clock was most recently started. */
	timespec time_end; /**< The time at which this timer's clock was most recently halted. */
};

/**
 * The base class that defines the general behaviour related to any
 * set of hardware counters.
 * @tparam NUM_COUNTERS The number of hardware counters used in the given subset.
 *
 * A papi_base object defines all the methods related to hardware
 * counters without knowing how many there are (the template parametre
 * NUM_COUNTERS) and without knowing what they are. These are specialised
 * within the specific subclasses.
 */
template < uint32_t NUM_COUNTERS >
class papi_base {

public:
	/**
	 * Resets all counters to zero.
	 */
	void reset() {
		for( uint32_t i = 0; i < NUM_COUNTERS; ++i ) {
			values_[ i ] = 0;
		}
	}

	/**
	 * Starts tracking all the hardware counters in this subset.
	 */
	void start( ) {
		if( NUM_COUNTERS <= 0 ) { return; }
		std::vector<int> eventsMutable( counters_, counters_ + NUM_COUNTERS );
		int retval = PAPI_start_counters(&eventsMutable[0], NUM_COUNTERS );
		if (retval == PAPI_OK) {
			papi_started_ = true;
		} else {
			printf("PAPI error %d: %s\n", retval, PAPI_strerror(retval));
			papi_started_ = false;
		}
	}

	/**
	 * Stops tracking all the hardware counters in this subset.
	 * @post The values accessible in this object have been updated
	 * to reflect the change in the actual, physical hardware counters
	 * since start() was invoked.
	 * @note Without calling stop(), none of the member variables of
	 * this object will have been updated.
	 */
	void stop( ) {
		if( NUM_COUNTERS <= 0 ) { return; }
		if ( papi_started_ ) {
			long long v[ NUM_COUNTERS ];
			int retval = PAPI_stop_counters(&v[0], NUM_COUNTERS );
			if (retval != PAPI_OK) handle_error(retval);
			for (uint32_t i = 0; i < NUM_COUNTERS; ++i) {
				values_[i] += v[i];
			}
		}
		else {
			for (uint32_t i = 0; i < NUM_COUNTERS; ++i) {
				values_[i] = -1;
			}
		}
		papi_started_ = false;
	}

	/**
	 * Calculates the piece-wise sum for each counter between this and another
	 * papi object and overwrites the values in this object with the results.
	 * @param other The other papi object with which to calculate the piece-wise sum.
	 * @return A reference to this object after modification.
	 * @post Each counter in this object has been incremented by the value of the
	 * corresponding counter in other.
	 */
	papi_base< NUM_COUNTERS > & operator+=(const papi_base< NUM_COUNTERS > & other) {
		for( uint32_t i = 0; i < NUM_COUNTERS; ++i ) {
			values_[ i ] += other.values_[ i ];
		}
		return *this;
	}

	/**
	 * Calculates the piece-wise absolute difference for each counter between this and
	 * another papi object.
	 * @param other The other papi object with which to calculate the piece-wise absolute
	 * difference.
	 * @return A new object in which each counter has a value equal to the absolute
	 * difference of the corresponding counter between this object and other.
	 */
	papi_base< NUM_COUNTERS > operator-(const papi_base< NUM_COUNTERS > & other) {
		papi_base< NUM_COUNTERS > diff;
		for( uint32_t i = 0; i < NUM_COUNTERS; ++i ) {
			diff.values_[ i ] = absolute_difference( values_[ i ], other.values_[ i ] );
		}
		return diff;
	}

	/**
	 * Divides each counter in this counter set by a scalar value.
	 * @param scalar The scalar value by which each counter should be divided.
	 * @return A new object in which each counter has a value equal to 1/scalar
	 * of what the value was in this object prior to invoking this operator.
	 */
	papi_base< NUM_COUNTERS > operator/(const uint32_t scalar) {
		papi_base< NUM_COUNTERS > diff;
		for( uint32_t i = 0; i < NUM_COUNTERS; ++i ) {
			diff.values_[ i ] = values_[ i ] / scalar;
		}
		return diff;
	}

	/**
	 * Returns a string of tab-separated human-readable names
	 * for this set of counters, in the same order that the
	 * counters would be printed to an output stream.
	 * @return The string of tab-separated headers.
	 */
	std::string inline header_string() {
		if( NUM_COUNTERS <= 0 ) { return ""; }
		std::string output( headers_[0] );
		for( uint32_t i = 1; i < NUM_COUNTERS; ++i ) {
			output = output + "\t" + headers_[ i ];
		}
		output = output + "\n";
		return output;
	}

protected:

	/**
	 * Constructs a new papi_base object with all counters
	 * initialized at zero.
	 */
	papi_base() : papi_started_ ( false ) {
		reset();
	}

	/**
	 * Registers a new counter with this object.
	 * @param index The array index in which this counter should be stored.
	 * @param counter The PAPI library encoding for the hardware counter that
	 * should be tracked.
	 * @param header A human-readable string indicating what is being tracked by
	 * this hardware counter.
	 */
	void inline register_counter( const uint32_t index, const long long counter,
			const std::string &header ) {

		counters_[ index ] = counter;
		headers_[ index ] = header;
	}

	friend std::ostream &operator<<( std::ostream &output, const papi_base< NUM_COUNTERS > &papi ) {
		if( NUM_COUNTERS <= 0 ) { return output; }
		output << papi.values_[0];
		for( uint32_t i = 1; i < NUM_COUNTERS; ++i ) {
			output << "\t" << papi.values_[ i ];
		}
		output << std::endl;
		return output;
	}

	long long values_[ NUM_COUNTERS ]; /**< An array containing the actual hardware counter values (recorded at stop() invocations). */

private:
	/**
	 * A helper function to calculate the absolute value of the
	 * difference between two long long values.
	 * @param v1 The first value.
	 * @param v2 The second value.
	 * @return The absolute value of the difference between v1 and v2.
	 */
	inline long long absolute_difference( const long long v1, const long long v2 ) {
		return ( v1 > v2 ? v1 - v2 : v2 - v1 );
	}

	/**
	 * Handles an error code caught from PAPI library calls.
	 * @param retval The error code returned by a PAPI library call.
	 */
	void handle_error(int retval) {
		printf("PAPI error %d: %s\n", retval, PAPI_strerror(retval));
		exit(1);
	}

	bool papi_started_; /**< A flag indicating whether or not this set of papi counters is currently being tracked. */
	long long counters_[ NUM_COUNTERS ]; /**< An array mapping the indexes used in this object to PAPI hardware counter ids. */
	std::string headers_[ NUM_COUNTERS ]; /**< An array that maps PAPI hardware counters onto human-readable strings. */
};


/**
 * A subset of PAPI hardware counters related to the distribution of instructions.
 */
class papi_instructions : public papi_base< 4 > {
public:

	papi_instructions() {
		this->register_counter( 0, PAPI_LD_INS, "loads" );//NOT on AMD
		this->register_counter( 1, PAPI_SR_INS, "stores" );//NOT on AMD
		this->register_counter( 2, PAPI_BR_INS, "branches" );
		this->register_counter( 3, PAPI_TOT_INS, "total" );
	}

	long long inline load_instructions() { return values_[0]; } /**< Returns the number of load instructions issued. */
	long long inline store_instructions() { return values_[1]; } /**< Returns the number of store instructions issued. */
	long long inline branch_instructions() { return values_[2]; } /**< Returns the number of branch instructions issued. */
	long long inline total_instructions() { return values_[3]; } /**< Returns the total number of instructions issued. */

	/**
	 * Returns the number of load instructions issued as a fraction of the
	 * total instructions.
	 */
	double load_instructions_ratio() {
		return load_instructions() / (double) total_instructions();
	}

	/**
	 * Returns the number of store instructions issued as a fraction of the
	 * total instructions.
	 */
	double store_instructions_ratio() {
		return store_instructions() / (double) total_instructions();
	}

	/**
	 * Returns the number of branch instructions issued as a fraction of the
	 * total instructions.
	 */
	double branch_instructions_ratio() {
		return branch_instructions() / (double) total_instructions();
	}
};

/**
 * A subset of PAPI hardware counters related to the distribution of cycles.
 */
class papi_cycles : public papi_base < 5 > {
public:


	papi_cycles() {
		this->register_counter( 0, PAPI_STL_ICY, "cycles_no_instructions_issue");
		this->register_counter( 1, PAPI_FUL_CCY, "cycles_max_instructions_completed"); //NOT on AMD
		this->register_counter( 2, PAPI_RES_STL, "cycles_stalled_any_resource");
		this->register_counter( 3, PAPI_TOT_CYC, "total_cycles");
		this->register_counter( 4, PAPI_TOT_INS, "total_instructions");
	}

	inline long long idle_cycles()		{ return values_[ 0 ]; } /**< The total number of cycles spent idling. */
	inline long long utilised_cycles()	{ return values_[ 1 ]; } /**< The total number of cycles that are maximally utilised. */
	inline long long stalled_cycles()	{ return values_[ 2 ]; } /**< The total number of cycles that are stalled. */
	inline long long cycles()			{ return values_[ 3 ]; } /**< The total number of cycles. */
	inline long long instructions()			{ return values_[ 4 ]; } /**< The total number of instructions retired. */

	/**
	 * Returns the fraction of cycles that are stalled due to any resource.
	 */
	double stalled_cycles_ratio() {
		return stalled_cycles() / (double) cycles();
	}

	/**
	 * Returns the fraction of cycles that are idled.
	 */
	double idle_cycles_ratio() {
		return idle_cycles() / (double) cycles();
	}

	/**
	 * Returns the fraction of cycles that are maximally utilised.
	 */
	double utilised_cycles_ratio() {
		return utilised_cycles() / (double) cycles();
	}

	/**
	 * Returns the average number of cycles spent retiring an instruction.
	 */
	double cycles_per_instruction() {
		return cycles() / (double) instructions();
	}
};

/**
 * A subset of PAPI hardware counters related to cache hit performance.
 */
class papi_cache : public papi_base < 4 > {
public:

	papi_cache() {
		this->register_counter( 0, PAPI_L2_TCM, "L2_total_cache_misses");
		this->register_counter( 1, PAPI_L2_TCA, "L2_total_cache_accesses");
		this->register_counter( 2, PAPI_L3_TCM, "L3_total_cache_misses");
		this->register_counter( 3, PAPI_L3_TCA, "L3_total_cache_accesses");
	}

	long long inline l2_cache_misses()		{ return values_[ 0 ]; } /**< The number of Level 2 total cache misses. */
	long long inline l2_cache_accesses()	{ return values_[ 1 ]; } /**< The number of Level 2 total cache accesses. */
	long long inline l3_cache_misses() 		{ return values_[ 2 ]; } /**< The number of Level 3 total cache misses. */
	long long inline l3_cache_accesses()	{ return values_[ 3 ]; } /**< The number of Level 3 total cache misses. */

	/**
	 * Returns the fraction of L2 cache accesses (both data and instruction) that were 
	 * missed (because the resource was not available in L2 cache).
	 */
	double l2_miss_ratio() {
		return l2_cache_misses() / (double) l2_cache_accesses();
	}

	/**
	 * Returns the fraction of L3 cache accesses (both data and instruction) that were
	 * missed (because the resource was not available in L3 cache).
	 */
	double l3_miss_ratio() {
		return l3_cache_misses() / (double) l3_cache_accesses();
	}
};

/**
 * A subset of PAPI hardware counters related to branch prediction performance.
 */
class papi_pred : public papi_base < 2 > {
public:

	papi_pred() {
		this->register_counter( 0, PAPI_BR_PRC, "conditional_branches_correctly_predicted");//NOT on AMD
		this->register_counter( 1, PAPI_BR_MSP, "conditional_branches_mispredicted");
	}

	long long inline branch_instructions()	{ return values_[ 0 ] + values_[ 1 ]; } /**< The number of branch instructions issued. */
	long long inline branch_misses()		{ return values_[ 1 ]; } /**< The number of branch instructions mispredicted. */
	long long inline branch_hits()			{ return values_[ 1 ]; } /**< The number of branch instructions correctly predicted. */

	/**
	 * Returns the fraction of conditional branches that were incorrectly
	 * predicted by the branch predictor.
	 * @note This can also be derived as 1.0 - prediction_ratio()
	 */
	double misprediction_ratio() {
		return branch_misses() / (double) branch_instructions();
	}

	/**
	 * Returns the fraction of conditional branches that were predicted
	 * correctly by the branch predictor.
	 * @note This can also be derived as 1.0 - misprediction_ratio()
	 */
	double prediction_ratio() {
		return branch_hits() / (double) branch_instructions();
	}
};

/**
 * A subset of PAPI hardware counters related to transaction lookaside buffer performance.
 */
class papi_tlb : public papi_base < 2 > {
public:

	papi_tlb() {
		this->register_counter( 0, PAPI_TLB_DM, "dtlb_misses" );
		this->register_counter( 1, PAPI_TLB_IM, "itlb_misses" );
	}

	long long inline data_tlbs()		{ return values_[ 0 ]; } /**< The total number of data tlb misses. */
	long long inline instruction_tlbs()	{ return values_[ 1 ]; } /**< The total number of instruction tlb misses. */
};

/**
 * An empty subset of PAPI hardware counters, for use when this library is enabled,
 * but shouldn't be used (e.g., in unit tests).
 */
class papi_none : public papi_base < 0 > {
public:

	papi_none() { }
};


#endif // PAPI_COUNTING_H
