
#pragma once

#include <vector>
#include <unordered_set>

#include "piecemeal/unordered_dimap.hpp"
#include "piecemeal/position_index.hpp"
#include "piecemeal/compile.hpp"
#include "piecemeal/logic.hpp"

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
      const vector<T> ids;
      const vector<prop<T,N>> queries;

      prop<T,N> convert(const prop<T,N>& p, keyword k) const {
        prop<T,N> result = p;
        result[0] = ids[k];
        return result;
      }
    };

    template <class T, size_t N>
    fixture<T,N> build_fixture(unordered_dimap<string>& tokens);

    template <class T, size_t N>
    struct context {
      const compile::parse<T,N> parse;
      const fixture<T,N> fixture;
    };

    template <class T, size_t N>
    context<T,N> build_context(const vector<string>& raw);

    template <class T, size_t N>
    struct simulator {
      context<T,N> context;
      askstate<T,N> state;

      simulator() { }
      simulator(const vector<string>& raw) : context(build_context<T,N>(raw)) { }

      template <class I>
      const unordered_set<prop<T,N>>& ask(const I& index, keyword k) {
        return logic::ask(index, context.fixture.queries[k], state);
      }

      prop<T,N> convert(const prop<T,N>& p, keyword k) const {
        return context.fixture.convert(p, k);
      }

      template <class C>
      vector<prop<T,N>> convert(const C& props, keyword k) const {
        vector<prop<T,N>> result;
        result.reserve(props.size());
        for (auto& p : props) result.push_back(convert(p, k));
        return result;
      }

      template <class I>
      vector<prop<T,N>> ask_convert(const I& index, keyword u, keyword v) {
        return convert(ask(index, v), u);
      }

      template <template <class,size_t> class I>
      I<T,N> create_index() {
        I<T,N> result;
        result.emplace_rules(context.parse.rules);
        bind_state(result);
        return result;
      }

      template <class I, class S = vector<prop<T,N>>>
      void bind_state(I& index, const S& turn = S()) {
        index.clear_props();
        index.emplace_props(context.parse.props);
        index.emplace_props(turn);
        state.clear();
      }

      template <class I>
      map<T,size_t> ask_role_map(const I& index) {
        map<T,size_t> result;
        auto roles = ask(index, ROLE);
        for (auto& role : roles) result[role[1]] = result.size();
        return result;
      }
    };

    template <class T, size_t N>
    vector<prop<T,N>> random_joint_move(
      const vector<vector<prop<T,N>>>& moves);

    template <class I = position_index<>>
    struct machine {
      typedef typename I::prop_type T;
      static const size_t N = I::prop_size;

      game::simulator<T,N> sim;
      map<T,size_t> role_map;
      vector<prop<T,N>> state;
      size_t turn;
      I index;
      
      machine(const vector<string>& raw);
      void move(const vector<prop<T,N>>& chosen);
      vector<vector<prop<T,N>>> moves();
      vector<uint16_t> goals();
      bool terminal();
      void restart();
    };
  }
}


