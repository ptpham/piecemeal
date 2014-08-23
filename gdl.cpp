
#include <map>

#include "gdl.h"

using namespace std;

template <class T>
static vector<vector<T>> cartesian(const vector<vector<T>>& original) {
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

    dag::node<string> deor_sentences(const dag::node<string> sentences) {
      auto result = dag::wrap<string>("");
      for (auto sentence : *sentences) {
        for (auto child : deor_sentence(sentence)) result->push_back(child);
      }
      return result;
    }

    void union_structure(dag::node<string>& canon, dag::cnode<string> other) {
      if (other == nullptr) return;
      if (canon == nullptr) canon = dag::wrap(string{});
      while (canon->size() < other->size()) {
        auto target = other->at(canon->size());
        canon->push_back(dag::wrap(string{}));
      }
      for (size_t i = 0; i < other->size(); i++) {
        union_structure(canon->at(i), other->at(i));
      }
    }

    void canonize_term(dag::cnode<string> canon, dag::node<string>& other) {
      if (canon == nullptr) return;
      if (other == nullptr) other = dag::wrap<string>("");
      while (other->size() < canon->size()) {
        auto created = dag::wrap<string>(other->value);
        other->push_back(created);
      }
      for (size_t i = 0; i < other->size(); i++) {
        canonize_term(canon->at(i), other->at(i));
      }
    }

    void canonize_sentences(const vector<dag::node<string>>& sentences) {
      if (sentences.size() == 0) return;
      map<string, dag::node<string>> index;
      vector<dag::node<string>> terms;

      // Gather all terms for iteration
      for (size_t i = 0; i < sentences.size(); i++) {
        for (auto child : *sentences[i]) {
          if (child->size() < 2) continue;
          terms.push_back(child);
        }
      }

      // Construct canonical versions of each relation
      for (auto term : terms) {
        union_structure(index[term->at(0)->value], term);
      }

      // Canonize all relations in all sentences
      for (auto term : terms) {
        canonize_term(index[term->at(0)->value], term);
      }
    }
  }
}





