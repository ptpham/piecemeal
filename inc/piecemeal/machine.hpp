
#include <stdint.h>
#include <stdlib.h>

#include "piecemeal/game.hpp"

namespace piecemeal {
  namespace game {
    using namespace std;

    template <class T, size_t N>
    vector<prop<T,N>> random_joint_move(
      const vector<vector<prop<T,N>>>& moves) {
      vector<prop<T,N>> result(moves.size());
      for (int i = 0; i < moves.size(); i++) {
        auto& selected = moves[i][rand() % moves[i].size()];
        result[i] = selected;
      }
      return result;
    }

    template <template <class,size_t> class I, 
      class T = uint16_t, size_t N = 12>
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

      vector<vector<prop<T,N>>> moves() {
        auto raw = sim.template ask_convert<DOES,LEGAL>(index);
        return index_by_position(raw, role_map, 1);
      }

      void move(const vector<prop<T,N>>& chosen) {
        index.emplace_props(chosen);
        state = sim.template ask_convert<TRUE, NEXT>(index);
        sim.bind_state(index, state);
        turn++;
      }

      bool terminal() {
        return sim.template ask<TERMINAL>(index).size() > 0;
      }

      vector<uint16_t> goals() {
        auto raw = sim.template ask<GOAL>(index);
        vector<uint16_t> result(role_map.size());
        auto& backward = sim.context.parse.tokens.backward;
        for (auto& goal : raw) {
          auto goal_index = goal[2];
          if (goal_index >= backward.size()) continue;
          result[role_map[goal[1]]] = atoi(backward[goal_index].c_str());
        }
        return result;
      }
    };
  }
}


