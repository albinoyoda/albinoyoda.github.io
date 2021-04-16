#include "../include/Helper_functions.hpp"

#include <algorithm>

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

double get_character_ap_equivalent(const Special_stats& special_stats, const Weapon& mh_wep, const Weapon& oh_wep,
                                   double sim_time, const std::vector<Use_effect>& use_effects)
{
    double attack_power = special_stats.attack_power;
    double mh_hit_crit_expertise_ap = get_hit_crit_expertise_ap_equivalent(special_stats, mh_wep.type);
    double oh_hit_crit_expertise_ap = get_hit_crit_expertise_ap_equivalent(special_stats, oh_wep.type);

    /// Weighted combination of ap from mh and oh, based on the hit-tables
    double hit_crit_expertise_ap = (mh_hit_crit_expertise_ap + oh_hit_crit_expertise_ap * 0.5) / 1.5;

    double mh_ap = ((mh_wep.max_damage + mh_wep.min_damage) / 2 + special_stats.bonus_damage) / mh_wep.swing_speed * 14;
    double oh_ap = ((oh_wep.max_damage + oh_wep.min_damage) / 2 + special_stats.bonus_damage) / oh_wep.swing_speed * 14;
    oh_ap *= 0.625;

    double special_stats_ap = attack_power + hit_crit_expertise_ap + mh_ap + oh_ap;

    double use_effects_ap = 0;
    double use_effects_shared_ap = 0;
    for (const auto& use_effect : use_effects)
    {
        double use_effect_ap = get_use_effect_ap_equivalent(use_effect, special_stats, special_stats_ap, sim_time);
        if (use_effect.effect_socket == Use_effect::Effect_socket ::shared)
        {
            if (use_effect_ap > use_effects_shared_ap)
            {
                use_effects_shared_ap = use_effect_ap;
            }
        }
        else
        {
            use_effects_ap += use_effect_ap;
        }
    }

    double hit_effects_ap = 0;
    for (const auto& hit_effect : mh_wep.hit_effects)
    {
        double hit_effect_ap = get_hit_effect_ap_equivalent(hit_effect, special_stats_ap, mh_wep.swing_speed, 1.0);
        hit_effects_ap += hit_effect_ap;
    }
    for (const auto& hit_effect : oh_wep.hit_effects)
    {
        double hit_effect_ap = get_hit_effect_ap_equivalent(hit_effect, special_stats_ap, oh_wep.swing_speed, 0.5);
        hit_effects_ap += hit_effect_ap;
    }

    return special_stats_ap + use_effects_ap + use_effects_shared_ap + hit_effects_ap;
}

double get_character_ap_equivalent(const Special_stats& special_stats, const Weapon& mh_wep, double sim_time,
                                   const std::vector<Use_effect>& use_effects)
{
    double attack_power = special_stats.attack_power;
    double hit_crit_expertise_ap = get_hit_crit_expertise_ap_equivalent(special_stats, mh_wep.type);

    double mh_ap = ((mh_wep.max_damage + mh_wep.min_damage) / 2 + special_stats.bonus_damage) / mh_wep.swing_speed * 14;

    double special_stats_ap = attack_power + hit_crit_expertise_ap + mh_ap;

    double use_effects_ap = 0;
    double use_effects_shared_ap = 0;
    for (const auto& use_effect : use_effects)
    {
        double use_effect_ap = get_use_effect_ap_equivalent(use_effect, special_stats, special_stats_ap, sim_time);
        if (use_effect.effect_socket == Use_effect::Effect_socket ::shared)
        {
            if (use_effect_ap > use_effects_shared_ap)
            {
                use_effects_shared_ap = use_effect_ap;
            }
        }
        else
        {
            use_effects_ap += use_effect_ap;
        }
    }

    double hit_effects_ap = 0;
    for (const auto& hit_effect : mh_wep.hit_effects)
    {
        double hit_effect_ap = get_hit_effect_ap_equivalent(hit_effect, special_stats_ap, mh_wep.swing_speed, 1.0);
        hit_effects_ap += hit_effect_ap;
    }

    return special_stats_ap + use_effects_ap + use_effects_shared_ap + hit_effects_ap;
}

