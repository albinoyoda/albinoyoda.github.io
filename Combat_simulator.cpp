#include "Combat_simulator.hpp"

constexpr double rage_factor = 15.0 / 4.0 / 230.6;

namespace
{
    constexpr double rage_generation(double damage, double weapon_speed, bool crit, bool is_main_hand)
    {
        if (damage > 0.0)
        {
            double hit_factor = 1.75 * (crit + 1) * (is_main_hand + 1);
            return damage * rage_factor + hit_factor * weapon_speed / 2;
        }
        return 0.0;
    }

    constexpr double armor_mitigation(double target_armor)
    {
        return target_armor / (target_armor + 400 + 85 * 60);
    }
}

Combat_simulator::Hit_outcome
Combat_simulator::generate_hit_mh(double damage, Hit_type hit_type, bool recklessness_active)
{
    double random_var = get_random_100();
    if (hit_type == Hit_type::white)
    {
        if (recklessness_active)
        {
            int outcome = std::lower_bound(hit_probabilities_recklessness_mh_.begin(),
                                           hit_probabilities_recklessness_mh_.end(),
                                           random_var) - hit_probabilities_recklessness_mh_.begin();
            return {damage * lookup_outcome_mh_white(outcome), Hit_result(outcome)};
        }
        else
        {
            int outcome = std::lower_bound(hit_probabilities_white_mh_.begin(), hit_probabilities_white_mh_.end(),
                                           random_var) - hit_probabilities_white_mh_.begin();
            return {damage * lookup_outcome_mh_white(outcome), Hit_result(outcome)};
        }
    }
    else
    {
        // Yellow hit
        if (recklessness_active)
        {
            int outcome = std::lower_bound(hit_probabilities_recklessness_yellow_.begin(),
                                           hit_probabilities_recklessness_yellow_.end(),
                                           random_var) - hit_probabilities_recklessness_yellow_.begin();
            if (outcome <= 1)
            {
                return {damage * lookup_outcome_mh_yellow(outcome), Hit_result(outcome)};
            }
            else
            {
                return {damage * lookup_outcome_mh_yellow(outcome),
                        Hit_result(outcome + 1)}; // +1 because yellow cant glance
            }
        }
        else
        {
            int outcome = std::lower_bound(hit_probabilities_yellow_.begin(), hit_probabilities_yellow_.end(),
                                           random_var) - hit_probabilities_yellow_.begin();
            if (outcome <= 1)
            {
                return {damage * lookup_outcome_mh_yellow(outcome), Hit_result(outcome)};
            }
            else
            {
                return {damage * lookup_outcome_mh_yellow(outcome),
                        Hit_result(outcome + 1)}; // +1 because yellow cant glance
            }
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
            simulator_cout("Offhand hitrate increased: mainhand queue heroic");
            double random_var = get_random_100();
            int outcome = std::lower_bound(hit_probabilities_recklessness_two_hand_.begin(),
                                           hit_probabilities_recklessness_two_hand_.end(),
                                           random_var) - hit_probabilities_recklessness_two_hand_.begin();
            return {damage * lookup_outcome_mh_white(outcome), Hit_result(outcome)};
        }
        else
        {
            double random_var = get_random_100();
            int outcome = std::lower_bound(hit_probabilities_recklessness_oh_.begin(),
                                           hit_probabilities_recklessness_oh_.end(),
                                           random_var) - hit_probabilities_recklessness_oh_.begin();
            return {damage * lookup_outcome_mh_white(outcome), Hit_result(outcome)};
        }
    }
    else
    {
        if (heroic_strike_active)
        {
            simulator_cout("Offhand hitrate increased: mainhand queue heroic");
            double random_var = get_random_100();
            int outcome = std::lower_bound(hit_probabilities_two_hand_.begin(),
                                           hit_probabilities_two_hand_.end(),
                                           random_var) - hit_probabilities_two_hand_.begin();
            return {damage * lookup_outcome_mh_white(outcome), Hit_result(outcome)};
        }
        else
        {
            double random_var = get_random_100();
            int outcome = std::lower_bound(hit_probabilities_white_oh_.begin(), hit_probabilities_white_oh_.end(),
                                           random_var) -
                          hit_probabilities_white_oh_.begin();
            return {damage * lookup_outcome_oh(outcome), Hit_result(outcome)};
        }
    }
}

