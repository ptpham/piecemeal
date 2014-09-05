#pragma once

#include <vector>

#include "dag.h"

namespace piecemeal {
  namespace gdl {
    using namespace std;

    extern const string ROLE, BASE, INPUT, INIT,
      GOAL, TERMINAL, TRUE, NEXT, LEGAL, DOES;

    vector<dag::node<string>> deor_sentence(dag::node<string> original);
    dag::node<string> deor_sentences(const dag::node<string> sentences);
    void union_structure(dag::node<string>& canon, dag::cnode<string> other);
    void canonize_term(dag::cnode<string> canon, dag::node<string>& other);
    void canonize_sentences(const vector<dag::node<string>>& sentences);

    dag::node<string> preprocess_standard(const string& raw);
  }
}


