#ifndef WOW_SIMULATOR_USE_EFFECTS_H
#define WOW_SIMULATOR_USE_EFFECTS_H

#include "include/Character.hpp"

namespace Use_effects
{
Sim_time is_time_available(const std::vector<std::pair<Sim_time, Use_effect>>& use_effect_timers, Sim_time check_time,
                         Sim_time duration);

Sim_time get_next_available_time(const std::vector<std::pair<Sim_time, Use_effect>>& use_effect_timers,
                                 Sim_time check_time, Sim_time duration);

std::vector<std::pair<Sim_time, Use_effect>> compute_use_effect_order(std::vector<Use_effect>& use_effects,
                                                                      const Special_stats& special_stats,
                                                                      Sim_time sim_time, double ap,
                                                                      int number_of_targets, double init_rage);

void shuffle_bloodrage(std::vector<std::pair<Sim_time, Use_effect>>& use_effect_timers, double init_rage);

void sort_use_effect_order(std::vector<std::pair<Sim_time, Use_effect>>& use_effect_order, Sim_time sim_time);

std::vector<Use_effect> sort_use_effects_by_power_ascending(std::vector<Use_effect>& shared_effects,
                                                            const Special_stats& special_stats, double total_ap);

double get_active_use_effect_ap_equivalent(const Use_effect& use_effect, const Special_stats& special_stats,
                                           double total_ap);

double get_use_effect_ap_equivalent(const Use_effect& use_effect, const Special_stats& special_stats, double total_ap,
                                    Sim_time sim_time_ms);

} // namespace Use_effects

#endif // WOW_SIMULATOR_USE_EFFECTS_H
