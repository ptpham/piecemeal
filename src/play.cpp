
#include <iostream>
#include <fstream>
#include <numeric>
#include <ctime>

#include "piecemeal/game.hpp"
#include "piecemeal/stdfmt.hpp"

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

  std::string line;
  vector<string> raw;
  std::ifstream file(argv[1]);
  while (std::getline(file, line)) { raw.push_back(line); }
  string hrole = argv[2];

  game::machine<> machine(raw);
  auto& parse = machine.context.parse;
  auto role_found = parse.tokens.forward.find(hrole);
  if (role_found == parse.tokens.forward.end()) {
    cout << "Role '" << hrole << "' invalid." << endl;
    return 1;
  }
  size_t hrole_index = machine.role_map[role_found->second];

  auto& backward = parse.tokens.backward;
  while (!machine.terminal()) {
    auto moves = machine.moves();
    auto chosen = random_joint_move(moves);

    auto human_moves = moves[hrole_index];
    cout << "Select move:" << endl;
    for (size_t i = 0; i < human_moves.size(); i++) {
      auto& move = human_moves[i];
      cout << i << ") " << human_readable(backward, move) << endl;
    }
    size_t selection;
    cin >> selection;
    chosen[hrole_index] = human_moves[selection];

    cout << "Playing: ";
    for (auto& move : chosen) {
      cout << human_readable(backward, move) << " ";
    }
    cout << endl;

    machine.move(chosen);
  }

  cout << "Goals: " << machine.goals() << endl;
  return 0;
}

