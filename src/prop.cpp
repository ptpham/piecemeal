#include "piecemeal/prop.hpp"
#include "piecemeal/stdaux.hpp"
#include "private/logic_export.hpp"

namespace piecemeal {
  namespace logic {
    template <class T, size_t N>
    prop<T,N> invert(const prop<T,N>& trans) {
      auto size = *stdaux::max_element_nullable(
        trans.begin(), trans.end(), prop<T,N>::empty());
      if (size == prop<T,N>::empty()) return trans;
      prop<T,N> result;
      for (T i = 0; i < trans.size(); i++) {
        if (trans[i] != prop<T,N>::empty()) result[trans[i]] = i;
      }
      return result;
    }

    template <class T, size_t N>
    vector<vector<prop<T,N>>> index_by_position(const vector<prop<T,N>>& props,
      const map<T,size_t>& coalesce, size_t position) {
      vector<vector<prop<T,N>>> result;
      for (auto& p : props) {
        auto found = coalesce.find(p[position]);
        if (found == coalesce.end()) continue;
        auto placement = found->second;
        if (result.size() <= placement) result.resize(placement + 1);
        result[placement].push_back(p);
      }
      return result;
    }

#define EXPORT(T,N) \
    template vector<vector<prop<T,N>>> index_by_position(\
      const vector<prop<T,N>>& props, const map<T,size_t>& coalesce, \
      size_t position); \
    template prop<T,N> invert(const prop<T,N>&);
    DEFAULT_LOGIC_EXPORT
#undef EXPORT
  }
}

