
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

