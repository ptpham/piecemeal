#pragma once

#include <stdint.h>

#include <array>
#include <numeric>

namespace piecemeal {
  template <class T, size_t N>
  struct isa : std::array<T,N> {
    isa() : std::array<T,N>({0, 0}) { }
    isa(T id, T size) : std::array<T,N>({id, size}) { }
    isa(T id, const std::initializer_list<T>& list) :
      std::array<T,N>({id,(T)list.size()}) {
      T i = 0; for (auto& t : list) (*this)[i++] = t;
    }

    static constexpr T empty() { return std::numeric_limits<T>::max(); }
    static isa null(T size=0) { return isa(empty(), size); }

    isa frame() const { return isa(id(), size()); }
    isa shadow() const { return frame().fill(empty()); }
    isa clone() const { isa result = *this; return result; }
    isa& fill(T value) { for (auto& v: *this) v = value; return *this; }
    isa& shade() { return fill(empty()); }

    T& operator[](size_t i) { return std::array<T,N>::operator[](i+2); }
    T& size() { return std::array<T,N>::operator[](1); }
    T& id() { return std::array<T,N>::operator[](0); }

    T* begin() { return &std::array<T,N>::operator[](2); }
    T* end() { return begin() + size(); }

    bool valid() const { return this->id() != empty(); }

    const T& operator[](size_t i) const { return std::array<T,N>::operator[](i+2); }
    const T& size() const { return std::array<T,N>::operator[](1); }
    const T& id() const { return std::array<T,N>::operator[](0); }

    const T* begin() const { return &std::array<T,N>::operator[](2); }
    const T* end() const { return begin() + size(); }

    static constexpr size_t extent = N - 2;
  };
}

