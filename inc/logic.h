#pragma once

#include <stdint.h>

#include <vector>
#include <array>

#include <unordered_set>
#include <unordered_map>

#include <iostream>

#include "hashes.h"
#include "stdaux.h"
#include "rule.h"

namespace piecemeal {
  namespace logic {
    using namespace std;

    template <class T, size_t N>
    struct knowledge {
      vector<rule<T,N>> rules;
      unordered_set<prop<T,N>> grounds;
    };

    template <class T, size_t N>
    using askstate = unordered_map<prop<T,N>, unordered_set<prop<T,N>>>;

    template <class I, class T, size_t N>
    const unordered_set<prop<T,N>>& ask(const I& index,
      const prop<T,N>& query, askstate<T,N>& state);
  }
}

namespace std {
  template <class T, size_t N>
  ostream& operator <<(ostream& out, const piecemeal::logic::prop<T,N>& t) {
    out << "[";
    for (size_t i = 0; i < N; i++) {
      if (i > 0) out << " ";
      if (t[i] == piecemeal::logic::prop<T,N>::empty()) out << "_";
      else out << (int64_t)t[i];
    }
    return out << "]";
  }

  template <class T, size_t N>
  ostream& operator << (ostream& out, const piecemeal::logic::term<T,N>& t) {
    return out << t.literal << " <- " << t.push << " -> " << t.pull;
  }
}

