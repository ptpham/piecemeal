
#include "logic.h"

namespace piecemeal {
  namespace logic {
    using namespace std;

    template <class I, class T, size_t N>
    const unordered_set<array<T,N>>& ask(const I& index, 
      const array<T,N>& query, askstate<T,N>& state) {
      static const unordered_set<array<T,N>>& empty = {};
      if (is_blank(query)) return empty;

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
      auto& grounds = known.grounds;
      if (is_grounded(query)) {
        if (grounds.find(query) != grounds.end()) result.emplace(query);
        auto& asked = ask(index, index.parent(query), state);
        if (asked.find(query) != asked.end()) result.emplace(query);
        return result;
      }

      // Check for grounds that have been explicitly specified
      for (auto& ground : grounds) {
        if (check_conflict(query, ground)) result.emplace(ground);
      }

      // Recursively sastisfy each of the positive terms. Once all terms are
      // satisfied, we check the negative terms and the distinct constraints.
      for (auto& rule : known.rules) {
        auto& head = rule.head;
        auto space = transfer(logic::empty_array<T,N>(), head.pull, query);
        function<void (decltype(space),size_t)> satisfy =
          [&](decltype(space) current, size_t i) {
          if (i == rule.positives.size()) {
            for (auto& distinct : rule.distincts) {
              if (!check_distinct(distinct, current)) return;
            }

            for (auto& negative : rule.negatives) {
              auto next = transfer(negative.literal, negative.push, current);
              if (ask(index, next, state).size() > 0) return;
            }

            auto ground = transfer(head.literal, head.push, current);
            result.emplace(ground);
          } else {
            auto& positive = rule.positives[i];
            auto next = transfer(positive.literal, positive.push, current);
            for (auto& entry : ask(index, next, state)) {
              auto next = transfer(current, positive.pull, entry);
              if (is_blank(next)) continue;
              satisfy(next , i + 1);
            }
          }
        };
        satisfy(space, 0);
      }

      return result;
    }

    template const unordered_set<array<uint8_t,8>>& ask(
      const prefix_index<uint8_t,8>& index, const array<uint8_t,8>& query,
      askstate<uint8_t,8>& state);
  }
}

