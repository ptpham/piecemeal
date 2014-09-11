
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

#define EXPORT(T,N) \
    template fixture<T,N> build_fixture(unordered_dimap<string>& tokens); \
    template context<T,N> build_context(const string& raw);

    EXPORT(uint8_t,8)
    EXPORT(uint16_t,8)
#undef EXPORT
  }
}




