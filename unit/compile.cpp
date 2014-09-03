
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
  string raw = "(heyo (bloop blarg))";
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
  string raw = "(heyo (?x ?y))";
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

TEST(parse_rule_positive) {
  ASSERT(false);
}

TEST(parse_rule_negative) {
  ASSERT(false);
}

TEST(parse_rule_distinct) {
  ASSERT(false);
}

TEST(parse_rule_prevent) {
  ASSERT(false);
}

TEST(parse_rule_prevent_flipped) {
  ASSERT(false);
}

