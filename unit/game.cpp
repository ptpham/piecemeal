
#include "ptest.hpp"
#include "piecemeal/game.hpp"

using namespace piecemeal;
using namespace stdaux;
using namespace game;

TEST(fixture_constants_match) {
  unordered_dimap<string> tokens;
  auto fixture = game::build_fixture<uint8_t,8>(tokens);

  for (size_t i = 0; i < KEYWORD_COUNT; i++) {
    auto& name = keyword_names[i];
    ASSERT(fixture.ids[i] == tokens.at(name));
    ASSERT(fixture.queries[i][0] == tokens.at(name));
  }
}


