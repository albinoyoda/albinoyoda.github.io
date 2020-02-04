#include "Combat_simulator.hpp"

#include <cmath>


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
                return {damage * (100.0 - glancing_factor_) / 100.0, Combat_simulator::Hit_result::glancing};
            case 3:
                return {damage * 2.2, Combat_simulator::Hit_result::crit};
            case 4:
                return {damage, Combat_simulator::Hit_result::hit};
            default:
                assert(false);
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
        }
    }
    return {0.0, Combat_simulator::Hit_result::miss};
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

//    std::cout << miss_chance << "% chance to miss \n";
//    std::cout << 100.0 - hit_probabilities_white_.back() << "% chance to white hit. (Negative value = crit capped) \n";
//    std::cout << crit_chance << "% chance to crit.\n\n";

    hit_probabilities_yellow_.clear();
    hit_probabilities_yellow_.push_back(miss_chance);
    hit_probabilities_yellow_.push_back(hit_probabilities_yellow_.back() + dodge_chance);
    hit_probabilities_yellow_.push_back(hit_probabilities_yellow_.back() + crit_chance);
}

std::vector<double>
Combat_simulator::simulate(const Character &character, double sim_time, int opponent_level, int n_damage_batches)
{
    std::vector<double> damage_snapshots;
    damage_snapshots.reserve(n_damage_batches);
    for (int iter = 0; iter < n_damage_batches; iter++)
    {
        double time = 0;
        double total_damage = 0;
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
        double crusader_proc_chance_mh = character.is_crusader_mh() * weapons[0].get_swing_speed() / 40;
        double crusader_proc_chance_oh = character.is_crusader_oh() * weapons[1].get_swing_speed() / 40;
        double crusader_mh_buff_timer = 0.0;
        double crusader_oh_buff_timer = 0.0;
        bool crusader_ap_active = false;
        compute_hit_table(opponent_level, character.get_weapon_skill(), special_stats);

        while (time < sim_time)
        {
            double dt = 1.0;
            if (blood_thirst_cd > 0.0)
            {
                dt = std::min(blood_thirst_cd, dt);
            }
            if (whirlwind_cd > 0.0)
            {
                dt = std::min(whirlwind_cd, dt);
            }
            if (global_cd > 0.0)
            {
                dt = std::min(global_cd, dt);
            }
            if (crusader_mh_buff_timer > 0.0)
            {
                dt = std::min(crusader_mh_buff_timer, dt);
            }
            if (crusader_oh_buff_timer > 0.0)
            {
                dt = std::min(crusader_oh_buff_timer, dt);
            }
            double wep_dt = weapons[0].get_internal_swing_timer() / (character_haste * flurry_dt_factor);
//            if (wep_dt > 0.0)
//            {
            dt = std::min(wep_dt, dt);
//            }
            wep_dt = weapons[1].get_internal_swing_timer() / (character_haste * flurry_dt_factor);
//            if (wep_dt > 0.0)
//            {
            dt = std::min(wep_dt, dt);
//            }
            dt = std::min(sim_time - time, dt);
            dt += 0.0000001;
            assert(dt > 0.0);
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
                    if (crusader_enabled_)
                    {
                        if (weapon.get_socket() == Weapon::Socket::main_hand)
                        {
                            double random_variable = rand() / static_cast<double>(RAND_MAX);
                            if (random_variable < crusader_proc_chance_mh)
                            {
                                crusader_mh_buff_timer = 15.0;
                            }
                        }
                        else
                        {
                            double random_variable = rand() / static_cast<double>(RAND_MAX);
                            if (random_variable < crusader_proc_chance_oh)
                            {
                                crusader_oh_buff_timer = 15.0;
                            }
                        }
                        if (crusader_mh_buff_timer > 0.0 || crusader_oh_buff_timer > 0.0)
                        {
                            if (!crusader_ap_active)
                            {
                                special_stats.attack_power += 200;
                                crusader_ap_active = true;
                            }
                        }
                        if (crusader_mh_buff_timer < 0.0 && crusader_oh_buff_timer < 0.0)
                        {
                            if (crusader_ap_active)
                            {
                                special_stats.attack_power -= 200;
                                crusader_ap_active = false;
                            }
                        }
                    }
                    // Unbridled wrath
                    if (talents_)
                    {
                        if (hit_outcome.hit_result != Hit_result::miss && hit_outcome.hit_result != Hit_result::dodge)
                        {
                            if (rand() % 2)
                            {
                                rage += 1;
                            }
                        }

                        // Manage flurry charges
                        if (hit_outcome.hit_result != Hit_result::miss && hit_outcome.hit_result != Hit_result::dodge)
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
                }
                blood_thirst_cd -= dt;
                whirlwind_cd -= dt;
                global_cd -= dt;
                crusader_mh_buff_timer -= dt;
                crusader_oh_buff_timer -= dt;
            }
            time += dt;
        }
        damage_snapshots.push_back(total_damage / time);
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

void Combat_simulator::enable_item_chance_on_hit_effects()
{
    item_chance_on_hit_ = true;
}

std::vector<Combat_simulator::Stat_weight>
Combat_simulator::compute_stat_weights(const Character &character, double sim_time, int opponent_level,
                                       int n_batches)
{

    double stat_permutation_amount = 10;
    double special_stat_permutation_amount = 1;
    auto stat_weight_agi = permute_stat(character, &Character::permutated_stats_, &Stats::agility, Stat::agility,
                                        stat_permutation_amount,
                                        sim_time, opponent_level, n_batches);
    auto stat_weight_str = permute_stat(character, &Character::permutated_stats_, &Stats::strength, Stat::strength,
                                        stat_permutation_amount,
                                        sim_time, opponent_level, n_batches);
    auto stat_weight_crit = permute_stat(character, &Character::permutated_special_stats_,
                                         &Special_stats::critical_strike, Stat::crit,
                                         special_stat_permutation_amount,
                                         sim_time, opponent_level, n_batches);
    auto stat_weight_hit = permute_stat(character, &Character::permutated_special_stats_,
                                        &Special_stats::hit, Stat::hit,
                                        special_stat_permutation_amount,
                                        sim_time, opponent_level, n_batches);
//    chance_extra_hit,
//            haste,
//            skill,

    return {stat_weight_agi, stat_weight_str, stat_weight_crit, stat_weight_hit};
}

std::ostream &operator<<(std::ostream &os, Combat_simulator::Stat_weight const &stats)
{
    switch (stats.stat)
    {
        case Combat_simulator::Stat::agility:
            os << "Agi stat weights: (" << stats.d_dps_plus << " +- " << 1.96 * stats.std_d_dps_minus << ", " <<
               stats.d_dps_minus << " +- " << 1.96 * stats.std_d_dps_minus << "). Incremented/decremented by: "
               << stats.amount
               << "\n";
            break;
        case Combat_simulator::Stat::strength:
            os << "Str stat weights: (" << stats.d_dps_plus << " +- " << 1.96 * stats.std_d_dps_minus << ", " <<
               stats.d_dps_minus << " +- " << 1.96 * stats.std_d_dps_minus << "). Incremented/decremented by: "
               << stats.amount
               << "\n";
            break;
        case Combat_simulator::Stat::crit:
            os << "Cri stat weights: (" << stats.d_dps_plus << " +- " << 1.96 * stats.std_d_dps_minus << ", " <<
               stats.d_dps_minus << " +- " << 1.96 * stats.std_d_dps_minus << "). Incremented/decremented by: "
               << stats.amount
               << "\n";
            break;
        case Combat_simulator::Stat::hit:
            os << "Hit stat weights: (" << stats.d_dps_plus << " +- " << 1.96 * stats.std_d_dps_minus << ", " <<
               stats.d_dps_minus << " +- " << 1.96 * stats.std_d_dps_minus << "). Incremented/decremented by: "
               << stats.amount
               << "\n";
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


