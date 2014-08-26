
#include <iostream>
#include <functional>
#include <string>
#include <map>

using namespace std;

static map<string, function<void()>> all_tests;

namespace ptest {
  size_t add(const string& name, function<void()> fn) {
    size_t result = all_tests.size();
    all_tests[name] = fn;
    return result;
  }
}

int main(int argc, char** argv) {
  string filter = (argc > 1) ? argv[1] : "";
  map<string,string> errors;
  size_t passed = 0;

  for (auto& entry : all_tests) {
    if (filter.size() > 0 && entry.first.find(filter) == string::npos) continue;
    string status = "passed";
    try {
      entry.second();
      passed++;
    } catch (std::runtime_error e) {
      errors[entry.first] = e.what();
      status = "FAILED";
    }
    cout << status << "  " << entry.first << endl;
  }

  size_t total = passed + errors.size();
  cout << "summary [" << passed << "/" << total << "]" << endl;
  
  for (auto& entry : errors) {
    cout << "  " << entry.second << endl;
  }
  
  return 0;
}

