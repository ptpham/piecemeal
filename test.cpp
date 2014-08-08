
#include <iostream>
#include <map>
#include <set>

#include "logic.h"
#include "kif.h"
#include "dag.h"

using namespace std;
using namespace piecemeal;
using namespace piecemeal::logic;

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

template <class T, size_t N>
ostream& operator << (ostream& out, const term<T,N>& t) {
  return out << t.literal << " <- " << t.push << " -> " << t.pull;
}


template <class T>
vector<vector<T>> cartesian(const vector<vector<T>>& original) {
  size_t dim = original.size();
  vector<vector<T>> result;
  if (dim == 0) return result;
  for (auto& v : original) if (v.size() == 0) return result;

  size_t pos[dim];
  fill(pos, pos + dim, 0);
  vector<T> cur(dim);

  while (pos[0] < original[0].size()) {
    // Store current position into result
    for (size_t i = 0; i < dim; i++) cur[i] = original[i][pos[i]];
    result.push_back(cur);

    // Find the next valid position
    for (size_t i = 1; i <= dim; i++) {
      size_t j = dim - i;
      if (++pos[j] < original[j].size()) break;
      if (j > 0) pos[j] = 0;
    }
  }

  return result;
}

namespace piecemeal {
  namespace gdl {

    vector<dag::node<string>> deor_sentence(dag::node<string> original) {
      vector<vector<dag::node<string>>> space;
      for (auto child : *original) {
        decltype(space)::value_type cur;
        if (child->size() > 0 && child->at(0)->value == "or") {
          for (size_t i = 1; i < child->size(); i++) {
            cur.push_back(child->at(i));
          }
        } else cur.push_back(child);
        space.push_back(cur);
      }

      vector<dag::node<string>> result;
      auto rendered = cartesian(space);
      for (auto& elem : rendered) result.push_back(dag::convert(elem));
      return result; 
    }

    dag::node<string> deor_sentences(dag::cnode<string> sentences) {
      auto result = dag::wrap<string>("");
      for (auto sentence : *sentences) {
        for (auto child : deor_sentence(sentence)) result->push_back(child);
      }
      return result;
    }

    dag::node<string> unify_relation_structure(dag::cnode<string> sentences) {
      dag::node<string> result;

      return result;
    }

  }
}

int main(int nargs, char** argv) {
  string raw_string = "(<= (q ?x) (p ?x))(r bloop) (<= (p ?y) (r ?y)) \
    (<= (p awesome) (or (r blip) (r blim) (r bloop)))";
  auto parsed = dag::loads_tree(raw_string);
  parsed = gdl::deor_sentences(parsed);
  cout << dag::dumps_tree(parsed) << endl;
  auto scope = kif::parse_sentences<uint8_t, 8>(parsed);
  logic::prefix_index<uint8_t, 8> index;

  for (auto& rule : scope.rules) {
    cout << rule.head << endl;
    for (auto term : rule.positives) cout << "\t" << term << endl;
    index.emplace(rule);
  }

  for (auto& ground : scope.grounds) {
    index.emplace(ground);
  }

  askstate<uint8_t, 8> state;
  auto proved = ask(index, scope.rules[0].head.literal, state);

  size_t total = 0; 
  for (auto& cache : state) total += cache.second.size();
  cout << "Proved " << total << " proposition(s)." << endl;
  return 0;
}

