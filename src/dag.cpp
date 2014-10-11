#include "piecemeal/dag.hpp"
#include <iostream>

namespace piecemeal {
  namespace tree {
    node<string> loads(const string& raw, char begin, char end) {
      vector<dag::node<string>> stack;
      stack.push_back(dag::wrap<string>(""));
      size_t mark = -1;

      for (size_t i = 0; i < raw.size(); i++) {
        bool is_control = raw[i] == end || raw[i] == begin;
        bool token_end = isspace(raw[i]) || is_control;
        if (token_end && mark != -1) {
          auto value = raw.substr(mark, i - mark);
          stack.back()->push_back(dag::wrap(value));
          mark = -1;
        }
        if (raw[i] == end) stack.pop_back();
        if (raw[i] == begin) {
          auto new_node = dag::wrap<string>("");
          stack.back()->push_back(new_node);
          stack.push_back(new_node);
        } else if (raw[i] != end && !isspace(raw[i]) && mark == -1) {
          mark = i; 
        }
      }
      return stack[0];
    }

    string dumps(node<string> ptr, char begin, char end) {
      string result;
      if (ptr->size() == 0) {
        result += ptr->value;
      } else {
        result += '(';
        for (size_t i = 0; i < ptr->size(); i++) {
          if (i > 0) result += ' ';
          result += dumps(ptr->at(i), begin, end);
        }
        result += ')';
      }
      return result;
    }

    string remove_degree_2(const string& original) {
      auto tree = loads(original);
      return dumps(remove_degree_2(tree));
    }
  }
}
