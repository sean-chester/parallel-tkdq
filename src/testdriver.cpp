/**
 * Main driver for ParallelTKDQ suite
 *
 * @date 12 Nov 2015
 * @author Sean Chester
 *
 */

#include <testdriver.h>

#include <cstdlib>
#include <stdio.h>

#include "naive/naive.h"
#include "refinement/refinement.h"
#include "partition_based/partition_based.h"
#include "util/utilities.h"
#include "util/timing.h"
#include "util/mem_usage.h"
#include "common/tkdq_solver.h"
//#include "util/papi_counting.h"


/**
 * Returns a templated version of a Naive TKDQ solver.
 */
TKDQ_Solver* new_Naive( uint32_t t, uint32_t n, uint32_t d, 
	float **data ) {

	if( d == 2 ) { return new Naive< 2 >( t, n, data ); }
	else if( d == 3 ) { return new Naive< 3 >( t, n, data ); }
	else if( d == 4 ) { return new Naive< 4 >( t, n, data ); }
	else if( d == 5 ) { return new Naive< 5 >( t, n, data ); }
	else if( d == 6 ) { return new Naive< 6 >( t, n, data ); }
	else if( d == 7 ) { return new Naive< 7 >( t, n, data ); }
	else if( d == 8 ) { return new Naive< 8 >( t, n, data ); }
	else if( d == 9 ) { return new Naive< 9 >( t, n, data ); }
	else if( d == 10 ) { return new Naive< 10 >( t, n, data ); }
	
	return NULL; //unsupported dimensionality.
}


/**
 * Returns a templated version of a Refinement TKDQ solver.
 */
TKDQ_Solver* new_Refinement( uint32_t t, uint32_t n, uint32_t d, 
	float **data ) {

	if( d == 2 ) { return new Refinement< 2 >( t, n, data ); }
	else if( d == 3 ) { return new Refinement< 3 >( t, n, data ); }
	else if( d == 4 ) { return new Refinement< 4 >( t, n, data ); }
	else if( d == 5 ) { return new Refinement< 5 >( t, n, data ); }
	else if( d == 6 ) { return new Refinement< 6 >( t, n, data ); }
	else if( d == 7 ) { return new Refinement< 7 >( t, n, data ); }
	else if( d == 8 ) { return new Refinement< 8 >( t, n, data ); }
	else if( d == 9 ) { return new Refinement< 9 >( t, n, data ); }
	else if( d == 10 ) { return new Refinement< 10 >( t, n, data ); }
	
	return NULL; //unsupported dimensionality.
}


/**
 * Returns a templated version of a Partition TKDQ solver.
 */
TKDQ_Solver* new_PartitionBased( uint32_t t, uint32_t n, uint32_t d, 
	float **data ) {

	if( d == 2 ) { return new PartitionBased< 2 >( t, n, data ); }
	else if( d == 3 ) { return new PartitionBased< 3 >( t, n, data ); }
	else if( d == 4 ) { return new PartitionBased< 4 >( t, n, data ); }
	else if( d == 5 ) { return new PartitionBased< 5 >( t, n, data ); }
	else if( d == 6 ) { return new PartitionBased< 6 >( t, n, data ); }
	else if( d == 7 ) { return new PartitionBased< 7 >( t, n, data ); }
	else if( d == 8 ) { return new PartitionBased< 8 >( t, n, data ); }
	else if( d == 9 ) { return new PartitionBased< 9 >( t, n, data ); }
	else if( d == 10 ) { return new PartitionBased< 10 >( t, n, data ); }
	
	return NULL; //unsupported dimensionality.
}


/**
 * Create multi-threaded TKDQ solver
 */
TKDQ_Solver* createMTSkyline(string alg_name, const uint32_t n, const uint32_t d,
    float** data, uint32_t threads ) {
    
  /*
  uint32_t papi_mode_val = PAPI_MODE_OFF;
  if ( papi_mode.compare( "branch" ) == 0 ) { papi_mode_val = PAPI_MODE_BRANCH; }
  if ( papi_mode.compare( "cache" ) == 0 ) { papi_mode_val = PAPI_MODE_CACHE; }
  if ( papi_mode.compare( "throughput" ) == 0 ) { papi_mode_val = PAPI_MODE_THROUGHPUT; }
  */
    
  if ( alg_name.compare( alg_naive ) == 0 ) {
    return new_Naive( threads, n, d, data );
  }
  else if ( alg_name.compare( alg_refinement ) == 0 ) {
    return new_Refinement( threads, n, d, data );
  }
  else if ( alg_name.compare( alg_partition ) == 0 ) {
    return new_PartitionBased( threads, n, d, data );
  }

  return NULL;
}

