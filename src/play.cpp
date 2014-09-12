
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

typedef uint16_t ltype;
constexpr size_t pwidth = 8;

template <class T, size_t N>
string human_readable(const vector<string>& backward, const prop<T,N>& p) {
  string result;
  for (size_t i = 0; i < N; i++) result += backward[p[i]] + " ";
  return result;
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Usage: ./cps <path to game> <role>" << endl;
    return 1;
  }

  std::string line, raw;
  std::ifstream file(argv[1]);
  while (std::getline(file, line)) { raw += line; }
  string hrole = argv[2];

  game::simulator<ltype,pwidth> sim(raw);
  auto index = sim.create_index<position_index>();
  auto role_map = sim.ask_role_map(index);
  auto initial = sim.ask_convert<TRUE, INIT>(index);

  auto& parse = sim.context.parse;
  auto role_found = parse.tokens.forward.find(hrole);
  if (role_found == parse.tokens.forward.end()) {
    cout << "Role '" << hrole << "' invalid." << endl;
    return 1;
  }
  ltype hrole_index = role_found->second;

  auto state = initial;
  auto& backward = parse.tokens.backward;
  sim.bind_state(index, state);
  do {
    auto moves = sim.ask_convert<DOES, LEGAL>(index);
    auto chosen = random_joint_move(moves, role_map);

    cout << "Select move:" << endl;
    for (size_t i = 0; i < moves.size(); i++) {
      auto& move = moves[i];
      if (move[1] != hrole_index) continue;
      cout << i << ") " << human_readable(backward, move) << endl;
    }
    size_t selection;
    cin >> selection;
    chosen[role_map[hrole_index]] = moves[selection];

    cout << "Playing: ";
    for (auto& move : chosen) {
      cout << human_readable(backward, move) << " ";
    }
    cout << endl;

    index.emplace_props(chosen);
    state = sim.ask_convert<TRUE, NEXT>(index);
    sim.bind_state(index, state);
  } while (sim.ask<TERMINAL>(index).size() == 0);
  for (auto& goal : sim.ask<GOAL>(index)) cout << human_readable(backward, goal) << endl;

  return 0;
}

