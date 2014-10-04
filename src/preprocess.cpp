
#include <map>
#include <iostream>
#include <unordered_set>
#include <unordered_map>

#include "piecemeal/preprocess.hpp"
#include "piecemeal/cartesian.hpp"

#include "piecemeal/stdfmt.hpp"

using namespace std;

namespace piecemeal {
  namespace preprocess {
  
    vector<string> remove_comments(const vector<string>& raw) {
      vector<string> result;
      
      for (auto& line : raw) {
        auto index = line.find(';');
        if (index == -1) result.push_back(line);
        else result.push_back(line.substr(0, index));
      }
      
      return result;
    }

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
      for (auto& elem : make_cartesian(space)) {
        result.push_back(dag::convert(elem));
      }
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
        if (other->size() + 1 == canon->size()) other->push_back(created);
        else other->push_back(dag::wrap<string>(""));
      }
      for (size_t i = 0; i < other->size(); i++) {
        canonize_term(canon->at(i), other->at(i));
      }
    }

    void canonize_sentences(const vector<dag::node<string>>& sentences) {
      if (sentences.size() == 0) return;
      map<string, dag::node<string>> index;
      vector<dag::node<string>> terms;
      unordered_set<string> keywords = { "not", "base", "input"
        "init", "true", "next", "legal", "does" };

      // Gather all terms for iteration
      for (size_t i = 0; i < sentences.size(); i++) {
        for (auto child : *sentences[i]) {
          if (child->size() < 1) continue;
          auto& value = child->at(0)->value;
          if (keywords.find(value) != keywords.end()) {
            auto child_size = child->size();
            if (child_size == 2 || child_size== 3) {
              child = child->at(child_size - 1);
            } else continue;
          }
          while (child->size() == 1) child = child->at(0);
          if (child->size() < 1) continue;
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

    dag::node<string> standard(const vector<string>& raw) {
      string concatenated;
      auto filtered = remove_comments(raw);
      for (auto& line : filtered) concatenated += line;
      auto loaded = dag::loads_tree(concatenated);
      auto deorded = deor_sentences(loaded);
      canonize_sentences(*deorded);
      return deorded;
    }
  }
}


