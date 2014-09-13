#include "piecemeal/compile.hpp"
#include "private/logic_export.hpp"

namespace piecemeal {
  namespace compile {
    template <class T, size_t N>
    prop<T,N> extract_literal(unordered_dimap<string>& tokens,
      const vector<dag::cnode<string>>& leaves) {
      prop<T,N> result(0);
      for (size_t i = 0; i < leaves.size(); i++) {
        if (!is_var(leaves[i])) result[i] = tokens.at(leaves[i]->value);
        else result[i] = prop<T,N>::empty();
      };
      return result;
    }

    template <class T, size_t N>
    prop<T,N> extract_push(unordered_dimap<string>& vars,
      const vector<dag::cnode<string>>& leaves) {
      prop<T,N> result;
      for (size_t i = 0; i < leaves.size(); i++) {
        if (!is_var(leaves[i])) continue;
        result[i] = vars.at(leaves[i]->value);
      }
      return result;
    }

    template <class T, size_t N>
    term<T,N> parse_term(unordered_dimap<string>& tokens,
      unordered_dimap<string>& vars, dag::cnode<string> node) {
      auto leaves = dag::gather::leaves(node);
      auto head = extract_literal<T,N>(tokens, leaves);
      auto push = extract_push<T,N>(vars, leaves);
      return term<T,N>(head, push, logic::invert(push));
    }

    pair<bool,bool> distinct_var_mask(dag::cnode<string> node) {
      auto first = node->at(1);
      auto second = node->at(2);
      return make_pair(is_var(first), is_var(second));
    }

    template <class T, size_t N>
    void parse_distinct(unordered_dimap<string>& tokens,
      unordered_dimap<string>& vars, rule<T,N>& rule,
      dag::cnode<string> term) {
      
      auto mask = distinct_var_mask(term);
      if (!mask.first && !mask.second) return;
      auto left = term->at(1), right = term->at(2);
      auto* left_map = &vars, *right_map = &vars;

      auto* target = &rule.distincts;
      if (mask.first ^ mask.second) {
        target = &rule.prevents;
        if (!mask.first) swap(left, right);
        right_map = &tokens;
      }
      
      T first = left_map->at(left->value);
      T second = right_map->at(right->value);
      target->push_back({first,second});
    }

    template <class T, size_t N>
    void parse_sentence(parse<T,N>& parse, const dag::cnode<string>& child) {
      if (child->size() == 0) return;
      rule<T,N> rule;

      // Find and index all variables for a consistent ordering
      unordered_dimap<string> vars;
      dag::traverse::depth(child, [&](dag::cnode<string> node) {
        if (is_var(node)) vars.at(node->value);
      });

      // Relations and propositions will not start with the leading '<='
      if (child->size() == 1 || child->at(0)->value != "<=") {
        dag::cnode<string> const_child = child;
        auto leaves = dag::gather::leaves(const_child);
        parse.props.emplace(extract_literal<T,N>(parse.tokens, leaves));
        return;
      }

      // Extract head and various body terms for full rules
      rule.head = parse_term<T,N>(parse.tokens, vars, child->at(1));
      for (size_t i = 2; i < child->size(); i++) {
        auto term = child->at(i);

        // Handle distinct terms
        if (term->size() > 1 && term->at(0)->value == "distinct") {
          parse_distinct(parse.tokens, vars, rule, term);
          continue;
        }

        // Handle positive and negative terms
        auto* dst = &rule.positives;
        if (term->size() > 1 && term->at(0)->value == "not") {
          dst = &rule.negatives;
          term = term->at(1);
        }
        dst->push_back(parse_term<T,N>(parse.tokens, vars, term));
      }

      parse.rules.push_back(rule);
    }

    template <class T, size_t N>
    parse<T,N> parse_sentences(dag::cnode<string> root) {
      parse<T,N> result;
      for (auto& child : *root) parse_sentence(result, child);
      return result;
    }

#define EXPORT(T,N) \
    template parse<T,N> parse_sentences(dag::cnode<string>);
    DEFAULT_LOGIC_EXPORT
#undef EXPORT
  }
}



