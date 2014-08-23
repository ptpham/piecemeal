#pragma once

#include <stdint.h>

#include <vector>
#include <array>

#include <numeric>
#include <unordered_set>
#include <unordered_map>

#include "hashes.h"
#include "stdaux.h"

namespace piecemeal {
  namespace logic {
    using namespace std;

    template <class T, size_t N>
    T empty() { return numeric_limits<T>::max(); }

    template <class T, size_t N>
    array<T,N> empty_array() { return stdaux::filled_array<T,N>(empty<T,N>());}

    template <class T, size_t N>
    bool is_grounded(const array<T,N>& a) {
      for (auto v : a) { if (v == empty<T,N>()) return false; } return true;
    }

    template <class T, size_t N>
    bool is_blank(const array<T,N>& a) {
      for (auto v : a) { if (v != empty<T,N>()) return false; } return true;
    }

    template <class T, size_t N> array<T,N> invert(const array<T,N>& trans);

    template <class T, size_t N>
    struct term {
      array<T,N> literal, push, pull;

      term() { }
      term(const array<T,N>& a) : literal(a),
        push(empty_array<T,N>()), pull(empty_array<T,N>()) { }
      term(const array<T,N>& a, const array<T,N>& push, const array<T,N>& pull) :
        literal(a), push(push), pull(pull) { }
    };

    template <class T, size_t N>
    struct rule {
      term<T,N> head;
      vector<term<T,N>> positives, negatives;
      vector<array<T,N>> distincts;
    };

    template <class T, size_t N>
    struct knowledge {
      vector<rule<T,N>> rules;
      unordered_set<array<T,N>> grounds;
    };

    template <class T, size_t N>
    using askstate = unordered_map<array<T,N>, unordered_set<array<T,N>>>;

    template <class I, class T, size_t N>
    const unordered_set<array<T,N>>& ask(const I& index,
      const array<T,N>& query, askstate<T,N>& state);
  }
}

namespace std {
  template <class T, size_t N>
  struct hash<array<T,N>> {
    size_t operator() (const array<T,N>& a) const {
      return piecemeal::hashes::sdbm(a.begin(), a.end());
    }
  };
}

