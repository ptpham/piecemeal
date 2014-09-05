#pragma once

#include <stdint.h>

#include <vector>
#include <array>

#include <numeric>
#include <unordered_set>
#include <unordered_map>

#include "logic.h"

namespace piecemeal {
  namespace logic {
    using namespace std;
    template <class T, size_t N>
    struct position_index : vector<knowledge<T,N>> {
      const knowledge<T,N> empty = {};
      size_t pos = 0;

      position_index() { }
      position_index(size_t pos) : pos(pos) { }

      template <class S>
      position_index(const S& scope, size_t pos = 0) {
        this->emplace_rules(scope.rules);
        this->emplace_props(scope.props);
      }

      void clear() { for (auto& k : *this) k.clear(); }

      knowledge<T,N>& assure(size_t id) {
        if (id >= this->size()) this->resize(id + 1);
        return this->at(id);
      }

      prop<T,N> parent(const prop<T,N>& ground) const {
        prop<T,N> result;
        result[pos] = ground[pos];
        return result;
      }

      void emplace(const prop<T,N>& ground) {
        assure(ground[pos]).grounds.emplace(ground);
      }

      template <class C>
      void emplace_props(const C& props) {
        for (auto& prop : props) emplace(prop);
      }

      void emplace(const rule<T,N>& rule) {
        assure(rule.head.literal[pos]).rules.push_back(rule);
      }

      template <class C>
      void emplace_rules(const C& rules) {
        for (auto& rule : rules) emplace(rule);
      }

      const knowledge<T,N>& operator[] (const prop<T,N>& query) const {
        if (query[pos] >= this->size()) return empty;
        else return this->at(query[pos]);
      }
    };
  }
}
