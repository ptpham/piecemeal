
#include <iostream>
#include <map>

#include "logic.h"
#include "dag.h"

using namespace std;
using namespace piecemeal;
using namespace piecemeal::logic;

template <class U, class V = size_t>
struct unordered_dimap {
  vector<U> backward;
  unordered_map<U,V> forward;

  V at(const U& s) {
    auto found = forward.find(s);
    if (found != forward.end()) return found->second;
    V result = backward.size();
    forward.insert(found, {s, result});
    return result;
  }
};

namespace kif {
  using namespace piecemeal;
  
  template <class T, size_t N>
  struct scope {
    unordered_dimap<string> tokens;
    vector<rule<T,N>> rules;
  };

  inline static bool is_var(dag::cnode<string> node) {
    return node->value.size() > 0 && node->value[0] == '?';
  }

  template <class T, size_t N>
  auto extract_literal(unordered_dimap<string>& tokens,
    const vector<dag::cnode<string>>& leaves) {
    auto id = tokens.at(leaves[0]->value);
    auto result = isa<T,N>(id, leaves.size() - 1).shade();
    for (size_t i = 1; i < leaves.size(); i++) {
      if (!is_var(leaves[i])) result[i - 1] = tokens.at(leaves[i]->value);
    };
    return result;
  }

  template <class T, size_t N>
  auto extract_push(const unordered_map<string,size_t>& vars,
    const vector<dag::cnode<string>>& leaves) {
    auto result = isa<T,N>::null(leaves.size()-1).shade();
    for (size_t i = 1; i < leaves.size(); i++) {
      auto found = vars.find(leaves[i]->value);
      if (found == vars.end()) continue;
      result[i-1] = found->second;
    }
    return result;
  }

  template <class T, size_t N>
  auto extract_term(unordered_dimap<string>& tokens,
    unordered_dimap<string>& vars, dag::cnode<string> node) {
    auto leaves = dag::gather::leaves(node);
    auto head = extract_literal<T,N>(tokens, leaves);
    auto push = extract_push<T,N>(vars.forward, leaves);
    return term<T,N>(head, push, logic::invert(push));
  }

  template <class T, size_t N>
  auto extract_distinct(unordered_dimap<string>& vars,
    dag::cnode<string> node) {
    array<T,N> result;
    fill(result.begin(), result.end(), isa<T,N>::empty());

    auto first = node->at(1);
    auto second = node->at(2);
    result[vars.at(first->value)] = vars.at(second->value);
    return result;
  }

  template <class T, size_t N>
  auto parse(dag::cnode<string> root) {
    scope<T,N> result;
    for (auto& child : *root) {
      if (child->size() == 0) continue;

      // Find and index all variables for a consistent ordering
      unordered_dimap<string> vars;
      dag::traverse::depth(child, [&](dag::node<string> node) {
        if (is_var(node)) vars.at(node->value);
      });

      // Relations and propositions will not start with the leading '<='
      bool full_rule = child->size() > 1 && child->at(0)->value == "<=";
      auto head_node = full_rule ? child->at(1) : child;
      rule<T,N> rule;

      // Extract head and various body terms
      rule.head = extract_term<T,N>(result.tokens, vars, head_node);
      for (size_t i = 1; i < child->size(); i++) {
        auto term = child->at(i);

        // Handle distinct terms
        if (term->size() > 1 && term->at(0)->value == "distinct") {
          rule.distincts.push_back(extract_distinct<T,N>(vars, term));
          continue;
        }

        // Handle positive and negative terms
        auto& dst = rule.positives;
        if (term->size() > 1 && term->at(0)->value == "not") {
          dst = rule.negatives;
          term = term->at(1);
        }
        dst.push_back(extract_term<T,N>(result.tokens, vars, term));
      }

      result.rules.push_back(rule);
    }
    return result;
  }
}

int main(int nargs, char** argv) {

  string raw_string = "(hello) ( \t <= blarg) \n ( <= (blarg) (bloop ?x ?y) )";
  //((hello) (<= blarg) (<= (blarg) (bloop ?x ?y)))
  auto parsed = dag::loads(raw_string);
  cout << dag::dumps(parsed);
  kif::parse<uint16_t, 16>(parsed);
  cout << endl;

  isa<uint8_t,16> q0 = {0, {0}};
  decltype(q0) q1 = {1, {0}};
  decltype(q0) q2 = {2, {0}};

  rule<uint8_t, 16> r0 = { q0, { q1 } };
  decltype(r0) r1 = { q1, { q2 } };
  decltype(r0) r2 = { q2 };

  askstate<uint8_t, 16> state;
  vector<vector<decltype(r0)>> ruleset = { {r0}, {r1}, {r2} };
  auto proved = ask(ruleset, q0, state);
  cout << "Proved " << state.index.size() << " proposition(s)." << endl;
  return 0;
}

