
#include "piecemeal/game.hpp"
#include "piecemeal/preprocess.hpp"
#include "piecemeal/position_index.hpp"
#include "piecemeal/compile.hpp"

#include "private/logic_export.hpp"

namespace piecemeal {
  namespace game {
    const vector<string> keyword_names = { "role", "base", "input",
      "init", "goal", "terminal", "true", "next", "legal", "does" };

    template <class T, size_t N>
    fixture<T,N> build_fixture(unordered_dimap<string>& tokens) {
      vector<prop<T,N>> queries(KEYWORD_COUNT);
      vector<T> ids(KEYWORD_COUNT);

      for (size_t i = 0; i < KEYWORD_COUNT; i++) {
        ids[i] = (T)tokens.at(keyword_names[i]);
        queries[i][0] = ids[i];
      }

      return { ids, queries };
    }

    template <class T, size_t N>
    context<T,N> build_context(const vector<string>& raw) {
      auto processed = preprocess::standard(raw);
      auto parse = compile::parse_sentences<T,N>(processed); 
      auto fixture = build_fixture<T,N>(parse.tokens);
      return { parse, fixture };
    }

    template <class T, size_t N>
    vector<prop<T,N>> random_joint_move(
      const vector<vector<prop<T,N>>>& moves) {
      vector<prop<T,N>> result(moves.size());
      for (int i = 0; i < moves.size(); i++) {
        auto& selected = moves[i][rand() % moves[i].size()];
        result[i] = selected;
      }
      return result;
    }

    template <class I>
    machine<I>::machine(const vector<string>& raw) : 
      context(build_context<T,N>(raw)) {
      index.emplace_rules(context.parse.rules);
      bind_state();
      
      // Extract roles
      auto roles = ask(ROLE);
      for (auto& role : roles) role_map[role[1]] = role_map.size();

      // Make sure we can recover the structure for any prop
      for (auto& entry : context.parse.depths) {
        depths[index.parent(entry.first)] = entry.second;
      }

      restart();
    }

    template <class I>
    string machine<I>::recover(const prop<T,N>& p) {
      auto& depth = depths[index.parent(p)];
      return compile::recover(context.parse.tokens.backward, p, depth);
    }

    template <class I>
    void machine<I>::bind_state(const vector<prop<T,N>>& turn) {
      index.clear_props();
      index.emplace_props(turn);
      index.emplace_props(context.parse.props);
      askstate.clear();
    }

    template <class I>
    auto machine<I>::ask_convert(keyword u, keyword v) -> vector<prop<T,N>> {
      auto props = ask(v);
      vector<prop<T,N>> result;
      result.reserve(props.size());
      for (auto& p : props) {
        auto converted = context.fixture.convert(p, u);
        result.push_back(converted);
      }
      return result;
    }

    template <class I>
    auto machine<I>::ask(keyword k) -> unordered_set<prop<T,N>> {
      return logic::ask(index, context.fixture.queries[k], askstate);
    }

    template <class I>
    void machine<I>::restart() {
      state = ask_convert(TRUE, INIT);
      bind_state(state);
      turn = 0;
    }

    template <class I>
    vector<vector<prop<typename machine<I>::T,machine<I>::N>>> machine<I>::moves() {
      auto raw = ask_convert(DOES, LEGAL);
      return index_by_position(raw, role_map, 1);
    }

    template <class I>
    void machine<I>::move(const vector<prop<machine<I>::T,machine<I>::N>>& chosen) {
      index.emplace_props(chosen);
      state = ask_convert(TRUE, NEXT);
      bind_state(state);
      turn++;
    }

    template <class I>
    bool machine<I>::terminal() {
      return ask(TERMINAL).size() > 0;
    }

    template <class I>
    vector<uint16_t> machine<I>::goals() {
      auto raw = ask(GOAL);
      vector<uint16_t> result(role_map.size());
      auto& backward = context.parse.tokens.backward;
      for (auto& goal : raw) {
        auto goal_index = goal[2];
        if (goal_index >= backward.size()) continue;
        result[role_map[goal[1]]] = atoi(backward[goal_index].c_str());
      }
      return result;
    }

#define EXPORT(T,N) \
    template fixture<T,N> build_fixture(unordered_dimap<string>& tokens); \
    template context<T,N> build_context(const vector<string>& raw); \
    template vector<prop<T,N>> random_joint_move( \
      const vector<vector<prop<T,N>>>& moves); \
    template class machine<position_index<T,N>>;

    DEFAULT_LOGIC_EXPORT
#undef EXPORT
  }
}




