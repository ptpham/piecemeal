
#include <iostream>
#include <map>

#include "logic.h"
#include "dag.h"

using namespace std;
using namespace piecemeal;
using namespace piecemeal::logic;

template <class T, size_t N>
ostream& operator << (ostream& out, array<T,N>& a) {
  for (int v : a) cout << " " << v;
  return out;
}

template <class T, size_t N>
ostream& operator << (ostream& out, term<T,N>& t) {
  return out << t.literal << " <- " << t.push << " -> " << t.pull;
}

template <class U, class V = size_t>
struct unordered_dimap {
  vector<U> backward;
  unordered_map<U,V> forward;

  V at(const U& s) {
    auto found = forward.find(s);
    if (found != forward.end()) return found->second;
    V result = backward.size();
    forward.insert(found, {s, result});
    backward.push_back(s);
    return result;
  }
};

namespace kif {
  using namespace piecemeal;
  
  template <class T, size_t N>
  struct scope {
    unordered_dimap<string> tokens;
    unordered_set<array<T,N>> known;
    vector<rule<T,N>> rules;
  };

  inline static bool is_var(dag::cnode<string> node) {
    return node->value.size() > 0 && node->value[0] == '?';
  }

  template <class T, size_t N>
  auto extract_literal(unordered_dimap<string>& tokens,
    const vector<dag::cnode<string>>& leaves) {
    auto result = logic::empty_array<T,N>();
    for (size_t i = 0; i < leaves.size(); i++) {
      if (!is_var(leaves[i])) result[i] = tokens.at(leaves[i]->value);
    };
    return result;
  }

  template <class T, size_t N>
  auto extract_push(const unordered_map<string,size_t>& vars,
    const vector<dag::cnode<string>>& leaves) {
    auto result = logic::empty_array<T,N>();
    for (size_t i = 0; i < leaves.size(); i++) {
      auto found = vars.find(leaves[i]->value);
      if (found == vars.end()) continue;
      result[i] = found->second;
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
    fill(result.begin(), result.end(), logic::empty<T,N>());

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
      rule<T,N> rule;

      // Find and index all variables for a consistent ordering
      unordered_dimap<string> vars;
      dag::traverse::depth(child, [&](dag::node<string> node) {
        if (is_var(node)) vars.at(node->value);
      });

      // Relations and propositions will not start with the leading '<='
      if (child->size() == 1 || child->at(0)->value != "<=") {
        dag::cnode<string> const_child = child;
        auto leaves = dag::gather::leaves(const_child);
        result.known.emplace(extract_literal<T,N>(result.tokens, leaves));
        continue;
      }

      // Extract head and various body terms for full rules
      rule.head = extract_term<T,N>(result.tokens, vars, child->at(1));
      for (size_t i = 2; i < child->size(); i++) {
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

  string raw_string = "(<= (q ?x) (p ?x)) (<= (p ?y) (r ?y)) (r bloop)";
  auto parsed = dag::loads(raw_string);
  cout << dag::dumps(parsed) << endl;
  auto scope = kif::parse<uint8_t, 16>(parsed);

  for (auto rule : scope.rules) {
    cout << rule.head << endl;
    for (auto term : rule.positives) cout << "\t" << term << endl;
  }

  askstate<uint8_t, 16> state;
  state.known = scope.known;
  
  auto ruleset = logic::index_by_position(scope.rules);
  cout << "Ruleset size: " << ruleset.size() << endl;
  auto proved = ask(ruleset,scope.rules[0].head.literal, state);
  cout << "Proved " << state.known.size() << " proposition(s)." << endl;
  return 0;
}

