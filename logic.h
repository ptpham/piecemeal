#pragma once

#include <stdint.h>

#include <vector>
#include <array>

#include <numeric>
#include <unordered_set>
#include <unordered_map>

#include <iostream>

#include "hashes.h"
#include "stdaux.h"

namespace piecemeal {
  namespace logic {
    using namespace std;

    template <class T, size_t N>
    struct unit : array<T,N> {
      unit(T value) { fill(this->begin(), this->end(), value); }
      unit() { }
    };

    template <class T, size_t N>
    T empty() { return numeric_limits<T>::max(); }

    template <class T, size_t N>
    unit<T,N> empty_unit() { return unit<T,N>(empty<T,N>()); }

    template <class T, size_t N>
    bool is_grounded(const unit<T,N>& a) {
      for (auto v : a) { if (v == empty<T,N>()) return false; } return true;
    }

    template <class T, size_t N>
    bool is_blank(const unit<T,N>& a) {
      for (auto v : a) { if (v != empty<T,N>()) return false; } return true;
    }

    template <class T, size_t N> unit<T,N> invert(const unit<T,N>& trans);

    template <class T, size_t N>
    struct term {
      unit<T,N> literal, push, pull;

      term() { }
      term(const unit<T,N>& a) : literal(a),
        push(empty_unit<T,N>()), pull(empty_unit<T,N>()) { }
      term(const unit<T,N>& a, const unit<T,N>& push, const unit<T,N>& pull) :
        literal(a), push(push), pull(pull) { }
    };

    template <class T, size_t N>
    struct rule {
      term<T,N> head;
      vector<term<T,N>> positives, negatives;
      vector<unit<T,N>> distincts;
    };

    template <class T, size_t N>
    struct knowledge {
      vector<rule<T,N>> rules;
      unordered_set<unit<T,N>> grounds;
    };

    template <class T, size_t N>
    using askstate = unordered_map<unit<T,N>, unordered_set<unit<T,N>>>;

    template <class I, class T, size_t N>
    const unordered_set<unit<T,N>>& ask(const I& index,
      const unit<T,N>& query, askstate<T,N>& state);
  }
}

namespace std {
  template <class T, size_t N>
  struct hash<piecemeal::logic::unit<T,N>> {
    size_t operator() (const piecemeal::logic::unit<T,N>& a) const {
      return piecemeal::hashes::sdbm(a.begin(), a.end());
    }
  };

  template <class T, size_t N>
  ostream& operator <<(ostream& out, const piecemeal::logic::unit<T,N>& t) {
    out << "[";
    for (size_t i = 0; i < N; i++) {
      if (i > 0) out << " ";
      if (t[i] == piecemeal::logic::empty<T,N>()) out << "_";
      else out << (int64_t)t[i];
    }
    return out << "]";
  }

  template <class T, size_t N>
  ostream& operator << (ostream& out, const piecemeal::logic::term<T,N>& t) {
    return out << t.literal << " <- " << t.push << " <- " << t.pull;
  }
}

