
#pragma once

#include <unordered_set>
#include "unordered_dimap.h"
#include "prop.h"

namespace piecemeal {
  namespace game {
    using namespace logic;
    using namespace stdaux;

    template <class T, size_t N>
    struct fixture {
      const T role_id, base_id, input_id,
        init_id, goal_id, terminal_id, true_id,
        next_id, legal_id, does_id;
      const prop<T,N> role_query, base_query, input_query,
        init_query, goal_query, terminal_query, true_query,
        next_query, legal_query, does_query;

      prop<T,N> convert(const prop<T,N>& p, T target_id) {
        prop<T,N> result = p;
        result[0] = target_id;
        return result;
      }

      prop<T,N> as_true(const prop<T,N>& p) { return convert(p, true_id); }
      prop<T,N> as_does(const prop<T,N>& p) { return convert(p, does_id); }
    };

    template <class T, size_t N>
    fixture<T,N> build_fixture(unordered_dimap<string>& tokens);
  }
}


