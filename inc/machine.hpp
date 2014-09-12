
#include "game.hpp"

namespace piecemeal {
  namespace game {
    using namespace std;

    template <template <class,size_t> class I, 
      class T = uint16_t, size_t N = 8>
    struct machine {
      game::simulator<T,N> sim;
      map<T,size_t> role_map;
      vector<prop<T,N>> state;
      I<T,N> index;
      size_t turn;
      
      machine(const string& raw) : sim(raw),
        index(sim.template create_index<position_index>()) {
        role_map = sim.ask_role_map(index);
        restart();
      }

      void restart() {
        auto state = sim.template ask_convert<TRUE,INIT>(index);
        sim.bind_state(index, state);
        turn = 0;
      }

      vector<prop<T,N>> moves() {
        return sim.template ask_convert<DOES,LEGAL>(index);
      }

      void move(const vector<prop<T,N>>& chosen) {
        index.emplace_props(chosen);
        state = sim.template ask_convert<TRUE, NEXT>(index);
        sim.bind_state(index, state);
        turn++;
      }

      bool is_terminal() {
        return sim.template ask<TERMINAL>(index).size() > 0;
      }

      unordered_set<prop<T,N>> goals() {
        return sim.template ask<GOAL>(index);
      }
    };
  }
}


