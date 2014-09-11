
#include <iostream>
#include <fstream>
#include <numeric>
#include <ctime>

#include "logic.hpp"
#include "compile.hpp"
#include "position_index.hpp"
#include "stdfmt.hpp"

#include "game.hpp"

using namespace std;
using namespace piecemeal;
using namespace logic;
using namespace game;

typedef uint8_t ltype;
constexpr size_t pwidth = 8;

template <class T>
T uniformly_at_random(T moves) {
  static vector<bool> moved(numeric_limits<ltype>::max());
  static vector<int> positions(numeric_limits<ltype>::max());
  static vector<int> seen(numeric_limits<ltype>::max());

  fill(moved.begin(), moved.end(), false);
  decltype(moves) result;
  for (auto move : moves) {
    auto player = move[1];
    if (!moved[player]) {
      positions[player] = result.size();
      result.resize(result.size() + 1);
      seen[player] = 1;
    } else if (rand() % seen[player] > 1) continue;

    moved[player] = true;
    result[positions[player]] = move;
    seen[player]++;
  }
  return result;
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Usage: ./cps <path to game> <iterations>" << endl;
    return 1;
  }

  std::string line, raw;
  std::ifstream file(argv[1]);
  size_t niterations = atoi(argv[2]);
  while (std::getline(file, line)) { raw += line; }

  game::simulator<ltype,pwidth> sim(raw);
  auto index = sim.create_index<position_index>();
  index.emplace_rules(sim.context.scope.rules);

  sim.bind_state(index);
  auto initial = sim.ask_convert<TRUE, INIT>(index);

  auto begin = clock();
  for (int i = 0; i < niterations; i++) {
    size_t turn = 0;
    auto state = initial;
    sim.bind_state(index, state);
    do {
      auto moves = sim.ask_convert<DOES, LEGAL>(index);
      auto chosen = uniformly_at_random(moves);
      index.emplace_props(chosen);
      state = sim.ask_convert<TRUE, NEXT>(index);
      sim.bind_state(index, state);
      turn++;
    } while (sim.ask<TERMINAL>(index).size() == 0);
  }
  auto seconds = (double)(clock() - begin)/CLOCKS_PER_SEC;
  cout << "Charges per second: " << (double)niterations/seconds << endl;;

  return 0;
}

