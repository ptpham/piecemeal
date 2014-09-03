
#include "dag.h"
#include "compile.h"
#include "stdfmt.h"

#include "ptest.h"

using namespace std;
using namespace piecemeal;
using namespace piecemeal::logic;
using namespace piecemeal::compile;

static rule<uint8_t,8> run_rule(const string& raw) {
  auto parsed = dag::loads_tree(raw);
  auto scope = compile::parse_sentences<uint8_t,8>(parsed);
  return scope.rules[0];
}

TEST(parse_term_no_vars) {
  string raw = "(a (b c))";
  unordered_dimap<string> tokens, vars;
  dag::cnode<string> node = dag::loads_tree(raw);
  auto result = parse_term<uint8_t,8>(tokens, vars, node);

  ASSERT(vars.backward.size() == 0);
  ASSERT(vars.forward.size() == 0);
  ASSERT(tokens.backward.size() == 3);
  ASSERT(tokens.forward.size() == 3);
  ASSERT(result.push.is_blank());
  ASSERT(result.pull.is_blank());
  ASSERT(result.literal.is_grounded());
}

TEST(parse_term_vars) {
  string raw = "(f (?x ?y))";
  unordered_dimap<string> tokens, vars;
  dag::cnode<string> node = dag::loads_tree(raw);
  auto result = parse_term<uint8_t,8>(tokens, vars, node);

  ASSERT(vars.backward.size() == 2);
  ASSERT(vars.forward.size() == 2);
  ASSERT(tokens.backward.size() == 1);
  ASSERT(tokens.forward.size() == 1);
  ASSERT(!result.push.is_blank());
  ASSERT(!result.pull.is_blank());
  ASSERT(!result.literal.is_grounded());
}

TEST(parse_sentence_proposition) {
  auto raw = "((a) (b))";
  auto parsed = dag::loads_tree(raw);
  auto scope = compile::parse_sentences<uint8_t,8>(parsed);
  ASSERT(scope.rules.size() == 0);
  ASSERT(scope.props.size() == 1);
}

TEST(parse_sentence_positive) {
  auto rule = run_rule("(<= (a) (b))");
  ASSERT(rule.head.literal.is_grounded());
  ASSERT(rule.positives.size() == 1);
  ASSERT(rule.negatives.size() == 0);
  ASSERT(rule.distincts.size() == 0);
  ASSERT(rule.prevents.size() == 0);
}

TEST(parse_sentence_negative) {
  auto rule = run_rule("(<= (a) (b) (not c))");
  ASSERT(rule.head.literal.is_grounded());
  ASSERT(rule.positives.size() == 1);
  ASSERT(rule.negatives.size() == 1);
  ASSERT(rule.distincts.size() == 0);
  ASSERT(rule.prevents.size() == 0);
}

TEST(parse_sentence_distinct) {
  auto rule = run_rule("(<= (a) (?x) (?y) (distinct ?x ?y))");
  ASSERT(rule.head.literal.is_grounded());
  ASSERT(rule.positives.size() == 2);
  ASSERT(rule.negatives.size() == 0);
  ASSERT(rule.distincts.size() == 1);
  ASSERT(rule.prevents.size() == 0);
}

TEST(parse_sentence_prevent) {
  auto rule = run_rule("(<= (a) (?x) (distinct ?x a))");
  ASSERT(rule.head.literal.is_grounded());
  ASSERT(rule.positives.size() == 1);
  ASSERT(rule.negatives.size() == 0);
  ASSERT(rule.distincts.size() == 0);
  ASSERT(rule.prevents.size() == 1);
  ASSERT(rule.prevents[0].first == rule.positives[0].push[0]);
}

TEST(parse_sentence_prevent_flipped) {
  auto rule = run_rule("(<= (a) (?x) (distinct a ?x))");
  ASSERT(rule.positives.size() == 1);
  ASSERT(rule.distincts.size() == 0);
  ASSERT(rule.prevents.size() == 1);
  ASSERT(rule.prevents[0].first == rule.positives[0].push[0]);
}

