#include "Use_effects.hpp"

#include <algorithm>

namespace Use_effects
{
double is_time_available(const std::vector<std::pair<double, Use_effect>>& use_effect_timers, double check_time,
                         double duration)
{
    for (const auto& pair : use_effect_timers)
    {
        if (check_time >= pair.first && check_time < (pair.first + pair.second.duration))
        {
            return pair.first + pair.second.duration;
        }
        if ((check_time + duration >= pair.first) && (check_time + duration) < (pair.first + pair.second.duration))
        {
            return pair.first + pair.second.duration;
        }
    }
    return check_time;
}

double get_next_available_time(const std::vector<std::pair<double, Use_effect>>& use_effect_timers, double check_time,
                               double duration)
{
    while (true)
    {
        double next_available = is_time_available(use_effect_timers, check_time, duration);
        if (next_available == check_time)
        {
            return next_available;
        }
        check_time = next_available;
    }
}

std::vector<std::pair<double, Use_effect>> compute_use_effect_order(std::vector<Use_effect>& use_effects,
                                                                    const Special_stats& special_stats, double sim_time,
                                                                    double total_ap, int number_of_targets,
                                                                    double extra_target_duration, double init_rage)
{
    std::vector<std::pair<double, Use_effect>> use_effect_timers;
    std::vector<Use_effect> shared_effects{};
    std::vector<Use_effect> unique_effects{};

    for (auto& use_effect : use_effects)
    {
        if (use_effect.effect_socket == Use_effect::Effect_socket::shared)
        {
            shared_effects.push_back(use_effect);
        }
        else
        {
            unique_effects.push_back(use_effect);
        }
    }

    if (number_of_targets > 0)
    {
    }
    else
    {
        auto sorted_shared_use_effects = sort_use_effects_by_power_ascending(shared_effects, special_stats, total_ap);

        for (auto& use_effect : sorted_shared_use_effects)
        {
            double test_time = 0.0 * extra_target_duration;
            for (int i = 0; i < 10; i++)
            {
                test_time = get_next_available_time(use_effect_timers, test_time, use_effect.duration);
                if (test_time >= sim_time)
                {
                    break;
                }
                use_effect_timers.emplace_back(test_time, use_effect);
                test_time += use_effect.cooldown;
            }
        }

        for (auto& use_effect : unique_effects)
        {
            double test_time = 0.0;
            for (int i = 0; i < 10; i++)
            {
                if (test_time >= sim_time)
                {
                    break;
                }
                use_effect_timers.emplace_back(test_time, use_effect);
                test_time += use_effect.cooldown;
            }
        }
    }
    sort_use_effect_order(use_effect_timers, sim_time);
    shuffle_bloodrage(use_effect_timers, init_rage);
    return use_effect_timers;
}

void shuffle_bloodrage(std::vector<std::pair<double, Use_effect>>& use_effect_timers, double init_rage)
{
    double projected_rage = init_rage;
    bool done{false};
    for (auto reverse_it = use_effect_timers.rbegin(); reverse_it != use_effect_timers.rend(); ++reverse_it)
    {
        if (reverse_it->first < 0.0)
        {
            projected_rage += reverse_it->second.rage_boost;
            if (projected_rage < 0.0)
            {
                // Reshuffle bloodrage since there is no rage
                auto bloodrage_reverse_it =
                    std::find_if(use_effect_timers.rbegin(), use_effect_timers.rend(),
                                 [](const std::pair<double, Use_effect>& ue) { return ue.second.name == "Bloodrage"; });
                if (bloodrage_reverse_it != use_effect_timers.rend())
                {
                    auto forward_it = --(reverse_it.base());
                    auto bloodrage_forward_it = --(bloodrage_reverse_it.base());
                    bloodrage_forward_it->first = reverse_it->first - 1.0;
                    std::rotate(bloodrage_forward_it, bloodrage_forward_it + 1, forward_it + 1);
                    done = true;
                }
            }
        }
        else
        {
            done = true;
        }
        if (done)
        {
            break;
        }
    }
}

void sort_use_effect_order(std::vector<std::pair<double, Use_effect>>& use_effect_order, double sim_time)
{
    for (auto& use_effect : use_effect_order)
    {
        use_effect.first = sim_time - use_effect.first - use_effect.second.duration;
    }
    std::sort(use_effect_order.begin(), use_effect_order.end(),
              [](const std::pair<double, Use_effect>& a, const std::pair<double, Use_effect>& b) {
                  return a.first > b.first;
              });
}

double get_active_use_effect_ap_equivalent(const Use_effect& use_effect, const Special_stats& special_stats,
                                           double total_ap)
{
    double use_effect_ap_boost = use_effect.get_special_stat_equivalent(special_stats).attack_power;

    double use_effect_haste_boost = total_ap * use_effect.special_stats_boost.haste;

    double use_effect_armor_boost{};
    if (use_effect.name == "badge_of_the_swarmguard")
    {
        // TODO this should be based on armor of the boss
        use_effect_armor_boost = total_ap * 0.07;
    }
    return use_effect_ap_boost + use_effect_haste_boost + use_effect_armor_boost;
}

std::vector<Use_effect> sort_use_effects_by_power_ascending(std::vector<Use_effect>& shared_effects,
                                                            const Special_stats& special_stats, double total_ap)
{
    std::vector<std::pair<double, Use_effect>> use_effect_pairs;
    for (auto& use_effect : shared_effects)
    {
        double est_ap = get_active_use_effect_ap_equivalent(use_effect, special_stats, total_ap);
        use_effect_pairs.emplace_back(est_ap, use_effect);
    }
    std::sort(use_effect_pairs.begin(), use_effect_pairs.end(),
              [](const std::pair<double, Use_effect>& a, const std::pair<double, Use_effect>& b) {
                  return a.first > b.first;
              });
    std::vector<Use_effect> sorted_ascending_use_effects;
    for (auto& use_effect : use_effect_pairs)
    {
        sorted_ascending_use_effects.push_back(use_effect.second);
    }
    return sorted_ascending_use_effects;
}

double get_use_effect_ap_equivalent(const Use_effect& use_effect, const Special_stats& special_stats, double total_ap,
                                    double sim_time)
{
    double ap_during_active = get_active_use_effect_ap_equivalent(use_effect, special_stats, total_ap);
    return ap_during_active * std::min(use_effect.duration / sim_time, 1.0);
}

} // namespace Use_effects