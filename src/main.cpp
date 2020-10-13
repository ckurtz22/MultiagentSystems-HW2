#include <random>
#include <ctime>
#include <thread>
#include <chrono>
#include <curses.h>

#include "game.h"

const int NUM_PLAYERS = 2;
const float EPSILON = 0.2;
const float GAMMA = 0.9;
const float LEARNING_RATE = 0.8;

int main(int argc, char** argv)
{
    // seed rng for various parts
    std::srand(std::time(NULL));

    // initialize curses for plotting game
    initscr();
    refresh();

    mas::Game game(NUM_PLAYERS, EPSILON, LEARNING_RATE, GAMMA);

    int iter = 0;   /// How many epochs we've ran (epoch ends when agent reaches goal)
    int total = 0;  /// Total number of moves made across all epochs
    while (true)
    {
        total += game.doEpoch(iter > 50000);
        // std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (iter++ % 100 == 0)
        {
            clear();
            printw("Average: %f\ttotal: %d\n", (float)total / (float)iter, iter);
            refresh();


        }
    }
}