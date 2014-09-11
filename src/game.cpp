
#include "game.hpp"
#include "preprocess.hpp"

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
    context<T,N> build_context(const string& raw) {
      auto processed = preprocess::standard(raw);
      auto parse = compile::parse_sentences<T,N>(processed); 
      auto fixture = build_fixture<T,N>(parse.tokens);
      return { parse, fixture };
    }

    template <class T, size_t N>
    vector<prop<T,N>> random_joint_move(const vector<prop<T,N>>& moves, const map<T,size_t>& role_map) {
      size_t nroles = role_map.size();
      vector<prop<T,N>> result(nroles);
      size_t seen[nroles];

      fill(seen, seen + nroles, 0);
      for (auto move : moves) {
        auto found = role_map.find(move[1]);
        if (found == role_map.end()) continue;
        auto index = found->second;
        if (seen[index] > 0 && rand() % seen[index] > 1) continue;
        result[index] = move;
        seen[index]++;
      }
      return result;
    }

#define EXPORT(T,N) \
    template fixture<T,N> build_fixture(unordered_dimap<string>& tokens); \
    template context<T,N> build_context(const string& raw); \
    template vector<prop<T,N>> random_joint_move( \
      const vector<prop<T,N>>&moves, const map<T,size_t>& role_map);

    EXPORT(uint8_t,8)
    EXPORT(uint16_t,8)
#undef EXPORT
  }
}




