
#include "piecemeal/dag.hpp"
#include "piecemeal/logic.hpp"
#include "piecemeal/preprocess.hpp"
#include "piecemeal/position_index.hpp"
#include "piecemeal/compile.hpp"

#include "piecemeal/stdfmt.hpp"

#include "ptest.hpp"

using namespace std;
using namespace piecemeal;
using namespace piecemeal::logic;

static set<prop<uint8_t,8>> run(const string& raw) {
  auto parsed = tree::loads(raw);
  auto scope = compile::parse_sentences<uint8_t,8>(parsed);
  logic::position_index<uint8_t, 8> index;
  index.emplace_rules(scope.rules);
  index.emplace_props(scope.props);

  askstate<uint8_t, 8> state;
  set<prop<uint8_t,8>> result;
  for (auto& prop : scope.props) ask(index, prop, state);
  for (auto& rule : scope.rules) ask(index, rule.head.literal, state);
  for (auto& cache : state) for (auto& p : cache.second.props) result.insert(p);
  return result;
}

TEST(ask_prop_positive_single) {
  ASSERT(run("(p a b)").size() == 1);
}

TEST(ask_prop_positive_backward_order) {
  ASSERT(run("(<= c b) (<= b a) (a)").size() == 3);
}

TEST(ask_prop_positive_forward_order) {
  ASSERT(run("(a) (<= b a) (<= c b)").size() == 3);
}

TEST(ask_prop_negative) {
  ASSERT(run("(<= c b) (<= b (not a))").size() == 2);
}

TEST(ask_prop_distinct_success) {
  ASSERT(run("(<= a (distinct b c))").size() == 1);
}

TEST(ask_var_positive) {
  ASSERT(run("(p a) (<= (q ?x) (p ?x))").size() == 2);
}

TEST(ask_var_missing) {
  ASSERT(run("(p a) (<= (q ?a) (p ?x))").size() == 1);
}

TEST(ask_var_negative_ungrounded) {
  ASSERT(run("(<= (q ?x) (not (p ?x)))").size() == 0);
}

TEST(ask_var_negative_grounded) {
  ASSERT(run("(p a) (<= (q ?x) (p ?x) (not (r ?x)))").size() == 2);
}

TEST(ask_var_distinct_success) {
  ASSERT(run("(p a) (p b) (<= (q ?x) (p ?x) (p ?y) \
    (distinct ?x ?y))").size() == 4);
}

TEST(ask_var_distinct_failure) {
  ASSERT(run("(p a) (<= (q ?x) (p ?x) (p ?y) (distinct ?x ?y))").size() == 1);
}

TEST(ask_var_prop_distinct_success) {
  ASSERT(run("(p a) (<= (q ?x) (p ?x) (distinct ?x b))").size() == 2);
}

TEST(ask_var_chain_positive) {
  ASSERT(run("(p a) (<= (q ?x) (p ?x)) (<= (r ?y) (q ?y))").size() == 3);
}

TEST(ask_var_prop_chain) {
  ASSERT(run("(p a) (<= (q ?x) (p ?x)) (<= r (q ?x))").size() == 3);
}

TEST(ask_var_recursive_grounded) {
  ASSERT(run("(p a b) (q a) (<= (q ?y) (q ?x) (p ?x ?y))").size() == 3);
}

TEST(ask_var_recursive_ungrounded) {
  ASSERT(run("(p a b) (<= (q ?y) (q ?x) (p ?x ?y))").size() == 1);
}

TEST(ask_var_structure_varied) {
  ASSERT(run("(p a) (p (a b)) (<= (q ?x) (p ?x))").size() == 4);
}

