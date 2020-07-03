#include <Armory.hpp>
#include <Character.hpp>
#include <Combat_simulator.hpp>

#include "sim_interface.hpp"

#include <sstream>

#include <iostream>
#include <ctime>
#include <algorithm>

namespace
{
    std::string percent_to_str(double value)
    {
        std::ostringstream stream;
        stream << std::setprecision(3) << value << "%";
        return stream.str();
    }

    bool find_string(const std::vector<std::string> &string_vec, const std::string &match_string)
    {
        for (const auto &string : string_vec)
        {
            if (string == match_string)
            {
                return true;
            }
        }
        return false;
    }

    inline double normalCDF(double value)
    {
        return 0.5 * erfc(-value * M_SQRT1_2);
    }

    double find_cdf_quantile(double target_quantile, double precision)
    {
        double x = 0.0;
        double quantile = 0.5;
        while (quantile < target_quantile)
        {
            x += precision;
            quantile = normalCDF(x);
        }
        return x;
    }

    constexpr int get_skill_of_type(const Special_stats &special_stats, Weapon_type weapon_type)
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
        }
        return 300;
    }

//std::vector<double> find_cdf_quantile(const std::vector<double>& target_quantiles, double precision)
//{
//    std::vector<double> x_values;
//    x_values.reserve(target_quantiles.size());
//    for (const auto& quantile : target_quantiles) {
//        x_values.emplace_back(find_cdf_quantile(quantile, precision));
//    }
//    return x_values;
//}

    Race get_race(const std::string &race)
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
            std::cout << "Race not found!!! picking human" << "\n";
            return Race::human;
        }
    }

    std::string string_with_precision(double amount, size_t precision)
    {
        std::ostringstream stream;
        stream << std::setprecision(precision) << amount;
        return stream.str();
    }

    constexpr double crit_w = 35;
    constexpr double crit_w_cap = 25;
    constexpr double hit_w = 40;
    constexpr double hit_w_cap = 15;
    constexpr double skill_w = 195.0 / 5;
    constexpr double skill_w_soft = 60.0 / 5;
    constexpr double skill_w_hard = 20.0 / 5;
    constexpr double ap_per_coh = 20 / 6.2;

    double get_ap_equivalent(const Special_stats &special_stats, int relevant_skill, double swing_speed,
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

        //ish good estimation
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

    double get_total_qp_equivalent(const Special_stats &special_stats, const Weapon &wep1, const Weapon &wep2,
                                   const std::vector<Use_effect> &use_effects, double sim_time)
    {
        double attack_power = special_stats.attack_power;
        int main_hand_skill = get_skill_of_type(special_stats, wep1.type);
        int off_hand_skill = get_skill_of_type(special_stats, wep2.type);
        double main_hand_ap = get_ap_equivalent(special_stats, main_hand_skill,
                                                wep1.swing_speed,
                                                (wep1.max_damage + wep1.min_damage) / 2);
        double off_hand_ap = get_ap_equivalent(special_stats, off_hand_skill,
                                               wep2.swing_speed,
                                               (wep2.max_damage + wep2.min_damage) / 2);
        double use_effects_ap = 0;
        double best_use_effects_shared_ap = 0;
        for (const auto &effect : use_effects)
        {
            double use_effect_ap = effect.get_special_stat_equivalent(special_stats).attack_power *
                                   std::min(effect.duration / sim_time, 1.0);
            if (effect.name == "badge_of_the_swarmguard")
            {
                use_effect_ap = 300 * std::min(effect.duration / sim_time, 1.0);
            }
            if (effect.effect_socket == Use_effect::Effect_socket::unique)
            {
                use_effects_ap += use_effect_ap;
            }
            else
            {
                if (use_effect_ap > best_use_effects_shared_ap)
                {
                    best_use_effects_shared_ap = use_effect_ap;
                }
            }
        }
        double hit_effects_ap = 0;
        for (const auto &effect : wep1.hit_effects)
        {
            if (effect.type == Hit_effect::Type::damage_magic_guaranteed ||
                effect.type == Hit_effect::Type::damage_magic ||
                effect.type == Hit_effect::Type::damage_physical)
            {
                hit_effects_ap += effect.probability * effect.damage * ap_per_coh;
            }
            else if (effect.type == Hit_effect::Type::extra_hit)
            {
                hit_effects_ap += effect.probability * hit_w;
            }
            else if (effect.type == Hit_effect::Type::stat_boost)
            {
                // Estimate empyrean demolisher as 10 DPS increase (okay since its only the filtering step)
                hit_effects_ap += 140;
            }
        }
        for (const auto &effect : wep2.hit_effects)
        {
            if (effect.type == Hit_effect::Type::damage_magic_guaranteed ||
                effect.type == Hit_effect::Type::damage_magic ||
                effect.type == Hit_effect::Type::damage_physical)
            {
                hit_effects_ap += effect.probability * effect.damage * ap_per_coh * 0.5;
            }
            else if (effect.type == Hit_effect::Type::extra_hit)
            {
                hit_effects_ap += effect.probability * hit_w * 0.5;
            }
            else if (effect.type == Hit_effect::Type::stat_boost)
            {
                // Estimate empyrean demolisher as 10 DPS increase (okay since its only the filtering step)
                hit_effects_ap += 140 * 0.5;
            }
        }
        return attack_power + (main_hand_ap + 0.625 * off_hand_ap) / 1.625 + use_effects_ap + best_use_effects_shared_ap
               + hit_effects_ap;
    }

    bool is_strictly_weaker(Special_stats special_stats1, Special_stats special_stats2)
    {
        bool geq = (special_stats2.hit >= special_stats1.hit) &&
                   (special_stats2.critical_strike >= special_stats1.critical_strike) &&
                   (special_stats2.attack_power >= special_stats1.attack_power) &&
                   (special_stats2.axe_skill >= special_stats1.axe_skill) &&
                   (special_stats2.sword_skill >= special_stats1.sword_skill) &&
                   (special_stats2.mace_skill >= special_stats1.mace_skill) &&
                   (special_stats2.dagger_skill >= special_stats1.dagger_skill);
        bool gre = (special_stats2.hit > special_stats1.hit) ||
                   (special_stats2.critical_strike > special_stats1.critical_strike) ||
                   (special_stats2.attack_power > special_stats1.attack_power) ||
                   (special_stats2.axe_skill > special_stats1.axe_skill) ||
                   (special_stats2.sword_skill > special_stats1.sword_skill) ||
                   (special_stats2.mace_skill > special_stats1.mace_skill) ||
                   (special_stats2.dagger_skill > special_stats1.dagger_skill);
        return geq && gre;
    }

    double estimate_special_stats_high(const Special_stats &special_stats)
    {
        int max_skill = std::max(special_stats.axe_skill, 0);
        max_skill = std::max(special_stats.sword_skill, max_skill);
        max_skill = std::max(special_stats.mace_skill, max_skill);
        max_skill = std::max(special_stats.dagger_skill, max_skill);

        double high_estimation = special_stats.attack_power + special_stats.hit * hit_w +
                                 special_stats.critical_strike * crit_w + max_skill * skill_w;
        return high_estimation;
    }

    double estimate_special_stats_low(const Special_stats &special_stats)
    {
        int max_skill = std::max(special_stats.axe_skill, 0);
        max_skill = std::max(special_stats.sword_skill, max_skill);
        max_skill = std::max(special_stats.mace_skill, max_skill);
        max_skill = std::max(special_stats.dagger_skill, max_skill);
        double low_estimation = special_stats.attack_power + special_stats.hit * hit_w_cap +
                                special_stats.critical_strike * crit_w_cap + max_skill * skill_w_hard;
        return low_estimation;
    }

    bool estimated_weaker(Special_stats special_stats1, Special_stats special_stats2)
    {
        double item_1_overest = estimate_special_stats_high(special_stats1);
        double item_2_underest = estimate_special_stats_low(special_stats2);
        return item_1_overest < item_2_underest;
    }
}

