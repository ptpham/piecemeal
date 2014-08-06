
#pragma once

#include "logic.h"
#include "dag.h"

namespace piecemeal {
  namespace kif {

    using namespace std;
    using namespace logic;
    using namespace stdaux;
        
    template <class T, size_t N>
    struct scope {
      unordered_dimap<string> tokens;
      unordered_set<array<T,N>> grounds;
      vector<rule<T,N>> rules;

      scope() { tokens.at(""); }
    };

    inline static bool is_var(dag::cnode<string> node) {
      return node->value.size() > 0 && node->value[0] == '?';
    }

    template <class T, size_t N>
    array<T,N> extract_literal(unordered_dimap<string>& tokens,
      const vector<dag::cnode<string>>& leaves);

    template <class T, size_t N>
    array<T,N> extract_push(const unordered_map<string,size_t>& vars,
      const vector<dag::cnode<string>>& leaves);

    template <class T, size_t N>
    term<T,N> extract_term(unordered_dimap<string>& tokens,
      unordered_dimap<string>& vars, dag::cnode<string> node);

    template <class T, size_t N>
    array<T,N> extract_distinct(unordered_dimap<string>& vars,
      dag::cnode<string> node);

    template <class T, size_t N>
    scope<T,N> parse(dag::cnode<string> root);
  }
}





