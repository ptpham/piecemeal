#pragma once

#include <stdint.h>

namespace piecemeal {
  namespace hashes {
    template <class T>  
    size_t sdbm(const T& begin, const T& end) {
      size_t hash = 0;
      for (auto i = begin; i < end; i++) {
        int64_t c = *i;
        hash = c + (hash << 6) + (hash << 16) - hash;
      }   
      return hash;
    };  
  }
}

