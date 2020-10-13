#include "game.h"

#include <algorithm>
#include <curses.h>
#include <thread>
#include <chrono>

mas::Game::Game(int num_players, float epsilon, float lr, float gamma)
{
    steps_ = 0;
    agents_.clear();
    for (int i = 0; i < GRID_X; ++i)
    {
        for (int j = 0; j < GRID_Y; ++j)
        {
            game_board_[i][j].clear();
        }
    }

    for (int i = 0; i < num_players+1; ++i)
    {
        Agent a(this, i);
        a.setup(GRID_X, GRID_Y, num_players, epsilon, lr, gamma);
        agents_.push_back(a);
        game_board_[a.X()][a.Y()].push_back(i);
    }
}

void mas::Game::reset()
{
    for (int i = 0; i < GRID_X; ++i)
    {
        for (int j = 0; j < GRID_Y; ++j)
        {
            game_board_[i][j].clear();
        }
    }

    for (auto &a : agents_)
    {
        a.reset();
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
    if ((cur_x + x_move < 0) || (cur_x + x_move >= mas::GRID_X) || (cur_y + y_move < 0) || (cur_y + y_move >= mas::GRID_Y))
        return false;

    auto iter_remove = std::remove(game_board_[cur_x][cur_y].begin(), game_board_[cur_x][cur_y].end(), agent_id);
    game_board_[cur_x][cur_y].erase(iter_remove, game_board_[cur_x][cur_y].end());

    agents_[agent_id].move(x_move, y_move);
    game_board_[cur_x + x_move][cur_y + y_move].push_back(agent_id);

    return true;
}

void mas::Game::update()
{
    State old_state = getState();
    for (auto &a : agents_)
    {
        a.update(old_state);
    }

    for (auto &a : agents_)
    {
        if (a == agents_[0])
        {
            a.addScore(30);
        }
        else
        {
            a.addScore(-1);
        }
    }
    State new_state = getState();

    for (auto &a : agents_)
    {
        a.updatePolicy(old_state, new_state);
    }
    
    steps_++;
}

mas::State mas::Game::getState()
{
    State s;
    for (auto &a : agents_)
    {
        s.push_back(std::make_pair(a.X(), a.Y()));
    }
    return s;
}

int mas::Game::doEpoch(bool print_game)
{
    reset();
    int iter = 0;
    int goal_reached = 0;
    do
    {
        update();
        if (print_game)
        {
            print();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        for (auto &a : agents_)
        {
            if (a == agents_[0])
            {
                goal_reached = a.agent_id_;
            }
        }
        iter++;
    } while (goal_reached == 0);
    // printw("agent %d reached goal in %d iterations\n", goal_reached, iter);
    // refresh();
    return iter;
}

void mas::Game::print()
{
    // if (steps_ % 100000 != 0) return;
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