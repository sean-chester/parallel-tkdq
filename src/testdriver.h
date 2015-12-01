/**
 * Auxiliary methods and constants for the main driver of
 * the ParallelTKDQ software suite.
 *
 * @date 12 Nov 2015
 * @author Sean Chester
 */

#ifndef TESTDRIVER_H_
#define TESTDRIVER_H_

#include <vector>
#include <string>

const std::string alg_naive = "naive";
const std::string alg_refinement = "refinement";
const std::string alg_partition = "partition";
const std::string alg_all = "naive refinement partition";

typedef struct Config {
  std::string input_fname;
  std::vector< std::string > algo;
  std::vector< std::string > threads;
  uint32_t k;
} Config;

#endif /* TESTDRIVER_H_ */
