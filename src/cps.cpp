
#include <iostream>
#include <fstream>
#include <numeric>
#include <ctime>

#include "piecemeal/game.hpp"
#include "piecemeal/stdfmt.hpp"
#include "piecemeal/ioaux.hpp"

using namespace std;
using namespace piecemeal;
using namespace logic;
using namespace game;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Usage: ./cps <path to game> <iterations>" << endl;
    return 1;
  }

  size_t niterations = atoi(argv[2]), transitions = 0;
  game::machine<> machine(ioaux::read_all(argv[1]));

  auto begin = clock();
  for (int i = 0; i < niterations; i++) {
    machine.restart();
    while (!machine.terminal()) {
      machine.move(random_joint_move(machine.moves()));
      transitions++;
    }
  }
  auto seconds = (double)(clock() - begin)/CLOCKS_PER_SEC;
  cout << "Charges per second: " << (double)niterations/seconds << endl;;
  cout << "Transitions per second: " << (double)transitions/seconds << endl;;

  return 0;
}