struct Item_popularity
{
    Item_popularity() = default;

    Item_popularity(std::string name, size_t counter)
            :
            name{std::move(name)},
            counter{counter} {}

    std::string name;
    size_t counter{};
};

bool operator<(const Item_popularity &left, const Item_popularity &right)
{
    return left.counter < right.counter;
}

bool operator==(const Item_popularity &left, const Item_popularity &right)
{
    return left.counter == right.counter;
}

bool operator<(const Item_popularity &left, const std::string &right)
{
    return left.name < right;
}

bool operator==(const Item_popularity &left, const std::string &right)
{
    return left.name == right;
}

class Item_optimizer
{
public:
    struct Sim_result_t
    {
        Sim_result_t() = default;

        Sim_result_t(size_t index, double mean_dps, double variance, double ap_equivalent)
                :
                index{index},
                mean_dps{mean_dps},
                variance{variance},
                ap_equivalent{ap_equivalent} {}

        size_t index{};
        double mean_dps{};
        double variance{};
        double ap_equivalent{};
    };

    static std::vector<std::vector<Weapon>> get_weapon_combinations(const std::vector<Weapon> &weapons);

    std::vector<std::vector<Armor>> get_combinations(const std::vector<Armor> &armors);

    void item_setup(const std::vector<std::string> &armor_vec, const std::vector<std::string> &weapons_vec);

    void compute_combinations();

    void extract_armors(const std::vector<std::string> &armor_vec);

    void extract_weapons(const std::vector<std::string> &weapon_vec);

    std::vector<size_t> get_item_ids(size_t index);

    Character generate_character(const std::vector<size_t> &item_ids);

    Character construct(size_t index);

    std::vector<Armor>
    remove_strictly_weaker_items(const std::vector<Armor> &armors, const Special_stats &special_stats,
                                 std::string &debug_message);

    void filter_weaker_items(const Special_stats &special_stats, std::string &debug_message);

    void fill_empty_armor();

    void fill_empty_weapons();

    void fill(std::vector<Armor> &vec, Socket socket, std::string name);

    std::vector<Armor> helmets;
    std::vector<Armor> necks;
    std::vector<Armor> shoulders;
    std::vector<Armor> backs;
    std::vector<Armor> chests;
    std::vector<Armor> wrists;
    std::vector<Armor> hands;
    std::vector<Armor> belts;
    std::vector<Armor> legs;
    std::vector<Armor> boots;
    std::vector<Armor> ranged;
    std::vector<Armor> rings;
    std::vector<Armor> trinkets;
    std::vector<Weapon> weapons;

    std::vector<std::vector<Weapon>> weapon_combinations;
    std::vector<std::vector<Armor>> ring_combinations;
    std::vector<std::vector<Armor>> trinket_combinations;

    size_t total_combinations{};
    std::vector<size_t> combination_vector{};
    std::vector<size_t> cum_combination_vector{};

    Race race;
    Buffs buffs;
    std::vector<std::string> buffs_vec;
    std::vector<std::string> ench_vec;
private:
    Armory armory;
};

bool operator<(const Item_optimizer::Sim_result_t &left, const Item_optimizer::Sim_result_t &right)
{
    return left.mean_dps < right.mean_dps;
}

std::vector<std::vector<Weapon>> Item_optimizer::get_weapon_combinations(const std::vector<Weapon> &weapons)
{
    std::vector<Weapon> main_hands;
    std::vector<Weapon> one_hands;
    std::vector<Weapon> off_hands;
    std::vector<std::vector<Weapon>> weapon_combinations;
    for (const auto &wep : weapons)
    {
        switch (wep.weapon_socket)
        {
            case Weapon_socket::main_hand:
                main_hands.emplace_back(wep);
                break;
            case Weapon_socket::one_hand:
                one_hands.emplace_back(wep);
                break;
            case Weapon_socket::off_hand:
                off_hands.emplace_back(wep);
                break;
            default:
                break;
        }
    }

    for (const auto &main_wep : main_hands)
    {
        for (const auto &one_wep : one_hands)
        {
            weapon_combinations.emplace_back(std::vector<Weapon>{main_wep, one_wep});
        }
        for (const auto &off_wep : off_hands)
        {
            weapon_combinations.emplace_back(std::vector<Weapon>{main_wep, off_wep});
        }
    }

    for (const auto &one_wep : one_hands)
    {
        for (const auto &off_wep : off_hands)
        {
            weapon_combinations.emplace_back(std::vector<Weapon>{one_wep, off_wep});
        }
    }

    // TODO unique tag needed here!!
    for (size_t i_1 = 0; i_1 < one_hands.size(); i_1++)
    {
        for (size_t i_2 = 0; i_2 < one_hands.size(); i_2++)
        {
            if (i_2 != i_1)
            {
                weapon_combinations.emplace_back(std::vector<Weapon>{one_hands[i_1], one_hands[i_2]});
            }
        }
    }

    return weapon_combinations;
}

