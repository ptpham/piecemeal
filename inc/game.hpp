
#pragma once

#include <vector>
#include <unordered_set>

#include "unordered_dimap.hpp"
#include "compile.hpp"
#include "logic.hpp"

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

      template <keyword K>
      prop<T,N> convert(const prop<T,N>& p) const {
        prop<T,N> result = p;
        result[0] = ids[K];
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
    context<T,N> build_context(const string& raw);

    template <class T, size_t N>
    struct simulator {
      context<T,N> context;
      askstate<T,N> state;

      simulator() { }
      simulator(const string& raw) : context(build_context<T,N>(raw)) { }

      template <keyword K, class I>
      const unordered_set<prop<T,N>>& ask(const I& index) {
        return logic::ask(index, context.fixture.queries[K], state);
      }

      template <keyword K>
      prop<T,N> convert(const prop<T,N>& p) const {
        // Not sure if this is a C++ issue or a bug with clang-503.0.40, but
        // simply calling context.fixture.convert<K>(p) complains about trying
        // to invoke a static method without an object. The function is
        // reimplemented here to avoid this error.
        prop<T,N> result = p;
        result[0] = context.fixture.ids[K];
        return result;
      }

      template <keyword K, class C>
      vector<prop<T,N>> convert(const C& props) const {
        vector<prop<T,N>> result;
        result.reserve(props.size());
        for (auto& p : props) result.push_back(convert<K>(p));
        return result;
      }

      template <keyword U, keyword V, class I>
      vector<prop<T,N>> ask_convert(const I& index) {
        return convert<U>(ask<V>(index));
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
        auto roles = ask<ROLE>(index);
        for (auto& role : roles) result[role[1]] = result.size();
        return result;
      }
    };

    template <class T, size_t N>
    vector<prop<T,N>> random_joint_move(const vector<prop<T,N>>& moves,
      const map<T,size_t>& role_map);
  }
}


