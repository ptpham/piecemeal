#pragma once

#include <stdint.h>

#include <vector>
#include <array>

#include <numeric>
#include <unordered_set>
#include <unordered_map>

#include "hashes.h"
#include "stdaux.h"

namespace piecemeal {
  namespace logic {
    using namespace std;

    template <class T, size_t N>
    T empty() { return numeric_limits<T>::max(); }

    template <class T, size_t N>
    array<T,N> empty_array() { return stdaux::filled_array<T,N>(empty<T,N>());}

    template <class T, size_t N>
    bool check_distinct(const array<T,N>& distinct, const array<T,N>& target) {
      for (size_t i = 0; i < target.size(); i++) {
        if (distinct[i] < target.size()
          && target[distinct[i]] == target[i]) return false;
      }
      return true;
    }

    template <class T, size_t N>
    bool is_grounded(const array<T,N>& a) {
      for (auto v : a) { if (v == empty<T,N>()) return false; } return true;
    }

    template <class T, size_t N>
    bool is_blank(const array<T,N>& a) {
      for (auto v : a) { if (v != empty<T,N>()) return false; } return true;
    }

    template <class T, size_t N>
    array<T,N> transfer(const array<T,N>& dst,
      const array<T,N>& trans, const array<T,N>& src) {
      auto result = dst;

      for (size_t i = 0; i < N; i++) {
        auto j = trans[i];
        if (j == empty<T,N>() || j > src.size()) continue;
        if (result[i] != empty<T,N>() && result[i] != src[j]) {
          return empty_array<T,N>();
        }
        result[i] = src[j];
      }
      return result;
    }

    template <class T, size_t N>
    auto invert(const array<T,N>& trans) {
      auto size = *stdaux::max_element_nullable(
        trans.begin(), trans.end(), empty<T,N>());
      if (size == empty<T,N>()) return trans;
      auto result = empty_array<T,N>();
      for (T i = 0; i < trans.size(); i++) {
        if (trans[i] != empty<T,N>()) result[trans[i]] = i;
      }
      return result;
    }

    template <class T, size_t N>
    struct term {
      array<T,N> literal, push, pull;

      term() { }
      term(const array<T,N>& a) : literal(a),
        push(array<T,N>(a.id(),0)), pull(array<T,N>(a.id(),0)) { }
      term(const array<T,N>& a, const array<T,N>& push, const array<T,N>& pull) :
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
      typedef unordered_map<array<T,N>, unordered_set<array<T,N>>> query_index;
      unordered_set<array<T,N>> known;
      stdaux::bitvector completed;
      query_index index;
    };

    template <class T, size_t N>
    const unordered_set<array<T,N>>& ask(const vector<vector<rule<T,N>>>& ruleset,
      const array<T,N>& query, askstate<T,N>& state);

    template <class T, size_t N>
    vector<vector<rule<T,N>>> index_by_position(
      const vector<rule<T,N>>& rules, int pos = 0) {
      auto size = stdaux::max_extraction(rules.begin(), rules.end(), 
        [=] (auto& rule) { return rule.head.literal[pos]; }) + 1;
      vector<vector<rule<T,N>>> result(size);
      for (auto& rule : rules) result[rule.head.literal[pos]].push_back(rule);
      return result; 
    }
  }
}

namespace std {
  template <class T, size_t N>
  struct hash<array<T,N>> {
    size_t operator() (const array<T,N>& a) const {
      return piecemeal::hashes::sdbm(a.begin(), a.end());
    }
  };
}

