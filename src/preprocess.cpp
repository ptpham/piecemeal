
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

    dag::node<string> standard(const vector<string>& raw) {
      string concatenated;
      auto filtered = remove_comments(raw);
      for (auto& line : filtered) concatenated += line;
      auto loaded = tree::loads(concatenated);
      auto deorded = deor_sentences(loaded);
      return deorded;
    }
  }
}


