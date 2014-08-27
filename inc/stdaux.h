
#pragma once

#include <vector>
#include <unordered_map>

namespace std {
  template <class T> T begin(const pair<T,T>& p) { return p.first; }
  template <class T> T end(const pair<T,T>& p) { return p.second; }
};

namespace piecemeal {
  namespace stdaux {
    using namespace std;

    template <class U, class V = size_t>
    struct unordered_dimap {
      vector<U> backward;
      unordered_map<U,V> forward;

      V at(const U& s) {
        auto found = forward.find(s);
        if (found != forward.end()) return found->second;
        V result = backward.size();
        forward.insert(found, {s, result});
        backward.push_back(s);
        return result;
      }
    };

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

    template <class I, class F>
    auto max_extraction(I begin, I end, F converter) {
      return converter(*std::max_element(begin, end, [&](auto i, auto j) {
        return converter(i) < converter(j);
      }));
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

