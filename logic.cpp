
#include "logic.h"

#include <iostream>

namespace piecemeal {
  namespace logic {
    using namespace std;

    namespace std {
      template <class T> T begin(const pair<T,T>& p) { return p.first; }
      template <class T> T end(const pair<T,T>& p) { return p.second; }
    };

    template <class I, class T, size_t N>
    const unordered_set<array<T,N>>& _ask(const I& index, 
      const array<T,N>& query, askstate<T,N>& state) {
      static const unordered_set<array<T,N>>& empty = {};
      cout << "Query: ";
      for (auto a : query) cout << (int)a << " ";
      cout << endl;
      if (is_blank(query)) return empty;

      auto found = state.find(query);
      if (found != state.end()) return found->second;
      auto& result = state[query];

      auto& known = index[query];
      auto& grounds = known.grounds;
      if (is_grounded(query)) {
        if (grounds.find(query) != grounds.end()) result.emplace(query);
        auto& asked = _ask(index, index.parent(query), state);
        if (asked.find(query) != asked.end()) result.emplace(query);
        return result;
      }

      cout << "Checking " << known.grounds.size() << " grounds." << endl;
      for (auto& ground : grounds) {
        if (check_conflict(query, ground)) result.emplace(ground);
      }

      cout << "Checking " << known.rules.size() << " rules." << endl;
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
              if (_ask(index, next, state).size() > 0) return;
            }

            auto ground = transfer(head.literal, head.push, current);
            result.emplace(ground);
          } else {
            auto& positive = rule.positives[i];
            auto next = transfer(positive.literal, positive.push, current);
            for (auto& entry : _ask(index, next, state)) {
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

    template <class T, size_t N>
    const unordered_set<array<T,N>>& ask(const prefix_index<T,N>& index,
      const array<T,N>& query, askstate<T,N>& state) {
      return _ask(index, query, state);
    }

    template const unordered_set<array<uint8_t,8>>& ask(
      const prefix_index<uint8_t,8>& index, const array<uint8_t,8>& query,
      askstate<uint8_t,8>& state);
  }
}