Combat_simulator::Hit_outcome Combat_simulator::generate_hit(double damage, Combat_simulator::Hit_type hit_type,
                                                             Hand weapon_hand, bool heroic_strike_active,
                                                             bool death_wish_active,
                                                             bool recklessness_active)
{
    if (weapon_hand == Hand::main_hand)
    {
        auto hit_outcome = generate_hit_mh(damage, hit_type, recklessness_active);
        hit_outcome.damage *= armor_reduction_factor_;
        if (death_wish_active)
        {
            hit_outcome.damage *= 1.2;
        }
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
                    simulator_cout("Ability glanced for: ", hit_outcome.damage, " damage.");
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
        return hit_outcome;
    }
    else
    {
        auto hit_outcome = generate_hit_oh(damage, heroic_strike_active, recklessness_active);
        hit_outcome.damage *= armor_reduction_factor_;
        if (death_wish_active)
        {
            hit_outcome.damage *= 1.2;
        }
        if (hit_type == Hit_type::white)
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
        return hit_outcome;
    }
}

void Combat_simulator::compute_hit_table_mh_(int opponent_level, int weapon_skill, Special_stats special_stats)
{
    int target_defence = opponent_level * 5;
    int skill_diff = target_defence - weapon_skill;
    int base_skill_diff = target_defence - 300;

    // Crit chance
    double crit_chance = special_stats.critical_strike - base_skill_diff * 0.2 - 1.8; // 1.8 flat aura modifier

    // Miss chance
    double base_miss_chance = (skill_diff > 10) ? (5.0 + skill_diff * 0.2) + 1 : (5.0 + skill_diff * 0.1);
    double dw_miss_chance = (base_miss_chance * 0.8 + 20.0);
    double miss_chance = dw_miss_chance - special_stats.hit;
    double two_hand_miss_chance = std::max(base_miss_chance - special_stats.hit, 0.0);

    // Dodge chance
    double dodge_chance = (5 + skill_diff * 0.1);

    // Glancing blows
    double glancing_chance = 40.0;
    double glancing_penalty_mh_;
    if (skill_diff > 8)
    {
        glancing_penalty_mh_ = 35.0 - (15.0 - skill_diff) * 4.0;
    }
    else
    {
        glancing_penalty_mh_ = 5.0;
    }
    glancing_factor_mh_ = (100.0 - glancing_penalty_mh_) / 100.0;

    // Order -> Miss, parry, dodge, block, glancing, crit, hit.
    hit_probabilities_white_mh_.clear();
    hit_probabilities_white_mh_.push_back(miss_chance);
    hit_probabilities_white_mh_.push_back(hit_probabilities_white_mh_.back() + dodge_chance);
    hit_probabilities_white_mh_.push_back(hit_probabilities_white_mh_.back() + glancing_chance);
    hit_probabilities_white_mh_.push_back(hit_probabilities_white_mh_.back() + crit_chance);

    hit_probabilities_two_hand_.clear();
    hit_probabilities_two_hand_.push_back(two_hand_miss_chance);
    hit_probabilities_two_hand_.push_back(hit_probabilities_two_hand_.back() + dodge_chance);
    hit_probabilities_two_hand_.push_back(hit_probabilities_two_hand_.back() + glancing_chance);
    hit_probabilities_two_hand_.push_back(hit_probabilities_two_hand_.back() + crit_chance);

    hit_probabilities_recklessness_mh_.clear();
    hit_probabilities_recklessness_mh_.push_back(miss_chance);
    hit_probabilities_recklessness_mh_.push_back(hit_probabilities_recklessness_mh_.back() + dodge_chance);
    hit_probabilities_recklessness_mh_.push_back(hit_probabilities_recklessness_mh_.back() + glancing_chance);
    hit_probabilities_recklessness_mh_.push_back(hit_probabilities_recklessness_mh_.back() + 100);

    hit_probabilities_recklessness_two_hand_.clear();
    hit_probabilities_recklessness_two_hand_.push_back(two_hand_miss_chance);
    hit_probabilities_recklessness_two_hand_.push_back(hit_probabilities_recklessness_two_hand_.back() + dodge_chance);
    hit_probabilities_recklessness_two_hand_
            .push_back(hit_probabilities_recklessness_two_hand_.back() + glancing_chance);
    hit_probabilities_recklessness_two_hand_.push_back(hit_probabilities_recklessness_two_hand_.back() + 100);

    hit_probabilities_yellow_.clear();
    hit_probabilities_yellow_.push_back(two_hand_miss_chance);
    hit_probabilities_yellow_.push_back(hit_probabilities_yellow_.back() + dodge_chance);
    hit_probabilities_yellow_.push_back(hit_probabilities_yellow_.back() + crit_chance);

    hit_probabilities_recklessness_yellow_.clear();
    hit_probabilities_recklessness_yellow_.push_back(miss_chance);
    hit_probabilities_recklessness_yellow_.push_back(hit_probabilities_recklessness_yellow_.back() + dodge_chance);
    hit_probabilities_recklessness_yellow_.push_back(hit_probabilities_recklessness_yellow_.back() + 100);
}

