
#pragma once

#include <vector>
#include <unordered_map>

namespace piecemeal {
  namespace stdaux {
    using namespace std;

    template <class U, class V = size_t>
    struct unordered_dimap {
      vector<U> backward;
      unordered_map<U,V> forward;

      V at(const U& s) {
        auto found = forward.find(s);
        if (found != forward.end()) return found->second;
        V result = backward.size();
        forward.insert(found, {s, result});
        backward.push_back(s);
        return result;
      }
    };
  }
}