std::vector<std::vector<Armor>> Item_optimizer::get_combinations(const std::vector<Armor> &armors)
{
    std::vector<std::vector<Armor>> combinations;
    for (size_t i_1 = 0; i_1 < armors.size() - 1; i_1++)
    {
        for (size_t i_2 = i_1 + 1; i_2 < armors.size(); i_2++)
        {
            combinations.emplace_back(std::vector<Armor>{armors[i_1], armors[i_2]});
        }
    }
    return combinations;
}

bool is_armor_valid(const std::string &name)
{
    return !(name.substr(0, 14) == "item_not_found");
}

void Item_optimizer::extract_armors(const std::vector<std::string> &armor_vec)
{
    for (const auto &armor_name : armor_vec)
    {
        for (;;)
        {
            auto armor = armory.find_armor(Socket::head, armor_name);
            if (is_armor_valid(armor.name))
            {
                helmets.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::neck, armor_name);
            if (is_armor_valid(armor.name))
            {
                necks.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::shoulder, armor_name);
            if (is_armor_valid(armor.name))
            {
                shoulders.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::back, armor_name);
            if (is_armor_valid(armor.name))
            {
                backs.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::chest, armor_name);
            if (is_armor_valid(armor.name))
            {
                chests.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::wrist, armor_name);
            if (is_armor_valid(armor.name))
            {
                wrists.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::hands, armor_name);
            if (is_armor_valid(armor.name))
            {
                hands.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::belt, armor_name);
            if (is_armor_valid(armor.name))
            {
                belts.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::legs, armor_name);
            if (is_armor_valid(armor.name))
            {
                legs.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::boots, armor_name);
            if (is_armor_valid(armor.name))
            {
                boots.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::ring, armor_name);
            if (is_armor_valid(armor.name))
            {
                rings.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::trinket, armor_name);
            if (is_armor_valid(armor.name))
            {
                trinkets.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::ranged, armor_name);
            if (is_armor_valid(armor.name))
            {
                ranged.emplace_back(armor);
                break;
            }
        }
    }
}

void Item_optimizer::extract_weapons(const std::vector<std::string> &weapon_vec)
{
    for (const auto &weapon_name : weapon_vec)
    {
        auto weapon = armory.find_weapon(weapon_name);
        if (is_armor_valid(weapon.name))
        {
            weapons.emplace_back(weapon);
        }
    }
}

void Item_optimizer::fill(std::vector<Armor> &vec, Socket socket, std::string name)
{
    if (socket == Socket::trinket || socket == Socket::ring)
    {
        while (vec.size() < 2)
        {
            vec.emplace_back(armory.find_armor(socket, name));
        }
    }
    else
    {
        if (vec.empty())
        {
            vec.emplace_back(armory.find_armor(socket, name));
        }
    }
}

std::vector<Armor>
Item_optimizer::remove_strictly_weaker_items(const std::vector<Armor> &armors, const Special_stats &special_stats,
                                             std::string &debug_message)
{
    struct Armor_equivalent_stats
    {
        Armor_equivalent_stats() = default;

        Armor_equivalent_stats(size_t index, Special_stats special_stats,
                               std::string name) : index(index),
                                                   special_stats(special_stats),
                                                   name(std::move(name)) {}

        size_t index{};
        Special_stats special_stats;
        Special_stats set_special_stats;
        std::string name;
        bool can_be_estimated{false};
        bool remove{false};
    };

    std::vector<Armor_equivalent_stats> armors_special_stats;
    for (size_t i = 0; i < armors.size(); ++i)
    {
        Special_stats armor_special_stats = armors[i].special_stats;
        armor_special_stats += armors[i].attributes.convert_to_special_stats(special_stats);
        Armor_equivalent_stats armor_equiv{i, armor_special_stats, armors[i].name};
        if (armors[i].set_name != Set::none)
        {
            double ap_equiv_max = 0;
            Special_stats best_special_stats;
            for (const auto &set_bonus : armory.set_bonuses)
            {
                if (armors[i].set_name == set_bonus.set)
                {
                    Special_stats set_special_stats = set_bonus.special_stats;
                    set_special_stats += set_bonus.attributes.convert_to_special_stats(special_stats);
                    double ap_equiv = estimate_special_stats_high(set_special_stats);
                    if (ap_equiv > ap_equiv_max)
                    {
                        ap_equiv_max = ap_equiv;
                        best_special_stats = set_special_stats;
                    }
                }
            }
            armor_equiv.set_special_stats = best_special_stats;
        }
        if (armors[i].use_effects.empty() && armors[i].hit_effects.empty())
        {
            armor_equiv.can_be_estimated = true;
        }
        armors_special_stats.push_back(armor_equiv);
    }

    for (auto &armor1 : armors_special_stats)
    {
        if (armor1.can_be_estimated)
        {
            for (const auto &armor2 : armors_special_stats)
            {
                if (armor1.index != armor2.index)
                {
                    if (is_strictly_weaker(armor1.special_stats + armor1.set_special_stats, armor2.special_stats))
                    {
                        armor1.remove = true;
                        debug_message += "<b>REMOVED: " + armor1.name + "</b>." + armor2.name
                                         + " is strictly better.<br>";
                        break;
                    }
                    if (estimated_weaker(armor1.special_stats + armor1.set_special_stats, armor2.special_stats))
                    {
                        armor1.remove = true;
                        debug_message += "<b>REMOVED: " + armor1.name + "</b>. High-est as: " +
                                         string_with_precision(estimate_special_stats_high(armor1.special_stats) +
                                                               estimate_special_stats_high(armor1.set_special_stats), 3)
                                         + "AP. Eliminated by: ";
                        debug_message += "<b>" + armor2.name + "</b> low-est as: " +
                                         string_with_precision(estimate_special_stats_low(armor2.special_stats), 3)
                                         + "AP.<br>";
                        break;
                    }
                }
            }
        }
    }

    debug_message += "Armors left:<br>";
    std::vector<Armor> filtered_armors;
    for (size_t i = 0; i < armors.size(); ++i)
    {
        if (!armors_special_stats[i].remove)
        {
            filtered_armors.push_back(armors[i]);
            debug_message += "<b>" + armors[i].name + "</b><br>";
        }
    }
    return filtered_armors;
}

