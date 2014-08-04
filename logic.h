#pragma once

#include <stdint.h>

#include <vector>
#include <array>

#include <numeric>
#include <unordered_set>
#include <unordered_map>

#include "hashes.h"
#include "stdaux.h"
#include "isa.h"

namespace piecemeal {
  namespace logic {
    using namespace std;

    template <class T, size_t N>
    bool check_distinct(const array<T,N>& distinct, const isa<T,N>& target) {
      for (size_t i = 0; i < target.size(); i++) {
        if (distinct[i] < target.size()
          && target[distinct[i]] == target[i]) return false;
      }
      return true;
    }

    template <class T, size_t N>
    bool is_grounded(const isa<T,N>& a) {
      for (auto v : a) { if (v == isa<T,N>::empty()) return false; } return true;
    }

    template <class T, size_t N>
    bool is_blank(const isa<T,N>& a) {
      for (auto v : a) { if (v != isa<T,N>::empty()) return false; } return true;
    }

    template <class T, size_t N>
    isa<T,N> transfer(const isa<T,N>& dst,
      const isa<T,N>& trans, const isa<T,N>& src) {
      auto result = dst.clone();
      T size = min(trans.size(), result.size());
      for (size_t i = 0; i < size; i++) {
        auto j = trans[i];
        if (j == isa<T,N>::empty() || j > src.size()) continue;
        if (result[i] != isa<T,N>::empty() && result[i] != src[j]) {
          return isa<T,N>::null();
        }
        result[i] = src[j];
      }
      return result;
    }

    template <class T, size_t N>
    auto invert(const isa<T,N>& trans) {
      auto empty = isa<T,N>::empty();
      auto size = *stdaux::max_element_nullable(
        trans.begin(), trans.end(), empty);
      if (size == empty) return trans;
      auto result = trans.frame().shade();
      for (T i = 0; i < trans.size(); i++) {
        if (trans[i] != empty) result[trans[i]] = i;
      }
      return result;
    }

    template <class T, size_t N>
    struct term {
      isa<T,N> literal, push, pull;

      term() { }
      term(const isa<T,N>& a) : literal(a),
        push(isa<T,N>(a.id(),0)), pull(isa<T,N>(a.id(),0)) { }
      term(const isa<T,N>& a, const isa<T,N>& push, const isa<T,N>& pull) :
        literal(a), push(push), pull(pull) { }
    };

    template <class T, size_t N>
    struct rule {
      term<T,N> head;
      vector<term<T,N>> positives, negatives;
      vector<array<T,N>> distincts;
    };

    template <class T, size_t N>
    struct askstate {
      typedef unordered_map<isa<T,N>, unordered_set<isa<T,N>>> query_index;
      unordered_set<isa<T,N>> known;
      stdaux::bitvector completed;
      query_index index;
    };

    template <class T, size_t N>
    const unordered_set<isa<T,N>>& ask(const vector<vector<rule<T,N>>>& ruleset,
      const isa<T,N>& query, askstate<T,N>& state);

    template <class T, size_t N>
    vector<vector<rule<T,N>>> index_by_head_id(const vector<rule<T,N>>& rules) {
      auto size = stdaux::max_extraction(rules.begin(), rules.end(), 
        [] (auto& rule) { return rule.head.literal.id(); }) + 1;
      vector<vector<rule<T,N>>> result(size);
      for (auto& rule : rules) result[rule.head.literal.id()].push_back(rule);
      return result; 
    }
  }
}

namespace std {
  template <class T, size_t N>
  struct hash<piecemeal::isa<T,N>> {
    size_t operator() (const piecemeal::isa<T,N>& a) const {
      return piecemeal::hashes::sdbm(&a.id(), &a[a.size()]);
    }
  };
}

