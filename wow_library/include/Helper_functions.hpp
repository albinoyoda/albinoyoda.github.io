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
constexpr double expertise_w = 45;
constexpr double hit_w_cap = 10;
constexpr double skill_w = 220.0 / 5;
constexpr double skill_w_soft = 70.0 / 5;
constexpr double skill_w_hard = 20.0 / 5;
constexpr double ap_per_coh = 50 / 6.2;

double is_time_available(const std::vector<std::pair<double, Use_effect>>& use_effect_timers, double check_time,
                         double duration);

double get_next_available_time(const std::vector<std::pair<double, Use_effect>>& use_effect_timers, double check_time,
                               double duration);

std::vector<std::pair<double, Use_effect>> compute_use_effect_order(std::vector<Use_effect>& use_effects,
                                                                    const Special_stats& special_stats, double sim_time,
                                                                    double ap, int number_of_targets,
                                                                    double extra_target_duration, double init_rage);

void shuffle_bloodrage(std::vector<std::pair<double, Use_effect>>& use_effect_timers, double init_rage);

void sort_use_effect_order(std::vector<std::pair<double, Use_effect>>& use_effect_order, double sim_time);

std::vector<Use_effect> sort_use_effects_by_power_ascending(std::vector<Use_effect>& shared_effects,
                                                            const Special_stats& special_stats, double total_ap);

double get_character_ap_equivalent(const Special_stats& special_stats, const Weapon& mh_wep, const Weapon& oh_wep,
                                   double sim_time, const std::vector<Use_effect>& use_effects);

double get_character_ap_equivalent(const Special_stats& special_stats, const Weapon& mh_wep, double sim_time,
                                   const std::vector<Use_effect>& use_effects);

double get_hit_crit_expertise_ap_equivalent(const Special_stats& special_stats, Weapon_type weapon_type);

double get_hit_effect_ap_equivalent(const Hit_effect& hit_effect, double total_ap, double swing_speed, double factor);

double get_active_use_effect_ap_equivalent(const Use_effect& use_effect, const Special_stats& special_stats,
                                           double total_ap);

double get_use_effect_ap_equivalent(const Use_effect& use_effect, const Special_stats& special_stats, double total_ap,
                                    double sim_time);

bool is_strictly_weaker(Special_stats special_stats1, Special_stats special_stats2);

double estimate_special_stats_high(const Special_stats& special_stats);

double estimate_special_stats_low(const Special_stats& special_stats);

bool estimate_special_stats_smart_no_skill(const Special_stats& special_stats1, const Special_stats& special_stats2);

double estimate_stat_diff(Special_stats special_stats1, Special_stats special_stats2);

std::string percent_to_str(double value);

std::string percent_to_str(const std::string& stat_name, double value, const std::string& description);

std::string percent_to_str(const std::string& stat_name, double value1, const std::string& description1, double value2,
                           const std::string& description2);

bool find_string(const std::vector<std::string>& string_vec, const std::string& match_string);

double find_value(const std::vector<std::string>& string_vec, const std::vector<double>& double_vec,
                  const std::string& match_string);

template <typename T>
class Find_values
{
public:
    Find_values(const std::vector<std::string>& string_vec, const std::vector<T>& value_vec);

    T find(const std::string& name);

private:
    const std::vector<std::string>& string_vec;
    const std::vector<T>& value_vec;
};

template <typename T>
bool does_vector_contain(const std::vector<T>& vec, const T& match);

Race get_race(const std::string& race);

Character get_character_of_race(const std::string& race);

std::string string_with_precision(int amount);

std::string string_with_precision(double amount, size_t precision);

#include "Helper_functions.tcc"

#endif // WOW_SIMULATOR_INTERFACE_HELPER_H
