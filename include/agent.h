#pragma once

#include "game.h"
#include "misc.h"

namespace mas
{
    // forward declare
    typedef std::vector<std::pair<int, int>> State;
    class Game;

    class QTable
    {
    public:
        QTable(int size_x, int size_y, int num_players, float epsilon, float lr, float gamma);
        // update the Q-table according to prev state, new state, reward and action
        void update(State old_state, Action action, State new_state, int reward);
        // Determine next input according to policy and current state
        Action determineInput(State states);
        // computes total number of states
        int StateCount();

    private:
        float epsilon_;         // epsilon for choosing to use random input rather than policy
        float **table_ = nullptr;   // Q-learning table (dynamic allocation)
        int x_size_;            // X size of game
        int y_size_;            // Y size of game
        int agent_count_;       // Number of agents

        float learning_rate_;   // learning rate for Q-learning update
        float gamma_;           // gamma val for Q-learning update

        int statesToIndex(State states);

    };  //class mas::QTable

    class Agent
    {
    public:
        Agent(int id, int size_x, int size_y, int num_players, float epsilon, float lr, float gamma);
        // reset agent to it's starting location for new epoch
        void reset();
        // determine move according to policy
        Action determineMove(State current_state);
        // update policy according to the old state, new state, and previous reward/input
        void updatePolicy(State old_state, State new_state);
        // move agent by x_amt and y_amt
        void move(int x_amt, int y_amt);
        // Add amt to total score of agent, and track it for later
        void addScore(int amt);

        int X();            // Getter for X pos
        int Y();            // Getter for Y pos
        int Score();        // Getter for score

        bool operator==(const Agent &agent);
        int agent_id_;      // Leaving this public so I can use it
        
    private:
        QTable policy_;     // Update policy for Q-learning
        int x_pos_;         // X position of agent in game
        int y_pos_;         // Y position of agent in game
        int score_;         // Current score of agent in game
        int last_reward_;   // Previous reward of agent for Q-learning update
        Action last_input_; // Previous action of agent for Q-learning update

    };  //class mas::Agent

} //namespace mas