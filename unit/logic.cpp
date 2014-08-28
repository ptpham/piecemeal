
#include "dag.h"
#include "gdl.h"
#include "logic.h"
#include "position_index.h"
#include "compile.h"

#include "stdfmt.h"

#include "ptest.h"

using namespace std;
using namespace piecemeal;
using namespace piecemeal::logic;

static set<prop<uint8_t,8>> run(const string& raw) {
  auto parsed = dag::loads_tree(raw);
  auto scope = compile::parse_sentences<uint8_t,8>(parsed);

  logic::position_index<uint8_t, 8> index;
  for (auto& rule : scope.rules) { index.emplace(rule); }
  for (auto& ground : scope.grounds) { index.emplace(ground); }

  askstate<uint8_t, 8> state;
  set<prop<uint8_t,8>> result;
  for (auto& rule : scope.rules) ask(index, rule.head.literal, state);
  for (auto& cache : state) for (auto& p : cache.second) result.insert(p);
  return result;
}

TEST(ask_prop_positive_backward_order) {
  ASSERT(run("(<= c b) (<= b a) (a)").size() == 3);
}

TEST(ask_prop_positive_forward_order) {
  ASSERT(run("(a) (<= b a) (<= c b)").size() == 3);
}

TEST(ask_prop_negative) {
  auto result = run("(<= c b) (<= b (not a))");
  ASSERT(result.size() == 2);
}

TEST(ask_prop_distinct) {
  ASSERT(false);
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

TEST(ask_var_distinct_failure) {
  ASSERT(run("(p a) (<= (q ?x) (p ?x) (p ?y) \
    (distinct ?x ?y))").size() == 1);
}

TEST(ask_var_distinct_success) {
  ASSERT(run("(p a) (p b) (<= (q ?x) (p ?x) (p ?y) \
    (distinct ?x ?y))").size() == 4);
}

TEST(ask_var_prop_distinct) {
  ASSERT(false);
}

TEST(ask_var_chain_positive) {
  ASSERT(run("(p a) (<= (q ?x) (p ?x)) (<= (r ?y) (q ?y))").size() == 3);
}

TEST(ask_var_prop_chain) {
  ASSERT(false);
}

TEST(end_to_end) {
  string raw_string = "(<= (z ?x) (q ?x)) (<= (q ?x) (p ?x))(r bloop) (<= (p ?y) (r ?y)) \
    (<= (p awesome) (or (r blip) (r blim) (r bloop))) (<= (z (f ?x)) (t ?x))";
  auto parsed = dag::loads_tree(raw_string);
  parsed = gdl::deor_sentences(parsed);
  gdl::canonize_sentences(*parsed);
  auto scope = compile::parse_sentences<uint8_t, 8>(parsed);
  logic::position_index<uint8_t, 8> index;

  for (auto& rule : scope.rules) { index.emplace(rule); }
  for (auto& ground : scope.grounds) { index.emplace(ground); }

  askstate<uint8_t, 8> state;
  auto proved = ask(index, scope.rules[0].head.literal, state);

  size_t total = 0; 
  for (auto& cache : state) { total += cache.second.size(); }
  ASSERT(total == 9);
}

