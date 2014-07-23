
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
      if (!query.valid()) return empty;

      auto& index = state.index;
      auto found = index.find(query);
      if (found != index.end()) return found->second;
      unordered_set<isa<T,N>>& result = index[query];

      for (auto& rule : ruleset[query.id()]) {
        auto& head = rule.head;
        auto space = transfer(head.pull.shadow(), head.pull, head.value);
        function<void (decltype(space),size_t)> satisfy =
          [&](decltype(space) current, size_t i) {
          if (!current.valid()) return;

          if (i == rule.positives.size()) {
            for (auto& distinct : rule.distincts) {
              if (!check_distinct(distinct, current)) return;
            }
            for (auto& negative : rule.negatives) {
              auto next = transfer(negative.value, negative.push, current);
              if (ask(ruleset, next, state).size() > 0) return;
            }
            result.emplace(transfer(head.value, head.push, current));
          } else {
            auto& positive = rule.positives[i];
            auto next = transfer(positive.value, positive.push, current);
            for (auto& entry : ask(ruleset, next, state)) {
              satisfy(transfer(current, positive.pull, entry), i + 1);
            }
          }
        };
        satisfy(space, 0);
      }
      return result;
    }

    template const unordered_set<isa<uint8_t,16>>& ask(
      const vector<vector<rule<uint8_t,16>>>& ruleset,
      const isa<uint8_t,16>& query, askstate<uint8_t,16>& state);
  }
}

