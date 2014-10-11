
#include <iostream>

#include "ptest.hpp"

#include "piecemeal/dag.hpp"

using namespace piecemeal;
using namespace tree;

TEST(tree_serialization_multiple) {
  ASSERT(tree::dumps(tree::loads("(a (b c))(d)(e)")) == "((a (b c)) (d) (e))");
}

TEST(tree_serialization_whitespace_tight) {
  ASSERT(tree::dumps(tree::loads("(a(b c))")) == "((a (b c)))");
}

TEST(tree_serialization_whitespace_extra) {
  ASSERT(tree::dumps(tree::loads("  ( a   (   b    c ) )  ")) == "((a (b c)))");
}


