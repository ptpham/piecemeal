
#include <iostream>

#include "ptest.hpp"

#include "preprocess.hpp"

using namespace piecemeal;
using namespace std;

TEST(canonize_sentences_negation) {
  auto sentences = dag::loads_tree("(r c) (not(r  (a   b)))");
  preprocess::canonize_sentences(*sentences);
  cout << dag::dumps_tree(sentences->at(0)) << endl;
  ASSERT(dag::dumps_tree(sentences->at(0)) == "(r (a b))");
}

