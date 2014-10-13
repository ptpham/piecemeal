
#include <map>
#include <iostream>
#include <unordered_set>
#include <unordered_map>

#include "piecemeal/preprocess.hpp"
#include "piecemeal/cartesian.hpp"
#include "piecemeal/compile.hpp"

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

    vector<dag::node<string>> extract_relations(dag::node<string> sentence) {
      vector<shared_ptr<dag::elem<string>>> iteration = *sentence;
      vector<dag::node<string>> result;
      if (sentence->size() == 0) return result;
      if (sentence->at(0)->value != "<=") iteration = { sentence };
      for (auto child : iteration) {
        if (child->size() < 1) continue;
        auto& value = child->at(0)->value;
        if (value == "not") child = child->at(1);
        if (child->size() == 0) continue;
        result.push_back(child);
      }
      return result;
    }

    map<string,dag::node<string>> canonize_term(dag::node<string> canon,
      dag::node<string>& other) {
      map<string,dag::node<string>> result;

      if (canon == nullptr) return result;
      if (other == nullptr) other = dag::wrap<string>("");
      size_t original_size = other->size();

      while (other->size() < canon->size()) {
        bool is_var = compile::is_var(other);
        string value = (is_var || original_size == other->size())
          ? other->value : "";

        if (is_var && value != "") result[value] = canon;
        if (is_var) value += ":" + to_string(other->size());
        other->push_back(dag::wrap<string>(value));
      }

      for (size_t i = 0; i < other->size() && i < canon->size(); i++) {
        for (auto& entry : canonize_term(canon->at(i), other->at(i))) {
          result.insert(entry);
        }
      }

      return result;
    }

    void apply_expansion(const map<string,dag::node<string>>& expansions, 
      dag::node<string> node) {
      auto found = expansions.find(node->value);
      if (found != expansions.end()) canonize_term(found->second, node);
      else for (auto& child : *node) apply_expansion(expansions, child);
    }

    void canonize_sentences(const vector<dag::node<string>>& sentences) {
      if (sentences.size() == 0) return;
      map<string, dag::node<string>> index;
      bool changed = true;

      while (changed) {
        changed = false;

        // Construct canonical versions of each relation globally
        for (auto sentence : sentences) {
          for (auto term : extract_relations(sentence)) {
            canonize_term(term, index[term->at(0)->value]);
          }
        }

        // Canonize relations in all sentences
        for (auto sentence : sentences) {
          map<string,dag::node<string>> expansions;
          for (auto term : extract_relations(sentence)) {
            for(auto& entry : canonize_term(index[term->at(0)->value], term)) {
              expansions.insert(entry);
            }
          }

          // Transfer expansions of variables to all relations in the sentence
          if (expansions.size() == 0) continue;
          for (auto term : extract_relations(sentence)) {
            apply_expansion(expansions, term);      
          }

          changed = true;
        }
      }
    }

    dag::node<string> standard(const vector<string>& raw) {
      string concatenated;
      auto filtered = remove_comments(raw);
      for (auto& line : filtered) concatenated += line;
      auto loaded = tree::loads(concatenated);
      auto deorded = deor_sentences(loaded);
      canonize_sentences(*deorded);
      return deorded;
    }
  }
}


