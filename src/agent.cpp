#include "agent.h"
#include <algorithm>
#include <random>
#include <cmath>

#include <curses.h>

mas::Agent::Agent(mas::Game *game, int id) : game_(game), agent_id_(id)
{
    reset();
}

void mas::Agent::reset()
{
    score_ = 0;
    if (agent_id_ == 0)
    {
        x_pos_ = 9;
        y_pos_ = 3;
    }
    else
    {
        x_pos_ = 4;
        y_pos_ = 2;
    }
}

void mas::Agent::update(State current_state)
{
    if (agent_id_ == 0)
    {
        while (!game_->moveAgent(agent_id_, Action(std::rand() % 4)));
    }
    else
    {
        do 
        {
            last_input_ = policy_.determineInput(current_state); 
        } while (!game_->moveAgent(agent_id_, last_input_));
    }
}

void mas::Agent::updatePolicy(State old_state, State new_state)
{
    if (agent_id_ == 0) return;
    policy_.update(old_state, last_input_, new_state, last_reward_);
}

void mas::Agent::setup(int size_x, int size_y, int num_players, float epsilon, float lr, float gamma)
{
    policy_.reset(size_x, size_y, num_players, epsilon, lr, gamma);
}

void mas::Agent::move(int x_amt, int y_amt)
{
    x_pos_ += x_amt;
    y_pos_ += y_amt;
}

void mas::Agent::addScore(int amt)
{
    if (agent_id_ != 0)
    {
        last_reward_ = amt;
        score_ += amt;
    }
}

int mas::Agent::X()
{
    return x_pos_;
}

int mas::Agent::Y()
{
    return y_pos_;
}

int mas::Agent::Score()
{
    return score_;
}


bool mas::Agent::operator==(const Agent &agent)
{
    return (this->x_pos_ == agent.x_pos_ && this->y_pos_ == agent.y_pos_);
}


mas::QTable::QTable()
{

}
#include <curses.h>

void mas::QTable::reset(int size_x, int size_y, int num_players, float epsilon, float lr, float gamma)
{
    // free up table if it already exists
    if (table_ != nullptr)
    {
        for (int i = 0; i < StateCount(); ++i)
        {
            delete(table_[i]);
        }
        delete(table_);
    }
    // initialize variables
    epsilon_ = epsilon;
    learning_rate_ = lr;
    gamma_ = gamma;
    x_size_ = size_x;
    y_size_ = size_y;
    agent_count_ = num_players + 1;
    table_ = new float*[StateCount()];
    for (size_t i = 0; i < StateCount(); ++i)
    {
        table_[i] = new float[4];
        for (size_t j = 0; j < 4; ++j)
        {
            table_[i][j] = 0;
        }
    }
}

int mas::QTable::StateCount()
{
    return std::pow(x_size_ * y_size_, agent_count_);
}

void mas::QTable::update(State old_state, Action action, State new_state, int reward)
{
    int old_state_index = statesToIndex(old_state);
    int new_state_index = statesToIndex(new_state);
    float max_new_state = *std::max_element(table_[new_state_index], table_[new_state_index] + 4);

    table_[old_state_index][(int)action] += learning_rate_ * (reward + gamma_ * max_new_state - table_[old_state_index][(int)action]);
}

// Policy
mas::Action mas::QTable::determineInput(State states)
{
    int idx;
    
    // Choose random direction epsilon % of the time
    if ((static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) < epsilon_)
    {
        idx = Action(std::rand() % 4);
    }
    else
    {
        // return Action(std::rand() % 4);
        auto q_table_state = table_[statesToIndex(states)];
        std::vector<int> max_index;
        float max_val = -INFINITY;

        // Iterate to find the best action
        for (size_t i = 0; i < 4; ++i)
        {
            if (q_table_state[i] > max_val)
            {
                max_index.clear();
                max_index.push_back(i);
                max_val = q_table_state[i];
            }
            else if (q_table_state[i] == max_val)
            {
                max_index.push_back(i);
            }
        }

        // If multiple max, choose random
        if (max_index.size() > 1)
        {
            idx = std::rand() % max_index.size();
        }
        else
        {
            idx = max_index.front();
        }
    }
    return Action(idx);
}


int mas::QTable::statesToIndex(State states)
{
    int multiplier = 1;
    int index = 0;
    for (auto &s : states)
    {
        index += multiplier * (s.first + s.second * GRID_Y);
        multiplier *= GRID_X * GRID_Y;
    }

    return index;
}