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
    struct position_index : vector<knowledge<T,N>> {
      const knowledge<T,N> empty = {};
      size_t pos = 0;

      position_index() { }
      position_index(size_t pos) : pos(pos) { }

      knowledge<T,N>& assure(size_t id) {
        if (id >= this->size()) this->resize(id + 1);
        return this->at(id);
      }

      unit<T,N> parent(const unit<T,N>& ground) const {
        unit<T,N>result = empty_unit<T,N>();
        result[pos] = ground[pos];
        return result;
      }

      void emplace(const unit<T,N>& ground) {
        assure(ground[pos]).grounds.emplace(ground);
      }

      void emplace(const rule<T,N>& rule) {
        assure(rule.head.literal[pos]).rules.push_back(rule);
      }

      const knowledge<T,N>& operator[] (const unit<T,N>& query) const {
        if (query[pos] >= this->size()) return empty;
        else return this->at(query[pos]);
      }
    };
  }
}
