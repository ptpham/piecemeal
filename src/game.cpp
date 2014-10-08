
#include "piecemeal/game.hpp"
#include "piecemeal/preprocess.hpp"
#include "piecemeal/position_index.hpp"
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
    machine<I>::machine(const vector<string>& raw) : sim(raw),
      index(sim.template create_index<position_index>()) {
      role_map = sim.ask_role_map(index);
      restart();
    }

    template <class I>
    void machine<I>::restart() {
      auto state = sim.ask_convert(index, TRUE, INIT);
      sim.bind_state(index, state);
      turn = 0;
    }

    template <class I>
    vector<vector<prop<typename machine<I>::T,machine<I>::N>>> machine<I>::moves() {
      auto raw = sim.ask_convert(index, DOES, LEGAL);
      return index_by_position(raw, role_map, 1);
    }

    template <class I>
    void machine<I>::move(const vector<prop<machine<I>::T,machine<I>::N>>& chosen) {
      index.emplace_props(chosen);
      state = sim.ask_convert(index, TRUE, NEXT);
      sim.bind_state(index, state);
      turn++;
    }

    template <class I>
    bool machine<I>::terminal() {
      return sim.ask(index, TERMINAL).size() > 0;
    }

    template <class I>
    vector<uint16_t> machine<I>::goals() {
      auto raw = sim.ask(index, GOAL);
      vector<uint16_t> result(role_map.size());
      auto& backward = sim.context.parse.tokens.backward;
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




