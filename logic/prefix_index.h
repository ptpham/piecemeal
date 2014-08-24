#pragma once

#include <stdint.h>

#include <vector>
#include <array>

#include <numeric>
#include <unordered_set>
#include <unordered_map>

namespace piecemeal {
  namespace logic {
    using namespace std;
    template <class T, size_t N>
    struct prefix_index : vector<knowledge<T,N>> {
      const knowledge<T,N> empty = {};

      knowledge<T,N>& assure(size_t id) {
        if (id >= this->size()) this->resize(id + 1);
        return this->at(id);
      }

      array<T,N> parent(const array<T,N>& ground) const {
        array<T,N>result = empty_array<T,N>();
        result[0] = ground[0];
        return result;
      }

      void emplace(const array<T,N>& ground) {
        assure(ground[0]).grounds.emplace(ground);
      }

      void emplace(const rule<T,N>& rule) {
        assure(rule.head.literal[0]).rules.push_back(rule);
      }

      const knowledge<T,N>& operator[] (const array<T,N>& query) const {
        if (query[0] >= this->size()) return empty;
        else return this->at(query[0]);
      }
    };
  }
}