#ifndef SIM_INPUT_MULT_HPP
#define SIM_INPUT_MULT_HPP

#include <string>
#include <vector>

struct Sim_input_mult
{
    Sim_input_mult() = default;

    Sim_input_mult(std::vector<std::string> race, std::vector<std::string> armor, std::vector<std::string> weapons,
                   std::vector<std::string> buffs, std::vector<std::string> enchants, std::vector<std::string> options,
                   std::vector<std::string> float_options_string, std::vector<double> float_options_val,
                   std::vector<std::string> talent_string, std::vector<int> talent_val)
        : race(std::move(race))
        , armor(std::move(armor))
        , weapons(std::move(weapons))
        , buffs(std::move(buffs))
        , enchants(std::move(enchants))
        , options(std::move(options))
        , float_options_string(std::move(float_options_string))
        , float_options_val(std::move(float_options_val))
        , talent_string(std::move(talent_string))
        , talent_val(std::move(talent_val))
    {
    }

    std::vector<std::string> race;
    std::vector<std::string> armor;
    std::vector<std::string> weapons;
    std::vector<std::string> buffs;
    std::vector<std::string> enchants;
    std::vector<std::string> options;
    std::vector<std::string> float_options_string;
    std::vector<double> float_options_val;
    std::vector<std::string> talent_string;
    std::vector<int> talent_val;
};

#endif // SIM_INPUT_MULT_HPP
