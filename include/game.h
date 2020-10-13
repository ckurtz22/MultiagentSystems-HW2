#pragma once

#include <vector>

#include "agent.h"
#include "misc.h"


namespace mas
{
    // forward declare
    class Agent;
    enum Action;
    typedef std::vector<std::pair<int, int>> State;

    const int GRID_X = 10;
    const int GRID_Y = 5;

    class Game
    {
    public:
        Game(int num_players, float epsilon, float lr, float gamma);
        void reset();
        bool moveAgent(int agent_id, Action action);
        void update();
        void print();
        int doEpoch(bool print);

    private:
        std::vector<Agent> agents_; /// index 0 is target, rest are players
        std::vector<int> game_board_[GRID_X][GRID_Y];
        int steps_;

        State getState();

    };

} //namespace mas