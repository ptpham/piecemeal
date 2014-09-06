#pragma once

#include <iostream>
#include <map>
#include <set>

namespace std {
  template <class C>
  ostream& apply(ostream& os, const C& collection,
    const string& begin, const string& separator, const string& end);

  template <class U, class V>
  ostream& operator<< (ostream& os, const pair<U,V>& pair) {
    os << "(" << pair.first << "," << pair.second << ")";
    return os; 
  }

  template <class T, size_t N>
  ostream& operator<< (ostream& os, const array<T,N>& c) {
    return apply(os, c, "[", ", ", "]");
  }

  template <class T>
  ostream& operator<< (ostream& os, const vector<T>& c) {
    return apply(os, c, "[", ", ", "]");
  }

  template <class T>
  ostream& operator<< (ostream& os, const set<T>& c) {
    return apply(os, c, "{", ", ", "}");
  }

  template <class T>
  ostream& operator<< (ostream& os, const unordered_set<T>& c) {
    return apply(os, c, "{", ", ", "}");
  }

  template <class U, class V>
  ostream& operator<< (ostream& os, const map<U, V>& c) {
    return apply(os, c, "<", ", ", ">");
  }

  template <class U, class V>
  ostream& operator<< (ostream& os, const unordered_map<U, V>& c) {
    return apply(os, c, "<", ", ", ">");
  }

  template <class C>
  ostream& apply(ostream& os, const C& collection,
    const string& begin, const string& separator, const string& end) {
    os << begin;
    bool first = true;
    for (auto& value : collection) {
      if (!first) os << separator;
      first = false;
      os << value;
    }
    os << end;
    return os; 
  }
}
