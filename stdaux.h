
#pragma once

#include <vector>

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
        if (*largest != null && *largest < *first) largest = first;
      }
      return largest;
    }

    struct bitvector : vector<bool> {
      bool contains(size_t i) const { return size() <= i ? false : at(i); }
      void set(size_t i, bool truth) {
        if (i >= size()) resize(i+1);
        at(i) = truth;
      }
    };
  }
}

