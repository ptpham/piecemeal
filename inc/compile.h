
#pragma once

#include <unordered_set>

#include "stdaux.h"
#include "rule.h"
#include "dag.h"

namespace piecemeal {
  namespace compile {

    using namespace std;
    using namespace logic;
    using namespace stdaux;
        
    template <class T, size_t N>
    struct scope {
      unordered_dimap<string> tokens;
      unordered_set<prop<T,N>> grounds;
      vector<rule<T,N>> rules;

      scope() { tokens.at(""); }
    };

    inline static bool is_var(dag::cnode<string> node) {
      return node->value.size() > 0 && node->value[0] == '?';
    }

    template <class T, size_t N>
    prop<T,N> extract_literal(unordered_dimap<string>& tokens,
      const vector<dag::cnode<string>>& leaves);

    template <class T, size_t N>
    prop<T,N> extract_push(const unordered_map<string,size_t>& vars,
      const vector<dag::cnode<string>>& leaves);

    template <class T, size_t N>
    term<T,N> parse_term(unordered_dimap<string>& tokens,
      unordered_dimap<string>& vars, dag::cnode<string> node);

    template <class T, size_t N>
    prop<T,N> parse_distinct(unordered_dimap<string>& vars,
      dag::cnode<string> node);

    template <class T, size_t N>
    void parse_sentence(scope<T,N>& scope, dag::cnode<string>& sentence);

    template <class T, size_t N>
    scope<T,N> parse_sentences(dag::cnode<string> sentences);
  }
}