void Item_optimizer::filter_weaker_items(const Special_stats &special_stats, std::string &debug_message)
{
    debug_message += "<br>Filtering <b> Helmets: </b><br>";
    helmets = remove_strictly_weaker_items(helmets, special_stats, debug_message);
    debug_message += "<br>Filtering <b> necks: </b><br>";
    necks = remove_strictly_weaker_items(necks, special_stats, debug_message);
    debug_message += "<br>Filtering <b> shoulders: </b><br>";
    shoulders = remove_strictly_weaker_items(shoulders, special_stats, debug_message);
    debug_message += "<br>Filtering <b> backs: </b><br>";
    backs = remove_strictly_weaker_items(backs, special_stats, debug_message);
    debug_message += "<br>Filtering <b> chests: </b><br>";
    chests = remove_strictly_weaker_items(chests, special_stats, debug_message);
    debug_message += "<br>Filtering <b> wrists: </b><br>";
    wrists = remove_strictly_weaker_items(wrists, special_stats, debug_message);
    debug_message += "<br>Filtering <b> hands: </b><br>";
    hands = remove_strictly_weaker_items(hands, special_stats, debug_message);
    debug_message += "<br>Filtering <b> belts: </b><br>";
    belts = remove_strictly_weaker_items(belts, special_stats, debug_message);
    debug_message += "<br>Filtering <b> legs: </b><br>";
    legs = remove_strictly_weaker_items(legs, special_stats, debug_message);
    debug_message += "<br>Filtering <b> boots: </b><br>";
    boots = remove_strictly_weaker_items(boots, special_stats, debug_message);
    debug_message += "<br>Filtering <b> ranged: </b><br>";
    ranged = remove_strictly_weaker_items(ranged, special_stats, debug_message);
    debug_message += "<br>Filtering <b> rings: </b><br>";
    rings = remove_strictly_weaker_items(rings, special_stats, debug_message);
    debug_message += "<br>Filtering <b> trinkets: </b><br>";
    trinkets = remove_strictly_weaker_items(trinkets, special_stats, debug_message);

}

void Item_optimizer::fill_empty_armor()
{
    fill(helmets, Socket::head, "helmet");
    fill(necks, Socket::neck, "neck");
    fill(shoulders, Socket::shoulder, "shoulder");
    fill(backs, Socket::back, "back");
    fill(chests, Socket::chest, "chest");
    fill(wrists, Socket::wrist, "wrists");
    fill(hands, Socket::hands, "hands");
    fill(belts, Socket::belt, "belt");
    fill(legs, Socket::legs, "legs");
    fill(boots, Socket::boots, "boots");
    fill(ranged, Socket::ranged, "ranged");
    fill(rings, Socket::ring, "ring");
    fill(trinkets, Socket::trinket, "trinket");
    fill(trinkets, Socket::trinket, "trinket");
}

void Item_optimizer::fill_empty_weapons()
{
    while (weapons.size() < 2)
    {
        weapons.emplace_back(armory.find_weapon("none"));
    }
    bool is_all_mh = true;
    bool is_all_oh = true;
    for (const auto &wep : weapons)
    {
        if (wep.weapon_socket == Weapon_socket::main_hand)
        {
            is_all_oh = false;
        }
        else if (wep.weapon_socket == Weapon_socket::off_hand)
        {
            is_all_mh = false;
        }
        else
        {
            is_all_mh = false;
            is_all_oh = false;
        }
    }
    if (is_all_mh || is_all_oh)
    {
        weapons.emplace_back(armory.find_weapon("none"));
    }
}

void Item_optimizer::item_setup(const std::vector<std::string> &armor_vec, const std::vector<std::string> &weapons_vec)
{
    extract_armors(armor_vec);
    fill_empty_armor();
    extract_weapons(weapons_vec);
    fill_empty_weapons();
}

void Item_optimizer::compute_combinations()
{
    weapon_combinations = get_weapon_combinations(weapons);
    ring_combinations = get_combinations(rings);
    trinket_combinations = get_combinations(trinkets);

    combination_vector.clear();
    combination_vector.push_back(helmets.size());
    combination_vector.push_back(necks.size());
    combination_vector.push_back(shoulders.size());
    combination_vector.push_back(backs.size());
    combination_vector.push_back(chests.size());
    combination_vector.push_back(wrists.size());
    combination_vector.push_back(hands.size());
    combination_vector.push_back(belts.size());
    combination_vector.push_back(legs.size());
    combination_vector.push_back(boots.size());
    combination_vector.push_back(ranged.size());
    combination_vector.push_back(ring_combinations.size());
    combination_vector.push_back(trinket_combinations.size());
    combination_vector.push_back(weapon_combinations.size());

    cum_combination_vector.clear();
    total_combinations = combination_vector[0];
    cum_combination_vector.push_back(combination_vector[0]);
    for (size_t i = 1; i < combination_vector.size(); i++)
    {
        cum_combination_vector.push_back(cum_combination_vector.back() * combination_vector[i]);
        total_combinations *= combination_vector[i];
    }
}

std::vector<size_t> Item_optimizer::get_item_ids(size_t index)
{
    std::vector<size_t> item_ids;
    item_ids.reserve(14);
    item_ids.push_back(index % combination_vector[0]);
    for (size_t i = 1; i < 14; i++)
    {
        item_ids.push_back(index / cum_combination_vector[i - 1] % combination_vector[i]);
    }
    return item_ids;
}

Character Item_optimizer::generate_character(const std::vector<size_t> &item_ids)
{
    Character character{race, 60};
    character.equip_armor(helmets[item_ids[0]]);
    character.equip_armor(necks[item_ids[1]]);
    character.equip_armor(shoulders[item_ids[2]]);
    character.equip_armor(backs[item_ids[3]]);
    character.equip_armor(chests[item_ids[4]]);
    character.equip_armor(wrists[item_ids[5]]);
    character.equip_armor(hands[item_ids[6]]);
    character.equip_armor(belts[item_ids[7]]);
    character.equip_armor(legs[item_ids[8]]);
    character.equip_armor(boots[item_ids[9]]);
    character.equip_armor(ranged[item_ids[10]]);
    character.equip_armor(ring_combinations[item_ids[11]][0]);
    character.equip_armor(ring_combinations[item_ids[11]][1]);
    character.equip_armor(trinket_combinations[item_ids[12]][0]);
    character.equip_armor(trinket_combinations[item_ids[12]][1]);
    character.equip_weapon(weapon_combinations[item_ids[13]][0],
                           weapon_combinations[item_ids[13]][1]
                          );
    return character;
}

