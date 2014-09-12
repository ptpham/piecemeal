
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

typedef uint16_t ltype;
constexpr size_t pwidth = 8;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Usage: ./cps <path to game> <iterations>" << endl;
    return 1;
  }

  std::string line, raw;
  std::ifstream file(argv[1]);
  size_t niterations = atoi(argv[2]);
  while (std::getline(file, line)) { raw += line; }
  game::machine<position_index,ltype,pwidth> machine(raw);

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