void Combat_simulator::compute_hit_table_oh_(int opponent_level, int weapon_skill, Special_stats special_stats)
{
    int target_defence = opponent_level * 5;
    int skill_diff = target_defence - weapon_skill;
    int base_skill_diff = target_defence - 300;

    // Crit chance
    double crit_chance = special_stats.critical_strike - base_skill_diff * 0.2 - 1.8; // 1.8 flat aura modifier

    // Miss chance
    double base_miss_chance = (skill_diff > 10) ? (5.0 + skill_diff * 0.2) + 1 : (5.0 + skill_diff * 0.1);
    double dw_miss_chance = (base_miss_chance * 0.8 + 20.0);
    double miss_chance = dw_miss_chance - special_stats.hit;

    // Dodge chance
    double dodge_chance = (5 + skill_diff * 0.1);

    // Glancing blows
    double glancing_chance = 40.0;
    double glancing_penalty_oh_;
    if (skill_diff > 8)
    {
        glancing_penalty_oh_ = 35.0 - (15.0 - skill_diff) * 4.0;
    }
    else
    {
        glancing_penalty_oh_ = 5.0;
    }
    glancing_factor_oh_ = (100.0 - glancing_penalty_oh_) / 100.0;

    // Order -> Miss, parry, dodge, block, glancing, crit, hit.
    hit_probabilities_white_oh_.clear();
    hit_probabilities_white_oh_.push_back(miss_chance);
    hit_probabilities_white_oh_.push_back(hit_probabilities_white_oh_.back() + dodge_chance);
    hit_probabilities_white_oh_.push_back(hit_probabilities_white_oh_.back() + glancing_chance);
    hit_probabilities_white_oh_.push_back(hit_probabilities_white_oh_.back() + crit_chance);

    hit_probabilities_recklessness_oh_.clear();
    hit_probabilities_recklessness_oh_.push_back(miss_chance);
    hit_probabilities_recklessness_oh_.push_back(hit_probabilities_recklessness_oh_.back() + dodge_chance);
    hit_probabilities_recklessness_oh_.push_back(hit_probabilities_recklessness_oh_.back() + glancing_chance);
    hit_probabilities_recklessness_oh_.push_back(hit_probabilities_recklessness_oh_.back() + 100);
}

void Combat_simulator::compute_hit_table(int opponent_level,
                                         int weapon_skill,
                                         Special_stats special_stats,
                                         Hand weapon_hand)
{
    if (weapon_hand == Hand::main_hand)
    {
        compute_hit_table_mh_(opponent_level, weapon_skill, special_stats);
    }
    else
    {
        compute_hit_table_oh_(opponent_level, weapon_skill, special_stats);
    }
}


