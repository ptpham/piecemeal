#pragma once

#include <stdint.h>

#include <vector>
#include <array>
#include <map>

#include <iostream>

#include "piecemeal/hashes.hpp"
#include "piecemeal/stdaux.hpp"
#include "piecemeal/prop.hpp"

namespace piecemeal {
  namespace logic {
    using namespace std;

    template <class T, size_t N>
    struct term {
      prop<T,N> literal, push, pull;

      term() { }
      term(const prop<T,N>& a) : literal(a) { }
      term(const prop<T,N>& a, const prop<T,N>& push, const prop<T,N>& pull) :
        literal(a), push(push), pull(pull) { }
    };

    template <class T, size_t N>
    struct rule {
      term<T,N> head;
      vector<term<T,N>> positives, negatives;
      vector<pair<T,T>> distincts, prevents;
    };

  }
}

