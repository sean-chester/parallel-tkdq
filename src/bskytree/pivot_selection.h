#pragma once

#include <limits.h>
#include <math.h>

#include <vector>

#include "common/common2.h"

using namespace std;

template<int DIMS>
class PivotSelection {
public:
	PivotSelection(const bool do_pruning, const int pivot_mode,
	    const vector<float> &min_list, const vector<float> &max_list) :
	    pruning_on_( do_pruning ), selection_mode_( pivot_mode ), min_list_(
	        min_list ), max_list_( max_list ) {

	}
	~PivotSelection(void) {

	}

	void Execute( vector<Tuple<DIMS> >& dataset );

private:
	void ExecuteMaxDom(vector<Tuple<DIMS> >& dataset);
	void ExecuteRandom(vector<Tuple<DIMS> >& dataset);
	void ExecuteBalanced(vector<Tuple<DIMS> >& dataset);

	vector<float> SetRangeList(const vector<float>& min_list,
			const vector<float>& max_list);
	float ComputeDistance(const float* value, const vector<float>& min_list,
			const vector<float>& range_list);
	float ComputeManhattanSum( const float* value );
	float ComputeCosine(const float* value, const vector<float>& min_list,
			const vector<float>& range_list);

	bool EvaluatePoint(const unsigned pos, vector<Tuple<DIMS> >& dataset);

	const bool pruning_on_; // pruning on/off
	const int selection_mode_; // pivot selection mode
	const vector<float> &min_list_;
	const vector<float> &max_list_;
};

