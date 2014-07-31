#pragma once

#include <vector>
#include <string>

namespace piecemeal {
  namespace dag {
    using namespace std;

    template <class T> struct elem : vector<shared_ptr<elem<T>>> { T value; };
    template <class T> using cnode = shared_ptr<const elem<T>>;
    template <class T> using node = shared_ptr<elem<T>>;

    template <class T>
    node<T> wrap(const T& value) {
      elem<T>* ptr = new elem<T>();
      ptr->value = value;
      return node<T>(ptr);
    }

    node<string> loads(const string& raw, char begin = '(', char end = ')');
    string dumps(node<string> ptr, char begin = '(', char end = ')');
  }
}

namespace piecemeal {
  namespace dag {
    namespace traverse {
      template <class T, class F>
      void depth(T node, F fn) {
        vector<T> stack;
        stack.reserve(128);
        stack.push_back(node);
        while (stack.size() > 0) {
          auto next = stack.back();
          stack.pop_back();
          fn(next);
          for (auto& child : *next) stack.push_back(child);
        }
      }

      template <class T, class F>
      void breadth(T node, F fn) {
        vector<decltype(node)> cur, next;
        next.reserve(128);
        cur.reserve(128);
        cur.push_back(node);

        while (cur.size() > 0) {
          for (auto child : cur) {
            fn(child);
            for (auto grandchild : *child) {
              next.push_back(grandchild);
            }
          }
          swap(cur, next);
          next.clear();
        }
      }
    }
  }
}

namespace piecemeal {
  namespace dag {
    namespace gather {
      template <class T>
      auto leaves(T node) {
        vector<T> result;
        traverse::breadth(node, [&](T child) {
          if (child->size() > 0) return;
          result.push_back(child);
        });
        return result;
      }
    }
  }
}



