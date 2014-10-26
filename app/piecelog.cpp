
#include <iostream>
#include <string>
#include <set>

#include "piecemeal/dag.hpp"
#include "piecemeal/logic.hpp"
#include "piecemeal/compile.hpp"
#include "piecemeal/preprocess.hpp"
#include "piecemeal/position_index.hpp"

#include "piecemeal/stdfmt.hpp"
#include "piecemeal/ioaux.hpp"

using namespace std;
using namespace piecemeal;
using namespace logic;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    cout << "Usage: ./piecelog <paths to kif>... " << endl;
    return 1;
  }

  position_index<> index;
  const size_t N = position_index<>::N;
  typedef position_index<>::T T;

  vector<string> all_code;
  for (int i = 1; i < argc; i++) {
    for (auto& line : ioaux::read_all(argv[i])) {
      all_code.push_back(line);
    }
  }

  auto processed = preprocess::standard(all_code);
  auto parse = compile::parse_sentences<T,N>(processed);
  index.emplace_rules(parse.rules);
  string line;

  while (true) {
    getline(cin, line);
    auto nodes = tree::loads(line);
    vector<prop<T,N>> extracted;
    for (auto node : *nodes) {
      auto p = compile::extract_literal<T,N>(parse.tokens, node);
      extracted.push_back(p);
    }

    prop<T,N> query;
    askstate<T,N> askstate;
    index.emplace_props(extracted);
    index.emplace_props(parse.props);

    for (size_t i = 0; i < index.size(); i++) {
      auto& knowledge = index[i];
      query[0] = i;

      for (auto& p : logic::ask(index, query, askstate)) {
        if (knowledge.props.find(p) != knowledge.props.end()) continue;
        cout << compile::recover(parse.tokens.backward, p, parse.depths[p[0]]) << " ";
      }
    }
    cout << endl;

    index.clear_props();
  }

  return 0;
}