Character Item_optimizer::construct(size_t index)
{
    Character character = generate_character(get_item_ids(index));
    if (find_string(ench_vec, "e+8 strength"))
    {
        character.add_enchant(Socket::head, Enchant::Type::strength);
    }
    else if (find_string(ench_vec, "e+1 haste"))
    {
        character.add_enchant(Socket::head, Enchant::Type::haste);
    }

    if (find_string(ench_vec, "s+30 attack_power"))
    {
        character.add_enchant(Socket::shoulder, Enchant::Type::attack_power);
    }

    if (find_string(ench_vec, "b+3 agility"))
    {
        character.add_enchant(Socket::back, Enchant::Type::agility);
    }

    if (find_string(ench_vec, "c+3 stats"))
    {
        character.add_enchant(Socket::chest, Enchant::Type::minor_stats);
    }
    else if (find_string(ench_vec, "c+4 stats"))
    {
        character.add_enchant(Socket::chest, Enchant::Type::major_stats);
    }

    if (find_string(ench_vec, "w+7 strength"))
    {
        character.add_enchant(Socket::wrist, Enchant::Type::strength7);
    }
    else if (find_string(ench_vec, "w+9 strength"))
    {
        character.add_enchant(Socket::wrist, Enchant::Type::strength9);
    }

    if (find_string(ench_vec, "h+7 strength"))
    {
        character.add_enchant(Socket::hands, Enchant::Type::strength);
    }
    else if (find_string(ench_vec, "h+7 agility"))
    {
        character.add_enchant(Socket::hands, Enchant::Type::agility);
    }
    else if (find_string(ench_vec, "h+15 agility"))
    {
        character.add_enchant(Socket::hands, Enchant::Type::greater_agility);
    }
    else if (find_string(ench_vec, "h+1 haste"))
    {
        character.add_enchant(Socket::hands, Enchant::Type::haste);
    }

    if (find_string(ench_vec, "l+8 strength"))
    {
        character.add_enchant(Socket::legs, Enchant::Type::strength);
    }
    else if (find_string(ench_vec, "l+1 haste"))
    {
        character.add_enchant(Socket::legs, Enchant::Type::haste);
    }

    if (find_string(ench_vec, "b+7 agility"))
    {
        character.add_enchant(Socket::boots, Enchant::Type::agility);
    }

    if (find_string(ench_vec, "mcrusader"))
    {
        character.add_enchant(Socket::main_hand, Enchant::Type::crusader);
    }

    if (find_string(ench_vec, "ocrusader"))
    {
        character.add_enchant(Socket::off_hand, Enchant::Type::crusader);
    }

    // rallying cry
    if (find_string(buffs_vec, "rallying_cry"))
    {
        character.add_buff(buffs.rallying_cry);
    }
    if (find_string(buffs_vec, "dire_maul"))
    {
        character.add_buff(buffs.dire_maul);
    }
    if (find_string(buffs_vec, "songflower"))
    {
        character.add_buff(buffs.songflower);
    }
    if (find_string(buffs_vec, "warchiefs_blessing"))
    {
        character.add_buff(buffs.warchiefs_blessing);
    }
    if (find_string(buffs_vec, "spirit_of_zandalar"))
    {
        character.add_buff(buffs.spirit_of_zandalar);
    }
    if (find_string(buffs_vec, "sayges_fortune"))
    {
        character.add_buff(buffs.sayges_fortune);
    }

    // Player buffs
    if (find_string(buffs_vec, "battle_shout"))
    {
        character.add_buff(buffs.battle_shout);
    }
    if (find_string(buffs_vec, "blessing_of_kings"))
    {
        character.add_buff(buffs.blessing_of_kings);
    }
    if (find_string(buffs_vec, "blessing_of_might"))
    {
        character.add_buff(buffs.blessing_of_might);
    }
    if (find_string(buffs_vec, "windfury_totem"))
    {
        character.add_buff(buffs.windfury_totem);
    }
    if (find_string(buffs_vec, "strength_of_earth_totem"))
    {
        character.add_buff(buffs.strength_of_earth_totem);
    }
    if (find_string(buffs_vec, "grace_of_air_totem"))
    {
        character.add_buff(buffs.grace_of_air_totem);
    }
    if (find_string(buffs_vec, "gift_of_the_wild"))
    {
        character.add_buff(buffs.gift_of_the_wild);
    }
    if (find_string(buffs_vec, "leader_of_the_pack"))
    {
        character.add_buff(buffs.leader_of_the_pack);
    }
    if (find_string(buffs_vec, "trueshot_aura"))
    {
        character.add_buff(buffs.trueshot_aura);
    }
    if (find_string(buffs_vec, "elixir_mongoose"))
    {
        character.add_buff(buffs.elixir_mongoose);
    }
    if (find_string(buffs_vec, "blessed_sunfruit"))
    {
        character.add_buff(buffs.blessed_sunfruit);
    }
    if (find_string(buffs_vec, "smoked_dessert_dumplings"))
    {
        character.add_buff(buffs.smoked_dessert_dumplings);
    }
    if (find_string(buffs_vec, "juju_power"))
    {
        character.add_buff(buffs.juju_power);
    }
    if (find_string(buffs_vec, "elixir_of_giants"))
    {
        character.add_buff(buffs.elixir_of_giants);
    }
    if (find_string(buffs_vec, "juju_might"))
    {
        character.add_buff(buffs.juju_might);
    }
    if (find_string(buffs_vec, "winterfall_firewater"))
    {
        character.add_buff(buffs.winterfall_firewater);
    }
    if (find_string(buffs_vec, "roids"))
    {
        character.add_buff(buffs.roids);
    }
    if (find_string(buffs_vec, "mighty_rage_potion"))
    {
        character.add_buff(buffs.mighty_rage_potion);
    }
    if (find_string(buffs_vec, "dense_stone_main_hand"))
    {
        character.add_weapon_buff(Socket::main_hand, buffs.dense_stone);
    }
    else if (find_string(buffs_vec, "elemental_stone_main_hand"))
    {
        character.add_buff(buffs.elemental_stone);
    }
    if (find_string(buffs_vec, "dense_stone_off_hand"))
    {

        character.add_weapon_buff(Socket::off_hand, buffs.dense_stone);
    }
    else if (find_string(buffs_vec, "elemental_stone_off_hand"))
    {
        character.add_buff(buffs.elemental_stone);
    }

    armory.compute_total_stats(character);
    return character;
}