std::vector<double> &
Combat_simulator::simulate(const Character &character, double sim_time, int opponent_level, int n_damage_batches)
{
    if (debug_mode_)
    {
        n_damage_batches = 1;
    }
    batch_damage_.clear();
    batch_damage_.reserve(n_damage_batches);
    damage_distribution_.clear();
    damage_distribution_.reserve(n_damage_batches);
    double character_haste = character.get_haste();
    auto special_stats = character.get_total_special_stats();
    auto weapons = character.get_weapons();
    double chance_for_extra_hit = character.get_chance_for_extra_hit();
    double crusader_proc_chance_mh = character.is_crusader_mh() * weapons[0].get_swing_speed() / 40;
    double crusader_proc_chance_oh = character.is_crusader_oh() * weapons[1].get_swing_speed() / 40;

    weapons[0].compute_average_damage(character.get_mh_bonus_damage());
    weapons[1].compute_average_damage(character.get_oh_bonus_damage());

    compute_hit_table(opponent_level, character.get_weapon_skill_mh(), special_stats, Hand::main_hand);
    compute_hit_table(opponent_level, character.get_weapon_skill_oh(), special_stats, Hand::off_hand);

    double armor_reduction_from_spells = 640 + 450 * 5 + 505;
    double boss_armor = 3731 - armor_reduction_from_spells; // Armor for Warrior class monsters
    double target_mitigation = armor_mitigation(boss_armor);
    armor_reduction_factor_ = 1 - target_mitigation;

    for (int iter = 0; iter < n_damage_batches; iter++)
    {
        time_keeper_.reset(); // Class variable that keeps track of the time spent, cooldowns, iteration number
        double total_damage = 0;
        Damage_sources damage_sources{};
        double rage = 0;
        int flurry_charges = 0;
        double flurry_speed_bonus = 1.3;
        double flurry_dt_factor = 1;

        bool heroic_strike_ = false;
        bool crusader_oh_active = false;
        bool crusader_mh_active = false;
        bool deathwish_active = false;
        bool recklessness_active = false;

        weapons[0].set_internal_swing_timer(0.0);
        weapons[1].set_internal_swing_timer(0.0);

        while (time_keeper_.time_ < sim_time)
        {
            double mh_dt = weapons[0].get_internal_swing_timer() / (character_haste * flurry_dt_factor);
            double oh_dt = weapons[1].get_internal_swing_timer() / (character_haste * flurry_dt_factor);
            double dt = time_keeper_.get_dynamic_time_step(mh_dt, oh_dt, sim_time);

            if (time_keeper_.time_ + dt > sim_time)
            {
                break;
            }

            assert(dt > 0.0);
            for (auto &weapon : weapons)
            {
                Combat_simulator::Hit_outcome hit_outcome{0.0, Hit_result::TBD};
                double swing_damage = weapon.step(character_haste * flurry_dt_factor * dt,
                                                  special_stats.attack_power, random_melee_hits_);

                // Check if heroic strike should be performed
                if (swing_damage > 0.0)
                {
                    if (spell_rotation_ &&
                        heroic_strike_ &&
                        (weapon.get_hand() == Hand::main_hand) &&
                        rage >= 13.0)
                    {
                        simulator_cout("Performing heroic strike");// Unbridled wrath
                        swing_damage += 157;
                        hit_outcome = generate_hit(swing_damage, Hit_type::yellow, weapon.get_hand(), heroic_strike_,
                                                   deathwish_active, recklessness_active);
                        heroic_strike_ = false;
                        rage -= 13;
                        damage_sources.heroic_strike += hit_outcome.damage;
                        simulator_cout(rage, " rage");
                    }
                    else
                    {
                        if (weapon.get_hand() == Hand::main_hand && heroic_strike_)
                        {
                            // Failed to pay rage for heroic strike
                            simulator_cout("Failed to pay heroic strike rage");
                            heroic_strike_ = false;
                        }

                        // Otherwise do white hit
                        hit_outcome = generate_hit(swing_damage, Hit_type::white, weapon.get_hand(), heroic_strike_,
                                                   deathwish_active, recklessness_active);
                        rage += rage_generation(hit_outcome.damage,
                                                weapon.get_swing_speed(),
                                                hit_outcome.hit_result == Hit_result::crit,
                                                weapon.get_hand() == Hand::main_hand);
                        rage = std::min(100.0, rage);
                        if (weapon.get_hand() == Hand::main_hand)
                        {
                            damage_sources.white_mh += hit_outcome.damage;
                        }
                        else
                        {
                            damage_sources.white_oh += hit_outcome.damage;
                        }
                        simulator_cout(rage, " rage");
                    }
                    total_damage += hit_outcome.damage;

                    if (hit_outcome.hit_result != Hit_result::miss &&
                        hit_outcome.hit_result != Hit_result::dodge)
                    {
                        if (item_chance_on_hit_)
                        {
                            if (chance_for_extra_hit > 0.0)
                            {
                                double random_variable = get_random_100();
                                if (random_variable < chance_for_extra_hit)
                                {
                                    simulator_cout("HoJ procc");
                                    double damage = weapons[0].swing(special_stats.attack_power);
                                    hit_outcome = generate_hit(damage, Hit_type::white, weapons[0].get_hand(),
                                                               heroic_strike_, deathwish_active, recklessness_active);
                                    rage += rage_generation(hit_outcome.damage,
                                                            weapons[0].get_swing_speed(),
                                                            hit_outcome.hit_result == Hit_result::crit,
                                                            true);
                                    rage = std::min(100.0, rage);
                                    weapons[0].reset_timer();
                                    damage_sources.extra_hit += hit_outcome.damage;
                                    total_damage += hit_outcome.damage;
                                    simulator_cout(rage, " rage");
                                }
                            }
                        }
                        if (crusader_enabled_)
                        {
                            if (weapon.get_hand() == Hand::main_hand)
                            {
                                double random_variable = get_random_1();
                                if (random_variable < crusader_proc_chance_mh)
                                {
                                    simulator_cout("MH crusader procc");
                                    time_keeper_.crusader_mh_buff_timer = 15.0;
                                    if (!crusader_mh_active)
                                    {
                                        special_stats.attack_power += 200;
                                        crusader_mh_active = true;
                                    }
                                }
                            }
                            if (time_keeper_.crusader_mh_buff_timer < 0.0 && crusader_mh_active)
                            {
                                simulator_cout("MH crusader procc run out");
                                special_stats.attack_power -= 200;
                                crusader_mh_active = false;
                            }
                            if (weapon.get_hand() == Hand::off_hand)
                            {
                                double random_variable = get_random_1();
                                if (random_variable < crusader_proc_chance_oh)
                                {
                                    simulator_cout("OH crusader procc");
                                    time_keeper_.crusader_oh_buff_timer = 15.0;
                                    if (!crusader_oh_active)
                                    {
                                        special_stats.attack_power += 200;
                                        crusader_oh_active = true;
                                    }
                                }
                            }
                            if (time_keeper_.crusader_oh_buff_timer < 0.0 && crusader_oh_active)
                            {
                                simulator_cout("OH crusader procc run out");
                                special_stats.attack_power -= 200;
                                crusader_oh_active = false;
                            }
                        }

                        if (talents_)
                        {
                            // Unbridled wrath
                            if (get_random_1() < 0.4)
                            {
                                rage += 1;
                                simulator_cout("Unbridled wrath, +1 rage");
                            }

                            // Manage flurry charges
                            flurry_charges--;
                            flurry_charges = std::max(0, flurry_charges);
                            if (hit_outcome.hit_result == Hit_result::crit)
                            {
                                flurry_charges = 3;
                            }
                            if (flurry_charges > 0)
                            {
                                flurry_dt_factor = flurry_speed_bonus;
                            }
                            else
                            {
                                flurry_dt_factor = 1.0;
                            }
                            switch (flurry_charges)
                            {
                                case 0:
                                    simulator_cout("No flurry charges");
                                    break;
                                case 1:
                                    simulator_cout("1 flurry charge");
                                    break;
                                case 2:
                                    simulator_cout("2 flurry charges");
                                    break;
                                case 3:
                                    simulator_cout("3 flurry charges");
                                    break;
                                default:
                                    break;

                            }
                        }
                    }
                }
            }

            if (spell_rotation_)
            {
                if (death_wish_enabled_)
                {
                    if (sim_time - time_keeper_.time_ < 31.0 && rage >= 10 && !deathwish_active)
                    {
                        deathwish_active = true;
                        rage -= 10;
                        time_keeper_.global_cd = 1.0;
                        simulator_cout("------------ Deathwish activated! ------------");
                    }
                }
                if (recklessness_enabled_)
                {
                    if (sim_time - time_keeper_.time_ < 16.0 && !recklessness_active)
                    {
                        recklessness_active = true;
                        time_keeper_.global_cd = 1.0;
                        simulator_cout("------------ Recklessness activated! ------------");
                    }
                }
                // Execute phase
                if (time_keeper_.time_ > sim_time * 0.83)
                {
                    if (time_keeper_.global_cd < 0 && rage > 10)
                    {
                        simulator_cout("Execute!");
                        double damage = 600 + (rage - 10) * 15;
                        auto hit_outcome = generate_hit(damage, Hit_type::yellow, Hand::main_hand, heroic_strike_,
                                                        deathwish_active, recklessness_active);
                        if (hit_outcome.hit_result == Hit_result::crit)
                        {
                            flurry_charges = 3;
                            flurry_dt_factor = flurry_speed_bonus;
                        }
                        time_keeper_.global_cd = 1.0;
                        if (hit_outcome.hit_result == Hit_result::dodge)
                        {
                            rage -= 10;
                        }
                        else
                        {
                            rage = 0;
                        }
                        total_damage += hit_outcome.damage;
                        damage_sources.execute += hit_outcome.damage;
                        simulator_cout(rage, " rage");
                    }
                }
                else
                {
                    if (time_keeper_.blood_thirst_cd < 0.0 && time_keeper_.global_cd < 0 && rage > 30)
                    {
                        simulator_cout("Bloodthirst!");
                        double damage = special_stats.attack_power * 0.45;
                        auto hit_outcome = generate_hit(damage, Hit_type::yellow, Hand::main_hand, heroic_strike_,
                                                        deathwish_active, recklessness_active);
                        if (hit_outcome.hit_result == Hit_result::crit)
                        {
                            flurry_charges = 3;
                            flurry_dt_factor = flurry_speed_bonus;
                        }
                        if (hit_outcome.hit_result != Hit_result::dodge)
                        {
                            rage -= 30;
                        }
                        time_keeper_.blood_thirst_cd = 6.0;
                        time_keeper_.global_cd = 1.0;
                        damage_sources.bloodthirst += hit_outcome.damage;
                        total_damage += hit_outcome.damage;
                        simulator_cout(rage, " rage");
                    }

                    if (time_keeper_.whirlwind_cd < 0.0 &&
                        rage > 25 &&
                        time_keeper_.global_cd < 0 &&
                        time_keeper_.blood_thirst_cd > 1.0)
                    {
                        simulator_cout("Whirlwind!");
                        double damage;
                        if (random_melee_hits_)
                        {
                            damage = weapons[0].random_normalized_swing(special_stats.attack_power);
                        }
                        else
                        {
                            damage = weapons[0].normalized_swing(special_stats.attack_power);
                        }
                        auto hit_outcome = generate_hit(damage, Hit_type::yellow, Hand::main_hand, heroic_strike_,
                                                        deathwish_active, recklessness_active);
                        if (hit_outcome.hit_result == Hit_result::crit)
                        {
                            flurry_charges = 3;
                            flurry_dt_factor = flurry_speed_bonus;
                        }
                        rage -= 25;
                        time_keeper_.whirlwind_cd = 10;
                        time_keeper_.global_cd = 1.0;
                        damage_sources.whirlwind += hit_outcome.damage;
                        total_damage += hit_outcome.damage;
                        simulator_cout(rage, " rage");
                    }

                    if (rage > 0 && !heroic_strike_)
                    {
                        heroic_strike_ = true;
                        simulator_cout("Heroic strike activated");
                    }
                }
                time_keeper_.increment(dt);
            }
        }
        // Remove crusader if the simulation ends
        if (crusader_oh_active)
        {
            special_stats.attack_power -= 200;
        }
        if (crusader_mh_active)
        {
            special_stats.attack_power -= 200;
        }
        batch_damage_.push_back(total_damage / time_keeper_.time_);
        damage_distribution_.emplace_back(damage_sources);
    }
    return batch_damage_;
}

