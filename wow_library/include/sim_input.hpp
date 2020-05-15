#ifndef COVID_INPUT_HPP
#define COVID_INPUT_HPP

#include <vector>

struct Sim_input
{
    Sim_input() = default;

    Sim_input(double fight_time, double target_level, double n_simulations)
            : fight_time(fight_time), target_level(target_level), n_simulations(n_simulations) {}

    double target_level;
    double fight_time;
    double n_simulations;
};

#endif // COVID_INPUT_HPP
