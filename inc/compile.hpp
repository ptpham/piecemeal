
#pragma once

#include <unordered_set>

#include "unordered_dimap.hpp"
#include "rule.hpp"
#include "dag.hpp"

namespace piecemeal {
  namespace compile {

    using namespace std;
    using namespace logic;
    using namespace stdaux;
        
    template <class T, size_t N>
    struct parse {
      unordered_dimap<string> tokens;
      unordered_set<prop<T,N>> props;
      vector<rule<T,N>> rules;

      parse() { tokens.at(""); }
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
    void parse_sentence(parse<T,N>& parse, dag::cnode<string>& sentence);

    template <class T, size_t N>
    parse<T,N> parse_sentences(dag::cnode<string> sentences);
  }
}



