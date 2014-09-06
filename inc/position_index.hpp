#pragma once

#include <stdint.h>

#include <vector>
#include <array>

#include <numeric>
#include <unordered_set>
#include <unordered_map>

#include "logic.hpp"

namespace piecemeal {
  namespace logic {
    using namespace std;
    template <class T, size_t N>
    struct position_index : vector<knowledge<T,N>> {
      const knowledge<T,N> empty = {};
      size_t pos = 0;

      position_index() { }
      position_index(size_t pos) : pos(pos) { }
      void clear() { for (auto& k : *this) k.clear(); }

      knowledge<T,N>& assure(size_t id) {
        if (id >= this->size()) this->resize(id + 1);
        return this->at(id);
      }

      prop<T,N> parent(const prop<T,N>& p) const {
        prop<T,N> result;
        result[pos] = p[pos];
        return result;
      }

      void emplace(const prop<T,N>& p) { assure(p[pos]).props.emplace(p); }
      void emplace(const rule<T,N>& rule) {
        assure(rule.head.literal[pos]).rules.push_back(rule);
      }

      template <class K>
      void emplace_knowledge(const K& scope) {
        for (auto& rule : scope.rules) emplace(rule);
        for (auto& prop : scope.props) emplace(prop);
      }

      const knowledge<T,N>& operator[] (const prop<T,N>& query) const {
        if (query[pos] >= this->size()) return empty;
        else return this->at(query[pos]);
      }
    };
  }
}
