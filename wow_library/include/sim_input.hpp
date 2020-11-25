#ifndef SIM_INPUT_HPP
#define SIM_INPUT_HPP

#include <string>
#include <vector>

struct Sim_input
{
    Sim_input() = default;

    Sim_input(std::vector<std::string> race, std::vector<std::string> armor, std::vector<std::string> weapons,
              std::vector<std::string> buffs, std::vector<std::string> enchants, std::vector<std::string> stat_weights,
              std::vector<std::string> options, std::vector<std::string> float_options_string,
              std::vector<double> float_options_val, std::vector<std::string> talent_string,
              std::vector<int> talent_val, std::vector<std::string> compare_armor,
              std::vector<std::string> compare_weapons)
        : race(std::move(race))
        , armor(std::move(armor))
        , weapons(std::move(weapons))
        , buffs(std::move(buffs))
        , enchants(std::move(enchants))
        , stat_weights(std::move(stat_weights))
        , options(std::move(options))
        , float_options_string(std::move(float_options_string))
        , float_options_val(std::move(float_options_val))
        , talent_string(std::move(talent_string))
        , talent_val(std::move(talent_val))
        , compare_armor(std::move(compare_armor))
        , compare_weapons(std::move(compare_weapons))
    {
    }

    std::vector<std::string> race;
    std::vector<std::string> armor;
    std::vector<std::string> weapons;
    std::vector<std::string> buffs;
    std::vector<std::string> enchants;
    std::vector<std::string> stat_weights;
    std::vector<std::string> options;
    std::vector<std::string> float_options_string;
    std::vector<double> float_options_val;
    std::vector<std::string> talent_string;
    std::vector<int> talent_val;
    std::vector<std::string> compare_armor;
    std::vector<std::string> compare_weapons;
};

#endif // SIM_INPUT_HPP
