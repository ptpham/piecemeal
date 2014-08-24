
#include "dag.h"
#include "gdl.h"
#include "logic.h"
#include "prefix_index.h"
#include "compile.h"

#include "stdfmt.h"

using namespace std;
using namespace piecemeal;
using namespace piecemeal::logic;

int main(int nargs, char** argv) {
  string raw_string = "(<= (z ?x) (q ?x)) (<= (q ?x) (p ?x))(r bloop) (<= (p ?y) (r ?y)) \
    (<= (p awesome) (or (r blip) (r blim) (r bloop))) (<= (z (f ?x)) (t ?x))";
  auto parsed = dag::loads_tree(raw_string);
  parsed = gdl::deor_sentences(parsed);
  gdl::canonize_sentences(*parsed);
  cout << dag::dumps_tree(parsed) << endl;
  auto scope = compile::parse_sentences<uint8_t, 8>(parsed);
  logic::prefix_index<uint8_t, 8> index;

  for (auto& rule : scope.rules) {
    cout << rule.head << endl;
    for (auto term : rule.positives) cout << "\t" << term << endl;
    index.emplace(rule);
  }

  for (auto& ground : scope.grounds) {
    cout << "Ground: " << ground << endl;
    index.emplace(ground);
  }

  askstate<uint8_t, 8> state;
  auto proved = ask(index, scope.rules[0].head.literal, state);

  size_t total = 0; 
  cout << "Proved: " << endl;
  for (auto& cache : state) {
    total += cache.second.size();
    for (auto elem : cache.second)  cout << elem << endl;
  }
  cout << "Proved " << total << " proposition(s)." << endl;
  return 0;
}

