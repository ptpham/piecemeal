
#include <iostream>
#include "logic.h"

using namespace std;
using namespace piecemeal::logic;

int main(int nargs, char** argv) {
  isa<uint8_t,16> q0 = {0, {0}};
  decltype(q0) q1 = {1, {0}};
  decltype(q0) q2 = {2, {0}};

  rule<uint8_t, 16> r0 = { q0, { q1 } };
  decltype(r0) r1 = { q1, { q2 } };
  decltype(r0) r2 = { q2 };

  askstate<uint8_t, 16> state;
  vector<vector<decltype(r0)>> ruleset = { {r0}, {r1}, {r2} };
  auto proved = ask(ruleset, q0, state);
  cout << "Proved " << state.index.size() << " proposition(s)." << endl;
  return 0;
}
