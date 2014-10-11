#pragma once

#include <vector>

#include "piecemeal/dag.hpp"

namespace piecemeal {
  namespace preprocess {
    using namespace std;

    vector<string> remove_comments(const vector<string>& raw);

    vector<dag::node<string>> deor_sentence(dag::node<string> original);
    dag::node<string> deor_sentences(const dag::node<string> sentences);
    dag::node<string> standard(const vector<string>& raw);
  }
}


