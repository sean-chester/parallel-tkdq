#pragma once

//#include "bskytree/point.h"
#include "bskytree/node.h"
#include "bskytree/pivot_selection.h"
#include "common/skyline_i.h"
#include "common/common2.h"

#include <map>
#include <vector>

using namespace std;

template<int DIMS>
class SkyTree: public SkylineI {

public:
  SkyTree(const uint32_t n, const uint32_t d, float** dataset,
      const bool useTree, const bool useDnC, const uint32_t pivot_type) :
      n_(n), d_(d), useTree_(useTree), useDnC_(useDnC), pivot_type_(pivot_type) {

    skytree_.lattice = 0;
    skyline_.reserve(1024);
    eqm_.reserve(1024);
    dominated_ = NULL;
  }

  ~SkyTree(void) {
    min_list_.clear();
    max_list_.clear();
    skyline_.clear();
    ClearSkyTree(skytree_);
    data_.clear();
    if (useDnC_)
      delete[] dominated_;
  }

  void Init(float** dataset);
  vector<int> Execute(void);

private:
  void ComputeSkyTree(const vector<float> min_list,
      const vector<float> max_list, vector<Tuple<DIMS> > &dataset, Node<DIMS>& skytree);

  map<uint32_t, vector<Tuple<DIMS> > > MapPointToRegion(vector<Tuple<DIMS> >& dataset);

  void PartialDominance(const uint32_t lattice, vector<Tuple<DIMS> >& dataset,
      Node<DIMS>& skytree);
  bool PartialDominance_with_trees(const uint32_t lattice, Node<DIMS>& left_tree,
      Node<DIMS>& right_tree);
  bool FilterPoint(const Tuple<DIMS> &cur_value, Node<DIMS>& skytree);
  bool FilterPoint_without_skytree(const Tuple<DIMS> &cur_value, Node<DIMS>& skytree);
  void TraverseSkyTree(const Node<DIMS>& skytree);

#ifndef NVERBOSE
  int MaxDepth(const Node<DIMS>& skytree, int d);
#endif

  const uint32_t n_;
  const uint32_t d_;
  vector<Tuple<DIMS> > data_;

  vector<float> min_list_;
  vector<float> max_list_;

  Node<DIMS> skytree_;
  vector<int> skyline_;
  vector<int> eqm_; // "equivalence matrix"

  /* runtime params. */
  bool useTree_; //using SkyTree data structure in FilterPoints()
  bool useDnC_; //divide-and-conquer
  uint32_t pivot_type_; //for non-balanced pivots
  bool *dominated_; //for DnC variant

#ifndef NVERBOSE
  map<int, int> skytree_levels_;
#endif
};
