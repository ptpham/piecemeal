
#include <iostream>
#include <map>
#include <set>

#include "kif.h"
#include "dag.h"
#include "gdl.h"

#include "logic.h"
#include "logic/prefix_index.h"

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

/*
    template <class T, size_t N>
    ostream& operator <<(ostream& out, const array<T,N>& t) {
      out << "[";
      for (size_t i = 0; i < N; i++) {
        if (i > 0) out << " ";
        out << (int)t[i];
      }
      return out << "]";
    }
*/

template <class T, size_t N>
ostream& operator << (ostream& out, const term<T,N>& t) {
  return out << t.literal << " <- " << t.push << " <- " << t.pull;
}

int main(int nargs, char** argv) {
  string raw_string = "(<= (z ?x) (q ?x)) (<= (q ?x) (p ?x))(r bloop) (<= (p ?y) (r ?y)) \
    (<= (p awesome) (or (r blip) (r blim) (r bloop))) (<= (z (f ?x)) (t ?x))";
  auto parsed = dag::loads_tree(raw_string);
  parsed = gdl::deor_sentences(parsed);
  gdl::canonize_sentences(*parsed);
  cout << dag::dumps_tree(parsed) << endl;
  auto scope = kif::parse_sentences<uint8_t, 8>(parsed);
  logic::prefix_index<uint8_t, 8> index;

  for (auto& rule : scope.rules) {
    cout << rule.head << endl;
    for (auto term : rule.positives) cout << "\t" << term << endl;
    index.emplace(rule);
  }

  for (auto& ground : scope.grounds) {
    cout << "Ground: " << ground << endl;
    index.emplace(ground);
  }

  askstate<uint8_t, 8> state;
  auto proved = ask(index, scope.rules[0].head.literal, state);

  size_t total = 0; 
  cout << "Proved: " << endl;
  for (auto& cache : state) {
    total += cache.second.size();
    for (auto elem : cache.second)  cout << elem << endl;
  }
  cout << "Proved " << total << " proposition(s)." << endl;
  return 0;
}

