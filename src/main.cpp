#include <random>
#include <ctime>
#include <thread>
#include <chrono>
#include <curses.h>

#include "game.h"

const float EPSILON = 0.2;
const float GAMMA = 0.9;
const float LEARNING_RATE = 0.8;

int main(int argc, char** argv)
{
    std::srand(std::time(NULL));
    initscr();
    refresh();

    mas::Game game(2, EPSILON, LEARNING_RATE, GAMMA);

    int iter = 0;
    int total = 0;
    while (true)
    {
        total += game.doEpoch(false);
        // std::this_thread::sleep_for(std::chrono::milliseconds(5));
        if (iter++ % 100 == 0)
        {
            clear();
            printw("Average: %f\ttotal: %d\n", (float)total / (float)iter, iter);
            refresh();

        }
    }
}