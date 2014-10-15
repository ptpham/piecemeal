
#include <fstream>

#include "piecemeal/ioaux.hpp"

namespace piecemeal {
  namespace ioaux {
    vector<string> read_all(const string& filename) {
      std::string line;
      vector<string> result;
      std::ifstream file(filename);
      while (std::getline(file, line)) { result.push_back(line); }
      return result;
    }
  }
}

