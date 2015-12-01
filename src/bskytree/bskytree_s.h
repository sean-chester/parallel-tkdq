/*
 * bskytree_s.h
 *
 *  Created on: Jul 1, 2014
 *      Author: darius
 *
 *  Implementation of BSkyTree-S (following closely Algorithm 3 in [1]).
 *  Does not assume distinct value condition (employs equivalence matrix).
 *
 *  [1] Jongwuk Lee and Seung-won Hwang. "BSkyTree: Scalable Skyline
 *      Computation Using A Balanced Pivot Selection." In EDBT 2010.
 */

#ifndef BSKYTREES_H_
#define BSKYTREES_H_

#include <map>
#include <vector>

#include <common/skyline_i.h>
#include <bskytree/node.h>

using namespace std;

template<int DIMS> class BSkyTreeS: public SkylineI {
public:
  BSkyTreeS(const uint32_t n, const uint32_t d, float** dataset) :
      n_(n), d_(d) {

    skyline_.reserve(1024);
    eqm_.reserve(1024);
  }

  ~BSkyTreeS() {
    skyline_.clear();
    data_.clear();
    eqm_.clear();
  }

  void Init(float** dataset);
  vector<int> Execute(void);

private:
  void BSkyTreeS_ALGO();
  void DoPartioning();

  // PivotSelection methods
  void SelectBalanced();
  vector<float> SetRangeList(const vector<float>& min_list,
      const vector<float>& max_list);
  float ComputeDistance(const float* value, const vector<float>& min_list,
      const vector<float>& range_list);
  bool EvaluatePoint(const uint32_t pos);

  const uint32_t n_;
  const uint32_t d_;
  vector<Tuple_S<DIMS> > data_;

  vector<int> skyline_;
  vector<int> eqm_; // "equivalence matrix"
};

#endif /* BSKYTREES_H_ */
