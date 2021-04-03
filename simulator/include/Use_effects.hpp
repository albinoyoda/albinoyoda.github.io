#ifndef WOW_SIMULATOR_USE_EFFECTS_H
#define WOW_SIMULATOR_USE_EFFECTS_H

#include "include/Character.hpp"

namespace Use_effects
{
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

double get_active_use_effect_ap_equivalent(const Use_effect& use_effect, const Special_stats& special_stats,
                                           double total_ap);

double get_use_effect_ap_equivalent(const Use_effect& use_effect, const Special_stats& special_stats, double total_ap,
                                    double sim_time);

} // namespace Use_effects

#endif // WOW_SIMULATOR_USE_EFFECTS_H
