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

void Combat_simulator::compute_hit_table(int opponent_level, int weapon_skill, Special_stats special_stats)
{
    // TODO level based glancing/hit/crit

    int target_defence = opponent_level * 5;
    int skill_diff = target_defence - weapon_skill;
    int base_skill_diff = target_defence - 300;

    // Crit chance
    double crit_chance = special_stats.critical_strike - base_skill_diff * 0.2;

    // Miss chance

    double base_miss_chance = (skill_diff > 10) ? (5.0 + skill_diff * 0.2) + 1 : (5.0 + skill_diff * 0.1);
    double dw_miss_chance = (base_miss_chance * 0.8 + 20.0);
    double miss_chance = dw_miss_chance - special_stats.hit;

    // Dodge chance
    double dodge_chance = (5 + skill_diff * 0.1);

    // Glancing blows
    double glancing_chance = 40.0;
    if (skill_diff > 8)
    {
        glancing_factor_ = 35.0 - (15.0 - skill_diff) * 4.0;
    }
    else
    {
        glancing_factor_ = 5.0;
    }

    // Order -> Miss, parry, dodge, block, glancing, crit, hit.
    hit_probabilities_white_.clear();
    hit_probabilities_white_.push_back(miss_chance);
    hit_probabilities_white_.push_back(hit_probabilities_white_.back() + dodge_chance);
    hit_probabilities_white_.push_back(hit_probabilities_white_.back() + glancing_chance);
    hit_probabilities_white_.push_back(hit_probabilities_white_.back() + crit_chance);

    std::cout << miss_chance << "% chance to miss \n";
    std::cout << 100.0 - hit_probabilities_white_.back() << "% chance to white hit. (Negative value = crit capped) \n";
    std::cout << crit_chance << "% chance to crit.\n";

    hit_probabilities_yellow_.clear();
    hit_probabilities_yellow_.push_back(miss_chance);
    hit_probabilities_yellow_.push_back(hit_probabilities_yellow_.back() + dodge_chance);
    hit_probabilities_yellow_.push_back(hit_probabilities_yellow_.back() + crit_chance);
}

std::vector<double>
Combat_simulator::simulate(const Character &character, double sim_time, double dt, int opponent_level)
{
    size_t N = sim_time / dt;
    double time = 0;
    double total_damage = 0;
    std::vector<double> damage_snapshots;
    double rage = 0;
    double blood_thirst_cd = 0;
    double whirlwind_cd = 0;
    double global_cd = 0;
    bool heroic_strike_ = false;
    int flurry_charges = 0;
    double flurry_speed_bonus = 1.3;
    double character_haste = character.get_haste();
    double flurry_dt_factor = 1;
    auto special_stats = character.get_total_special_stats();
    auto weapons = character.get_weapons();
    double chance_for_extra_hit = character.get_chance_for_extra_hit();

    compute_hit_table(opponent_level, character.get_weapon_skill(), special_stats);
    for (size_t iter = 0; iter < N; iter++)
    {
        for (auto &weapon : weapons)
        {
            Combat_simulator::Hit_outcome hit_outcome{0.0, Hit_result::TBD};
            Weapon::Step_result step_result = weapon.step(character_haste * flurry_dt_factor * dt,
                                                          special_stats.attack_power);

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
                    rage -= 13;
                    rage = std::min(100.0, rage);
                }
                else
                {
                    // Otherwise do white hit
                    hit_outcome = generate_hit(step_result.damage, Hit_type::white);
                    rage += hit_outcome.damage * 7.5 / 230.6;
                    rage = std::min(100.0, rage);
                }
                total_damage += hit_outcome.damage;

                if (item_chance_on_hit_)
                {
                    if (chance_for_extra_hit > 0.0)
                    {
                        double random_variable = 100 * rand() / static_cast<double>(RAND_MAX);
                        if (random_variable < chance_for_extra_hit)
                        {
                            double damage = weapons[0].swing(special_stats.attack_power);
                            hit_outcome = generate_hit(damage, Hit_type::white);
                            total_damage += hit_outcome.damage;
                            rage += hit_outcome.damage * 7.5 / 230.6;
                            rage = std::min(100.0, rage);
                            weapons[0].reset_timer();
                        }
                    }
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
                    flurry_dt_factor = flurry_speed_bonus;
                }
                else
                {
                    flurry_dt_factor = 1.0;
                }
            }

            // Unbridled wrath
            if (talents_)
            {
                if (step_result.did_swing)
                {
                    if (rand() % 2)
                    {
                        rage += 1;
                    }
                }
            }
        }

        if (spell_rotation_)
        {
            if (blood_thirst_cd < 0.0 && global_cd < 0 && rage > 30)
            {
                double damage = special_stats.attack_power * 0.45;
                auto hit_outcome = generate_hit(damage, Hit_type::yellow);
                if (hit_outcome.hit_result == Hit_result::crit)
                {
                    flurry_charges = 3;
                    flurry_dt_factor = flurry_speed_bonus;
                }
                total_damage += hit_outcome.damage;
                rage -= 30;
                blood_thirst_cd = 6.0;
                global_cd = 1.0;
            }
            if (whirlwind_cd < 0.0 && rage > 25 && global_cd < 0 && blood_thirst_cd > 0)
            {
                double damage = weapons[0].get_average_damage() +
                                special_stats.attack_power * weapons[0].get_swing_speed() / 14;
                auto hit_outcome = generate_hit(damage, Hit_type::yellow);
                if (hit_outcome.hit_result == Hit_result::crit)
                {
                    flurry_charges = 3;
                    flurry_dt_factor = flurry_speed_bonus;
                }
                total_damage += hit_outcome.damage;
                rage -= 25;
                whirlwind_cd = 10;
                global_cd = 1.0;
            }
            if (rage > 75 && !heroic_strike_)
            {
                heroic_strike_ = true;
//                std::cout << "heroic" << "\n";
            }
            blood_thirst_cd -= dt;
            whirlwind_cd -= dt;
            global_cd -= dt;

            //            overpower = baseDamageMH + 35 + attackPower / 14 * normalizedSpeed
//            execute = 600 + (rage - 15 + impExecuteRage) * 15
        }
        time += dt;
        if (((iter + 1) % (N / 10)) == 0)
        {
            damage_snapshots.push_back(total_damage / time);
        }
    }

    return damage_snapshots;
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
