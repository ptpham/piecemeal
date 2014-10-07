#pragma once

#include <vector>
#include <string>

namespace piecemeal {
  namespace dag {
    using namespace std;

    template <class T> struct elem : vector<shared_ptr<elem<T>>> { T value; };
    template <class T> using node = shared_ptr<elem<T>>;

    template <class T>
    node<T> wrap(const T& value) {
      elem<T>* ptr = new elem<T>();
      ptr->value = value;
      return node<T>(ptr);
    }

    template <class T>
    node<T> convert(const vector<node<T>>& elems) {
      auto result = wrap(T{});
      for (auto elem : elems) result->push_back(elem);
      return result;
    }

    node<string> loads_tree(const string& raw, char begin = '(', char end = ')');
    string dumps_tree(node<string> ptr, char begin = '(', char end = ')');
  }
}