double get_hit_effect_ap_equivalent(const Hit_effect& hit_effect, double total_ap, double swing_speed, double factor)
{
    double hit_effects_ap = 0.0;
    if (hit_effect.type == Hit_effect::Type::damage_magic || hit_effect.type == Hit_effect::Type::damage_physical)
    {
        hit_effects_ap += hit_effect.probability * hit_effect.damage * ap_per_coh / swing_speed * factor;
    }
    else if (hit_effect.type == Hit_effect::Type::extra_hit)
    {
        // Estimate extra hit as 75% the value of crit, since abilities might reset swing to early
        hit_effects_ap += 100.0 * hit_effect.probability * swing_speed * crit_w * factor * 0.75;
    }
    else if (hit_effect.type == Hit_effect::Type::stat_boost)
    {
        // Empyrean demolisher, assume uptime of 25%
        hit_effects_ap += total_ap * hit_effect.special_stats_boost.haste * 0.25 * factor;
    }
    return hit_effects_ap;
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

double get_use_effect_ap_equivalent(const Use_effect& use_effect, const Special_stats& special_stats, double total_ap,
                                    double sim_time)
{
    double ap_during_active = get_active_use_effect_ap_equivalent(use_effect, special_stats, total_ap);
    return ap_during_active * std::min(use_effect.duration / sim_time, 1.0);
}

double get_hit_crit_expertise_ap_equivalent(const Special_stats& special_stats, Weapon_type weapon_type)
{
    int target_defence_level = 365;
    int skill_diff = target_defence_level - 350;
    int base_skill_diff = 15;
    double crit_chance = special_stats.critical_strike - base_skill_diff * 0.2 - 1.8; // 1.8 flat aura modifier

    // Miss chance
    double base_miss_chance;
    int hit_penalty = 0;
    if (skill_diff > 10)
    {
        base_miss_chance = 5.0 + skill_diff * 0.2;
        hit_penalty = 1;
    }
    else if (skill_diff > 0)
    {
        base_miss_chance = 5.0 + skill_diff * 0.1;
    }
    else
    {
        base_miss_chance = 5.0;
    }
    double miss_chance_yellow = base_miss_chance + hit_penalty;
    double dw_miss_chance = (base_miss_chance * 0.8 + 20.0);
    double miss_chance = dw_miss_chance - std::max(special_stats.hit - hit_penalty, 0.0);

    // Dodge chance
    double dodge_chance;
    if (weapon_type == Weapon_type::sword)
    {
        if (base_skill_diff > 0)
        {
            dodge_chance = std::max(std::max(5 + skill_diff * 0.1, 5.0) - (special_stats.expertise + special_stats.sword_expertise), 0.0);
        }
        else
        {
            dodge_chance = std::max(std::max(5 - base_skill_diff * 0.04, 0.0) - (special_stats.expertise + special_stats.sword_expertise), 0.0);
        }
    }
    else if (weapon_type == Weapon_type::mace)
    {
        if (base_skill_diff > 0)
        {
            dodge_chance = std::max(std::max(5 + skill_diff * 0.1, 5.0) - (special_stats.expertise + special_stats.mace_expertise), 0.0);
        }
        else
        {
            dodge_chance = std::max(std::max(5 - base_skill_diff * 0.04, 0.0) - (special_stats.expertise + special_stats.mace_expertise), 0.0);
        }
    }
    else if (weapon_type == Weapon_type::axe)
    {
        if (base_skill_diff > 0)
        {
            dodge_chance = std::max(std::max(5 + skill_diff * 0.1, 5.0) - (special_stats.expertise + special_stats.axe_expertise), 0.0);
        }
        else
        {
            dodge_chance = std::max(std::max(5 - base_skill_diff * 0.04, 0.0) - (special_stats.expertise + special_stats.axe_expertise), 0.0);
        }
    }
    else
    {
        if (base_skill_diff > 0)
        {
            dodge_chance = std::max(std::max(5 + skill_diff * 0.1, 5.0) - special_stats.expertise, 0.0);
        }
        else
        {
            dodge_chance = std::max(std::max(5 - base_skill_diff * 0.04, 0.0) - special_stats.expertise, 0.0);
        }
    }
    double crit_cap = 100 - (miss_chance + dodge_chance + 40);

    double ap_from_crit{};
    if (crit_chance > crit_cap)
    {
        ap_from_crit = (crit_chance - crit_cap) * crit_w_cap + crit_cap * crit_w;
    }
    else
    {
        ap_from_crit = crit_chance * crit_w;
    }

    double ap_from_hit{};
    if (special_stats.hit > miss_chance_yellow)
    {
        ap_from_hit = (special_stats.hit - miss_chance_yellow) * hit_w_cap + miss_chance_yellow * hit_w;
    }
    else
    {
        ap_from_hit = special_stats.hit * hit_w;
    }

    double ap_from_expertise{};
    if (special_stats.expertise < dodge_chance)
    {
        ap_from_expertise = special_stats.expertise * expertise_w;
    }
    else
    {
        ap_from_expertise = 0;
    }

    return ap_from_crit + ap_from_hit + ap_from_expertise;
}

bool is_strictly_weaker(Special_stats special_stats1, Special_stats special_stats2)
{
    bool geq = (special_stats2.hit >= special_stats1.hit) &&
               (special_stats2.critical_strike >= special_stats1.critical_strike) &&
               (special_stats2.attack_power >= special_stats1.attack_power) &&
               (special_stats2.axe_skill >= special_stats1.axe_skill) &&
               (special_stats2.sword_skill >= special_stats1.sword_skill) &&
               (special_stats2.mace_skill >= special_stats1.mace_skill) &&
               (special_stats2.dagger_skill >= special_stats1.dagger_skill) &&
               (special_stats2.bonus_damage >= special_stats1.bonus_damage) &&
               (special_stats2.damage_mod_physical >= special_stats1.damage_mod_physical) &&
               (special_stats2.two_hand_sword_skill >= special_stats1.two_hand_sword_skill) &&
               (special_stats2.two_hand_mace_skill >= special_stats1.two_hand_mace_skill) &&
               (special_stats2.two_hand_axe_skill >= special_stats1.two_hand_axe_skill);
    bool gre = (special_stats2.hit > special_stats1.hit) ||
               (special_stats2.critical_strike > special_stats1.critical_strike) ||
               (special_stats2.attack_power > special_stats1.attack_power) ||
               (special_stats2.axe_skill > special_stats1.axe_skill) ||
               (special_stats2.sword_skill > special_stats1.sword_skill) ||
               (special_stats2.mace_skill > special_stats1.mace_skill) ||
               (special_stats2.dagger_skill > special_stats1.dagger_skill) ||
               (special_stats2.bonus_damage > special_stats1.bonus_damage) ||
               (special_stats2.damage_mod_physical >= special_stats1.damage_mod_physical) ||
               (special_stats2.two_hand_sword_skill > special_stats1.two_hand_sword_skill) ||
               (special_stats2.two_hand_mace_skill > special_stats1.two_hand_mace_skill) ||
               (special_stats2.two_hand_axe_skill > special_stats1.two_hand_axe_skill);
    return geq && gre;
}

double estimate_special_stats_high(const Special_stats& special_stats)
{

    // Assume 1.8 speed for the high estimation
    double high_estimation = special_stats.bonus_damage / 1.8 * 14;

    // Assume 3k AP for the high estimate of the damage multiplier
    high_estimation += special_stats.damage_mod_physical * 3000 +
                       special_stats.damage_mod_physical * 3000 * special_stats.critical_strike / 100;

    high_estimation +=
        special_stats.attack_power + special_stats.hit * hit_w + special_stats.critical_strike * crit_w;
    return high_estimation;
}

double estimate_special_stats_low(const Special_stats& special_stats)
{

    // Assume 2.6 speed for the low estimation
    double low_estimation = special_stats.bonus_damage / 2.6 * 14;

    // Assume 1.5k AP for the high estimate of the damage multiplier
    low_estimation += special_stats.damage_mod_physical * 1500 +
                      special_stats.damage_mod_physical * 1500 * special_stats.critical_strike / 100;

    low_estimation += special_stats.attack_power + special_stats.hit * hit_w_cap +
                      special_stats.critical_strike * crit_w_cap;
    return low_estimation;
}

bool estimate_special_stats_smart_no_skill(const Special_stats& special_stats1, const Special_stats& special_stats2)
{
    Special_stats diff = special_stats2 - special_stats1;
    Special_stats res_1{};
    Special_stats res_2{};
    diff.critical_strike > 0 ? res_2.critical_strike = diff.critical_strike :
                               res_1.critical_strike = -diff.critical_strike;
    diff.hit > 0 ? res_2.hit = diff.hit : res_1.hit = -diff.hit;
    diff.attack_power > 0 ? res_2.attack_power = diff.attack_power : res_1.attack_power = -diff.attack_power;
    diff.bonus_damage > 0 ? res_2.bonus_damage = diff.bonus_damage : res_1.bonus_damage = -diff.bonus_damage;
    double ap_1 = estimate_special_stats_high(res_1);
    double ap_2 = estimate_special_stats_low(res_2);
    return ap_2 > ap_1;
}

double estimate_stat_diff(Special_stats special_stats1, Special_stats special_stats2)
{
    Special_stats diff = special_stats2 - special_stats1;
    Special_stats res_1{};
    Special_stats res_2{};
    diff.critical_strike > 0 ? res_2.critical_strike = diff.critical_strike :
                               res_1.critical_strike = -diff.critical_strike;
    diff.hit > 0 ? res_2.hit = diff.hit : res_1.hit = -diff.hit;
    diff.attack_power > 0 ? res_2.attack_power = diff.attack_power : res_1.attack_power = -diff.attack_power;
    diff.bonus_damage > 0 ? res_2.bonus_damage = diff.bonus_damage : res_1.bonus_damage = -diff.bonus_damage;
    diff.damage_mod_physical > 0 ? res_2.damage_mod_physical = diff.damage_mod_physical :
                                   res_1.damage_mod_physical = -diff.damage_mod_physical;
    diff.axe_skill > 0 ? res_2.axe_skill = diff.axe_skill : res_1.axe_skill = -diff.axe_skill;
    diff.sword_skill > 0 ? res_2.sword_skill = diff.sword_skill : res_1.sword_skill = -diff.sword_skill;
    diff.mace_skill > 0 ? res_2.mace_skill = diff.mace_skill : res_1.mace_skill = -diff.mace_skill;
    diff.dagger_skill > 0 ? res_2.dagger_skill = diff.dagger_skill : res_1.dagger_skill = -diff.dagger_skill;
    diff.two_hand_sword_skill > 0 ? res_2.two_hand_sword_skill = diff.two_hand_sword_skill :
                                    res_1.two_hand_sword_skill = -diff.two_hand_sword_skill;
    diff.two_hand_axe_skill > 0 ? res_2.two_hand_axe_skill = diff.two_hand_axe_skill :
                                  res_1.two_hand_axe_skill = -diff.two_hand_axe_skill;
    diff.two_hand_mace_skill > 0 ? res_2.two_hand_mace_skill = diff.two_hand_mace_skill :
                                   res_1.two_hand_mace_skill = -diff.two_hand_mace_skill;
    double ap_1 = estimate_special_stats_high(res_1);
    double ap_2 = estimate_special_stats_low(res_2);
    return ap_2 - ap_1;
}

std::string percent_to_str(double value)
{
    std::ostringstream stream;
    stream << std::setprecision(3) << value << "%";
    return stream.str();
}

std::string percent_to_str(const std::string& stat_name, double value, const std::string& description)
{
    std::ostringstream stream;
    stream << stat_name << ": " << std::setprecision(3) << "<b>" << value << "%</b> " << description << "<br>";
    return stream.str();
}

std::string percent_to_str(const std::string& stat_name, double value1, const std::string& description1, double value2,
                           const std::string& description2)
{
    std::ostringstream stream;
    stream << stat_name << ": " << std::setprecision(3) << "<b>" << value1 << "%</b> " << description1 << ". (<b>"
           << value2 << "%</b> " << description2 << ")<br>";
    return stream.str();
}

bool find_string(const std::vector<std::string>& string_vec, const std::string& match_string)
{
    for (const auto& string : string_vec)
    {
        if (string == match_string)
        {
            return true;
        }
    }
    return false;
}

double find_value(const std::vector<std::string>& string_vec, const std::vector<double>& double_vec,
                  const std::string& match_string)
{
    size_t index = 0;
    for (const auto& string : string_vec)
    {
        if (string == match_string)
        {
            return double_vec[index];
        }
        index++;
    }
    std::cout << "WARN: Could not find: " << match_string << std::endl;
    return 0.0;
}

Race get_race(const std::string& race)
{
    if (race == "human")
    {
        return Race::human;
    }
    else if (race == "gnome")
    {
        return Race::gnome;
    }
    else if (race == "dwarf")
    {
        return Race::dwarf;
    }
    else if (race == "night_elf")
    {
        return Race::night_elf;
    }
    else if (race == "draenei")
    {
        return Race::draenei;
    }
    else if (race == "orc")
    {
        return Race::orc;
    }
    else if (race == "troll")
    {
        return Race::troll;
    }
    else if (race == "undead")
    {
        return Race::undead;
    }
    else if (race == "tauren")
    {
        return Race::tauren;
    }
    else
    {
        std::cout << "Race not found!!! picking human"
                  << "\n";
        return Race::human;
    }
}

Character get_character_of_race(const std::string& race)
{
    if (race == "human")
    {
        return {Race::human, 70};
    }
    else if (race == "gnome")
    {
        return {Race::gnome, 70};
    }
    else if (race == "dwarf")
    {
        return {Race::dwarf, 70};
    }
    else if (race == "night_elf")
    {
        return {Race::night_elf, 70};
    }
        else if (race == "draenei")
    {
        return {Race::draenei, 70};
    }
    else if (race == "orc")
    {
        return {Race::orc, 70};
    }
    else if (race == "troll")
    {
        return {Race::troll, 70};
    }
    else if (race == "undead")
    {
        return {Race::undead, 70};
    }
    else if (race == "tauren")
    {
        return {Race::tauren, 70};
    }
    else
    {
        std::cout << "Race not found!!! picking human"
                  << "\n";
        return {Race::human, 70};
    }
}

std::string string_with_precision(int amount)
{
    std::ostringstream stream;
    stream << amount;
    return stream.str();
}

std::string string_with_precision(double amount, size_t precision)
{
    std::ostringstream stream;
    stream << std::setprecision(precision) << amount;
    return stream.str();
}