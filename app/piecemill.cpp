
/*
Piecemill was intended to provide multithreaded prover functionality to another
process through the standard io streams to avoid needing a language specific
binding. All commands must be prefixed by some string indicating the "game
instance". "add" provides kif lines into a buffer. "build" compiles the
previously added lines into a game. "ask" uses previous lines as inputs into
the prover using the specified game instance. "done" will delete/clean up the
instance. Here's an example input sequence:

1 add (<= (next ?x) (true ?x))
1 build
1 add (true a)
1 ask

The last output you should see is the following note that it also returns the
inputs you give it:
1 (next a) (true a)
*/

#include <functional>
#include <iostream>
#include <string>
#include <regex>

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include <list>

#include "private/piecelog.hpp"

using namespace std;

typedef uint16_t T;
const size_t N = 12;

struct task {
  vector<string> values;
  list<pair<string,string>> unprocessed;
  piecelog::parse<T,N> parse;
  string command, prefix;
  bool working;
};

typedef shared_ptr<task> frame;

map<string, frame> frames;
list<frame> queue;

mutex gmutex, cout_mutex;
condition_variable work;

void worker() {
  while (true) {
    frame frame;
    unique_lock<mutex> glock(gmutex);
    while (queue.size() == 0) work.wait(glock);
    frame = move(queue.front());
    queue.pop_front();
    glock.unlock();

    string result = "";
    if (frame->command == "build") {
      auto processed = piecelog::preprocess::standard(frame->values);
      frame->parse = piecelog::compile::parse_sentences<T,N>(processed);
    } else if (frame->command == "ask") {
      string line;
      for (auto& value : frame->values) line += value;
      result = piecelog::run(frame->parse, line);
    }

    cout_mutex.lock();
    cout << frame->prefix << " " << result << endl;
    cout_mutex.unlock();

    frame->values.clear();
    frame->working = false;
  }
}

int main(int argc, char* argv[]) {
  size_t nthreads = 4;
  string prefix, command;
  vector<thread> threads;

  for (size_t i = 0; i < nthreads; i++) {
    threads.push_back(thread(worker)); 
  }

  while (!cin.eof()) {
    string line;
    cin >> prefix >> command;
    if (command == "add") getline(cin, line);

    if (frames.find(prefix) == frames.end()) {
      shared_ptr<task> created(new task());
      created->prefix = prefix;
      frames[prefix] = created;
    }

    lock_guard<mutex> guard(gmutex);
    auto& cur = frames[prefix]; 
    cur->unprocessed.push_back({command, move(line)});
    if (cur->working) continue;

    auto& unprocessed = cur->unprocessed;
    while (unprocessed.size() > 0) {
      auto& pair = unprocessed.front();
      auto command = move(pair.first);
      auto line = move(pair.second);
      unprocessed.pop_front();

      if (command == "add") {
        cur->values.push_back(move(line));
      } else if (command == "done") {
        frames.erase(prefix);
        break;
      } else {
        cur->command = command;
        cur->working = true;
        
        queue.push_back(cur);
        work.notify_one();
        break;
      }
    }
  }

  return 0;
}

