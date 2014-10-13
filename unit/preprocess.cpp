
#include <iostream>

#include "ptest.hpp"

#include "piecemeal/preprocess.hpp"

using namespace piecemeal;
using namespace std;

TEST(canonize_sentences_negation) {
  auto sentences = *tree::loads("(r c) (not(r  (a   b))) (not (r d))");
  preprocess::canonize_sentences(sentences);
  ASSERT(tree::dumps(sentences[0]) == "(r (c ))");
  ASSERT(tree::dumps(sentences[1]) == "(not (r (a b)))");
  ASSERT(tree::dumps(sentences[2]) == "(not (r (d )))");
}