void Combat_simulator::enable_spell_rotation()
{
    spell_rotation_ = true;
}

void Combat_simulator::enable_talents()
{
    talents_ = true;
}

void Combat_simulator::enable_item_chance_on_hit_effects()
{
    item_chance_on_hit_ = true;
}

std::vector<Combat_simulator::Stat_weight>
Combat_simulator::compute_stat_weights(const Character &character, double sim_time, int opponent_level,
                                       int n_batches, double mean_init, double sample_std_init)
{
    if (!debug_mode_)
    {
        double stat_permutation_amount = 8;
        double special_stat_permutation_amount = 1;
        double skill_permutation_amount = 5;
        double damage_permutation_amount = 8;

        auto stat_weight_agi = permute_stat(character, &Character::permutated_stats_, &Stats::agility, Stat::agility,
                                            stat_permutation_amount,
                                            sim_time, opponent_level, n_batches, mean_init, sample_std_init);
        auto stat_weight_str = permute_stat(character, &Character::permutated_stats_, &Stats::strength, Stat::strength,
                                            stat_permutation_amount,
                                            sim_time, opponent_level, n_batches, mean_init, sample_std_init);
        auto stat_weight_crit = permute_stat(character, &Character::permutated_special_stats_,
                                             &Special_stats::critical_strike, Stat::crit,
                                             special_stat_permutation_amount,
                                             sim_time, opponent_level, n_batches, mean_init, sample_std_init);
        auto stat_weight_hit = permute_stat(character, &Character::permutated_special_stats_,
                                            &Special_stats::hit, Stat::hit,
                                            special_stat_permutation_amount,
                                            sim_time, opponent_level, n_batches, mean_init, sample_std_init);
        auto stat_weight_haste = permute_stat(character, &Character::set_extra_haste, Stat::haste,
                                              special_stat_permutation_amount,
                                              sim_time, opponent_level, n_batches, mean_init, sample_std_init);
        auto stat_weight_extra_hit = permute_stat(character, &Character::increase_chance_for_extra_hit,
                                                  Stat::chance_extra_hit,
                                                  special_stat_permutation_amount,
                                                  sim_time, opponent_level, n_batches, mean_init, sample_std_init);
        auto stat_weight_dense_stone = permute_stat(character, &Character::increase_weapon_damage,
                                                    Stat::weapon_damage, damage_permutation_amount,
                                                    sim_time, opponent_level, n_batches, mean_init, sample_std_init);

        Combat_simulator::Stat_weight stat_weight_extra_skill_sword{Stat::NONE};
        Combat_simulator::Stat_weight stat_weight_extra_skill_axe{Stat::NONE};
        Combat_simulator::Stat_weight stat_weight_extra_skill_mace{Stat::NONE};
        Combat_simulator::Stat_weight stat_weight_extra_skill_dagger{Stat::NONE};

        if (character.get_weapons()[0].get_weapon_type() == Skill_type::sword ||
            character.get_weapons()[1].get_weapon_type() == Skill_type::sword)
        {
            stat_weight_extra_skill_sword = permute_stat(character, &Character::set_extra_weapon_skill_sword,
                                                         Stat::skill_sword, skill_permutation_amount,
                                                         sim_time, opponent_level, n_batches, mean_init,
                                                         sample_std_init);
        }
        if (character.get_weapons()[0].get_weapon_type() == Skill_type::axe ||
            character.get_weapons()[1].get_weapon_type() == Skill_type::axe)
        {
            stat_weight_extra_skill_axe = permute_stat(character, &Character::set_extra_weapon_skill_axe,
                                                       Stat::skill_axe, skill_permutation_amount,
                                                       sim_time, opponent_level, n_batches, mean_init,
                                                       sample_std_init);
        }
        if (character.get_weapons()[0].get_weapon_type() == Skill_type::mace ||
            character.get_weapons()[1].get_weapon_type() == Skill_type::mace)
        {
            stat_weight_extra_skill_mace = permute_stat(character, &Character::set_extra_weapon_skill_mace,
                                                        Stat::skill_mace, skill_permutation_amount,
                                                        sim_time, opponent_level, n_batches, mean_init,
                                                        sample_std_init);
        }
        if (character.get_weapons()[0].get_weapon_type() == Skill_type::dagger ||
            character.get_weapons()[1].get_weapon_type() == Skill_type::dagger)
        {
            stat_weight_extra_skill_dagger = permute_stat(character, &Character::set_extra_weapon_skill_dagger,
                                                          Stat::skill_dagger, skill_permutation_amount,
                                                          sim_time, opponent_level, n_batches, mean_init,
                                                          sample_std_init);

        }
        return {stat_weight_agi, stat_weight_str, stat_weight_crit, stat_weight_hit, stat_weight_haste,
                stat_weight_extra_hit, stat_weight_dense_stone, stat_weight_extra_skill_sword,
                stat_weight_extra_skill_axe, stat_weight_extra_skill_mace, stat_weight_extra_skill_dagger};
    }
    return {};
}

