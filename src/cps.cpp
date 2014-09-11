
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
  index.emplace_rules(sim.context.parse.rules);

  sim.bind_state(index);
  auto roles = sim.ask<ROLE>(index);
  map<ltype,size_t> role_map;
  for (auto& role : roles) role_map[role[1]] = role_map.size();
  auto initial = sim.ask_convert<TRUE, INIT>(index);

  auto begin = clock();
  for (int i = 0; i < niterations; i++) {
    size_t turn = 0;
    auto state = initial;
    sim.bind_state(index, state);
    do {
      auto moves = sim.ask_convert<DOES, LEGAL>(index);
      auto chosen = random_joint_move(moves, role_map);
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

