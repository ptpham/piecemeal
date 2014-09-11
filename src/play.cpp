
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
vector<prop<T,N>> random_joint_move(const vector<prop<T,N>>& moves, const map<T,size_t>& role_map) {
  size_t nroles = role_map.size();
  vector<prop<T,N>> result(nroles);
  size_t seen[nroles];

  fill(seen, seen + nroles, 0);
  for (auto move : moves) {
    auto found = role_map.find(move[1]);
    if (found == role_map.end()) continue;
    auto index = found->second;
    if (seen[index] > 0 && rand() % seen[index] > 1) continue;
    result[index] = move;
    seen[index]++;
  }
  return result;
}

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
  string hrole = argv[2];
  while (std::getline(file, line)) { raw += line; }

  game::simulator<ltype,pwidth> sim(raw);
  auto index = sim.create_index<position_index>();
  index.emplace_rules(sim.context.parse.rules);

  sim.bind_state(index);
  auto roles = sim.ask<ROLE>(index);
  map<ltype,size_t> role_map;
  for (auto& role : roles) role_map[role[1]] = role_map.size();
  ltype hrole_index = sim.context.parse.tokens.forward.find(hrole)->second;
  auto initial = sim.ask_convert<TRUE, INIT>(index);

  size_t turn = 0;
  auto state = initial;
  auto& backward = sim.context.parse.tokens.backward;
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
    turn++;
  } while (sim.ask<TERMINAL>(index).size() == 0);
  for (auto& goal : sim.ask<GOAL>(index)) cout << human_readable(backward, goal) << endl;

  return 0;
}

