
#include <iostream>
#include <fstream>
#include <numeric>
#include <ctime>

#include "logic.hpp"
#include "compile.hpp"
#include "position_index.hpp"
#include "stdfmt.hpp"

#include "machine.hpp"

using namespace std;
using namespace piecemeal;
using namespace logic;
using namespace game;

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

  game::machine<position_index> machine(raw);
  auto& sim = machine.sim;

  auto& parse = sim.context.parse;
  auto role_found = parse.tokens.forward.find(hrole);
  if (role_found == parse.tokens.forward.end()) {
    cout << "Role '" << hrole << "' invalid." << endl;
    return 1;
  }
  size_t hrole_index = role_found->second;

  auto& backward = parse.tokens.backward;
  while (!machine.is_terminal()) {
    auto moves = machine.moves();
    auto chosen = random_joint_move(moves, machine.role_map);

    cout << "Select move:" << endl;
    for (size_t i = 0; i < moves.size(); i++) {
      auto& move = moves[i];
      if (move[1] != hrole_index) continue;
      cout << i << ") " << human_readable(backward, move) << endl;
    }
    size_t selection;
    cin >> selection;
    chosen[machine.role_map[hrole_index]] = moves[selection];

    cout << "Playing: ";
    for (auto& move : chosen) {
      cout << human_readable(backward, move) << " ";
    }
    cout << endl;

    machine.move(chosen);
  }

  auto goals = machine.goals();
  for (auto& goal : goals) cout << human_readable(backward, goal) << endl;
  return 0;
}

