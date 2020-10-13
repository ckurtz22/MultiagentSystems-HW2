#include "game.h"

#include <algorithm>
#include <curses.h>
#include <thread>
#include <chrono>

mas::Game::Game(int num_players, float epsilon, float lr, float gamma)
{
    steps_ = 0;
    // Initialize num_players + 1 for the target
    for (int i = 0; i < num_players+1; ++i)
    {
        // Initialize a new agent, passing parameters for Q-learning
        Agent a(i, GRID_X, GRID_Y, num_players, epsilon, lr, gamma);
        agents_.push_back(a);

        // Mark the agent on the board for plotting game
        game_board_[a.X()][a.Y()].push_back(i);
    }
}

void mas::Game::reset()
{
    steps_ = 0;
    // clear game board for new epoch
    for (int i = 0; i < GRID_X; ++i)
    {
        for (int j = 0; j < GRID_Y; ++j)
        {
            game_board_[i][j].clear();
        }
    }

    for (auto &a : agents_)
    {
        // reset each agent to initial location
        a.reset();

        // reset gameboard
        game_board_[a.X()][a.Y()].push_back(a.agent_id_);
    }
}

bool mas::Game::moveAgent(int agent_id, mas::Action action)
{
    int x_move = 0;
    int y_move = 0;
    switch (action)
    {
        case MOVE_DOWN:
            y_move = 1;
            break;

        case MOVE_UP:
            y_move = -1;
            break;

        case MOVE_LEFT:
            x_move = -1;
            break;

        case MOVE_RIGHT:
            x_move = 1;
            break;
    }
    int cur_x = agents_[agent_id].X();
    int cur_y = agents_[agent_id].Y();
    // check if agent going to move off board
    if ((cur_x + x_move < 0) || (cur_x + x_move >= mas::GRID_X) || (cur_y + y_move < 0) || (cur_y + y_move >= mas::GRID_Y))
        return false;

    // remove old piece from game board
    auto iter_remove = std::remove(game_board_[cur_x][cur_y].begin(), game_board_[cur_x][cur_y].end(), agent_id);
    game_board_[cur_x][cur_y].erase(iter_remove, game_board_[cur_x][cur_y].end());

    // move piece and update game board
    agents_[agent_id].move(x_move, y_move);
    game_board_[cur_x + x_move][cur_y + y_move].push_back(agent_id);

    return true;
}

int mas::Game::update()
{
    int goal_reached = 0; // tracks which agent reaches goal. 0 is the goal itself and doesn't matter
    // track initial state for calculating Q update
    State old_state = getState();
    for (auto &a : agents_)
    {
        // Keep picking moves till we get one that keeps us on the board
        while (!moveAgent(a.agent_id_, a.determineMove(old_state)));
    }

    // update score for each agent in new state
    for (auto &a : agents_)
    {
        if (a == agents_[0])
        {
            goal_reached = a.agent_id_;
            a.addScore(30);
        }
        else
        {
            a.addScore(-1);
        }
    }
    // get new state for Q update
    State new_state = getState();

    // Update Q table for each agent
    for (auto &a : agents_)
    {
        a.updatePolicy(old_state, new_state);
    }
    
    steps_++;
    return goal_reached;
}

mas::State mas::Game::getState()
{
    // Generates a vector of coordinate pairs for indexing Q-table
    State s;
    for (auto &a : agents_)
    {
        s.push_back(std::make_pair(a.X(), a.Y()));
    }
    return s;
}

int mas::Game::doEpoch(bool print_game)
{
    // reset agents to original locations
    reset();
    do
    {
        // don't print the game every iteration. slooooow
        if (print_game)
        {
            print();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // keep iterating till update returns a non zero agent reached goal
    } while (update() == 0);

    // printw("agent %d reached goal in %d iterations\n", goal_reached, steps_);
    // refresh();
    return steps_;
}

void mas::Game::print()
{
    clear();
    printw("--------------------\n");
    for (int j = 0; j < mas::GRID_Y; ++j)
    {
        printw("|");
        for (int i = 0; i < mas::GRID_X; ++i)
        {
            if (std::find(game_board_[i][j].begin(), game_board_[i][j].end(), 0) != game_board_[i][j].end())
            {
                if (game_board_[i][j].size() > 1)
                    printw("!|");
                else
                    printw("O|");
            }
            else if (game_board_[i][j].size() > 0)
            {
                printw("X|");
            }
            else
            {
                printw(" |");
            }
        }
        printw("\n--------------------\n");
    }

    printw("\n\n");
    for (size_t i = 1; i < agents_.size(); ++i)
    {
        printw("Player %d score: %d\n", i, agents_[i].Score());
    }
    refresh();
}