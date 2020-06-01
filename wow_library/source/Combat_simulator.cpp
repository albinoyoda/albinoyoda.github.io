#include "../include/Combat_simulator.hpp"

namespace
{
    constexpr double rage_factor = 15.0 / 230.6 / 2.0;

    constexpr double rage_from_damage_taken(double damage)
    {
        return damage * 5 / 2 / 230.6;
    }

    constexpr double rage_generation(double damage)
    {
        return damage * rage_factor;
    }

    constexpr double armor_mitigation(double target_armor, int target_level)
    {
        return target_armor / (target_armor + 400 + 85 * target_level);
    }

    std::vector<double> create_hit_table(double miss, double dodge, double glancing, double crit)
    {
        // Order -> Miss, parry, dodge, block, glancing, crit, hit.
        return {miss, miss + dodge, miss + dodge + glancing, miss + dodge + glancing + crit};
    }
}

void Combat_simulator::cout_damage_parse(Combat_simulator::Hit_type hit_type, Socket weapon_hand,
                                         Combat_simulator::Hit_outcome hit_outcome)
{
    if (weapon_hand == Socket::main_hand)
    {
        if (hit_type == Hit_type::white)
        {
            switch (hit_outcome.hit_result)
            {
                case Hit_result::glancing:
                    simulator_cout("Mainhand glancing hit for: ", int(hit_outcome.damage), " damage.");
                    break;
                case Hit_result::hit:
                    simulator_cout("Mainhand white hit for: ", int(hit_outcome.damage), " damage.");
                    break;
                case Hit_result::crit:
                    simulator_cout("Mainhand crit for: ", int(hit_outcome.damage), " damage.");
                    break;
                case Hit_result::dodge:
                    simulator_cout("Mainhand hit dodged");
                    break;
                case Hit_result::miss:
                    simulator_cout("Mainhand hit missed");
                    break;
                case Hit_result::TBD:
                    // Should never happen!!
                    simulator_cout("BUUUUUUUUUUGGGGGGGGG");
                    break;
            }
        }
        else
        {
            switch (hit_outcome.hit_result)
            {
                case Hit_result::glancing:
                    simulator_cout("BUG: Ability glanced for: ", int(hit_outcome.damage), " damage.");
                    break;
                case Hit_result::hit:
                    simulator_cout("Ability hit for: ", int(hit_outcome.damage), " damage.");
                    break;
                case Hit_result::crit:
                    simulator_cout("Ability crit for: ", int(hit_outcome.damage), " damage.");
                    break;
                case Hit_result::dodge:
                    simulator_cout("Ability dodged");
                    break;
                case Hit_result::miss:
                    simulator_cout("Ability missed");
                    break;
                case Hit_result::TBD:
                    simulator_cout("BUUUUUUUUUUGGGGGGGGG");
                    break;
            }
        }
    }
    else
    {
        switch (hit_outcome.hit_result)
        {
            case Hit_result::glancing:
                simulator_cout("Offhand glancing hit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::hit:
                simulator_cout("Offhand white hit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::crit:
                simulator_cout("Offhand crit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::dodge:
                simulator_cout("Offhand hit dodged");
                break;
            case Hit_result::miss:
                simulator_cout("Offhand hit missed");
                break;
            case Hit_result::TBD:
                simulator_cout("BUUUUUUUUUUGGGGGGGGG");
                break;
        }
    }
}

Combat_simulator::Hit_outcome
Combat_simulator::generate_hit_mh(double damage, Hit_type hit_type, bool recklessness_active)
{
    double random_var = get_uniform_random(100);
    if (hit_type == Hit_type::white)
    {
        if (recklessness_active)
        {
            simulator_cout("Drawing outcome from MH recklessness table");
            int outcome = std::lower_bound(hit_probabilities_recklessness_mh_.begin(),
                                           hit_probabilities_recklessness_mh_.end(),
                                           random_var) - hit_probabilities_recklessness_mh_.begin();
            return {damage * lookup_outcome_mh(outcome), Hit_result(outcome)};
        }
        else
        {
            simulator_cout("Drawing outcome from MH hit table");
            int outcome = std::lower_bound(hit_probabilities_white_mh_.begin(), hit_probabilities_white_mh_.end(),
                                           random_var) - hit_probabilities_white_mh_.begin();
            return {damage * lookup_outcome_mh(outcome), Hit_result(outcome)};
        }
    }
    else
    {
        // Yellow hit
        if (recklessness_active)
        {
            simulator_cout("Drawing outcome from recklessness yellow table");
            int outcome = std::lower_bound(hit_probabilities_recklessness_yellow_.begin(),
                                           hit_probabilities_recklessness_yellow_.end(),
                                           random_var) - hit_probabilities_recklessness_yellow_.begin();
            return {damage * lookup_outcome_mh(outcome), Hit_result(outcome)};
        }
        else
        {
            simulator_cout("Drawing outcome from yellow table");
            int outcome = std::lower_bound(hit_probabilities_yellow_.begin(), hit_probabilities_yellow_.end(),
                                           random_var) - hit_probabilities_yellow_.begin();
            return {damage * lookup_outcome_mh(outcome), Hit_result(outcome)};
        }
    }
}

Combat_simulator::Hit_outcome Combat_simulator::generate_hit_oh(double damage, bool recklessness_active)
{
    if (recklessness_active)
    {
        if (ability_queue_manager.is_ability_queued())
        {
            simulator_cout("Drawing outcome from OH recklessness twohanded hit table");
            double random_var = get_uniform_random(100);
            int outcome = std::lower_bound(hit_probabilities_recklessness_two_hand_.begin(),
                                           hit_probabilities_recklessness_two_hand_.end(),
                                           random_var) - hit_probabilities_recklessness_two_hand_.begin();
            return {damage * lookup_outcome_oh(outcome), Hit_result(outcome)};
        }
        else
        {
            simulator_cout("Drawing outcome from OH recklessness hit table");
            double random_var = get_uniform_random(100);
            int outcome = std::lower_bound(hit_probabilities_recklessness_oh_.begin(),
                                           hit_probabilities_recklessness_oh_.end(),
                                           random_var) - hit_probabilities_recklessness_oh_.begin();
            return {damage * lookup_outcome_oh(outcome), Hit_result(outcome)};
        }
    }
    else
    {
        if (ability_queue_manager.is_ability_queued())
        {
            simulator_cout("Drawing outcome from OH twohanded hit table");
            double random_var = get_uniform_random(100);
            int outcome = std::lower_bound(hit_probabilities_two_hand_.begin(),
                                           hit_probabilities_two_hand_.end(),
                                           random_var) - hit_probabilities_two_hand_.begin();
            return {damage * lookup_outcome_oh(outcome), Hit_result(outcome)};
        }
        else
        {
            simulator_cout("Drawing outcome from OH hit table");
            double random_var = get_uniform_random(100);
            int outcome = std::lower_bound(hit_probabilities_white_oh_.begin(), hit_probabilities_white_oh_.end(),
                                           random_var) -
                          hit_probabilities_white_oh_.begin();
            return {damage * lookup_outcome_oh(outcome), Hit_result(outcome)};
        }
    }
}

Combat_simulator::Hit_outcome Combat_simulator::generate_hit(double damage, Combat_simulator::Hit_type hit_type,
                                                             Socket weapon_hand,
                                                             const Special_stats &special_stats,
                                                             bool recklessness_active,
                                                             bool boss_target)
{
    if (weapon_hand == Socket::main_hand)
    {
        auto hit_outcome = generate_hit_mh(damage, hit_type, recklessness_active);
        if (boss_target)
        {
            hit_outcome.damage *= armor_reduction_factor_ * (1 + special_stats.damage_multiplier);
        }
        else
        {
            hit_outcome.damage *= armor_reduction_factor_add * (1 + special_stats.damage_multiplier);
        }
        cout_damage_parse(hit_type, weapon_hand, hit_outcome);
        return hit_outcome;
    }
    else
    {
        auto hit_outcome = generate_hit_oh(damage, recklessness_active);
        if (boss_target)
        {
            hit_outcome.damage *= armor_reduction_factor_ * (1 + special_stats.damage_multiplier);
        }
        else
        {
            hit_outcome.damage *= armor_reduction_factor_add * (1 + special_stats.damage_multiplier);
        }
        cout_damage_parse(hit_type, weapon_hand, hit_outcome);
        return hit_outcome;
    }
}

void Combat_simulator::compute_hit_table(int level_difference,
                                         int weapon_skill,
                                         Special_stats special_stats,
                                         Socket weapon_hand)
{
    int target_defence_level = config.opponent_level * 5;
    int skill_diff = target_defence_level - weapon_skill;
    int base_skill_diff = level_difference * 5;

    // Crit chance
    double crit_chance;
    if (level_difference == 3)
    {
        crit_chance = special_stats.critical_strike - base_skill_diff * 0.2 - 1.8; // 1.8 flat aura modifier
    }
    else if (level_difference > 0)
    {
        crit_chance = special_stats.critical_strike - base_skill_diff * 0.2;
    }
    else
    {
        crit_chance = special_stats.critical_strike + base_skill_diff * 0.04;
    }

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
    double dw_miss_chance = (base_miss_chance * 0.8 + 20.0);
    double miss_chance = dw_miss_chance - std::max(special_stats.hit - hit_penalty, 0.0);
    double two_hand_miss_chance = std::max(base_miss_chance - special_stats.hit, 0.0);

    // Dodge chance
    double dodge_chance;
    if (level_difference > 0)
    {
        dodge_chance = std::max(5 + skill_diff * 0.1, 5.0);
    }
    else
    {
        dodge_chance = std::max(5 - base_skill_diff * 0.04, 0.0);
    }

    // Glancing blows
    double glancing_chance = 0.0;
    if (level_difference > 0)
    {
        glancing_chance = 10 + level_difference * 10;
    }

    double glancing_penalty;
    if (skill_diff >= 8)
    {
        glancing_penalty = 35.0 - (15.0 - skill_diff) * 4.0;
    }
    else
    {
        glancing_penalty = 5.0;
    }

    if (weapon_hand == Socket::main_hand)
    {
        glancing_factor_mh_ = (100.0 - glancing_penalty) / 100.0;

        hit_probabilities_white_mh_ = create_hit_table(miss_chance, dodge_chance, glancing_chance, crit_chance);

        hit_probabilities_two_hand_ = create_hit_table(two_hand_miss_chance, dodge_chance, glancing_chance,
                                                       crit_chance);

        hit_probabilities_recklessness_mh_ = create_hit_table(miss_chance, dodge_chance, glancing_chance, 100);

        hit_probabilities_recklessness_two_hand_ = create_hit_table(two_hand_miss_chance, dodge_chance, glancing_chance,
                                                                    100);

        hit_probabilities_yellow_ = create_hit_table(two_hand_miss_chance, dodge_chance, 0, crit_chance);

        hit_probabilities_recklessness_yellow_ = create_hit_table(two_hand_miss_chance, dodge_chance, 0, 100);
    }
    else
    {
        glancing_factor_oh_ = (100.0 - glancing_penalty) / 100.0;

        hit_probabilities_white_oh_ = create_hit_table(miss_chance, dodge_chance, glancing_chance, crit_chance);

        hit_probabilities_recklessness_oh_ = create_hit_table(miss_chance, dodge_chance, glancing_chance, 100);
    }
}

void Combat_simulator::manage_flurry(Hit_result hit_result, Special_stats &special_stats, int &flurry_charges,
                                     bool is_ability)
{
    if (config.talents.flurry)
    {
        bool flurry_active = (flurry_charges > 0);
        if (!is_ability)
        {
            flurry_charges--;
            flurry_charges = std::max(0, flurry_charges);
        }
        if (hit_result == Hit_result::crit)
        {
            flurry_charges = 3;
            if (!flurry_active)
            {
                special_stats += {0, 0, 0, 0, 0.05 + 0.05 * config.talents.flurry};
            }
        }
        else if (flurry_active && flurry_charges == 0)
        {
            special_stats -= {0, 0, 0, 0, 0.05 + 0.05 * config.talents.flurry};
        }
        simulator_cout(flurry_charges, " flurry charges");
        assert(special_stats.haste > -.1);
        assert(special_stats.haste < .9);
    }
}

void Combat_simulator::bloodthirst(Weapon_sim &main_hand_weapon, Special_stats &special_stats, double &rage,
                                   bool &recklessness_active,
                                   Damage_sources &damage_sources, int &flurry_charges)
{
    simulator_cout("Bloodthirst!");
    double damage = special_stats.attack_power * 0.45;
    auto hit_outcome = generate_hit(damage, Hit_type::yellow, Socket::main_hand,
                                    special_stats, recklessness_active);
    if (hit_outcome.hit_result == Hit_result::dodge ||
        hit_outcome.hit_result == Hit_result::miss)
    {
        rage -= 6;
    }
    else
    {
        rage -= 30;
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats,
                    rage, recklessness_active, damage_sources, flurry_charges);
    }
    time_keeper_.blood_thirst_cd = 6.0;
    time_keeper_.global_cd = 1.5;
    manage_flurry(hit_outcome.hit_result, special_stats, flurry_charges, true);
    damage_sources.add_damage(Damage_source::bloodthirst, hit_outcome.damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::whirlwind(Weapon_sim &main_hand_weapon, Special_stats &special_stats,
                                 double &rage,
                                 bool &recklessness_active, Damage_sources &damage_sources, int &flurry_charges)
{
    simulator_cout("Whirlwind! #targets = boss + ", adds_in_melee_range, " adds");
    simulator_cout("Whirlwind hits: ", std::min(adds_in_melee_range + 1, 4), " targets");
    double damage = main_hand_weapon.normalized_swing(special_stats.attack_power);
    std::vector<Hit_outcome> hit_outcomes{};
    hit_outcomes.reserve(4);
    for (int i = 0; i < std::min(adds_in_melee_range + 1, 4); i++)
    {
        hit_outcomes.emplace_back(generate_hit(damage, Hit_type::yellow, Socket::main_hand,
                                               special_stats, recklessness_active, i == 0));
    }
    rage -= 25;
    if (hit_outcomes[0].hit_result != Hit_result::dodge &&
        hit_outcomes[0].hit_result != Hit_result::miss)
    {
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats,
                    rage, recklessness_active, damage_sources, flurry_charges);
    }
    time_keeper_.whirlwind_cd = 10;
    time_keeper_.global_cd = 1.5;
    Hit_result result_used_for_flurry = Hit_result::TBD;
    double total_damage = 0;
    for (const auto &hit_outcome : hit_outcomes)
    {
        total_damage += hit_outcome.damage;
        if (hit_outcome.hit_result == Hit_result::crit)
        {
            result_used_for_flurry = Hit_result::crit;
        }
    }
    manage_flurry(result_used_for_flurry, special_stats, flurry_charges, true);
    damage_sources.add_damage(Damage_source::whirlwind, total_damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::execute(Weapon_sim &main_hand_weapon, Special_stats &special_stats,
                               double &rage,
                               bool &recklessness_active, Damage_sources &damage_sources, int &flurry_charges,
                               double execute_rage_cost)
{
    simulator_cout("Execute!");
    double damage = 600 + (rage - execute_rage_cost) * 15;
    auto hit_outcome = generate_hit(damage, Hit_type::yellow, Socket::main_hand,
                                    special_stats, recklessness_active);
    time_keeper_.global_cd = 1.5;
    if (hit_outcome.hit_result == Hit_result::dodge ||
        hit_outcome.hit_result == Hit_result::miss)
    {
        rage *= 0.85;
    }
    else
    {
        rage = 0;
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats,
                    rage, recklessness_active, damage_sources, flurry_charges);
    }
    manage_flurry(hit_outcome.hit_result, special_stats, flurry_charges, true);
    damage_sources.add_damage(Damage_source::execute, hit_outcome.damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::hit_effects(Weapon_sim &weapon, Weapon_sim &main_hand_weapon, Special_stats &special_stats,
                                   double &rage,
                                   bool &recklessness_active, Damage_sources &damage_sources, int &flurry_charges)
{
    for (const auto &hit_effect : weapon.hit_effects)
    {
        double r = get_uniform_random(1);
        if (r < hit_effect.probability)
        {
            if (hit_effect.type != Hit_effect::Type::damage_magic_guaranteed)
            {
                buff_manager_.increment_proc(hit_effect.name);
            }
            switch (hit_effect.type)
            {
                case Hit_effect::Type::extra_hit:
                    simulator_cout("PROC: extra hit from: ", hit_effect.name);
                    swing_weapon(main_hand_weapon, main_hand_weapon, special_stats,
                                 rage,
                                 recklessness_active, damage_sources, flurry_charges, hit_effect.attack_power_boost);
                    break;
                case Hit_effect::Type::damage_magic:
                    damage_sources.add_damage(Damage_source::item_hit_effects, hit_effect.damage * 0.85,
                                              time_keeper_.time);
                    simulator_cout("PROC: ", hit_effect.name, " does ", hit_effect.damage * 0.85, " magic damage.");
                    break;
                case Hit_effect::Type::damage_magic_guaranteed:
                    simulator_cout("Weapon swing with: ", hit_effect.name, " does ", hit_effect.damage * 0.85,
                                   " magic damage.");
                    damage_sources.add_damage(Damage_source::item_hit_effects, hit_effect.damage * 0.85,
                                              time_keeper_.time, false);
                    break;
                case Hit_effect::Type::damage_physical:
                {
                    auto hit = generate_hit(hit_effect.damage, Hit_type::yellow,
                                            Socket::main_hand,
                                            special_stats, recklessness_active);
                    damage_sources.add_damage(Damage_source::item_hit_effects, hit.damage, time_keeper_.time);
                    if (config.display_combat_debug)
                    {
                        std::string result;
                        switch (hit.hit_result)
                        {
                            case Hit_result::hit:
                                result = " hit";
                                break;
                            case Hit_result::crit:
                                result = " crit";
                                break;
                            case Hit_result::dodge:
                                result = " dodge";
                                break;
                            case Hit_result::miss:
                                result = " miss";
                                break;
                            default:
                                result = " bugs";
                                break;
                        }
                        simulator_cout("PROC: ", hit_effect.name, result, " does ", hit.damage, " physical damage");
                    }
                }
                    break;
                case Hit_effect::Type::stat_boost:
                    simulator_cout("PROC: ", hit_effect.name, " stats increased for ", hit_effect.duration, "s");
                    buff_manager_.add(weapon.socket_name + "_" + hit_effect.name,
                                      hit_effect.get_special_stat_equivalent(special_stats),
                                      hit_effect.duration);
                    break;
                default:
                    std::cout << ":::::::::::FAULTY HIT EFFECT IN SIMULATION!!!:::::::::";
                    break;
            }
        }
    }
}

void Combat_simulator::swing_weapon(Weapon_sim &weapon, Weapon_sim &main_hand_weapon, Special_stats &special_stats,
                                    double &rage,
                                    bool &recklessness_active, Damage_sources &damage_sources, int &flurry_charges,
                                    double attack_power_bonus)
{
    std::vector<Hit_outcome> hit_outcomes{};
    hit_outcomes.reserve(2);
    double swing_damage = weapon.swing(special_stats.attack_power + attack_power_bonus);
    if (weapon.socket == Socket::off_hand)
    {
        swing_damage *= (0.5 + 0.025 * config.talents.dual_wield_specialization);
    }
    weapon.internal_swing_timer = weapon.swing_speed / (1 + special_stats.haste);

    // Check if heroic strike should be performed
    if (ability_queue_manager.heroic_strike_queued &&
        weapon.socket == Socket::main_hand &&
        rage >= heroic_strike_rage_cost)
    {
        simulator_cout("Performing heroic strike");
        swing_damage += 138;
        hit_outcomes.emplace_back(generate_hit(swing_damage, Hit_type::yellow, weapon.socket,
                                               special_stats, recklessness_active));
        ability_queue_manager.heroic_strike_queued = false;
        rage -= heroic_strike_rage_cost;
        damage_sources.add_damage(Damage_source::heroic_strike, hit_outcomes[0].damage, time_keeper_.time);
        simulator_cout("Current rage: ", int(rage));
    }
    else if (ability_queue_manager.cleave_queued &&
             weapon.socket == Socket::main_hand &&
             rage >= cleave_rage_cost)
    {
        simulator_cout("Performing cleave! #targets = boss + ", adds_in_melee_range, " adds");
        simulator_cout("Cleave hits: ", std::min(adds_in_melee_range + 1, 2), " targets");
        swing_damage += 50; //talents

        for (int i = 0; i < std::min(adds_in_melee_range + 1, 2); i++)
        {
            hit_outcomes.emplace_back(generate_hit(swing_damage, Hit_type::yellow, weapon.socket,
                                                   special_stats, recklessness_active, i == 0));
        }
        ability_queue_manager.cleave_queued = false;
        rage -= 20;
        double total_damage = 0;
        for (const auto &hit_outcome : hit_outcomes)
        {
            total_damage += hit_outcome.damage;
        }
        damage_sources.add_damage(Damage_source::cleave, total_damage, time_keeper_.time);
        simulator_cout("Current rage: ", int(rage));
    }
    else
    {
        if (weapon.socket == Socket::main_hand && ability_queue_manager.is_ability_queued())
        {
            if (ability_queue_manager.heroic_strike_queued)
            {
                // Failed to pay rage for heroic strike
                simulator_cout("Failed to pay rage for heroic strike");
                ability_queue_manager.heroic_strike_queued = false;
            }
            else
            {
                // Failed to pay rage for cleave
                simulator_cout("Failed to pay rage for cleave");
                ability_queue_manager.cleave_queued = false;
            }
        }

        // Otherwise do white hit
        hit_outcomes.emplace_back(generate_hit(swing_damage, Hit_type::white, weapon.socket,
                                               special_stats, recklessness_active));

        rage += rage_generation(hit_outcomes[0].damage);
        if (hit_outcomes[0].hit_result == Hit_result::dodge)
        {
            simulator_cout("Rage gained from enemy dodging");
            rage += rage_generation(swing_damage * armor_reduction_factor_);
        }
        rage = std::min(100.0, rage);
        if (weapon.socket == Socket::main_hand)
        {
            damage_sources.add_damage(Damage_source::white_mh, hit_outcomes[0].damage, time_keeper_.time);
        }
        else
        {
            damage_sources.add_damage(Damage_source::white_oh, hit_outcomes[0].damage, time_keeper_.time);
        }
        simulator_cout("Current rage: ", int(rage));
    }

    Hit_result result_used_for_flurry = Hit_result::TBD;
    for (const auto &hit_outcome : hit_outcomes)
    {
        if (hit_outcome.hit_result == Hit_result::crit)
        {
            result_used_for_flurry = Hit_result::crit;
            break;
        }
    }
    manage_flurry(result_used_for_flurry, special_stats, flurry_charges);

    if (hit_outcomes[0].hit_result != Hit_result::miss &&
        hit_outcomes[0].hit_result != Hit_result::dodge)
    {
        hit_effects(weapon, main_hand_weapon, special_stats,
                    rage,
                    recklessness_active, damage_sources, flurry_charges);

        // Unbridled wrath
        if (get_uniform_random(1) < config.talents.unbridled_wrath * 0.08)
        {
            rage += 1;
            simulator_cout("Unbridled wrath, +1 rage");
        }
    }
}

std::vector<double> &Combat_simulator::simulate(const Character &character)
{
    int n_damage_batches = config.n_batches;
    if (config.display_combat_debug)
    {
        n_damage_batches = 1;
    }
    debug_topic_ = "";
    buff_manager_.aura_uptime.auras.clear();
    reset_damage_instances();
    batch_damage_.clear();
    batch_damage_.reserve(n_damage_batches);
    damage_distribution_.clear();
    damage_distribution_.reserve(n_damage_batches);
    flurry_uptime_mh_.clear();
    flurry_uptime_mh_.reserve(n_damage_batches);
    flurry_uptime_oh_.clear();
    flurry_uptime_oh_.reserve(n_damage_batches);
    const auto starting_special_stats = character.total_special_stats;
    std::vector<Weapon_sim> weapons;
    for (const auto &wep : character.weapons)
    {
        weapons.emplace_back(wep.swing_speed, wep.min_damage, wep.max_damage, wep.socket, wep.type,
                             wep.weapon_socket, wep.hit_effects);
        weapons.back().compute_weapon_damage(wep.buff.bonus_damage);
        compute_hit_table(config.opponent_level - character.level,
                          get_weapon_skill(character.total_special_stats, wep.type),
                          starting_special_stats, wep.socket);
    }

    heroic_strike_rage_cost = 15.0 - config.talents.improved_heroic_strike;
    double execute_rage_cost = 15 - static_cast<int>(2.5 * config.talents.improved_execute);
    double armor_reduction_from_spells = 0.0;
    armor_reduction_from_spells += 640 * config.curse_of_recklessness_active;
    armor_reduction_from_spells += 450 * config.n_sunder_armor_stacks;
    armor_reduction_from_spells += 505 * config.faerie_fire_feral_active;

    double boss_armor = 3731 - armor_reduction_from_spells; // Armor for Warrior class monsters
    double target_mitigation = armor_mitigation(boss_armor, 63);
    double add_mitigation = armor_mitigation(3000, 62);
    armor_reduction_factor_ = 1 - target_mitigation;
    armor_reduction_factor_add = 1 - add_mitigation;

    double sim_time = config.sim_time;
    if (config.use_sim_time_ramp)
    {
        sim_time -= 2.0;
    }

    // Pick out the best use effect if there are several that shares cooldown
    std::vector<Use_effect> use_effects_all = character.use_effects;
    std::vector<int> shared_items;
    size_t best_idx = 0;
    double best_value = 0;
    for (size_t i = 0; i < use_effects_all.size(); i++)
    {
        if (use_effects_all[i].effect_socket == Use_effect::Effect_socket::shared)
        {
            shared_items.push_back(i);
            double value = use_effects_all[i].get_special_stat_equivalent(starting_special_stats).attack_power *
                           use_effects_all[i].duration;
            if (value > best_value)
            {
                best_idx = i;
                best_value = value;
            }
        }
    }

    std::vector<Use_effect> use_effects;
    for (size_t i = 0; i < use_effects_all.size(); i++)
    {
        if (!(std::find(shared_items.begin(), shared_items.end(), i) != shared_items.end()))// Add if item is not shared
        {
            use_effects.emplace_back(use_effects_all[i]);
        }
        else
        {
            if (i == best_idx) // Pick the best one
            {
                use_effects.emplace_back(use_effects_all[i]);
            }
        }
    }

    for (int iter = 0; iter < n_damage_batches; iter++)
    {
        time_keeper_.reset(); // Class variable that keeps track of the time spent, cooldowns, iteration number
        auto special_stats = starting_special_stats;
        // TODO better haste fix?
        Damage_sources damage_sources{};
        double rage = 0;
        buff_manager_.set_target(special_stats, use_effects);
        ability_queue_manager.reset();

        int flurry_charges = 0;
        bool deathwish_used{false};
        bool recklessness_active = false;
        bool execute_phase = false;
        bool bloodrage_active = false;
        int bloodrage_ticks = 0;
        int anger_management_ticks = 0;
        int burning_adrenaline_ticks = 0;
        int fuel_ticks = 0;
        double bloodrage_init_time = 0.0;
        double bloodrage_cooldown = -1e-5;

        double mh_hits = 0;
        double mh_hits_w_flurry = 0;
        double oh_hits = 0;
        double oh_hits_w_flurry = 0;

        for (auto &wep : weapons)
        {
            wep.internal_swing_timer = 0.0;
        }

        // To avoid local max/min results from running a specific run time
        if (config.use_sim_time_ramp)
        {
            sim_time += 2.0 / config.n_batches;
        }

        // Combat configuration
        adds_in_melee_range = 0;
        int removed_adds = 0;
        if (config.mode.golemagg)
        {
            adds_in_melee_range = 2;
            remove_adds_timer = sim_time * 2;
        }
        else if (config.mode.sulfuron_harbinger)
        {
            adds_in_melee_range = 4;
            remove_adds_timer = sim_time / 2 / 4;
            buff_manager_.add("sulfuron_demo_shout",
                              {0, 0, -300},
                              300);
        }

        while (time_keeper_.time < sim_time)
        {
            double mh_dt = weapons[0].internal_swing_timer;
            double oh_dt = (weapons.size() == 2) ? weapons[1].internal_swing_timer : 100.0;
            double buff_dt = buff_manager_.get_dt();
            double dt = time_keeper_.get_dynamic_time_step(mh_dt, oh_dt, buff_dt, sim_time);
            time_keeper_.increment(dt);
            std::vector<std::string> debug_msg;
            buff_manager_.increment(dt, sim_time - time_keeper_.time, rage, time_keeper_.global_cd, debug_msg,
                                    config.display_combat_debug);
            for (const auto &msg : debug_msg)
            {
                simulator_cout(msg);
            }
            if (sim_time - time_keeper_.time < 0.0)
            {
                break;
            }

            if (time_keeper_.time > remove_adds_timer * (1 + removed_adds))
            {
                if (adds_in_melee_range > 0)
                {
                    removed_adds++;
                    adds_in_melee_range--;
                    simulator_cout("Add #", removed_adds, " dies. Targets left: boss + ", adds_in_melee_range,
                                   "adds");
                }
            }

            bool mh_swing = weapons[0].time_for_swing(dt);
            bool oh_swing = (weapons.size() == 2) ? weapons[1].time_for_swing(dt) : false;

            if (mh_swing)
            {
                mh_hits++;
                if (flurry_charges > 0) { mh_hits_w_flurry++; }
                swing_weapon(weapons[0], weapons[0], special_stats, rage,
                             recklessness_active, damage_sources, flurry_charges);
            }

            if (oh_swing)
            {
                oh_hits++;
                if (flurry_charges > 0) { oh_hits_w_flurry++; }
                swing_weapon(weapons[1], weapons[0], special_stats, rage,
                             recklessness_active, damage_sources, flurry_charges);
            }

            if (config.fuel_extra_rage)
            {
                if (time_keeper_.time - config.extra_rage_interval * fuel_ticks > 0.0)
                {
                    rage += rage_from_damage_taken(config.extra_rage_damage_amount);
                    rage = std::min(100.0, rage);
                    fuel_ticks++;
                    simulator_cout("Rage from damage: ", config.extra_rage_damage_amount, " damage, yielding: ",
                                   rage_from_damage_taken(config.extra_rage_damage_amount), " rage");
                }
            }

            if (config.talents.anger_management)
            {
                if (time_keeper_.time - 3 * anger_management_ticks > 0.0)
                {
                    simulator_cout("Anger management tick, +1 rage");
                    rage += 1;
                    rage = std::min(100.0, rage);
                    anger_management_ticks++;
                }
            }

            if (config.mode.vaelastrasz)
            {
                if (time_keeper_.time - 1 * burning_adrenaline_ticks > 0.0)
                {
                    simulator_cout("burning_adrenaline_ticks, +20 rage");
                    rage += 10;
                    rage = std::min(100.0, rage);
                    burning_adrenaline_ticks++;
                }
            }

            if (config.enable_bloodrage)
            {
                if (!bloodrage_active && bloodrage_cooldown < 0.0)
                {
                    bloodrage_active = true;
                    rage += 10;
                    rage = std::min(100.0, rage);
                    simulator_cout("Bloodrage activated! rage: ", int(rage));
                    bloodrage_init_time = time_keeper_.time;
                    bloodrage_cooldown = 60.0 + dt;
                }
                if (bloodrage_active && time_keeper_.time - (bloodrage_init_time + bloodrage_ticks) > 1.0)
                {
                    rage += 1;
                    rage = std::min(100.0, rage);
                    simulator_cout("Bloodrage tick, rage: ", int(rage));
                    bloodrage_ticks++;
                    if (bloodrage_ticks == 10)
                    {
                        bloodrage_active = false;
                        bloodrage_ticks = 0;
                    }
                }
                bloodrage_cooldown -= dt;
            }

            if (config.talents.death_wish)
            {
                if (sim_time - time_keeper_.time < 31.0 && rage >= 10 && !deathwish_used)
                {
                    deathwish_used = true;
                    simulator_cout("------------ Deathwish activated! ------------");
                    buff_manager_.add("Deathwish", Special_stats{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, .2}, 30);
                    rage -= 10;
                    time_keeper_.global_cd = 1.5;
                }
            }
            if (config.enable_recklessness)
            {
                if (sim_time - time_keeper_.time < 16.0 && !recklessness_active)
                {
                    recklessness_active = true;
                    time_keeper_.global_cd = 1.5;
                    simulator_cout("------------ Recklessness activated! ------------");
                }
            }

            // Execute phase
            if (config.mode.vaelastrasz)
            {
                if (time_keeper_.time > sim_time * 0.33)
                {
                    if (!execute_phase)
                    {
                        simulator_cout("------------ Execute phase! ------------");
                        execute_phase = true;
                    }
                }
            }
            else
            {
                if (time_keeper_.time > sim_time * 0.85)
                {
                    if (!execute_phase)
                    {
                        simulator_cout("------------ Execute phase! ------------");
                        execute_phase = true;
                    }
                }
            }
            if (execute_phase)
            {
                if (rage > heroic_strike_rage_cost && !ability_queue_manager.heroic_strike_queued
                    && config.combat.use_hs_in_exec_phase)
                {
                    ability_queue_manager.queue_heroic_strike();
                    simulator_cout("Heroic strike activated");
                }
                if (config.combat.use_bt_in_exec_phase)
                {
                    if (time_keeper_.blood_thirst_cd < 0.0 && time_keeper_.global_cd < 0 && rage > 30)
                    {
                        bloodthirst(weapons[0], special_stats, rage,
                                    recklessness_active, damage_sources, flurry_charges);
                    }
                }
                if (time_keeper_.global_cd < 0 && rage > execute_rage_cost)
                {
                    execute(weapons[0], special_stats, rage,
                            recklessness_active, damage_sources, flurry_charges, execute_rage_cost);
                }
            }
            else
            {
                if (time_keeper_.blood_thirst_cd < 0.0 && time_keeper_.global_cd < 0 && rage > 30)
                {
                    bloodthirst(weapons[0], special_stats, rage,
                                recklessness_active, damage_sources, flurry_charges);
                }

                if (time_keeper_.whirlwind_cd < 0.0 &&
                    rage > config.combat.whirlwind_rage_thresh &&
                    rage > 25 &&
                    time_keeper_.global_cd < 0 &&
                    time_keeper_.blood_thirst_cd > config.combat.whirlwind_bt_cooldown_thresh)
                {
                    whirlwind(weapons[0], special_stats, rage,
                              recklessness_active, damage_sources, flurry_charges);
                }
                if (adds_in_melee_range > 0 && config.combat.cleave_if_adds)
                {
                    if (rage > config.combat.cleave_rage_thresh &&
                        !ability_queue_manager.cleave_queued)
                    {
                        ability_queue_manager.queue_cleave();
                        simulator_cout("Cleave activated");
                    }
                }
                else
                {
                    if (rage > config.combat.heroic_strike_rage_thresh &&
                        !ability_queue_manager.heroic_strike_queued)
                    {
                        ability_queue_manager.queue_heroic_strike();
                        simulator_cout("Heroic strike activated");
                    }
                }
            }

        }
        batch_damage_.push_back(damage_sources.sum_damage_sources() / sim_time);
        damage_distribution_.emplace_back(damage_sources);
        add_damage_source_to_time_lapse(damage_sources.damage_instances, n_damage_batches);
        flurry_uptime_mh_.emplace_back(mh_hits_w_flurry / mh_hits);
        flurry_uptime_oh_.emplace_back(oh_hits_w_flurry / oh_hits);
    }
    return batch_damage_;
}

std::vector<double> &Combat_simulator::simulate(const Character &character, int n_batches)
{
    config.n_batches = n_batches;
    return simulate(character);
}

const std::vector<double> &Combat_simulator::get_hit_probabilities_white_mh() const
{
    return hit_probabilities_white_mh_;
}

const std::vector<double> &Combat_simulator::get_hit_probabilities_white_oh() const
{
    return hit_probabilities_white_oh_;
}

const std::vector<double> &Combat_simulator::get_hit_probabilities_yellow() const
{
    return hit_probabilities_yellow_;
}

double Combat_simulator::get_glancing_penalty_mh() const
{
    return glancing_factor_mh_;
}

double Combat_simulator::get_glancing_penalty_oh() const
{
    return glancing_factor_oh_;
}

void Combat_simulator::print_damage_distribution() const
{
    print_damage_source_vector(damage_distribution_);
}

std::vector<Damage_sources> Combat_simulator::get_damage_distribution() const
{
    return damage_distribution_;
}

std::vector<std::string> Combat_simulator::get_aura_uptimes() const
{
    std::vector<std::string> aura_uptimes;
    double total_sim_time = config.n_batches * config.sim_time;
    for (const auto &aura : buff_manager_.aura_uptime.auras)
    {
        std::string aura_name = aura.id;
        double uptime = aura.duration / total_sim_time;
        aura_uptimes.emplace_back(aura.id + " " + std::to_string(100 * uptime));
    }
    aura_uptimes.emplace_back("Flurry_main_hand " + std::to_string(100 * Statistics::average(flurry_uptime_mh_)));
    aura_uptimes.emplace_back("Flurry_off_hand " + std::to_string(100 * Statistics::average(flurry_uptime_oh_)));
    return aura_uptimes;
}

std::vector<std::string> Combat_simulator::get_proc_statistics() const
{
    std::vector<std::string> proc_counter;
    for (const auto &proc : buff_manager_.procs)
    {
        std::string aura_name = proc.id;
        double counter = static_cast<double>(proc.counter) / config.n_batches;
        proc_counter.emplace_back(proc.id + " " + std::to_string(counter));
    }
    return proc_counter;
}

void Combat_simulator::reset_damage_instances()
{
    double resolution = .50;
    std::vector<double> history;
    history.reserve(config.sim_time / resolution);
    for (double t = 0; t < config.sim_time; t += resolution)
    {
        history.push_back(0);
    }
    if (!(damage_time_lapse.empty()))
    {
        damage_time_lapse.clear();
    }
    for (int i = 0; i < 8; i++)
    {
        damage_time_lapse.push_back(history);
    }
}

void Combat_simulator::add_damage_source_to_time_lapse(std::vector<Damage_instance> &damage_instances, int n_iter)
{
    double resolution = .50;
    for (const auto &damage_instance : damage_instances)
    {
        size_t first_idx = source_map.at(damage_instance.damage_source);
        size_t second_idx = damage_instance.time_stamp / resolution; // automatically floored
        damage_time_lapse[first_idx][second_idx] += damage_instance.damage / n_iter;
    }
}

std::vector<std::vector<double>> Combat_simulator::get_damage_time_lapse() const
{
    return damage_time_lapse;
}

std::string Combat_simulator::get_debug_topic() const
{
    return debug_topic_;
}