std::ostream &operator<<(std::ostream &os, Combat_simulator::Stat_weight const &stats)
{
    switch (stats.stat)
    {
        case Combat_simulator::Stat::agility:
            os << "Agi stat weights: (" << stats.d_dps_plus << " +- " << 1.96 * stats.std_d_dps_minus << ", " <<
               stats.d_dps_minus << " +- " << 1.96 * stats.std_d_dps_minus << "). Incremented/decremented by: "
               << stats.amount << "\n";
            break;
        case Combat_simulator::Stat::strength:
            os << "Str stat weights: (" << stats.d_dps_plus << " +- " << 1.96 * stats.std_d_dps_minus << ", " <<
               stats.d_dps_minus << " +- " << 1.96 * stats.std_d_dps_minus << "). Incremented/decremented by: "
               << stats.amount << "\n";
            break;
        case Combat_simulator::Stat::crit:
            os << "Cri stat weights: (" << stats.d_dps_plus << " +- " << 1.96 * stats.std_d_dps_minus << ", " <<
               stats.d_dps_minus << " +- " << 1.96 * stats.std_d_dps_minus << "). Incremented/decremented by: "
               << stats.amount << "\n";
            break;
        case Combat_simulator::Stat::hit:
            os << "Hit stat weights: (" << stats.d_dps_plus << " +- " << 1.96 * stats.std_d_dps_minus << ", " <<
               stats.d_dps_minus << " +- " << 1.96 * stats.std_d_dps_minus << "). Incremented/decremented by: "
               << stats.amount << "\n";
            break;
        case Combat_simulator::Stat::haste:
            os << "Haste stat weights: (" << stats.d_dps_plus << " +- " << 1.96 * stats.std_d_dps_minus << ", " <<
               stats.d_dps_minus << " +- " << 1.96 * stats.std_d_dps_minus << "). Incremented/decremented by: "
               << stats.amount << "\n";
            break;
        case Combat_simulator::Stat::chance_extra_hit:
            os << "Extra swing stat weights: (" << stats.d_dps_plus << " +- " << 1.96 * stats.std_d_dps_minus
               << ", " <<
               stats.d_dps_minus << " +- " << 1.96 * stats.std_d_dps_minus << "). Incremented/decremented by: "
               << stats.amount << "\n";
            break;
        case Combat_simulator::Stat::weapon_damage:
            os << "Weapon damage stat weights: (" << stats.d_dps_plus << " +- " << 1.96 * stats.std_d_dps_minus
               << ", " << stats.d_dps_minus << " +- " << 1.96 * stats.std_d_dps_minus
               << "). Incremented/decremented by: " << stats.amount << "\n";
            break;
        case Combat_simulator::Stat::skill_sword:
            os << "Sword skill stat weights: (" << stats.d_dps_plus << " +- " << 1.96 * stats.std_d_dps_minus
               << ", " << stats.d_dps_minus << " +- " << 1.96 * stats.std_d_dps_minus
               << "). Incremented/decremented by: "
               << stats.amount << "\n";
            break;
        case Combat_simulator::Stat::skill_axe:
            os << "Axe skill stat weights: (" << stats.d_dps_plus << " +- " << 1.96 * stats.std_d_dps_minus
               << ", " <<
               stats.d_dps_minus << " +- " << 1.96 * stats.std_d_dps_minus << "). Incremented/decremented by: "
               << stats.amount << "\n";
            break;
        case Combat_simulator::Stat::skill_mace:
            os << "Mace skill stat weights: (" << stats.d_dps_plus << " +- " << 1.96 * stats.std_d_dps_minus
               << ", " <<
               stats.d_dps_minus << " +- " << 1.96 * stats.std_d_dps_minus << "). Incremented/decremented by: "
               << stats.amount << "\n";
            break;
        case Combat_simulator::Stat::skill_dagger:
            os << "Dagger skill stat weights: (" << stats.d_dps_plus << " +- " << 1.96 * stats.std_d_dps_minus
               << ", " <<
               stats.d_dps_minus << " +- " << 1.96 * stats.std_d_dps_minus << "). Incremented/decremented by: "
               << stats.amount << "\n";
            break;
        case Combat_simulator::Stat::NONE:
            break;
        default:
            assert(false);
    }
    return os;
}