void doPerformanceTest(Config &cfg) {
  vector<vector<float> > vvf = read_data(cfg.input_fname.c_str(), false, false);
  const uint32_t n = vvf.size();
  const uint32_t d = vvf.front().size();
#if COUNT_DT==1
  extern uint64_t dt_count;
  extern uint64_t dt_count_dom;
  extern uint64_t dt_count_incomp;
#endif

  float** data = AllocateDoubleArray(n, d);
  redistribute_data(vvf, data);
  vvf.clear();

  long msec = 0;
  std::vector< std::vector< uint32_t > > results;

#if PAPI_ON == 1
  int retval;
  if ( (retval = PAPI_library_init( PAPI_VER_CURRENT )) != PAPI_VER_CURRENT )
  ERROR_RETURN( 1 );

  const int num_events = 5;
  int EventSet = PAPI_NULL;
  // Get some native event codes:
  int LLC_REF = 0, LLC_MISSES = 0, BRANCH_TAKEN = 0, BRANCH_MISSED = 0,
  CYCLES_STALLED = 0;
  if ( (PAPI_event_name_to_code( "LLC_REFERENCES", &LLC_REF ) != PAPI_OK)
      || (PAPI_event_name_to_code( "LLC_MISSES", &LLC_MISSES ) != PAPI_OK)
      || (PAPI_event_name_to_code( "BRANCH_INSTRUCTIONS_RETIRED",
              &BRANCH_TAKEN ) != PAPI_OK)
      || (PAPI_event_name_to_code( "MISPREDICTED_BRANCH_RETIRED",
              &BRANCH_MISSED ) != PAPI_OK)
      || (PAPI_event_name_to_code( "DTLB_LOAD_ACCESS", &CYCLES_STALLED )
          != PAPI_OK) )
  ERROR_RETURN( 1 );
  int event_codes[num_events] = {PAPI_TOT_INS, LLC_REF, LLC_MISSES,
    BRANCH_TAKEN, BRANCH_MISSED};
#endif

  for (uint32_t a = 0; a < cfg.algo.size(); ++a) {
		for (uint32_t t = 0; t < cfg.threads.size(); ++t) {
#if COUNT_DT==1
			dt_count = 0;
			dt_count_dom = 0;
			dt_count_incomp = 0;
#endif
			const uint32_t num_threads = atoi(cfg.threads[t].c_str());
			TKDQ_Solver* solver = createMTSkyline( cfg.algo[a], n, d, data,
					num_threads );
			if ( solver != NULL) {
				msec = GetTime();
				// initialization:
				solver->Init(data);

				// skyline computation:
				std::vector< uint32_t > res = solver->Execute( cfg.k );
				std::cout << ( GetTime() - msec ) << " ";
				
				results.push_back(res);
				delete solver;
			} else {
				printf("Warning: unknown multi-threaded algorithm '%s' is skipped\n",
						cfg.algo[a].c_str());
			}
		}
  }
  printf("\n");

  if ( results.size() > 1 ) {
    for (uint32_t i = 1; i < results.size(); ++i) {
      if ( !CompareTwoLists( results[0], results[i], cfg.k, true ) ) {
        fprintf( stderr, "ERROR: Skylines of run #%u (|sky|=%lu) "
            "and #%u (|sky|=%lu) do not match!!!\n", 0, results[0].size(), i,
            results[i].size());
      }
    }
  }
}

