#ifndef COVID_INPUT_HPP
#define COVID_INPUT_HPP

#include <vector>

struct Sim_input
{
    Sim_input() = default;

    Sim_input(std::vector<std::string> armor, std::vector<std::string> weapons,
              double fight_time, double target_level, double n_simulations)
            : armor(std::move(armor)), weapons(std::move(weapons)), fight_time(fight_time), target_level(target_level),
            n_simulations(n_simulations) {}

    std::vector<std::string> armor;
    std::vector<std::string> weapons;
    double fight_time;
    double target_level;
    double n_simulations;
};

#endif // COVID_INPUT_HPP
