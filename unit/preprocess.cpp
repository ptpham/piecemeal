
#include <iostream>

#include "ptest.hpp"

#include "piecemeal/preprocess.hpp"

using namespace piecemeal;
using namespace std;

TEST(canonize_sentences_negation) {
  auto sentences = *tree::loads("((r c) (not(r  (a   b))))");
  preprocess::canonize_sentences(sentences);
  ASSERT(tree::dumps(sentences[0]->at(0)) == "(r ( c))");
  ASSERT(tree::dumps(sentences[0]->at(1)) == "(not (r (a b)))");
}