Sim_output_mult Sim_interface::simulate_mult(const Sim_input_mult &input)
{
    clock_t start_time_main = clock();
    Buffs buffs{};
    Item_optimizer item_optimizer;

    Race race = get_race(input.race[0]);
    item_optimizer.race = race;
    item_optimizer.buffs = buffs;
    item_optimizer.buffs_vec = input.buffs;
    item_optimizer.ench_vec = input.enchants;
    item_optimizer.item_setup(input.armor, input.weapons);

    // Combat settings
    auto temp_buffs = input.buffs;

    if (find_string(input.options, "mighty_rage_potion"))
    {
        //temporary solution
        temp_buffs.emplace_back("mighty_rage_potion");
    }

    // Simulator & Combat settings
    Combat_simulator_config config{};
    config.sim_time = input.fight_time;
    config.opponent_level = input.target_level;

    if (find_string(input.options, "curse_of_recklessness"))
    {
        config.curse_of_recklessness_active = true;
    }
    if (find_string(input.options, "faerie_fire"))
    {
        config.faerie_fire_feral_active = true;
    }
    if (find_string(input.options, "death_wish"))
    {
        config.talents.death_wish = true;
    }
    if (find_string(input.options, "recklessness"))
    {
        config.enable_recklessness = true;
    }
    if (find_string(input.options, "sulfuron_harbinger"))
    {
        config.mode.sulfuron_harbinger = true;
    }
    if (find_string(input.options, "golemagg"))
    {
        config.mode.golemagg = true;
    }
    if (find_string(input.options, "vaelastrasz"))
    {
        config.mode.vaelastrasz = true;
    }
    if (find_string(input.options, "chromaggus"))
    {
        config.mode.chromaggus = true;
    }
    if (find_string(input.options, "use_bt_in_exec_phase"))
    {
        config.combat.use_bt_in_exec_phase = true;
    }
    if (find_string(input.options, "use_hs_in_exec_phase"))
    {
        config.combat.use_hs_in_exec_phase = true;
    }
    if (find_string(input.options, "cleave_if_adds"))
    {
        config.combat.cleave_if_adds = true;
    }
    if (find_string(input.options, "use_hamstring"))
    {
        config.combat.use_hamstring = true;
    }
    config.n_sunder_armor_stacks = input.sunder_armor;

    config.talents.improved_heroic_strike = 2;
    config.talents.unbridled_wrath = 5;
    config.talents.flurry = 5;
    config.talents.anger_management = true;
    config.talents.impale = 2;
    config.talents.improved_execute = 2;
    config.talents.dual_wield_specialization = 5;

    config.combat.heroic_strike_rage_thresh = input.heroic_strike_rage_thresh;
    config.combat.cleave_rage_thresh = input.cleave_rage_thresh;
    config.combat.whirlwind_rage_thresh = input.whirlwind_rage_thresh;
    config.combat.hamstring_cd_thresh = input.hamstring_cd_thresh;
    config.combat.hamstring_thresh_dd = input.hamstring_thresh_dd;
    config.combat.initial_rage = input.initial_rage;

    config.use_sim_time_ramp = true;
    config.enable_bloodrage = true;
    config.use_seed = true;
    config.seed = clock();
//    config.seed = 10000;
    config.fuel_extra_rage = false;
    config.performance_mode = true;

    Combat_simulator simulator(config);

    std::string debug_message;
    item_optimizer.compute_combinations();

    std::cout << "init. Combinations: " << std::to_string(item_optimizer.total_combinations) << "\n";
    debug_message += "Total item combinations: " + std::to_string(item_optimizer.total_combinations) + "<br>";
    debug_message += "Filtering weaker items.<br>";

    {
        auto character = item_optimizer.construct(0);
        item_optimizer.filter_weaker_items(character.total_special_stats, debug_message);
    }

    item_optimizer.compute_combinations();
    debug_message += "Item filter done. Combinations: " + std::to_string(item_optimizer.total_combinations) + "<br>";
    std::cout << "Item filter done. Combinations: " << std::to_string(item_optimizer.total_combinations) << "\n";

//    std::vector<Item_optimizer::Sim_result_t> keepers;
//    keepers.reserve(item_optimizer.total_combinations);
//    for (size_t j = 0; j < item_optimizer.total_combinations; ++j)
//    {
//        keepers.emplace_back(j, 0, 0);
//    }

//    if (item_optimizer.total_combinations > 5000)
//    {
    debug_message += "Applying a heuristic filter. Computing...<br>";
    clock_t start_filter = clock();
    double highest_attack_power = 0;
    double lowest_attack_power = 1e12;
    for (size_t i = 0; i < item_optimizer.total_combinations; ++i)
    {
        Character character = item_optimizer.construct(i);
        double ap_equivalent = get_total_qp_equivalent(character.total_special_stats, character.weapons[0],
                                                       character.weapons[1], character.use_effects,
                                                       config.sim_time);
        if (ap_equivalent > highest_attack_power)
        {
            highest_attack_power = ap_equivalent;
        }
        if (ap_equivalent < lowest_attack_power)
        {
            lowest_attack_power = ap_equivalent;
        }
    }

    debug_message += "Equivalent attack power range: [" + std::to_string(lowest_attack_power) + ", " +
                     std::to_string(highest_attack_power) + "]<br>";
    double filter_value = .6;
    double filtering_ap = (1 - filter_value) * lowest_attack_power + filter_value * highest_attack_power;
    debug_message += "Removing bottom " + string_with_precision(filter_value * 100, 2)
                     + "% sets. AP < " + string_with_precision(filtering_ap, 4) + "<br>";

    std::vector<Item_optimizer::Sim_result_t> keepers;
    keepers.reserve(item_optimizer.total_combinations / 2);
    for (size_t i = 0; i < item_optimizer.total_combinations; ++i)
    {
        Character character = item_optimizer.construct(i);
        double ap_equivalent = get_total_qp_equivalent(character.total_special_stats, character.weapons[0],
                                                       character.weapons[1], character.use_effects,
                                                       config.sim_time);
        if (ap_equivalent > filtering_ap)
        {
            keepers.emplace_back(i, 0, 0, ap_equivalent);
        }
    }
    debug_message += "Set filter done. Combinations: " + std::to_string(keepers.size()) + "<br>";
    std::cout << "Set filter done. Combinations: " << std::to_string(keepers.size()) << "\n";

    if (keepers.size() > 12000)
    {
        debug_message += "To many combinations. Combinations: " + std::to_string(keepers.size()) + "<br>";
        debug_message += "Increasing filter value until 20.000 sets remaining.<br>";
        std::cout << "Set filter done. Combinations: " << std::to_string(keepers.size()) << "\n";
        std::cout << "Increasing filter value until 20.000 sets remaining.<br>\n";
        while (keepers.size() > 12000)
        {
            filter_value += .02;
            filtering_ap = (1 - filter_value) * lowest_attack_power + filter_value * highest_attack_power;
            debug_message += "Removing bottom " + string_with_precision(filter_value * 100, 2)
                             + "% sets. AP < " + string_with_precision(filtering_ap, 4) + "<br>";
            std::cout << "Removing bottom " << string_with_precision(filter_value * 100, 2) << "%\n";
            std::vector<Item_optimizer::Sim_result_t> temp_keepers;
            temp_keepers.reserve(keepers.size());
            for (const auto &keeper : keepers)
            {
                if (keeper.ap_equivalent > filtering_ap)
                {
                    temp_keepers.emplace_back(keeper);
                }
            }
            keepers = temp_keepers;
            debug_message += "Sets remaining: " + std::to_string(keepers.size()) + "<br>";
            std::cout << "Sets remaining: " << std::to_string(keepers.size()) << "\n";
        }
    }
    double time_spent_filter = double(clock() - start_filter) / (double) CLOCKS_PER_SEC;
    debug_message += "Set filter done. Combinations: " + std::to_string(keepers.size()) + "<br>";
    if (filter_value > 0.8)
    {
        debug_message += "WARNING! Pushed the heuristic filter to: " + string_with_precision(filter_value * 100, 2) +
                         "%. Might have filtered away good sets. <br>";
        std::cout << "WARNING! Pushed the heuristic filter to: " << string_with_precision(filter_value * 100, 2)
                  << "\n";
    }
    debug_message += "Time spent filtering: " + std::to_string(time_spent_filter) + "s.<br><br>";

    debug_message += "Starting optimizer! Current combinations:" + std::to_string(keepers.size()) + "<br>";
    std::vector<size_t> batches_per_iteration = {20};
    std::vector<size_t> cumulative_simulations = {0};
    for (int i = 0; i < 40; i++)
    {
        batches_per_iteration.push_back(batches_per_iteration.back() * 1.2);
        cumulative_simulations.push_back(cumulative_simulations.back() + batches_per_iteration[i]);
    }
    cumulative_simulations.push_back(cumulative_simulations.back() + batches_per_iteration.back());
    size_t n_sim{};
    size_t performed_iterations{};
    for (size_t i = 0; i < batches_per_iteration.size(); i++)
    {
        clock_t optimizer_start_time = clock();
        debug_message += "Iteration " + std::to_string(i + 1) + " of " + std::to_string(
                batches_per_iteration.size()) + "<br>";
        debug_message += "Total keepers: " + std::to_string(keepers.size()) + "<br>";

        std::cout << "Iter: " + std::to_string(i) + ". Total keepers: " + std::to_string(keepers.size()) << "\n";

        double best_dps = 0;
        double best_dps_variance = 0;
        size_t iter = 0;
        for (auto &keeper : keepers)
        {
            Character character = item_optimizer.construct(keeper.index);
            simulator.simulate(character, batches_per_iteration[i], keeper.mean_dps,
                               keeper.variance, cumulative_simulations[i]);
            keeper.mean_dps = simulator.get_dps_mean();
            keeper.variance = simulator.get_dps_variance();
            if (keeper.mean_dps > best_dps)
            {
                best_dps = keeper.mean_dps;
                best_dps_variance = keeper.variance;
            }

            // Time taken
            n_sim += batches_per_iteration[i];
            iter++;
            if (keepers.size() < 200)
            {
                if (iter == 2)
                {
                    double time_spent = double(clock() - optimizer_start_time) / (double) CLOCKS_PER_SEC;
                    double n_samples = keepers.size() / double(iter);
                    debug_message += "Batch done in: " + std::to_string(time_spent * n_samples) + " seconds.";
                }
            }
            else
            {
                if (iter == 20)
                {
                    double time_spent = double(clock() - optimizer_start_time) / (double) CLOCKS_PER_SEC;
                    double n_samples = keepers.size() / double(iter);
                    debug_message += "Batch done in: " + std::to_string(time_spent * n_samples) + " seconds.<br>";
                }
            }
        }

        // Check if max time is exceeded
        double time = double(clock() - start_time_main) / (double) CLOCKS_PER_SEC;
        if (time > input.max_optimize_time)
        {
            debug_message += "<b>Maximum time limit (" + string_with_precision(input.max_optimize_time,
                                                                               3) + ") exceeded! </b><br>";
            break;
        }

        // If there are more than 10 item sets: remove weak sets
        if (keepers.size() > 5)
        {
            double quantile = find_cdf_quantile(1 - 1 / static_cast<double>(keepers.size()), 0.01);
            double best_dps_sample_std = Statistics::sample_deviation(std::sqrt(best_dps_variance),
                                                                      cumulative_simulations[i + 1]);
            filter_value = best_dps - quantile * best_dps_sample_std;
            debug_message += "Best combination DPS: " + std::to_string(best_dps) + ", removing sets below: " +
                             std::to_string(best_dps - quantile * best_dps_sample_std) + "<br>";
            std::vector<Item_optimizer::Sim_result_t> temp_keepers;
            temp_keepers.reserve(keepers.size());
            for (const auto &keeper : keepers)
            {
                double keeper_sample_std = Statistics::sample_deviation(std::sqrt(keeper.variance),
                                                                        cumulative_simulations[i + 1]);
                if (keeper.mean_dps + keeper_sample_std * quantile >= filter_value)
                {
                    temp_keepers.emplace_back(keeper);
                }
            }
            // Removed to many sets
            if (temp_keepers.size() < 5)
            {
                size_t attempts = 0;
                while (temp_keepers.size() < 5)
                {
                    std::cout << "removed to many sets. Adding them back again" << "\n";
                    quantile *= 1.01;
                    for (const auto &keeper : keepers)
                    {
                        double keeper_sample_std = Statistics::sample_deviation(std::sqrt(keeper.variance),
                                                                                cumulative_simulations[i + 1]);
                        if (keeper.mean_dps + keeper_sample_std * quantile >= filter_value)
                        {
                            temp_keepers.emplace_back(keeper);
                        }
                    }
                    if (attempts > 200)
                    {
                        break;
                    }
                }
            }
            keepers = temp_keepers;
        }

        if (keepers.size() <= 5 && time > 20)
        {
            debug_message += +"<b>: 20 seconds passed with 5 or less combinations remaining. breaking! </b><br>";
            break;
        }
        performed_iterations = i;
    }

    std::sort(keepers.begin(), keepers.end());
    std::reverse(keepers.begin(), keepers.end());

    std::vector<Character> best_characters;
    best_characters.reserve(keepers.size());
    for (const auto &keeper: keepers)
    {
        best_characters.emplace_back(item_optimizer.construct(keeper.index));
    }
    std::vector<std::vector<Item_popularity>> item_popularity;
    item_popularity.reserve(best_characters.size());
    for (size_t j = 0; j < best_characters[0].armor.size(); j++)
    {
        Item_popularity a{"none", 0};
        item_popularity.push_back({a});
    }
    std::vector<std::vector<Item_popularity>> weapon_popularity;
    weapon_popularity.reserve(best_characters.size());
    for (size_t j = 0; j < 2; j++)
    {
        Item_popularity a{"none", 0};
        weapon_popularity.push_back({a});
    }
    for (auto &best_character : best_characters)
    {
        for (size_t j = 0; j < best_character.armor.size(); j++)
        {
            bool found = false;
            for (auto &item : item_popularity[j])
            {
                if (item.name == best_character.armor[j].name)
                {
                    item.counter++;
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                Item_popularity a{best_character.armor[j].name, 1};
                item_popularity[j].push_back(a);
            }
        }
        for (size_t j = 0; j < 2; j++)
        {
            bool found = false;
            for (auto &item : weapon_popularity[j])
            {
                if (item.name == best_character.weapons[j].name)
                {
                    item.counter++;
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                Item_popularity a{best_character.weapons[j].name, 1};
                weapon_popularity[j].push_back(a);
            }
        }
    }

    for (auto &item : item_popularity)
    {
        std::sort(item.begin(), item.end());
        std::reverse(item.begin(), item.end());
    }

    for (auto &item : weapon_popularity)
    {
        std::sort(item.begin(), item.end());
        std::reverse(item.begin(), item.end());
    }

    std::vector<std::string> socket_names = {"Helmet", "Neck", "Shoulder", "Back", "Chest", "Wrists", "Hands", "Belt",
                                             "Legs", "Boots", "Ranged", "Ring 1", "Ring 2", "Trinket 1", "Trinket 2"};

    std::vector<size_t> socket_combinations = {item_optimizer.helmets.size(), item_optimizer.necks.size(),
                                               item_optimizer.shoulders.size(),
                                               item_optimizer.backs.size(), item_optimizer.chests.size(),
                                               item_optimizer.wrists.size(), item_optimizer.hands.size(),
                                               item_optimizer.belts.size(), item_optimizer.legs.size(),
                                               item_optimizer.boots.size(), item_optimizer.ranged.size(),
                                               item_optimizer.ring_combinations.size(),
                                               item_optimizer.ring_combinations.size(),
                                               item_optimizer.trinket_combinations.size(),
                                               item_optimizer.trinket_combinations.size()};

    std::vector<std::string> weapon_names = {"Main-hand", "Off-hand"};
    size_t weapon_combinations = item_optimizer.weapon_combinations.size();

    std::string message = +"<b>Total number of simulations performed: </b>" + std::to_string(n_sim) + "<br>";
    if (keepers.size() > 5)
    {
        message += "Optimizer did not converge! Item presence in the remaining sets:</b><br>";
        for (size_t j = 0; j < best_characters[0].armor.size(); j++)
        {
            message += "<b>" + socket_names[j] + "</b>" + ": ";
            for (const auto &item : item_popularity[j])
            {
                if (item.name != "none")
                {
                    message += item.name + " - (" +
                               percent_to_str(100.0 * item.counter / best_characters.size()) + "), ";
                }
            }
            message += "<br>";
        }
        for (size_t j = 0; j < 2; j++)
        {
            message += "<b>" + weapon_names[j] + "</b>" + ": ";
            for (const auto &item : weapon_popularity[j])
            {
                if (item.name != "none")
                {
                    message += item.name + " - (" +
                               percent_to_str(100.0 * item.counter / best_characters.size()) + "), ";
                }
            }
            message += "<br>";
        }
        message += "<br>";
    }

    size_t max_number = 5;
    max_number = std::min(best_characters.size(), max_number);

    message += "<b>Showing the " + std::to_string(
            std::min(best_characters.size(), max_number)) + " best item sets <u>sorted</u> by DPS:</b><br>";
    message += "<b>Formatting</b>: <br>(Total slot combinations) - socket - item_name<br>";


    for (size_t i = 0; i < max_number; i++)
    {
        message += "<b>Set " + std::to_string(i + 1) + ":</b><br>";
        message += "DPS: " + string_with_precision(keepers[i].mean_dps, 5) + " (+<b>" + string_with_precision(
                keepers[i].mean_dps - keepers[max_number - 1].mean_dps, 3) + "</b>)<br> ";
        double error_margin = Statistics::sample_deviation(std::sqrt(keepers[i].variance),
                                                           cumulative_simulations[performed_iterations + 1]);
        message += "Error margin DPS: " + string_with_precision(error_margin, 3) + "<br>";
        message += "<b>Stats:</b><br>";
        message += "Hit: " + string_with_precision(best_characters[i].total_special_stats.hit, 3) + " %<br>";
        message += "Crit: " + string_with_precision(best_characters[i].total_special_stats.critical_strike,
                                                    3) + " %<br>";
        message += "Attackpower: " + string_with_precision(
                best_characters[i].total_special_stats.attack_power, 4) + " <br>";
//        message += "Equivalent attackpower: " + string_with_precision(
//                keepers[i].ap_equivalent, 4) + " <br>";
        message += "<b>Armor:</b><br>";
        for (size_t j = 0; j < best_characters[i].armor.size(); j++)
        {
            auto item_pop = *std::find(item_popularity[j].begin(), item_popularity[j].end(),
                                       best_characters[i].armor[j].name);
//            message += "[" + std::to_string(item_pop.counter) + "/" + std::to_string(best_characters.size()) + "] ";
            message += "(" + std::to_string(socket_combinations[j]) + ") ";
            message += "<b>" + socket_names[j] + "</b>" + " - " + best_characters[i].armor[j].name + "<br>";
        }
        message += "<b>Weapons:</b><br>";
        for (size_t j = 0; j < 2; j++)
        {
            auto item_pop = (*std::find(weapon_popularity[j].begin(), weapon_popularity[j].end(),
                                        best_characters[i].weapons[j].name));
//            message += "[" + std::to_string(item_pop.counter) + "/" + std::to_string(best_characters.size()) + "] ";
            message += "(" + std::to_string(weapon_combinations) + ") ";
            message += "<b>" + weapon_names[j] + "</b>" + " - " + best_characters[i].weapons[j].name + "<br>";
        }
        message += "<br>";
    }

    return {{message, debug_message}};
}
