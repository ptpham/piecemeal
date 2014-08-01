
#include "logic.h"

namespace piecemeal {
  namespace logic {
    using namespace std;

    namespace std {
      template <class T> T begin(const pair<T,T>& p) { return p.first; }
      template <class T> T end(const pair<T,T>& p) { return p.second; }
    };

    template <class T, size_t N>
    const unordered_set<isa<T,N>>& ask(const vector<vector<rule<T,N>>>& ruleset,
      const isa<T,N>& query, askstate<T,N>& state) {
      static const unordered_set<isa<T,N>>& empty = {};
      static const unordered_set<isa<T,N>>& identity = { isa<T,N>::null() };
      if (!query.valid()) return empty;
      
      if (is_grounded(query)) {
        auto& known = state.known;
        if (known.find(query) != known.end()) return identity;
        if (state.completed.contains(query.id())) return empty;
      }

      auto& index = state.index;
      auto found = index.find(query);
      if (found != index.end()) return found->second;
      unordered_set<isa<T,N>>& result = index[query];

      for (auto& rule : ruleset[query.id()]) {
        auto& head = rule.head;
        auto space = transfer(head.pull.shadow(), head.pull, head.literal);
        function<void (decltype(space),size_t)> satisfy =
          [&](decltype(space) current, size_t i) {
          if (!current.valid()) return;

          if (i == rule.positives.size()) {
            for (auto& distinct : rule.distincts) {
              if (!check_distinct(distinct, current)) return;
            }
            for (auto& negative : rule.negatives) {
              auto next = transfer(negative.literal, negative.push, current);
              if (ask(ruleset, next, state).size() > 0) return;
            }

            auto proposition = transfer(head.literal, head.push, current);
            state.known.emplace(proposition);
            result.emplace(proposition);
          } else {
            auto& positive = rule.positives[i];
            auto next = transfer(positive.literal, positive.push, current);
            for (auto& entry : ask(ruleset, next, state)) {
              satisfy(transfer(current, positive.pull, entry), i + 1);
            }
          }
        };
        satisfy(space, 0);
      }

      if (is_blank(query)) state.completed.set(query.id(), true);
      return result;
    }

    template const unordered_set<isa<uint8_t,16>>& ask(
      const vector<vector<rule<uint8_t,16>>>& ruleset,
      const isa<uint8_t,16>& query, askstate<uint8_t,16>& state);
  }
}

