
#include "game.hpp"
#include "gdl.hpp"

namespace piecemeal {
  namespace game {
    using namespace gdl;

    template <class T, size_t N>
    fixture<T,N> build_fixture(unordered_dimap<string>& tokens) {
      prop<T,N> role_query, base_query, input_query,
        init_query, goal_query, terminal_query, true_query,
        next_query, legal_query, does_query;
      role_query[0] = (T)tokens.at(ROLE);
      base_query[0] = (T)tokens.at(BASE);
      init_query[0] = (T)tokens.at(INIT);
      goal_query[0] = (T)tokens.at(GOAL);
      true_query[0] = (T)tokens.at(TRUE);
      next_query[0] = (T)tokens.at(NEXT);
      does_query[0] = (T)tokens.at(DOES);
      legal_query[0] = (T)tokens.at(LEGAL);
      input_query[0] = (T)tokens.at(INPUT);
      terminal_query[0] = (T)tokens.at(TERMINAL);

      return {
        (T)tokens.at(ROLE), (T)tokens.at(BASE), (T)tokens.at(INPUT),
        (T)tokens.at(INIT), (T)tokens.at(GOAL), (T)tokens.at(TERMINAL),
        (T)tokens.at(TRUE), (T)tokens.at(NEXT), (T)tokens.at(LEGAL),
        (T)tokens.at(DOES),

        role_query, base_query, input_query,
        init_query, goal_query, terminal_query, true_query,
        next_query, legal_query, does_query
      };
    }

    template <class T, size_t N>
    context<T,N> build_context(const string& raw) {
      auto processed = gdl::preprocess_standard(raw);
      auto scope = compile::parse_sentences<T,N>(processed); 
      auto fixture = build_fixture<T,N>(scope.tokens);
      return { scope, fixture };
    }

    template fixture<uint8_t,8> build_fixture(unordered_dimap<string>& tokens);
    template context<uint8_t,8> build_context(const string& raw);
  }
}




