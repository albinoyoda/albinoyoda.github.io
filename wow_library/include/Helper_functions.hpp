#ifndef WOW_SIMULATOR_INTERFACE_HELPER_H
#define WOW_SIMULATOR_INTERFACE_HELPER_H

#include "Character.hpp"

#include <iomanip>
#include <string>
#include <vector>

constexpr int get_skill_of_type(const Special_stats& special_stats, Weapon_type weapon_type)
{
    switch (weapon_type)
    {
    case Weapon_type::sword:
        return special_stats.sword_skill;
    case Weapon_type::axe:
        return special_stats.axe_skill;
    case Weapon_type::dagger:
        return special_stats.dagger_skill;
    case Weapon_type::mace:
        return special_stats.mace_skill;
    case Weapon_type::unarmed:
        return special_stats.fist_skill;
    default:
        return 300;
    }
}

std::string percent_to_str(double value);

std::string percent_to_str(const std::string& stat_name, double value, const std::string& description);

std::string percent_to_str(const std::string& stat_name, double value1, const std::string& description1, double value2,
                           const std::string& description2);

bool find_string(const std::vector<std::string>& string_vec, const std::string& match_string);

Race get_race(const std::string& race);

Character get_character_of_race(const std::string& race);

std::string string_with_precision(double amount, size_t precision);

#endif // WOW_SIMULATOR_INTERFACE_HELPER_H
