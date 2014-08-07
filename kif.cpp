#include "kif.h"

namespace piecemeal {
  namespace kif {
    template <class T, size_t N>
    array<T,N> extract_literal(unordered_dimap<string>& tokens,
      const vector<dag::cnode<string>>& leaves) {
      auto result = stdaux::filled_array<T,N>(0);
      for (size_t i = 0; i < leaves.size(); i++) {
        if (!is_var(leaves[i])) result[i] = tokens.at(leaves[i]->value);
        else result[i] = logic::empty<T,N>();
      };
      return result;
    }

    template <class T, size_t N>
    array<T,N> extract_push(const unordered_map<string,size_t>& vars,
      const vector<dag::cnode<string>>& leaves) {
      auto result = logic::empty_array<T,N>();
      for (size_t i = 0; i < leaves.size(); i++) {
        auto found = vars.find(leaves[i]->value);
        if (found == vars.end()) continue;
        else result[i] = found->second;
      }
      
      return result;
    }

    template <class T, size_t N>
    term<T,N> parse_term(unordered_dimap<string>& tokens,
      unordered_dimap<string>& vars, dag::cnode<string> node) {
      auto leaves = dag::gather::leaves(node);
      auto head = extract_literal<T,N>(tokens, leaves);
      auto push = extract_push<T,N>(vars.forward, leaves);
      return term<T,N>(head, push, logic::invert(push));
    }

    template <class T, size_t N>
    array<T,N> parse_distinct(unordered_dimap<string>& vars,
      dag::cnode<string> node) {
      array<T,N> result;
      fill(result.begin(), result.end(), logic::empty<T,N>());

      auto first = node->at(1);
      auto second = node->at(2);
      result[vars.at(first->value)] = vars.at(second->value);
      return result;
    }

    template <class T, size_t N>
    void parse_sentence(scope<T,N>& scope, const dag::cnode<string>& child) {
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
        scope.grounds.emplace(extract_literal<T,N>(scope.tokens, leaves));
        return;
      }

      // Extract head and various body terms for full rules
      rule.head = parse_term<T,N>(scope.tokens, vars, child->at(1));
      for (size_t i = 2; i < child->size(); i++) {
        auto term = child->at(i);

        // Handle distinct terms
        if (term->size() > 1 && term->at(0)->value == "distinct") {
          rule.distincts.push_back(parse_distinct<T,N>(vars, term));
          continue;
        }

        // Handle positive and negative terms
        auto& dst = rule.positives;
        if (term->size() > 1 && term->at(0)->value == "not") {
          dst = rule.negatives;
          term = term->at(1);
        }
        dst.push_back(parse_term<T,N>(scope.tokens, vars, term));
      }

      scope.rules.push_back(rule);
    }

    template <class T, size_t N>
    scope<T,N> parse_sentences(dag::cnode<string> root) {
      scope<T,N> result;
      for (auto& child : *root) parse_sentence(result, child);
      return result;
    }

    template scope<uint8_t,8> parse_sentences(dag::cnode<string>);
  }
}




