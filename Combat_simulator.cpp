#include "Combat_simulator.hpp"

Combat_simulator::Hit_outcome Combat_simulator::generate_hit(double damage, Hit_type hit_type)
{
    double random_var = 100 * rand() / static_cast<double>(RAND_MAX);
    if (hit_type == Hit_type::white)
    {
        int outcome = std::lower_bound(hit_probabilities_white_.begin(), hit_probabilities_white_.end(), random_var) -
                      hit_probabilities_white_.begin();
        switch (outcome)
        {
            case 0:
                return {0.0, Combat_simulator::Hit_result::miss};
            case 1:
                return {0.0, Combat_simulator::Hit_result::dodge};
            case 2:
                return {damage * 0.85, Combat_simulator::Hit_result::glancing};
            case 3:
                return {damage * 2.2, Combat_simulator::Hit_result::crit};
            case 4:
                return {damage, Combat_simulator::Hit_result::hit};
            default:
                assert(false);
                return {0.0, Combat_simulator::Hit_result::miss};
        }
    }
    else
    {
        int outcome = std::lower_bound(hit_probabilities_yellow_.begin(), hit_probabilities_yellow_.end(), random_var) -
                      hit_probabilities_yellow_.begin();
        switch (outcome)
        {
            case 0:
                return {0.0, Combat_simulator::Hit_result::miss};
            case 1:
                return {0.0, Combat_simulator::Hit_result::dodge};
            case 2:
                return {damage * 2.2, Combat_simulator::Hit_result::crit};
            case 3:
                return {damage, Combat_simulator::Hit_result::hit};
            default:
                assert(false);
                return {0.0, Combat_simulator::Hit_result::miss};
        }
    }
}

void Combat_simulator::compute_hit_table(double level, double weapon_skill, Special_stats special_stats)
{
    // TODO level based glancing/hit/crit

    double target_defence = level * 5;
    double skill_diff = target_defence - weapon_skill;

    // Crit chance
    double crit_chance = special_stats.critical_strike - skill_diff * 0.2;

    // Miss chance
    double base_miss_chance = (5 + skill_diff * 0.1);
    double dw_miss_chance = (base_miss_chance * 0.8 + 20);
    double miss_chance = dw_miss_chance - special_stats.hit;

    // Dodge chance
    double dodge_chance = (5 + skill_diff * 0.1);

    // Glancing blows
    double glancing_chance = 40;

    // Order -> Miss, parry, dodge, block, glancing, crit, hit.
    hit_probabilities_white_.clear();
    hit_probabilities_white_.push_back(miss_chance);
    hit_probabilities_white_.push_back(hit_probabilities_white_.back() + dodge_chance);
    hit_probabilities_white_.push_back(hit_probabilities_white_.back() + glancing_chance);
    hit_probabilities_white_.push_back(hit_probabilities_white_.back() + crit_chance);

    std::cout << 100 - hit_probabilities_white_.back() << "% chance to white hit. Negative value = crit capped \n";

    hit_probabilities_yellow_.clear();
    hit_probabilities_yellow_.push_back(miss_chance);
    hit_probabilities_yellow_.push_back(hit_probabilities_yellow_.back() + dodge_chance);
    hit_probabilities_yellow_.push_back(hit_probabilities_yellow_.back() + crit_chance);
}

double Combat_simulator::simulate(const Character &character, double sim_time, double dt, int opponent_level)
{
    double time = 0;
    double total_damage = 0;
    double rage = 0;
    double blood_thirst_cd = 0;
    double whirlwind_cd = 0;
    bool heroic_strike_ = false;
    int flurry_charges = 0;
    double flurry_speed_bonus = 1.3;
    double weapon_dt_factor = 1;

    auto special_stats = character.get_total_special_stats();
    auto weapons = character.get_weapons();

    compute_hit_table(opponent_level, character.get_weapon_skill(), special_stats);
    while (time < sim_time)
    {
        for (auto &weapon : weapons)
        {
            Combat_simulator::Hit_outcome hit_outcome{0.0, Hit_result::TBD};
            Weapon::Step_result step_result = weapon.step(weapon_dt_factor * dt, special_stats.attack_power);

            // Check if heroic strike should be performed
            if (step_result.did_swing)
            {
                if (spell_rotation_ &&
                    heroic_strike_ &&
                    (weapon.get_socket() == Weapon::Socket::main_hand))
                {
                    step_result.damage += 157;
                    hit_outcome = generate_hit(step_result.damage, Hit_type::yellow);
                    heroic_strike_ = false;
                    rage -= hit_outcome.damage * 7.5 / 230.6;
                }
                else
                {
                    // Otherwise do white hit
                    hit_outcome = generate_hit(step_result.damage, Hit_type::white);
                }
            }

            // Manage flurry charges
            if (talents_)
            {
                if (step_result.did_swing)
                {
                    flurry_charges--;
                    flurry_charges = std::max(0, flurry_charges);
                }
                if (hit_outcome.hit_result == Hit_result::crit)
                {
                    flurry_charges = 3;
                }
                if (flurry_charges > 0)
                {
                    weapon_dt_factor = flurry_speed_bonus;
                }
                else
                {
                    weapon_dt_factor = 1.0;
                }
            }

            rage += hit_outcome.damage * 7.5 / 230.6;
            rage = std::min(100.0, rage);
            total_damage += hit_outcome.damage;
        }

        if (spell_rotation_)
        {
            if (blood_thirst_cd < 0.0 && rage > 30)
            {
                total_damage += special_stats.attack_power * 0.45;
                rage -= 30;
                blood_thirst_cd = 6.0;
            }
            if (whirlwind_cd < 0.0 && rage > 25 && blood_thirst_cd > 0)
            {
                total_damage += weapons[0].get_average_damage() +
                                special_stats.attack_power * weapons[0].get_swing_speed() / 14;
                rage -= 25;
                whirlwind_cd = 10;
            }
            if (rage > 75)
            {
                heroic_strike_ = true;
                std::cout << "heroic" << "\n";
            }
            blood_thirst_cd -= dt;
            whirlwind_cd -= dt;

            //            overpower = baseDamageMH + 35 + attackPower / 14 * normalizedSpeed
//            execute = 600 + (rage - 15 + impExecuteRage) * 15
        }
//        std::cout << flurry_charges << "\n";
        time += dt;
    }

    return total_damage / sim_time;
}

void Combat_simulator::enable_spell_rotation()
{
    spell_rotation_ = true;
}

void Combat_simulator::enable_talents()
{
    talents_ = true;
}

void Combat_simulator::enable_item_change_on_hit_effects()
{
    item_chance_on_hit_ = true;
}
