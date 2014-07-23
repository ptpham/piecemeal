#include <stdint.h>

#include <vector>
#include <array>

#include <numeric>
#include <unordered_set>
#include <unordered_map>

#include "hashes.h"

namespace piecemeal {
  namespace logic {
    using namespace std;

    template <class T, size_t N>
    struct isa : array<T,N> {
      isa() : array<T,N>({0, 0}) { }
      isa(T id, T size) : array<T,N>({id, size}) { }
      isa(T id, const initializer_list<T>& list) :array<T,N>({id,(T)list.size()}) {
        T i = 0; for (auto& t : list) (*this)[i++] = t;
      }

      static T empty() { return numeric_limits<T>::max(); }
      static isa null() { return isa(empty(), 0); }

      isa frame() const { return isa(id(), size()); }
      isa shadow() const { return frame().fill(empty()); }
      isa clone() const { isa result = *this; return result; }
      isa& fill(T value) { for (auto& v: *this) v = value; return *this; }
      T& operator[](size_t i) { return array<T,N>::operator[](i+2); }
      T& id() { return array<T,N>::operator[](0); }

      T* begin() { return &array<T,N>::operator[](2); }
      T* end() { return begin() + size(); }

      bool valid() const { return this->id() != empty(); }

      const T& operator[](size_t i) const { return array<T,N>::operator[](i+2); }
      const T& size() const { return array<T,N>::operator[](1); }
      const T& id() const { return array<T,N>::operator[](0); }

      const T* begin() const { return &array<T,N>::operator[](2); }
      const T* end() const { return begin() + size(); }

      static constexpr size_t extent = N - 2;
    };

    template <class T, size_t N>
    bool check_distinct(const isa<T,N>& distinct, const isa<T,N>& target) {
      for (size_t i = 0; i < distinct.size() && i < target.size(); i++) {
        if (distinct[i] < target.size()
          && target[distinct[i]] == target[i]) return false;
      }
      return true;
    }

    template <class T, size_t N>
    bool grounded(const isa<T,N>& a) {
      for (auto v : a) { if (v == isa<T,N>::empty()) return false; } return true;
    }

    template <class T, size_t N>
    isa<T,N> transfer(const isa<T,N>& dst,
      const isa<T,N>& trans, const isa<T,N>& src) {
      auto result = dst.clone();
      T size = min(trans.size(), result.size());
      for (size_t i = 0; i < size; i++) {
        auto j = trans[i];
        if (j == isa<T,N>::empty()) continue;
        if (result[i] != isa<T,N>::empty() && result[i] != src[j]) {
          return isa<T,N>::null();
        }
        result[i] = src[j];
      }
      return result;
    }

    template <class T, size_t N>
    struct term {
      isa<T,N> value, push, pull;

      term() { }
      term(const isa<T,N>& a) : value(a), push(a.shadow()), pull(a.shadow()) { }
    };

    template <class T, size_t N>
    struct rule {
      term<T,N> head;
      vector<term<T,N>> positives, negatives;
      vector<isa<T,N>> distincts;
    };

    template <class T, size_t N>
    struct askstate {
      typedef unordered_map<isa<T,N>, unordered_set<isa<T,N>>> query_index;
      typedef vector<unordered_set<isa<T,N>>> knowledge;
      query_index index;
      knowledge known;
    };

    template <class T, size_t N>
    const unordered_set<isa<T,N>>& ask(const vector<vector<rule<T,N>>>& ruleset,
      const isa<T,N>& query, askstate<T,N>& state);
  }
}

namespace std {
  template <class T, size_t N>
  struct hash<piecemeal::logic::isa<T,N>> {
    size_t operator() (const piecemeal::logic::isa<T,N>& a) const {
      return piecemeal::hashes::sdbm(&a.id(), &a[a.size()]);
    }
  };
}

