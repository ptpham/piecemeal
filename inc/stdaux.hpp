
#pragma once

#include <vector>

namespace std {
  template <class T> T begin(const pair<T,T>& p) { return p.first; }
  template <class T> T end(const pair<T,T>& p) { return p.second; }
};

namespace piecemeal {
  namespace stdaux {
    using namespace std;

    template <class T, size_t N>
    array<T,N> filled_array(T value) {
      array<T,N> result;
      result.fill(value);
      return result;
    }

    template <class I, class T>
    I max_element_nullable(I first, I last, T null) {
      if (first == last) return last;
      I largest = first;
      for (++first; first != last; ++first) {
        if (*first == null) continue;
        if (*largest == null || *largest < *first) largest = first;
      }
      return largest;
    }

  }
}

