
#include "piecemeal/dag.hpp"
#include "piecemeal/logic.hpp"
#include "piecemeal/compile.hpp"
#include "piecemeal/preprocess.hpp"
#include "piecemeal/position_index.hpp"

#include "piecemeal/stdfmt.hpp"
#include "piecemeal/ioaux.hpp"

namespace piecelog {
  using namespace std;
  using namespace piecemeal;
  using namespace compile;
  using namespace logic;

  template <class T = uint16_t, size_t N = 12>
  string run(compile::parse<T,N>& parse, string line) {
    auto nodes = tree::loads(line);
    vector<prop<T,N>> extracted;
    for (auto node : *nodes) {
      auto p = compile::extract_literal<T,N>(parse.tokens, node);
      extracted.push_back(p);
    }

    prop<T,N> query;
    askstate<T,N> askstate;
    position_index<T,N> index;
    index.emplace_rules(parse.rules);
    index.emplace_props(extracted);
    index.emplace_props(parse.props);

    string result = "";
    for (size_t i = 0; i < index.size(); i++) {
      auto& knowledge = index[i];
      query[0] = i;

      for (auto& p : logic::ask(index, query, askstate)) {
        result += compile::recover(parse.tokens.backward, p, parse.depths[p[0]]) + " ";
      }
    }

    return result;
  }

  template <class T = uint16_t, size_t N = 12>
  compile::parse<T,N> load(const vector<string>& args, function<string (void)> input) {
    string arg2(args[0]);
    size_t num_code_lines = 0;
    if (arg2 == "-n" && args.size() > 1) num_code_lines = strtol(args[1].c_str(), NULL, 0);

    vector<string> all_code;
    for (auto& path : args) {
      for (auto& line : ioaux::read_all(path)) {
        all_code.push_back(line);
      }
    }

    for (size_t i = 0; i < num_code_lines; i++) {
      all_code.push_back(input());
    }

    auto processed = preprocess::standard(all_code);
    return compile::parse_sentences<T,N>(processed);
  }
}
