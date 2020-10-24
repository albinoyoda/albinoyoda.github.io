#ifndef WOW_SIMULATOR_INTERFACE_HELPER_H
#define WOW_SIMULATOR_INTERFACE_HELPER_H

#include "Character.hpp"

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

constexpr double crit_w = 35;
constexpr double crit_w_cap = 25;
constexpr double hit_w = 45;
constexpr double hit_w_cap = 10;
constexpr double skill_w = 220.0 / 5;
constexpr double skill_w_soft = 70.0 / 5;
constexpr double skill_w_hard = 20.0 / 5;
constexpr double ap_per_coh = 20 / 6.2;

double get_ap_equivalent(const Special_stats& special_stats, int relevant_skill, double swing_speed,
                         double weapon_damage);

bool is_strictly_weaker(Special_stats special_stats1, Special_stats special_stats2);

double estimate_special_stats_high(const Special_stats& special_stats);

double estimate_special_stats_low(const Special_stats& special_stats);

bool estimate_special_stats_smart_no_skill(const Special_stats& special_stats1, const Special_stats& special_stats2);

double estimate_stat_diff(Special_stats special_stats1, Special_stats special_stats2);

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

template <typename T>
bool does_vector_contain(const std::vector<T>& vec, const T& match);

bool find_string(const std::vector<std::string>& string_vec, const std::string& match_string);

Race get_race(const std::string& race);

Character get_character_of_race(const std::string& race);

std::string string_with_precision(int amount);

std::string string_with_precision(double amount, size_t precision);

#include "Helper_functions.tcc"

#endif // WOW_SIMULATOR_INTERFACE_HELPER_H