double Combat_simulator::average(const std::vector<double> &vec)
{
    double sum = 0;
    for (double value : vec)
        sum += value;
    return sum / vec.size();
}

double Combat_simulator::variance(const std::vector<double> &vec, double average)
{
    double sum = 0;
    double inverse = 1.0 / static_cast<double>(vec.size());
    for (double value : vec)
    {
        sum += std::pow(static_cast<double>(value) - average, 2);
    }
    return inverse * sum;
}

double Combat_simulator::standard_deviation(const std::vector<double> &vec, double average)
{
    return std::sqrt(variance(vec, average));
}

double Combat_simulator::sample_deviation(double mean, int n_samples)
{
    return mean / std::sqrt(static_cast<double>(n_samples));
}

double Combat_simulator::add_standard_deviations(double std1, double std2)
{
    return std::sqrt(std1 * std1 + std2 * std2);
}

void Combat_simulator::enable_crusader()
{
    crusader_enabled_ = true;
}

const std::vector<double> &Combat_simulator::get_hit_probabilities_white_mh() const
{
    return hit_probabilities_white_mh_;
}

const std::vector<Combat_simulator::Damage_sources> &Combat_simulator::get_damage_distribution() const
{
    return damage_distribution_;
}

void Combat_simulator::print_damage_distribution() const
{
    Damage_sources total_sources{};
    double total_damage{0.0};
    for (const auto &damage_source : damage_distribution_)
    {
        total_sources = total_sources + damage_source;
        total_damage += damage_source.sum();
    }

    double white_mh_std = damage_source_std(&Damage_sources::white_mh);
    double white_oh_std = damage_source_std(&Damage_sources::white_oh);
    double bloodthirst_std = damage_source_std(&Damage_sources::bloodthirst);
    double execute_std = damage_source_std(&Damage_sources::execute);
    double heroic_strike_std = damage_source_std(&Damage_sources::heroic_strike);
    double whirlwind_std = damage_source_std(&Damage_sources::whirlwind);
    double extra_hit_std = damage_source_std(&Damage_sources::extra_hit);

    std::cout << "Damage_sources (%):\n" <<
              "White hits: " << total_sources.white_mh / total_damage + total_sources.white_oh / total_damage
              << ". ("
              << total_sources.white_mh / total_damage << " +- "
              << white_mh_std << ", " << total_sources.white_oh / total_damage << " +- " << white_oh_std
              << ") mainhand/offhand" <<
              "\nbloodthirst: " << total_sources.bloodthirst / total_damage << " +- " << bloodthirst_std <<
              "\nexecute: " << total_sources.execute / total_damage << " +- " << execute_std <<
              "\nheroic_strike: " << total_sources.heroic_strike / total_damage << " +- " << heroic_strike_std <<
              "\nwhirlwind: " << total_sources.whirlwind / total_damage << " +- " << whirlwind_std <<
              "\nextra_hit: " << total_sources.extra_hit / total_damage << " +- " << extra_hit_std << "\n\n";
}

void Combat_simulator::enable_death_wish()
{
    death_wish_enabled_ = true;
}

void Combat_simulator::enable_recklessness()
{
    recklessness_enabled_ = true;
}


