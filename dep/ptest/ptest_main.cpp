
#include <iostream>
#include <functional>
#include <numeric>
#include <vector>
#include <string>
#include <map>

#include "ptest.h"

using namespace std;

static map<pair<string,string>, function<void()>> all_tests;

namespace ptest {
  size_t add(const string& file, const string& name, function<void()> fn) {
    size_t result = all_tests.size();
    all_tests[make_pair(file, name)] = fn;
    return result;
  }
}

static string common_prefix(const vector<string>& strings) {
  if (strings.size() == 0) return 0;
  auto prefix = strings[0];
  for (auto& str : strings) {
    for (size_t i = 0; i < prefix.size() && i < str.size(); i++) {
      if (prefix[i] != str[i]) prefix = prefix.substr(0, i);
    }
  }
  return prefix;
}

static string get_fullname(const pair<string,string>& p) {
  return p.first + ":" + p.second;
}

int main(int argc, char** argv) {
  string filter = (argc > 1) ? argv[1] : "";
  map<string,string> errors;
  size_t passed = 0;

  decltype(all_tests) filtered;
  for (auto& entry : all_tests) {
    auto full_name = get_fullname(entry.first);
    if (filter.size() > 0 && full_name.find(filter) == string::npos) continue;
    filtered.insert(entry);
  }

  vector<string> all_names;
  for (auto& entry : filtered) {
    auto full_name = get_fullname(entry.first);
    all_names.push_back(full_name);
  }
  auto prefix = common_prefix(all_names);

  for (auto& entry : filtered) {
    auto full_name = get_fullname(entry.first);
    string status = "passed";

    try {
      entry.second();
      passed++;
    } catch (ptest::error e) {
      errors[full_name] = e.file + ":" + entry.first.second + ":" +
        to_string(e.line) + " " + e.what();
      status = "FAILED";
    }
    cout << status << "  " << full_name.substr(prefix.size()) << endl;
  }

  size_t total = passed + errors.size();
  cout << "summary [" << passed << "/" << total << "]" << endl;
  
  if (errors.size() > 0) cout << endl;
  for (auto& entry : errors) {
    cout << "  " << entry.second.substr(prefix.size()) << endl;
  }
  
  return errors.size() > 0 ? 1 : 0;
}

