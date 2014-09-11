
#include "logic.hpp"
#include "position_index.hpp"

#include <iostream>
using namespace std;

namespace piecemeal {
  namespace logic {
    using namespace std;

    template <class T, size_t N>
    bool check_conflict(const prop<T,N>& left, const prop<T,N>& right) {
      auto empty = prop<T,N>::empty();
      for (size_t i = 0; i < N; i++) {
        auto first = left[i], second = right[i];
        if (first == empty || second == empty) continue;
        if (first != second) return false;
      }
      return true;
    }

    template <class T, size_t N>
    prop<T,N> transfer(const prop<T,N>& dst,
      const prop<T,N>& trans, const prop<T,N>& src) {
      auto result = dst;

      for (size_t i = 0; i < N; i++) {
        auto j = trans[i];
        if (j == prop<T,N>::empty() || j > src.size()) continue;
        if (result[i] != prop<T,N>::empty() && result[i] != src[j]) {
          return prop<T,N>();
        }
        result[i] = src[j];
      }
      return result;
    }

    template <class T, size_t N>
    prop<T,N> invert(const prop<T,N>& trans) {
      auto size = *stdaux::max_element_nullable(
        trans.begin(), trans.end(), prop<T,N>::empty());
      if (size == prop<T,N>::empty()) return trans;
      prop<T,N> result;
      for (T i = 0; i < trans.size(); i++) {
        if (trans[i] != prop<T,N>::empty()) result[trans[i]] = i;
      }
      return result;
    }

    template <class I, class T, size_t N>
    const unordered_set<prop<T,N>>& ask(const I& index, 
      const prop<T,N>& query, askstate<T,N>& state) {
      static const unordered_set<prop<T,N>>& empty = {};
      if (query.is_blank()) return empty;

      // Check in the cache -- this check also serves the purpose of preventing
      // infinite loops on improperly grounded recursive rules
      auto found = state.find(query);
      if (found != state.end()) return found->second;
      auto& result = state[query];

      // If this is a grounded query, we can do a find instead of a filtration
      // on the set of grounds in the known scope. In order to check for a
      // negation, we ask the index to come up with a query that is "the next
      // level up" in abstraction and check for the ground in that set.
      auto& known = index[query];
      auto& props = known.props;
      if (query.is_grounded()) {
        if (props.find(query) != props.end()) result.emplace(query);
        auto& asked = ask(index, index.parent(query), state);
        if (asked.find(query) != asked.end()) result.emplace(query);
        return result;
      }

      // Check for props that have been explicitly specified
      for (auto& prop : props) {
        if (check_conflict(query, prop)) result.emplace(prop);
      }

      // Recursively sastisfy each of the positive terms. Once all terms are
      // satisfied, we check the negative terms and the distinct constraints.
      for (auto& rule : known.rules) {
        auto& head = rule.head;
        auto space = transfer(prop<T,N>(), head.pull, query);
        function<void (decltype(space),size_t)> satisfy =
          [&](decltype(space) current, size_t i) {
          if (i == rule.positives.size()) {
            for (auto& distinct : rule.distincts) {
              if (current[distinct.first] == current[distinct.second]) return;
            }
            
            for (auto& prevent : rule.prevents) {
              if (current[prevent.first] == prevent.second) return;
            }

            for (auto& negative : rule.negatives) {
              auto next = transfer(negative.literal, negative.push, current);
              if (ask(index, next, state).size() > 0) return;
            }

            auto prop = transfer(head.literal, head.push, current);
            if (!prop.is_grounded()) return;
            result.emplace(prop);
          } else {
            auto& positive = rule.positives[i];
            auto next = transfer(positive.literal, positive.push, current);
            for (auto& entry : ask(index, next, state)) {
              bool empty_pull = positive.pull.is_blank();
              auto next = empty_pull ? current
                : transfer(current, positive.pull, entry);
              if (!empty_pull && next.is_blank()) continue;
              satisfy(next, i + 1);
            }
          }
        };
        satisfy(space, 0);
      }

      return result;
    }

#define EXPORT(T,N) \
    template const unordered_set<prop<T,N>>& ask( \
      const position_index<T,N>& index, const prop<T,N>& query, \
      askstate<T,N>& state); \
    template prop<T,N> invert(const prop<T,N>&);

    EXPORT(uint8_t,8)
    EXPORT(uint16_t,8)
#undef EXPORT
  }
}

