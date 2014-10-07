
#pragma once

#include <vector>
#include <string>
#include <iostream>

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

    static inline string trim(const string& s) {
      size_t begin = s.find_first_not_of(" \t");
      size_t end = s.find_last_not_of(" \t");
      if (begin == string::npos) begin = s.length();
      if (end == string::npos) end = s.length(); else end++;
      return s.substr(begin, end - begin);
    }
  }
}

