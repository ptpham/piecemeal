
#pragma once

#include <numeric>
#include <array>

namespace piecemeal {
  namespace logic {
    using namespace std;

    template <class T, size_t N>
    struct prop : array<T,N> {
      prop(T value) { fill(this->begin(), this->end(), value); }
      prop() { }
    };

    template <class T, size_t N>
    T empty() { return numeric_limits<T>::max(); }

    template <class T, size_t N> prop<T,N> invert(const prop<T,N>& trans);

    template <class T, size_t N>
    prop<T,N> empty_prop() { return prop<T,N>(empty<T,N>()); }

    template <class T, size_t N>
    bool is_grounded(const prop<T,N>& a) {
      for (auto v : a) { if (v == empty<T,N>()) return false; } return true;
    }

    template <class T, size_t N>
    bool is_blank(const prop<T,N>& a) {
      for (auto v : a) { if (v != empty<T,N>()) return false; } return true;
    }

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

