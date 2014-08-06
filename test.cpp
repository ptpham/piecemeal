
#include <iostream>
#include <map>

#include "logic.h"
#include "kif.h"
#include "dag.h"

using namespace std;
using namespace piecemeal;
using namespace piecemeal::logic;

template <class T, size_t N>
ostream& operator << (ostream& out, const array<T,N>& a) {
  for (int v : a) cout << " " << v;
  return out;
}

template <class T, size_t N>
ostream& operator << (ostream& out, const term<T,N>& t) {
  return out << t.literal << " <- " << t.push << " -> " << t.pull;
}

int main(int nargs, char** argv) {

  string raw_string = "(<= (q ?x) (p ?x))(r bloop) (<= (p ?y) (r ?y)) (r blip)";
  auto parsed = dag::loads(raw_string);
  cout << dag::dumps(parsed) << endl;
  auto scope = kif::parse<uint8_t, 8>(parsed);
  logic::prefix_index<uint8_t, 8> index;

  for (auto& rule : scope.rules) {
    cout << rule.head << endl;
    for (auto term : rule.positives) cout << "\t" << term << endl;
    index.emplace(rule);
  }

  for (auto& ground : scope.grounds) {
    index.emplace(ground);
  }

  askstate<uint8_t, 8> state;
  auto proved = ask(index, scope.rules[0].head.literal, state);

  size_t total = 0; 
  for (auto& cache : state) total += cache.second.size();
  cout << "Proved " << total << " proposition(s)." << endl;
  return 0;
}

