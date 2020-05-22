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
                    simulator_cout("Mainhand glancing hit for: ", hit_outcome.damage, " damage.");
                    break;
                case Hit_result::hit:
                    simulator_cout("Mainhand white hit for: ", hit_outcome.damage, " damage.");
                    break;
                case Hit_result::crit:
                    simulator_cout("Mainhand crit for: ", hit_outcome.damage, " damage.");
                    break;
                case Hit_result::dodge:
                    simulator_cout("Mainhand hit dodged");
                    break;
                case Hit_result::miss:
                    simulator_cout("Mainhand hit missed");
                    break;
                case Hit_result::TBD:
                    simulator_cout("BUUUUUUUUUUGGGGGGGGG");
                    break;
            }
        }
        else
        {
            switch (hit_outcome.hit_result)
            {
                case Hit_result::glancing:
                    simulator_cout("BUG: Ability glanced for: ", hit_outcome.damage, " damage.");
                    break;
                case Hit_result::hit:
                    simulator_cout("Ability hit for: ", hit_outcome.damage, " damage.");
                    break;
                case Hit_result::crit:
                    simulator_cout("Ability crit for: ", hit_outcome.damage, " damage.");
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
                simulator_cout("Offhand glancing hit for: ", hit_outcome.damage, " damage.");
                break;
            case Hit_result::hit:
                simulator_cout("Offhand white hit for: ", hit_outcome.damage, " damage.");
                break;
            case Hit_result::crit:
                simulator_cout("Offhand crit for: ", hit_outcome.damage, " damage.");
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

Combat_simulator::Hit_outcome
Combat_simulator::generate_hit_oh(double damage, bool heroic_strike_active, bool recklessness_active)
{
    if (recklessness_active)
    {
        if (heroic_strike_active)
        {
            simulator_cout("Drawing outcome from recklessness twohanded hit table");
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
        if (heroic_strike_active)
        {
            simulator_cout("Drawing outcome from twohanded hit table");
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
                                                             Socket weapon_hand, bool heroic_strike_active,
                                                             const Special_stats &special_stats,
                                                             bool recklessness_active)
{
    if (weapon_hand == Socket::main_hand)
    {
        auto hit_outcome = generate_hit_mh(damage, hit_type, recklessness_active);
        hit_outcome.damage *= armor_reduction_factor_ * (1 + special_stats.damage_multiplier);
        cout_damage_parse(hit_type, weapon_hand, hit_outcome);
        return hit_outcome;
    }
    else
    {
        auto hit_outcome = generate_hit_oh(damage, heroic_strike_active, recklessness_active);
        hit_outcome.damage *= armor_reduction_factor_ * (1 + special_stats.damage_multiplier);
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
        dodge_chance = (5 + skill_diff * 0.1);
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
    if (skill_diff > 8)
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

void Combat_simulator::swing_weapon(Weapon_sim &weapon, Weapon_sim &main_hand_weapon, Special_stats &special_stats,
                                    bool &heroic_strike_active, double &rage, double &heroic_strike_rage_cost,
                                    bool &recklessness_active, Damage_sources &damage_sources, int &flurry_charges)
{
    Combat_simulator::Hit_outcome hit_outcome{0.0, Hit_result::TBD};
    double swing_damage = weapon.swing(special_stats.attack_power);
    if (weapon.socket == Socket::off_hand)
    {
        swing_damage *= (0.5 + 0.025 * config.talents.dual_wield_specialization);
    }
    weapon.internal_swing_timer = weapon.swing_speed / (1 + special_stats.haste);

    // Check if heroic strike should be performed
    if (config.enable_spell_rotation &&
        heroic_strike_active &&
        weapon.socket == Socket::main_hand &&
        rage >= heroic_strike_rage_cost)
    {
        simulator_cout("Performing heroic strike");// Unbridled wrath
        swing_damage += 138;
        hit_outcome = generate_hit(swing_damage, Hit_type::yellow, weapon.socket, heroic_strike_active,
                                   special_stats, recklessness_active);
        heroic_strike_active = false;
        rage -= heroic_strike_rage_cost;
        damage_sources.add_damage(Damage_source::heroic_strike, hit_outcome.damage);
        simulator_cout(rage, " rage");
    }
    else
    {
        if (weapon.socket == Socket::main_hand && heroic_strike_active)
        {
            // Failed to pay rage for heroic strike
            simulator_cout("Failed to pay heroic strike rage");
            heroic_strike_active = false;
        }

        // Otherwise do white hit
        hit_outcome = generate_hit(swing_damage, Hit_type::white, weapon.socket, heroic_strike_active,
                                   special_stats, recklessness_active);

        rage += rage_generation(hit_outcome.damage);
        if (hit_outcome.hit_result == Hit_result::dodge)
        {
            simulator_cout("Rage gained from enemy dodging");
            rage += rage_generation(swing_damage * armor_reduction_factor_);
        }
        rage = std::min(100.0, rage);
        if (weapon.socket == Socket::main_hand)
        {
            damage_sources.add_damage(Damage_source::white_mh, hit_outcome.damage);
        }
        else
        {
            damage_sources.add_damage(Damage_source::white_oh, hit_outcome.damage);
        }
        simulator_cout(rage, " rage");
    }

    if (hit_outcome.hit_result != Hit_result::miss &&
        hit_outcome.hit_result != Hit_result::dodge)
    {
        for (const auto &hit_effect : weapon.hit_effects)
        {
            double r = get_uniform_random(1);
            if (r < hit_effect.probability)
            {
                switch (hit_effect.type)
                {
                    case Hit_effect::Type::extra_hit:
                        simulator_cout("PROC: extra hit from: ", hit_effect.name);
                        swing_weapon(main_hand_weapon, main_hand_weapon, special_stats,
                                     heroic_strike_active, rage, heroic_strike_rage_cost,
                                     recklessness_active, damage_sources, flurry_charges);
                        break;
                    case Hit_effect::Type::damage_magic:
                        simulator_cout("PROC: damage from: ", hit_effect.name);
                        damage_sources.add_damage(Damage_source::item_hit_effects, hit_effect.damage * 0.85);
                        break;
                    case Hit_effect::Type::damage_physical:
                        simulator_cout("PROC: damage from: ", hit_effect.name);
                        damage_sources.add_damage(Damage_source::item_hit_effects,
                                                  generate_hit(hit_effect.damage, Hit_type::yellow,
                                                               Socket::main_hand, heroic_strike_active,
                                                               special_stats, recklessness_active).damage);
                        break;
                    case Hit_effect::Type::stat_boost:
                        simulator_cout("PROC: stats increased: ", hit_effect.name);
                        buff_manager_.add(weapon.socket_name + "_" + hit_effect.name,
                                          hit_effect.get_special_stat_equivalent(),
                                          hit_effect.duration);
                        break;
                    default:
                        std::cout << ":::::::::::FAULTY HIT EFFECT IN SIMULATION!!!:::::::::";
                        break;
                }
            }
        }

        // Unbridled wrath
        if (get_uniform_random(1) < config.talents.unbridled_wrath * 0.08)
        {
            rage += 1;
            simulator_cout("Unbridled wrath, +1 rage");
        }
        manage_flurry(hit_outcome.hit_result, special_stats, flurry_charges);
    }
}

std::vector<double> &Combat_simulator::simulate(const Character &character)
{
    int n_damage_batches = config.n_batches;
    if (config.display_combat_debug)
    {
        n_damage_batches = 1;
    }
    buff_manager_.aura_uptime.auras = {}; // Clear aura uptimes
    batch_damage_.clear();
    batch_damage_.reserve(n_damage_batches);
    damage_distribution_.clear();
    damage_distribution_.reserve(n_damage_batches);
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

    double heroic_strike_rage_cost = 15.0 - config.talents.improved_heroic_strike;
    double execute_rage_cost = 15 - static_cast<int>(2.5 * config.talents.improved_execute);
    double armor_reduction_from_spells = 0.0;
    armor_reduction_from_spells += 640 * config.curse_of_recklessness_active;
    armor_reduction_from_spells += config.sunder_armor_active * 450 * config.n_sunder_armor_stacks;
    armor_reduction_from_spells += config.faerie_fire_feral_active * 505;

    double boss_armor = 3731 - armor_reduction_from_spells; // Armor for Warrior class monsters
    double target_mitigation = armor_mitigation(boss_armor, 63);
    armor_reduction_factor_ = 1 - target_mitigation;

    double sim_time = config.sim_time;
    double sim_time_increment = config.sim_time / 5.0 / n_damage_batches;
    if (config.use_sim_time_ramp)
    {
        sim_time -= config.sim_time / 10.0;
    }

    for (int iter = 0; iter < n_damage_batches; iter++)
    {
        time_keeper_.reset(); // Class variable that keeps track of the time spent, cooldowns, iteration number
        auto special_stats = starting_special_stats;
        // TODO better haste fix?
        Damage_sources damage_sources{};
        double rage = 0;
        buff_manager_.set_target(special_stats);
        int flurry_charges = 0;

        bool deathwish_used{false};
        bool heroic_strike_active = false;
        bool recklessness_active = false;
        bool have_printed_execute_phase = false;
        bool bloodrage_active = false;
        bool used_mighty_rage_potion = false;
        bool reset_mighty_rage_potion = false;
        double mightyrage_init_time = 10000000.0;
        int bloodrage_ticks = 0;
        int anger_management_ticks = 0;
        int fuel_ticks = 0;
        double bloodrage_init_time = 0.0;
        double bloodrage_cooldown = -1e-5;

        for (auto &wep : weapons)
        {
            wep.internal_swing_timer = 0.0;
        }

        // To avoid local max/min results from running a specific run time
        if (config.use_sim_time_ramp)
        {
            sim_time += sim_time_increment;
        }

        while (time_keeper_.time < sim_time)
        {
            double mh_dt = weapons[0].internal_swing_timer;
            double oh_dt = (weapons.size() == 2) ? weapons[1].internal_swing_timer : 100.0;
            double buff_dt = buff_manager_.get_dt();
            double dt = time_keeper_.get_dynamic_time_step(mh_dt, oh_dt, buff_dt, sim_time);
            time_keeper_.increment(dt);
            buff_manager_.increment(dt);

            bool mh_swing = weapons[0].time_for_swing(dt);
            bool oh_swing = (weapons.size() == 2) ? weapons[1].time_for_swing(dt) : false;

            if (mh_swing)
            {
                swing_weapon(weapons[0], weapons[0], special_stats, heroic_strike_active, rage, heroic_strike_rage_cost,
                             recklessness_active, damage_sources, flurry_charges);
            }

            if (oh_swing)
            {
                swing_weapon(weapons[1], weapons[0], special_stats, heroic_strike_active, rage, heroic_strike_rage_cost,
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

            if (config.enable_bloodrage)
            {
                if (!bloodrage_active && bloodrage_cooldown < 0.0)
                {
                    simulator_cout("Bloodrage activated!");
                    bloodrage_active = true;
                    rage += 10;
                    rage = std::min(100.0, rage);
                    simulator_cout(rage, " rage");
                    bloodrage_init_time = time_keeper_.time;
                    bloodrage_cooldown = 60.0 + dt;
                }
                if (bloodrage_active && time_keeper_.time - (bloodrage_init_time + bloodrage_ticks) > 1.0)
                {
                    simulator_cout("Bloodrage tick, +1 rage");
                    rage += 1;
                    rage = std::min(100.0, rage);
                    bloodrage_ticks++;
                    if (bloodrage_ticks == 10)
                    {
                        bloodrage_active = false;
                        bloodrage_ticks = 0;
                    }
                }
                bloodrage_cooldown -= dt;
            }

            if (config.use_mighty_rage_potion)
            {
                if (sim_time - time_keeper_.time < 30.0 && !used_mighty_rage_potion)
                {
                    simulator_cout("------------ Mighty Rage Potion! ------------");
                    rage += 45 + 30 * get_uniform_random(1);
                    rage = std::min(100.0, rage);
                    special_stats.attack_power += 132;
                    used_mighty_rage_potion = true;
                    mightyrage_init_time = time_keeper_.time;
                }
                if (time_keeper_.time - mightyrage_init_time > 20.0 && !reset_mighty_rage_potion)
                {
                    simulator_cout("------------ Mighty Rage Potion wears off! ------------");
                    special_stats.attack_power -= 132;
                    reset_mighty_rage_potion = true;
                }
            }

            if (config.enable_spell_rotation)
            {
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

                // Execute phase, starts at 85% in
                if (time_keeper_.time > sim_time * 0.85)
                {
                    if (!have_printed_execute_phase)
                    {
                        simulator_cout("------------ Execute phase! ------------");
                        have_printed_execute_phase = true;
                    }
                    if (rage > heroic_strike_rage_cost && !heroic_strike_active)
                    {
                        heroic_strike_active = true;
                        simulator_cout("Heroic strike activated");
                    }
                    if (time_keeper_.global_cd < 0 && rage > execute_rage_cost)
                    {
                        simulator_cout("Execute!");
                        double damage = 600 + (rage - execute_rage_cost) * 15;
                        auto hit_outcome = generate_hit(damage, Hit_type::yellow, Socket::main_hand,
                                                        heroic_strike_active, special_stats, recklessness_active);
                        time_keeper_.global_cd = 1.5;
                        if (hit_outcome.hit_result == Hit_result::dodge || hit_outcome.hit_result == Hit_result::miss)
                        {
                            rage *= 0.85;
                        }
                        else
                        {
                            rage = 0;
                        }
                        manage_flurry(hit_outcome.hit_result, special_stats, flurry_charges, true);
                        damage_sources.add_damage(Damage_source::execute, hit_outcome.damage);
                        simulator_cout(rage, " rage");
                    }
                }
                else
                {
                    if (time_keeper_.blood_thirst_cd < 0.0 && time_keeper_.global_cd < 0 && rage > 30)
                    {
                        simulator_cout("Bloodthirst!");
                        double damage = special_stats.attack_power * 0.45;
                        auto hit_outcome = generate_hit(damage, Hit_type::yellow, Socket::main_hand,
                                                        heroic_strike_active, special_stats, recklessness_active);
                        if (hit_outcome.hit_result == Hit_result::dodge || hit_outcome.hit_result == Hit_result::miss)
                        {
                            rage -= 6;
                        }
                        else
                        {
                            rage -= 30;
                        }
                        time_keeper_.blood_thirst_cd = 6.0;
                        time_keeper_.global_cd = 1.5;
                        manage_flurry(hit_outcome.hit_result, special_stats, flurry_charges, true);
                        damage_sources.add_damage(Damage_source::bloodthirst, hit_outcome.damage);
                        simulator_cout(rage, " rage");
                    }

                    if (time_keeper_.whirlwind_cd < 0.0 &&
                        rage > 25 &&
                        time_keeper_.global_cd < 0 &&
                        time_keeper_.blood_thirst_cd > 1.0)
                    {
                        simulator_cout("Whirlwind!");
                        double damage = weapons[0].normalized_swing(special_stats.attack_power);
                        auto hit_outcome = generate_hit(damage, Hit_type::yellow, Socket::main_hand,
                                                        heroic_strike_active, special_stats, recklessness_active);
                        rage -= 25;
                        time_keeper_.whirlwind_cd = 10;
                        time_keeper_.global_cd = 1.5;
                        manage_flurry(hit_outcome.hit_result, special_stats, flurry_charges, true);
                        damage_sources.add_damage(Damage_source::whirlwind, hit_outcome.damage);
                        simulator_cout(rage, " rage");
                    }
                    if (rage > 60 && !heroic_strike_active)
                    {
                        heroic_strike_active = true;
                        simulator_cout("Heroic strike activated");
                    }
                }
            }
        }
        batch_damage_.push_back(damage_sources.sum_damage_sources() / time_keeper_.time);
        damage_distribution_.emplace_back(damage_sources);
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
        aura_uptimes.emplace_back(aura.id + ": " + std::to_string(uptime) + "%");
    }
    return aura_uptimes;
}
