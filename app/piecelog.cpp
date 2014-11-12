
#include <functional>
#include <iostream>
#include <string>

#include "private/piecelog.hpp"

using namespace std;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    cout << "Usage: ./piecelog [-n <# of code lines>] <paths to kif>... " << endl;
    return 1;
  }

  string line;
  vector<string> args;
  for (int i = 1; i < argc; i++) args.push_back(string(argv[i]));
  auto parse = piecelog::load(args, [&](){ getline(cin, line); return line; });

  while (true) {
    getline(cin, line);
    cout << piecelog::run(parse, line) << endl;
  }

  return 0;
}
