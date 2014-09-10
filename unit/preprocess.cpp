
#include <iostream>

#include "ptest.hpp"

#include "preprocess.hpp"

using namespace piecemeal;
using namespace std;

TEST(canonize_sentences_negation) {
  auto sentences = *dag::loads_tree("((r c) (not(r  (a   b))))");
  preprocess::canonize_sentences(sentences);
  ASSERT(dag::dumps_tree(sentences[0]->at(0)) == "(r ( c))");
  ASSERT(dag::dumps_tree(sentences[0]->at(1)) == "(not (r (a b)))");
}


