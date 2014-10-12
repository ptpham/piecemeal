
#pragma once

#include <vector>
#include <unordered_set>

#include "piecemeal/unordered_dimap.hpp"
#include "piecemeal/position_index.hpp"
#include "piecemeal/compile.hpp"

namespace piecemeal {
  namespace game {
    using namespace logic;
    using namespace stdaux;

    enum keyword : size_t {
      ROLE, BASE, INPUT, INIT, GOAL, TERMINAL, TRUE, NEXT, LEGAL, DOES,
      KEYWORD_COUNT
    };
    
    extern const vector<string> keyword_names;

    template <class T, size_t N>
    struct fixture {
      vector<T> ids;
      vector<prop<T,N>> queries;

      inline prop<T,N> convert(const prop<T,N>& p, keyword k) const {
        prop<T,N> result = p;
        result[0] = ids[k];
        return result;
      }
    };

    template <class T, size_t N>
    fixture<T,N> build_fixture(unordered_dimap<string>& tokens);

    template <class T, size_t N>
    struct context {
      compile::parse<T,N> parse;
      map<T,size_t> role_map;
      fixture<T,N> fixture;
    };

    template <class T, size_t N>
    context<T,N> build_context(const vector<string>& raw);

    template <class T, size_t N>
    vector<prop<T,N>> random_joint_move(
      const vector<vector<prop<T,N>>>& moves);

    template <class I = position_index<>>
    struct machine {
      typedef typename I::prop_type T;
      static const size_t N = I::prop_size;

      context<T,N> context;
      askstate<T,N> askstate;
      vector<prop<T,N>> state;
      size_t turn;
      I index;
      
      machine(const vector<string>& raw);
      string recover(const prop<T,N>& p);

      void bind_state(const vector<prop<T,N>>& turn = vector<prop<T,N>>());
      unordered_set<prop<T,N>> ask(keyword k);
      vector<prop<T,N>> ask_convert(keyword u, keyword v);

      void move(const vector<prop<T,N>>& chosen);
      vector<vector<prop<T,N>>> moves();
      vector<uint16_t> goals();
      bool terminal();
      void restart();
    };
  }
}


