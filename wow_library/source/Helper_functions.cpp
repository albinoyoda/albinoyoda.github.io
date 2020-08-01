#include "Helper_functions.hpp"

double get_ap_equivalent(const Special_stats& special_stats, int relevant_skill, double swing_speed,
                         double weapon_damage)
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

    double ap_equiv{};
    if (crit_chance > crit_cap)
    {
        ap_equiv = (crit_chance - crit_cap) * crit_w_cap + crit_cap * crit_w;
    }
    else
    {
        ap_equiv = crit_chance * crit_w;
    }

    if (special_stats.hit > miss_chance_yellow)
    {
        ap_equiv += (special_stats.hit - miss_chance_yellow) * hit_w_cap + miss_chance_yellow * hit_w;
    }
    else
    {
        ap_equiv += special_stats.hit * hit_w;
    }

    // ish good estimation
    ap_equiv += special_stats.chance_for_extra_hit / 2 * hit_w;

    int extra_skill = relevant_skill - 300;
    if (extra_skill > 0)
    {
        ap_equiv += std::min(5, extra_skill) * skill_w;
    }
    if (extra_skill > 5)
    {
        ap_equiv += std::min(5, extra_skill - 5) * skill_w_soft;
    }
    if (extra_skill > 10)
    {
        ap_equiv += extra_skill - 10 * skill_w_hard;
    }

    ap_equiv += (weapon_damage + special_stats.bonus_damage) / swing_speed * 14;

    return ap_equiv;
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
               (special_stats2.bonus_damage >= special_stats1.bonus_damage);
    bool gre = (special_stats2.hit > special_stats1.hit) ||
               (special_stats2.critical_strike > special_stats1.critical_strike) ||
               (special_stats2.attack_power > special_stats1.attack_power) ||
               (special_stats2.axe_skill > special_stats1.axe_skill) ||
               (special_stats2.sword_skill > special_stats1.sword_skill) ||
               (special_stats2.mace_skill > special_stats1.mace_skill) ||
               (special_stats2.dagger_skill > special_stats1.dagger_skill) ||
               (special_stats2.bonus_damage > special_stats1.bonus_damage);
    return geq && gre;
}

double estimate_special_stats_high(const Special_stats& special_stats)
{
    int max_skill = std::max(special_stats.axe_skill, 0);
    max_skill = std::max(special_stats.sword_skill, max_skill);
    max_skill = std::max(special_stats.mace_skill, max_skill);
    max_skill = std::max(special_stats.dagger_skill, max_skill);

    // Assume 1.8 speed for the high estimation
    double high_estimation = special_stats.bonus_damage / 1.8 * 14;

    high_estimation += special_stats.attack_power + special_stats.hit * hit_w + special_stats.critical_strike * crit_w +
                       max_skill * skill_w;
    return high_estimation;
}

double estimate_special_stats_low(const Special_stats& special_stats)
{
    int max_skill = std::max(special_stats.axe_skill, 0);
    max_skill = std::max(special_stats.sword_skill, max_skill);
    max_skill = std::max(special_stats.mace_skill, max_skill);
    max_skill = std::max(special_stats.dagger_skill, max_skill);

    // Assume 2.6 speed for the low estimation
    double low_estimation = special_stats.bonus_damage / 2.6 * 14;

    low_estimation += special_stats.attack_power + special_stats.hit * hit_w_cap +
                      special_stats.critical_strike * crit_w_cap + max_skill * skill_w_hard;
    return low_estimation;
}

bool estimate_special_stats_smart(const Special_stats& special_stats1, const Special_stats& special_stats2)
{
    Special_stats diff = special_stats2 - special_stats1;
    Special_stats res_1{};
    Special_stats res_2{};
    diff.critical_strike > 0 ? res_2.critical_strike = diff.critical_strike :
                               res_1.critical_strike = -diff.critical_strike;
    diff.hit > 0 ? res_2.hit = diff.hit : res_1.hit = -diff.hit;
    diff.attack_power > 0 ? res_2.attack_power = diff.attack_power : res_1.attack_power = -diff.attack_power;
    diff.bonus_damage > 0 ? res_2.bonus_damage = diff.bonus_damage : res_1.bonus_damage = -diff.bonus_damage;
    diff.axe_skill > 0 ? res_2.axe_skill = diff.axe_skill : res_1.axe_skill = -diff.axe_skill;
    diff.sword_skill > 0 ? res_2.sword_skill = diff.sword_skill : res_1.sword_skill = -diff.sword_skill;
    diff.mace_skill > 0 ? res_2.mace_skill = diff.mace_skill : res_1.mace_skill = -diff.mace_skill;
    diff.dagger_skill > 0 ? res_2.dagger_skill = diff.dagger_skill : res_1.dagger_skill = -diff.dagger_skill;
    double ap_1 = estimate_special_stats_high(res_1);
    double ap_2 = estimate_special_stats_low(res_2);
    return ap_2 > ap_1;
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

bool estimated_weaker(Special_stats special_stats1, Special_stats special_stats2)
{
    return estimate_special_stats_smart(special_stats1, special_stats2);
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
        return {Race::human, 60};
    }
    else if (race == "gnome")
    {
        return {Race::gnome, 60};
    }
    else if (race == "dwarf")
    {
        return {Race::dwarf, 60};
    }
    else if (race == "night_elf")
    {
        return {Race::night_elf, 60};
    }
    else if (race == "orc")
    {
        return {Race::orc, 60};
    }
    else if (race == "troll")
    {
        return {Race::troll, 60};
    }
    else if (race == "undead")
    {
        return {Race::undead, 60};
    }
    else if (race == "tauren")
    {
        return {Race::tauren, 60};
    }
    else
    {
        std::cout << "Race not found!!! picking human"
                  << "\n";
        return {Race::human, 60};
    }
}

std::string string_with_precision(double amount, size_t precision)
{
    std::ostringstream stream;
    stream << std::setprecision(precision) << amount;
    return stream.str();
}