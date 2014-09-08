
#include <iostream>

#include "ptest.hpp"

#include "dag.hpp"

using namespace piecemeal;
using namespace dag;

TEST(tree_serialization_multiple) {
  ASSERT(dumps_tree(loads_tree("(a (b c))(d)(e)")) == "((a (b c)) (d) (e))");
}

TEST(tree_serialization_whitespace_tight) {
  ASSERT(dumps_tree(loads_tree("(a(b c))")) == "((a (b c)))");
}

TEST(tree_serialization_whitespace_extra) {
  ASSERT(dumps_tree(loads_tree("  ( a   (   b    c ) )  ")) == "((a (b c)))");
}


