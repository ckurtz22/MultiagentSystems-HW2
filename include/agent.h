#pragma once

#include "game.h"
#include "misc.h"

namespace mas
{
    typedef std::vector<std::pair<int, int>> State;
    // forward declare
    class Game;

    class QTable
    {
    public:
        QTable();
        void reset(int size_x, int size_y, int num_players, float epsilon, float lr, float gamma);
        void update(State old_state, Action action, State new_state, int reward);
        Action determineInput(State states);
        int StateCount();

    private:
        float epsilon_;
        float **table_ = nullptr;
        int x_size_;
        int y_size_;
        int agent_count_;

        float learning_rate_;
        float gamma_;

        int statesToIndex(State states);

    };  //class mas::QTable

    class Agent
    {
    public:
        Agent(Game *game, int id);
        void setup(int size_x, int size_y, int num_players, float epsilon, float lr, float gamma);
        void reset();
        void update(State current_state);
        void updatePolicy(State old_state, State new_state);
        void move(int x_amt, int y_amt);
        void addScore(int amt);

        int X();
        int Y();
        int Score();

        bool operator==(const Agent &agent);
        int agent_id_;
        
    private:
        Game *game_;
        QTable policy_;
        int x_pos_;
        int y_pos_;
        int score_;
        int last_reward_;
        Action last_input_;

    };  //class mas::Agent

} //namespace mas