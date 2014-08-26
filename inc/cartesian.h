#pragma once

#include <vector>

namespace piecemeal {
  using namespace std;

  template <class T>
  class cartesian {
    const vector<vector<T>>* slices;
  public:
    cartesian(const vector<vector<T>>& slices) : slices(&slices) { }

    class iterator {
      vector<T> cur;
      vector<size_t> pos;
      const vector<vector<T>>* slices;
    public:
      iterator(const vector<vector<T>>* slices) :
        slices(slices), cur(slices->size()), pos(slices->size()) { }
      iterator() : slices(nullptr) { }

      iterator& operator++() {
        size_t dim = pos.size();
        for (size_t i = 1; i <= dim; i++) {
          size_t j = dim - i;
          if (++pos[j] < slices->at(j).size()) break;
          if (j > 0) pos[j] = 0;
        }
        return *this;
      }

      bool is_done() const {
        return slices == nullptr || pos[0] >= slices->at(0).size();
      }

      bool operator==(const iterator& other) const {
        if (this->is_done() && other.is_done()) return true;
        if (this->slices != other.slices) return false;
        return this->pos == other.pos;
      }

      bool operator!=(const iterator& other) const { return !(*this == other); }

      vector<T>& operator*() {
        if (is_done()) return cur;
        for (size_t i = 0; i < slices->size(); i++) {
          cur[i] = slices->at(i)[pos[i]];
        } return cur;
      }
    };

    iterator begin() {
      if (slices->size() == 0) return end();
      for (auto& v : *slices) if (v.size() == 0) return end();
      return iterator(slices);
    }

    iterator end() { return iterator(); }
  };

  template <class T>
  cartesian<T> make_cartesian(const vector<vector<T>>& slices) {
    return cartesian<T>(slices);
  }
}

