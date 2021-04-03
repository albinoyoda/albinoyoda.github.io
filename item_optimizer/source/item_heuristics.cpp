#include "item_heuristics.hpp"

#include "Use_effects.hpp"

#include <algorithm>

int get_weapon_skill(const Special_stats& special_stats, Weapon_type weapon_type, Weapon_socket weapon_socket)
{
    if (weapon_socket == Weapon_socket::two_hand)
    {
        switch (weapon_type)
        {
        case Weapon_type::sword:
            return special_stats.two_hand_sword_skill;
        case Weapon_type::axe:
            return special_stats.two_hand_axe_skill;
        case Weapon_type::mace:
            return special_stats.two_hand_mace_skill;
        default:
            std::cout << "\nAttempted to retrieve two hand skill which is not axe/sword/mace. Aborting!\n";
            assert(false);
            return 0;
        }
    }
    else
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
            std::cout << "Attempted to retrieve skill which is not axe/sword/mace. Aborting!\n";
            assert(false);
            return 0;
        }
    }
}

double get_character_ap_equivalent(const Special_stats& special_stats, const Weapon& mh_wep, const Weapon& oh_wep,
                                   double sim_time, const std::vector<Use_effect>& use_effects)
{
    double attack_power = special_stats.attack_power;
    int mh_skill = get_weapon_skill(special_stats, mh_wep.type, mh_wep.weapon_socket);
    double mh_hit_crit_skill_ap = get_hit_crit_skill_ap_equivalent(special_stats, mh_skill);

    int oh_skill = get_weapon_skill(special_stats, oh_wep.type, mh_wep.weapon_socket);
    double oh_hit_crit_skill_ap = get_hit_crit_skill_ap_equivalent(special_stats, oh_skill);

    /// Weighted combination of ap from mh and oh, based on the hit-tables
    double hit_crit_skill_ap = (mh_hit_crit_skill_ap + oh_hit_crit_skill_ap * 0.5) / 1.5;

    double mh_ap = ((mh_wep.max_damage + mh_wep.min_damage) / 2 + special_stats.bonus_damage) / mh_wep.swing_speed * 14;
    double oh_ap = ((oh_wep.max_damage + oh_wep.min_damage) / 2 + special_stats.bonus_damage) / oh_wep.swing_speed * 14;
    oh_ap *= 0.625;

    double special_stats_ap = attack_power + hit_crit_skill_ap + mh_ap + oh_ap;

    double use_effects_ap = 0;
    double use_effects_shared_ap = 0;
    for (const auto& use_effect : use_effects)
    {
        double use_effect_ap =
            Use_effects::get_use_effect_ap_equivalent(use_effect, special_stats, special_stats_ap, sim_time);
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
    int mh_skill = get_weapon_skill(special_stats, mh_wep.type, mh_wep.weapon_socket);
    double hit_crit_skill_ap = get_hit_crit_skill_ap_equivalent(special_stats, mh_skill);

    double mh_ap = ((mh_wep.max_damage + mh_wep.min_damage) / 2 + special_stats.bonus_damage) / mh_wep.swing_speed * 14;

    double special_stats_ap = attack_power + hit_crit_skill_ap + mh_ap;

    double use_effects_ap = 0;
    double use_effects_shared_ap = 0;
    for (const auto& use_effect : use_effects)
    {
        double use_effect_ap =
            Use_effects::get_use_effect_ap_equivalent(use_effect, special_stats, special_stats_ap, sim_time);
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

double get_hit_crit_skill_ap_equivalent(const Special_stats& special_stats, int relevant_skill)
{
    int target_defence_level = 315;
    int skill_diff = target_defence_level - relevant_skill;
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
    double dodge_chance = std::max(5 + skill_diff * 0.1, 5.0);
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

    int extra_skill = relevant_skill - 300;
    double ap_from_skill{};
    if (extra_skill > 8)
    {
        ap_from_skill = (extra_skill - 8) * skill_w_hard + 3 * skill_w_soft + 5 * skill_w;
    }
    else if (extra_skill > 5)
    {
        ap_from_skill = (extra_skill - 5) * skill_w_soft + 5 * skill_w;
    }
    else if (extra_skill > 0)
    {
        ap_from_skill = extra_skill * skill_w;
    }

    return ap_from_crit + ap_from_hit + ap_from_skill;
}

double estimate_special_stats_high(const Special_stats& special_stats)
{
    int max_skill = std::max(special_stats.axe_skill, 0);
    max_skill = std::max(special_stats.sword_skill, max_skill);
    max_skill = std::max(special_stats.mace_skill, max_skill);
    max_skill = std::max(special_stats.dagger_skill, max_skill);
    max_skill = std::max(special_stats.two_hand_sword_skill, max_skill);
    max_skill = std::max(special_stats.two_hand_mace_skill, max_skill);
    max_skill = std::max(special_stats.two_hand_axe_skill, max_skill);

    double ap_from_skill = max_skill <= 5 ? max_skill * skill_w : 5 * skill_w + (max_skill - 5) * skill_w_soft;

    // Assume 1.8 speed for the high estimation
    double high_estimation = special_stats.bonus_damage / 1.8 * 14;

    // Assume 3k AP for the high estimate of the damage multiplier
    high_estimation += special_stats.damage_mod_physical * 3000 +
                       special_stats.damage_mod_physical * 3000 * special_stats.critical_strike / 100;

    high_estimation +=
        special_stats.attack_power + special_stats.hit * hit_w + special_stats.critical_strike * crit_w + ap_from_skill;
    return high_estimation;
}

double estimate_special_stats_low(const Special_stats& special_stats)
{
    int max_skill = std::max(special_stats.axe_skill, 0);
    max_skill = std::max(special_stats.sword_skill, max_skill);
    max_skill = std::max(special_stats.mace_skill, max_skill);
    max_skill = std::max(special_stats.dagger_skill, max_skill);
    max_skill = std::max(special_stats.two_hand_sword_skill, max_skill);
    max_skill = std::max(special_stats.two_hand_mace_skill, max_skill);
    max_skill = std::max(special_stats.two_hand_axe_skill, max_skill);

    // Assume 2.6 speed for the low estimation
    double low_estimation = special_stats.bonus_damage / 2.6 * 14;

    // Assume 1.5k AP for the high estimate of the damage multiplier
    low_estimation += special_stats.damage_mod_physical * 1500 +
                      special_stats.damage_mod_physical * 1500 * special_stats.critical_strike / 100;

    low_estimation += special_stats.attack_power + special_stats.hit * hit_w_cap +
                      special_stats.critical_strike * crit_w_cap + max_skill * skill_w_hard;
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
