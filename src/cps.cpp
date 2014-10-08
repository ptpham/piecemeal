
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

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Usage: ./cps <path to game> <iterations>" << endl;
    return 1;
  }

  std::string line;
  vector<string> raw;
  std::ifstream file(argv[1]);
  size_t niterations = atoi(argv[2]);
  while (std::getline(file, line)) { raw.push_back(line); }
  game::machine<> machine(raw);

  auto begin = clock();
  for (int i = 0; i < niterations; i++) {
    machine.restart();
    while (!machine.terminal()) {
      machine.move(random_joint_move(machine.moves()));
    }
  }
  auto seconds = (double)(clock() - begin)/CLOCKS_PER_SEC;
  cout << "Charges per second: " << (double)niterations/seconds << endl;;

  return 0;
}