void doVerboseTest(Config &cfg) {
#if COUNT_DT==1
  extern uint64_t dt_count;
  extern uint64_t dt_count_dom;
  extern uint64_t dt_count_incomp;
#endif
  long msec = 0;
  
  std::vector< std::vector< uint32_t > > results;

  printf("Input reading (%s)\n", cfg.input_fname.c_str());
  msec = GetTime();
  vector<vector<float> > vvf = read_data(cfg.input_fname.c_str(), false, false);
  const uint32_t n = vvf.size();
  const uint32_t d = vvf.front().size();
  msec = GetTime() - msec;
  printf(" d=%d;\n n=%d\n", d, n);
  printf(" duration: %ld msec\n", msec);

  float** data = AllocateDoubleArray(n, d);
  redistribute_data(vvf, data);
  vvf.clear();


  for (uint32_t a = 0; a < cfg.algo.size(); ++a) {
		for (uint32_t t = 0; t < cfg.threads.size(); ++t) {
#if COUNT_DT==1
			dt_count = 0;
			dt_count_dom = 0;
			dt_count_incomp = 0;
#endif
			const uint32_t num_threads = atoi(cfg.threads[t].c_str());
			TKDQ_Solver* solver = createMTSkyline(cfg.algo[a], n, d, data,
					num_threads );
			if ( solver != NULL) {
				printf("#%u: %s (t=%u)\n", a, cfg.algo[a].c_str(), num_threads);
				msec = GetTime();
				// initialization:
				solver->Init(data);
				long elapsed_msec = GetTime() - msec;
				printf(" init: %ld msec \n", elapsed_msec);

				// skyline computation:
				std::vector< uint32_t > res = solver->Execute( cfg.k );
				elapsed_msec = GetTime() - msec;

#if PAPI_ON == 1
				// Take measurements and cleanup
				if ( (retval = PAPI_stop( EventSet, values )) != PAPI_OK )
				ERROR_RETURN( retval );

				printf( " PAPI stuff:\n" );
				printf( " - #instructions: %lld\n", values[0] );
				//          printf(" - total cycles: %lld\n", values[0]);
				//          printf(" - CPI: %f\n", values[1] / (double) values[0]);
				printf( " - LLC miss rate: %f %%\t(%lld of %lld)\n",
						values[2] / (double) values[1] * 100.0, values[2], values[1] );
				printf( " - BRA miss rate: %f %%\t(%lld of %lld)\n",
						values[4] / (double) values[3] * 100.0, values[4], values[3] );

				if ( (retval = PAPI_remove_events( EventSet, event_codes, num_events ))
						!= PAPI_OK )
				ERROR_RETURN( retval );
				if ( (retval = PAPI_destroy_eventset( &EventSet )) != PAPI_OK )
				ERROR_RETURN( retval );
#endif

				printf(" runtime: %ld msec ", elapsed_msec);
				PrintTime(elapsed_msec);
				results.push_back(res);
				delete solver;
				
#if COUNT_DT==1
				printf( " DT/pt: %.2f\n", dt_count / (float) n );
				printf( " DT-dom/pt: %.2f\n", dt_count_dom / (float) n );
				printf( " DT-incomp/pt: %.2f\n", dt_count_incomp / (float) n );
#endif
			} else {
				printf("Warning: unknown multi-threaded algorithm '%s' is skipped\n",
						cfg.algo[a].c_str());
			}
		}
  }

  if ( results.size() > 1 ) {
    bool correct = true;
    for (uint32_t i = 1; i < results.size(); ++i) {
      if ( !CompareTwoLists( results[0], results[i], cfg.k, true ) ) {
        fprintf( stderr, "ERROR: Output of run #%u (|tkdq|=%lu) and "
            "#%u (|tkdq|=%lu) do not match!!!\n", 0, results[0].size(), i,
            results[i].size());
        correct = false;
      }
    }
    if (correct)
      printf("Comparison tests: PASSED!\n");
    else
      printf("Comparison tests: FAILED!\n");
  }

  if (!results.empty())
    printf(" |skyline| = %lu (%.2f %%)\n", results[0].size(),
        results[0].size() * 100.0 / n);

  FreeDoubleArray(n, data);
}

void printUsage() {
  std::cout << "\nParallelTKDQ - a benchmark for skyline algorithms" << std::endl << std::endl;
  std::cout << "USAGE: ./ParallelTKDQ -f filename [-t \"num_threads\"] [-v]" << std::endl;
  std::cout << "       [-a size] [-q size]" << std::endl;
  std::cout << " -f: input filename\n" << std::endl;
  std::cout << " -t: run with num_threads, e.g., \"1 2 4\" (default \"4\")" << std::endl;
  std::cout << "     Note: used only with multi-threaded algorithms" << std::endl;
  std::cout << " -a: algorithms to run, by default runs all" << std::endl;
  std::cout << "     Supported algorithms: " << alg_all  << std::endl;
  std::cout << " -p: papi counters to monitor (none, branch, cache, or throughput)" << std::endl;
  std::cout << " -a: alpha block size (q_accum)" << std::endl;
  std::cout << " -k: number of points to return" << std::endl;
  std::cout << " -v: verbose mode (don't use for performance experiments!)" << std::endl << std::endl;
  std::cout << "Example: " ;
  std::cout << "./ParallelTKDQ -k 5 -f ../workloads/house.csv -s \"partition naive\"" << std::endl << std::endl;
}

int main(int argc, char** argv) {
  bool verbose = false;
  Config cfg;
  string algorithms = alg_all;
  string num_threads = "4";
  std::string k = "5";
  cfg.input_fname = ""; // "../workloads/house-U-6-127931.csv";
  int index;
  int c;

  opterr = 0;

  while ( ( c = getopt( argc, argv, "f:t:k:a:v:" ) ) != -1 ) {
    switch ( c ) {
    case 'f':
      cfg.input_fname = string(optarg);
      break;
    case 'v':
      verbose = true;
      break;
    case 'k':
      k = string( optarg );
      break;
    case 'a':
      algorithms = string(optarg);
      break;
    case 't':
      num_threads = string(optarg);
      break;
    default:
      if ( isprint( optopt ) ) {
        fprintf( stderr, "Unknown option `-%c'.\n", optopt);
      }
      printUsage();
      return 1;
    }
  }

  if (argc == 1 || optind != argc || cfg.input_fname.empty()) {
    printUsage();
    return 1;
  }

  cfg.threads = my_split(num_threads, ' ');
  cfg.algo = my_split(algorithms, ' ');
  cfg.k = std::stoi(k);

  if (verbose) {
    printf("Running in verbose (-v) mode\n");
    doVerboseTest(cfg);
  } else {
    // Experiments for high performance
    doPerformanceTest(cfg);
  }

  return 0;
}
