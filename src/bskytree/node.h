#pragma once

#include <stdint.h>

#include <vector>
#include <stack>

#include "common/common2.h"

template<int DIMS>
struct Node {
  uint32_t lattice;
  Tuple<DIMS> point;
  std::vector<Node<DIMS> > children;

  Node(void) {
    lattice = 0;
  }
  Node(uint32_t _lattice) :
      lattice(_lattice) {
  }
};

template<int DIMS>
void ClearSkyTree(Node<DIMS>& skytree) {
  std::stack<Node<DIMS> > tree_stack;
  PushStack(tree_stack, skytree);

  while (!tree_stack.empty()) {
    tree_stack.top().children.clear();
    tree_stack.pop();
  }
}

template<int DIMS>
void PushStack(std::stack<Node<DIMS> >& tree_stack, Node<DIMS>& skytree) {
  if (skytree.children.size() > 0) {
    tree_stack.push(skytree);

    const uint32_t num_child = skytree.children.size();
    for (unsigned i = 0; i < num_child; i++)
      PushStack(tree_stack, skytree.children[i]);
  }
}
