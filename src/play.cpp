
#include <iostream>
#include <fstream>

#include "logic.hpp"
#include "compile.hpp"
#include "position_index.hpp"
#include "stdfmt.hpp"

#include "game.hpp"

using namespace std;
using namespace piecemeal;
using namespace logic;
using namespace game;

static const string default_game = "(role x) (role o)"
    "(succ 0 1)(succ 1 2)(succ 2 3)(succ 3 4)(succ 4 5)(succ 5 6)(succ 6 7)"
    "(succ 7 8)(succ 8 9)(succ 9 10)"
    "(init (count 0))"
    "(<= (legal ?p dance) (role ?p))"
    "(<= (next (count ?y)) (does ?p dance) (true (count ?x)) (succ ?x ?y))"
    "(<= terminal (true (count 10)))"
    "(<= (goal ?p 100) (role ?p))";

int main(int argc, char* argv[]) {
  std::ifstream file("games/ttt.kif");
  std::string line, raw;
  while (std::getline(file, line)) { raw += line; }
  cout << "Rules: " << raw << endl;

  game::simulator<uint8_t,8> sim(raw);
  auto index = sim.create_index<position_index>();
  index.emplace_rules(sim.context.scope.rules);
  vector<bool> moved(256);

  sim.bind_state(index);
  auto initial = sim.ask_convert<TRUE, INIT>(index);

  for (int i = 0; i < 1; i++) {
    size_t turn = 0;
    auto state = initial;
    sim.bind_state(index, state);
    do {
      auto moves = sim.ask_convert<DOES, LEGAL>(index);
      fill(moved.begin(), moved.end(), false);
      decltype(moves) chosen;
      for (auto move : moves) {
        auto player = move[1];
        if (moved[player]) continue;
        moved[player] = true;
        chosen.push_back(move);
      }
      index.emplace_props(chosen);
      state = sim.ask_convert<TRUE, NEXT>(index);
      sim.bind_state(index, state);
      turn++;
    } while (sim.ask<TERMINAL>(index).size() == 0);
  }

  return 0;
}



