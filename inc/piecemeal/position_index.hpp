#pragma once

#include <stdint.h>

#include <vector>
#include <array>

#include <numeric>
#include <unordered_set>
#include <unordered_map>

#include "piecemeal/logic.hpp"

namespace piecemeal {
  namespace logic {
    using namespace std;
    template <class T = uint16_t, size_t N = 12>
    struct position_index : vector<knowledge<T,N>> {
      typedef T prop_type;
      static const size_t prop_size = N;

      const knowledge<T,N> empty = {};
      size_t pos = 0;

      position_index() { }
      position_index(size_t pos) : pos(pos) { }
      void clear() { for (auto& k : *this) k.clear(); }
      void clear_props() { for (auto& k : *this) k.props.clear(); }

      knowledge<T,N>& assure(size_t id) {
        if (id >= this->size()) this->resize(id + 1);
        return this->at(id);
      }

      prop<T,N> parent(const prop<T,N>& p) const {
        prop<T,N> result;
        result[pos] = p[pos];
        return result;
      }

      template <class C>
      void emplace_props(const C& props) {
        for (auto& p : props) assure(p[pos]).props.emplace(p);
      }

      template <class C>
      void emplace_rules(const C& rules) {
        for (auto& rule : rules) {
          auto& location = assure(rule.head.literal[pos]).rules;
          location.push_back(rule);
        }
      }

      const knowledge<T,N>& operator[] (const prop<T,N>& query) const {
        if (query[pos] >= this->size()) return empty;
        else return this->at(query[pos]);
      }
    };
  }
}
