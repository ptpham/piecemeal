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
      for (size_t i = 0; i < N; i++) {
        if (distinct[i] == empty<T,N>()) continue;
        auto left = target[i], right = target[distinct[i]];
        if (left == empty<T,N>() || right == empty<T,N>()) continue;
        if (left == right) return false;
      }
      return true;
    }

    template <class T, size_t N>
    bool check_conflict(const array<T,N>& left, const array<T,N>& right) {
      for (size_t i = 0; i < N; i++) {
        auto first = left[i], second = right[i];
        if (first == empty<T,N>() || second == empty<T,N>()) continue;
        if (first != second) return false;
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
    array<T,N> invert(const array<T,N>& trans) {
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
        push(empty_array<T,N>()), pull(empty_array<T,N>()) { }
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
    struct knowledge {
      vector<rule<T,N>> rules;
      unordered_set<array<T,N>> grounds;
    };

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

    template <class T, size_t N>
    using askstate = unordered_map<array<T,N>, unordered_set<array<T,N>>>;

    template <class I, class T, size_t N>
    const unordered_set<array<T,N>>& ask(const I& index,
      const array<T,N>& query, askstate<T,N>& state);
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

