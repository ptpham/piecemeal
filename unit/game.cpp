
#include "ptest.hpp"
#include "game.hpp"
#include "gdl.hpp"

using namespace piecemeal;
using namespace stdaux;
using namespace gdl;

TEST(fixture) {
  unordered_dimap<string> tokens;
  auto fixture = game::build_fixture<uint8_t,8>(tokens);

  ASSERT(fixture.role_id == tokens.at(ROLE));
  ASSERT(fixture.base_id == tokens.at(BASE));
  ASSERT(fixture.input_id == tokens.at(INPUT));
  ASSERT(fixture.init_id == tokens.at(INIT));
  ASSERT(fixture.goal_id == tokens.at(GOAL));
  ASSERT(fixture.terminal_id == tokens.at(TERMINAL));
  ASSERT(fixture.true_id == tokens.at(TRUE));
  ASSERT(fixture.next_id == tokens.at(NEXT));
  ASSERT(fixture.legal_id == tokens.at(LEGAL));
  ASSERT(fixture.does_id == tokens.at(DOES));

  ASSERT(fixture.role_query[0] == tokens.at(ROLE));
  ASSERT(fixture.base_query[0] == tokens.at(BASE));
  ASSERT(fixture.input_query[0] == tokens.at(INPUT));
  ASSERT(fixture.init_query[0] == tokens.at(INIT));
  ASSERT(fixture.goal_query[0] == tokens.at(GOAL));
  ASSERT(fixture.terminal_query[0] == tokens.at(TERMINAL));
  ASSERT(fixture.true_query[0] == tokens.at(TRUE));
  ASSERT(fixture.next_query[0] == tokens.at(NEXT));
  ASSERT(fixture.legal_query[0] == tokens.at(LEGAL));
  ASSERT(fixture.does_query[0] == tokens.at(DOES));
}


