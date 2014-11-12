
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
  list<string> unprocessed;
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
    cur->unprocessed.push_back(move(line));
    if (cur->working) continue;

    auto& unprocessed = cur->unprocessed;
    while (unprocessed.size() > 0) {
      auto line = move(unprocessed.front());
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

