#ifndef WOW_SIMULATOR_INTERFACE_HELPER_H
#define WOW_SIMULATOR_INTERFACE_HELPER_H

#include "Character.hpp"

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
constexpr double ap_per_coh = 50 / 6.2;

double get_character_ap_equivalent(const Special_stats& special_stats, const Weapon& mh_wep, const Weapon& oh_wep,
                                   double sim_time, const std::vector<Use_effect>& use_effects);

double get_character_ap_equivalent(const Special_stats& special_stats, const Weapon& mh_wep, double sim_time,
                                   const std::vector<Use_effect>& use_effects);

double get_hit_crit_skill_ap_equivalent(const Special_stats& special_stats, int relevant_skill);

double get_hit_effect_ap_equivalent(const Hit_effect& hit_effect, double total_ap, double swing_speed, double factor);

double estimate_special_stats_high(const Special_stats& special_stats);

double estimate_special_stats_low(const Special_stats& special_stats);

bool estimate_special_stats_smart_no_skill(const Special_stats& special_stats1, const Special_stats& special_stats2);

double estimate_stat_diff(Special_stats special_stats1, Special_stats special_stats2);

int get_weapon_skill(const Special_stats& special_stats, Weapon_type weapon_type, Weapon_socket weapon_socket);

#endif // WOW_SIMULATOR_INTERFACE_HELPER_H
