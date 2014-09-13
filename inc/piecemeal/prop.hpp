
#pragma once

#include <stdint.h>
#include <numeric>
#include <vector>
#include <array>
#include <map>

#include "piecemeal/hashes.hpp"

namespace piecemeal {
  namespace logic {
    using namespace std;

    template <class T, size_t N>
    struct prop : array<T,N> {
      prop(T value) { fill(this->begin(), this->end(), value); }
      prop() : prop(empty()) { }

      bool is_blank() const {
        for (auto v : *this) if (v != empty()) return false;
        return true;
      }

      bool is_grounded() const {
        for (auto v : *this) if (v == empty()) return false;
        return true;
      }

      static T empty() { return numeric_limits<T>::max(); }
    };

    template <class T, size_t N> prop<T,N> invert(const prop<T,N>& trans);

    template <class T, size_t N>
    vector<vector<prop<T,N>>> index_by_position(const vector<prop<T,N>>& props,
      const map<T,size_t>& coalesce, size_t position);
  }
}


namespace std {
  template <class T, size_t N>
  struct hash<piecemeal::logic::prop<T,N>> {
    size_t operator() (const piecemeal::logic::prop<T,N>& a) const {
      return piecemeal::hashes::sdbm(a.begin(), a.end());
    }
  };
}

