#include "Combat_simulator.hpp"

#include "include/Statistics.hpp"

#include <algorithm>

namespace
{
constexpr double rage_factor = 15.0 / 230.6 / 2.0;

constexpr double rage_from_damage_taken(double damage)
{
    return damage * 5.0 / 2.0 / 230.6;
}

constexpr double rage_generation(double damage)
{
    return damage * rage_factor;
}

constexpr double armor_mitigation(int target_armor, int target_level)
{
    return static_cast<double>(target_armor) / static_cast<double>(target_armor + 400 + 85 * target_level);
}

std::vector<double> create_hit_table(double miss, double dodge, double glancing, double crit)
{
    // Order -> Miss, parry, dodge, block, glancing, crit, hit.
    return {miss, miss + dodge, miss + dodge + glancing, miss + dodge + glancing + crit};
}

std::vector<double> create_hit_table_yellow(double miss, double dodge, double crit)
{
    double double_roll_factor = (100 - miss - dodge) / 100;
    // Order -> Miss, parry, dodge, block, glancing, crit, hit.
    // double_roll_factor compensates for the crit suppression caused by ability double roll
    return {miss, miss + dodge, miss + dodge, miss + dodge + double_roll_factor * crit};
}

std::vector<double> create_multipliers(double glancing_factor, double crit_damage_bonus, double bonus_crit_multiplier)
{
    // Order -> Miss, parry, dodge, block, glancing, crit, hit.
    return {0.0, 0.0, glancing_factor, 1.0 + multiplicative_addition(1.0 + crit_damage_bonus, bonus_crit_multiplier),
            1.0};
}
} // namespace

void Combat_simulator::set_config(const Combat_simulator_config& new_config)
{
    config = new_config;

    heroic_strike_rage_cost = 15 - config.talents.improved_heroic_strike;
    p_unbridled_wrath_ = 0.08 * config.talents.unbridled_wrath;
    execute_rage_cost_ = 15 - static_cast<int>(2.51 * config.talents.improved_execute);

    armor_reduction_from_spells_ = 0.0;
    armor_reduction_from_spells_ += 450 * config.n_sunder_armor_stacks;
    armor_reduction_from_spells_ += 640 * config.curse_of_recklessness_active;
    armor_reduction_from_spells_ += 505 * config.faerie_fire_feral_active;
    if (config.exposed_armor)
    {
        armor_reduction_delayed_ = 2550 - 450 * config.n_sunder_armor_stacks;
    }

    flurry_haste_factor_ = (config.talents.flurry > 0) ? 0.05 + 0.05 * config.talents.flurry : 0.0;
    dual_wield_damage_factor_ = 0.5 + 0.025 * config.talents.dual_wield_specialization;
    cleave_bonus_damage_ = 50 * (1.0 + 0.4 * config.talents.improved_cleave);
    slam_manager.slam_cast_time_ = 1.5 - 0.1 * config.talents.improved_slam;

    tactical_mastery_rage_ = 5.0 * config.talents.tactical_mastery;
    deep_wounds_ = config.talents.deep_wounds && config.combat.deep_wounds;
    use_bloodthirst_ = config.talents.bloodthirst && config.combat.use_bloodthirst;
    use_mortal_strike_ = config.talents.mortal_strike && config.combat.use_mortal_strike;
    use_sweeping_strikes_ =
        config.talents.sweeping_strikes && config.combat.use_sweeping_strikes && config.multi_target_mode_;

    over_time_effects_.clear();
    use_effects_all_.clear();

    if (config.talents.death_wish && config.combat.use_death_wish)
    {
        use_effects_all_.emplace_back(deathwish);
    }

    if (config.enable_recklessness)
    {
        use_effects_all_.emplace_back(recklessness);
    }

    if (config.enable_bloodrage)
    {
        use_effects_all_.emplace_back(bloodrage);
    }

    if (config.enable_berserking)
    {
        use_effects_all_.emplace_back(Use_effect{"Berserking",
                                                 Use_effect::Effect_socket::unique,
                                                 {},
                                                 {0, 0, 0, 0, config.berserking_haste_ / 100.0},
                                                 0,
                                                 10,
                                                 180,
                                                 false});
    }

    if (config.essence_of_the_red_)
    {
        over_time_effects_.push_back(essence_of_the_red);
    }

    if (config.take_periodic_damage_)
    {
        double rage_per_tick = rage_from_damage_taken(config.periodic_damage_amount_);
        Over_time_effect rage_from_damage = {"Rage gained from damage taken",  {}, rage_per_tick, 0,
                                             config.periodic_damage_interval_, 600};
        over_time_effects_.push_back(rage_from_damage);
    }

    if (config.talents.anger_management)
    {
        over_time_effects_.push_back(anger_management);
    }
}

std::string Combat_simulator::hit_result_to_string(const Combat_simulator::Hit_result hit_result)
{
    switch (hit_result)
    {
    case Hit_result::hit:
        return " hit";
    case Hit_result::crit:
        return " crit";
    case Hit_result::dodge:
        return " dodge";
    case Hit_result::miss:
        return " miss";
    default:
        return " bugs";
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
                // Should never happen
                simulator_cout("BUG");
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

Combat_simulator::Hit_outcome Combat_simulator::generate_hit_mh(double damage, Hit_type hit_type, bool is_overpower)
{
    if (hit_type == Hit_type::white)
    {
        simulator_cout("Drawing outcome from MH hit table");
        double random_var = get_uniform_random(100);
        int outcome = std::lower_bound(hit_table_white_mh_.begin(), hit_table_white_mh_.end(), random_var) -
                      hit_table_white_mh_.begin();
        return {damage * damage_multipliers_white_mh_[outcome], Hit_result(outcome)};
    }
    else
    {
        simulator_cout("Drawing outcome from yellow table");
        double random_var = get_uniform_random(100);
        if (is_overpower)
        {
            int outcome = std::lower_bound(hit_table_overpower_.begin(), hit_table_overpower_.end(), random_var) -
                          hit_table_overpower_.begin();
            return {damage * damage_multipliers_yellow_[outcome], Hit_result(outcome)};
        }
        else
        {
            int outcome = std::lower_bound(hit_table_yellow_.begin(), hit_table_yellow_.end(), random_var) -
                          hit_table_yellow_.begin();
            return {damage * damage_multipliers_yellow_[outcome], Hit_result(outcome)};
        }
    }
}

Combat_simulator::Hit_outcome Combat_simulator::generate_hit_oh(double damage)
{
    if (ability_queue_manager.is_ability_queued())
    {
        simulator_cout("Drawing outcome from OH hit table (without DW penalty)");
        double random_var = get_uniform_random(100);
        int outcome = std::lower_bound(hit_table_two_hand_.begin(), hit_table_two_hand_.end(), random_var) -
                      hit_table_two_hand_.begin();
        return {damage * damage_multipliers_white_oh_[outcome], Hit_result(outcome)};
    }
    else
    {
        simulator_cout("Drawing outcome from OH hit table");
        double random_var = get_uniform_random(100);
        int outcome = std::lower_bound(hit_table_white_oh_.begin(), hit_table_white_oh_.end(), random_var) -
                      hit_table_white_oh_.begin();
        return {damage * damage_multipliers_white_oh_[outcome], Hit_result(outcome)};
    }
}

Combat_simulator::Hit_outcome Combat_simulator::generate_hit(const Weapon_sim& weapon, double damage,
                                                             Combat_simulator::Hit_type hit_type, Socket weapon_hand,
                                                             const Special_stats& special_stats,
                                                             Damage_sources& damage_sources, bool boss_target,
                                                             bool is_overpower, bool can_sweep)
{
    Combat_simulator::Hit_outcome hit_outcome;
    if (weapon_hand == Socket::main_hand)
    {
        hit_outcome = generate_hit_mh(damage, hit_type, is_overpower);
        if (boss_target)
        {
            hit_outcome.damage *= armor_reduction_factor_ * (1 + special_stats.damage_mod_physical);
        }
        else
        {
            hit_outcome.damage *= armor_reduction_factor_add * (1 + special_stats.damage_mod_physical);
        }
        cout_damage_parse(hit_type, weapon_hand, hit_outcome);
    }
    else
    {
        hit_outcome = generate_hit_oh(damage);
        if (boss_target)
        {
            hit_outcome.damage *= armor_reduction_factor_ * (1 + special_stats.damage_mod_physical);
        }
        else
        {
            hit_outcome.damage *= armor_reduction_factor_add * (1 + special_stats.damage_mod_physical);
        }
        cout_damage_parse(hit_type, weapon_hand, hit_outcome);
    }

    if (sweeping_strikes_charges_ > 0)
    {
        if (hit_outcome.damage > 0.0 && can_sweep)
        {
            double sweeping_strike_damage{};
            if (boss_target)
            {
                sweeping_strike_damage = hit_outcome.damage / armor_reduction_factor_ * armor_reduction_factor_add;
                damage_sources.add_damage(Damage_source::sweeping_strikes, sweeping_strike_damage, time_keeper_.time);
            }
            else
            {
                double rand_var = get_uniform_random(1);
                if (number_of_extra_targets_ > 0 && rand_var > 1.0 - 1.0 / number_of_extra_targets_)
                {
                    sweeping_strike_damage = hit_outcome.damage / armor_reduction_factor_add * armor_reduction_factor_;
                }
                else
                {
                    sweeping_strike_damage = hit_outcome.damage;
                }
                damage_sources.add_damage(Damage_source::sweeping_strikes, sweeping_strike_damage, time_keeper_.time);
            }
            simulator_cout("Sweeping strikes hits a nearby target.");
            cout_damage_parse(Hit_type::yellow, Socket::main_hand,
                              Hit_outcome{sweeping_strike_damage, Hit_result::hit});
            sweeping_strikes_charges_--;
            simulator_cout("Sweeping strikes charges left: ", sweeping_strikes_charges_);
        }
    }

    if (deep_wounds_)
    {
        if (hit_outcome.hit_result == Combat_simulator::Hit_result::crit)
        {
            buff_manager_.add_over_time_effect(
                {"Deep_wounds",
                 {},
                 0,
                 (1 + special_stats.damage_mod_physical) * weapon.swing(special_stats.attack_power) / 4,
                 3,
                 12},
                int(time_keeper_.time));
        }
    }

    if (hit_outcome.hit_result == Combat_simulator::Hit_result::dodge)
    {
        simulator_cout("Overpower aura gained!");
        buff_manager_.add("overpower_aura", {}, 5.0);
    }
    return hit_outcome;
}

void Combat_simulator::compute_hit_table(int weapon_skill, const Special_stats& special_stats, Socket weapon_hand,
                                         Weapon_socket weapon_socket)
{
    int level_difference = config.main_target_level - 60;
    int target_defence_level = config.main_target_level * 5;
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
    crit_chance = std::max(crit_chance, 0.0);

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
    double dw_miss_chance =
        (weapon_socket == Weapon_socket::two_hand) ? base_miss_chance : (base_miss_chance * 0.8 + 20.0);
    double corrected_hit = special_stats.hit - hit_penalty;
    double miss_chance = dw_miss_chance - std::max(corrected_hit, 0.0);
    miss_chance = std::max(miss_chance, 0.0);
    double two_hand_miss_chance = std::max(base_miss_chance - corrected_hit, 0.0);

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
        hit_table_white_mh_ = create_hit_table(miss_chance, dodge_chance, glancing_chance, crit_chance);
        damage_multipliers_white_mh_ =
            create_multipliers((100.0 - glancing_penalty) / 100.0, 0.0, special_stats.crit_multiplier);

        hit_table_yellow_ = create_hit_table_yellow(two_hand_miss_chance, dodge_chance, crit_chance);
        hit_table_overpower_ =
            create_hit_table_yellow(two_hand_miss_chance, 0, crit_chance + 25 * config.talents.overpower - 3.0);
        damage_multipliers_yellow_ =
            create_multipliers(1.0, 0.1 * config.talents.impale, special_stats.crit_multiplier);
    }
    else
    {
        hit_table_white_oh_ = create_hit_table(miss_chance, dodge_chance, glancing_chance, crit_chance);
        damage_multipliers_white_oh_ =
            create_multipliers((100.0 - glancing_penalty) / 100.0, 0.0, special_stats.crit_multiplier);

        hit_table_two_hand_ = create_hit_table(two_hand_miss_chance, dodge_chance, glancing_chance, crit_chance);
    }
}

void Combat_simulator::manage_flurry(Hit_result hit_result, Special_stats& special_stats, int& flurry_charges,
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
                special_stats += {0, 0, 0, 0, flurry_haste_factor_};
            }
        }
        else if (flurry_active && flurry_charges == 0)
        {
            special_stats -= {0, 0, 0, 0, flurry_haste_factor_};
        }
        simulator_cout(flurry_charges, " flurry charges");
    }
}

bool Combat_simulator::start_cast_slam(bool mh_swing, double rage, double& swing_time_left)
{
    bool use_sl = true;
    if (use_bloodthirst_)
    {
        use_sl &= time_keeper_.blood_thirst_cd > config.combat.slam_cd_thresh;
    }
    if (use_mortal_strike_)
    {
        use_sl &= time_keeper_.mortal_strike_cd > config.combat.slam_cd_thresh;
    }
    if (config.combat.use_whirlwind)
    {
        use_sl &= time_keeper_.whirlwind_cd > config.combat.slam_cd_thresh;
    }
    if (use_sl)
    {
        if (swing_time_left > config.combat.slam_spam_max_time)
        {
            if ((mh_swing && rage > config.combat.slam_rage_dd) || rage > config.combat.slam_spam_rage)
            {
                simulator_cout("Starting to channel slam.");
                slam_manager.queue_slam(time_keeper_.time);
                time_keeper_.global_cd = 1.5;
                // We have started 'channeling' so set a value for the swing time for now which is larger than GCD
                swing_time_left = 1.6;
                return true;
            }
        }
    }
    return false;
}

void Combat_simulator::slam(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                            Damage_sources& damage_sources, int& flurry_charges)
{
    if (config.dpr_settings.compute_dpr_sl_)
    {
        get_uniform_random(100) < hit_table_yellow_[1] ? rage -= 3 : rage -= 15;
        return;
    }
    simulator_cout("Slam!");
    double damage = main_hand_weapon.swing(special_stats.attack_power) + 87.0;
    auto hit_outcome =
        generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats, damage_sources);
    if (hit_outcome.hit_result == Hit_result::dodge || hit_outcome.hit_result == Hit_result::miss)
    {
        rage -= 3;
    }
    else
    {
        rage -= 15;
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges);
    }
    manage_flurry(hit_outcome.hit_result, special_stats, flurry_charges, true);
    damage_sources.add_damage(Damage_source::slam, hit_outcome.damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::mortal_strike(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                                     Damage_sources& damage_sources, int& flurry_charges)
{
    if (config.dpr_settings.compute_dpr_ms_)
    {
        get_uniform_random(100) < hit_table_yellow_[1] ? rage -= 6 : rage -= 30;
        return;
    }
    simulator_cout("Mortal Strike!");
    double damage = main_hand_weapon.normalized_swing(special_stats.attack_power) + 160;
    auto hit_outcome =
        generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats, damage_sources);
    if (hit_outcome.hit_result == Hit_result::dodge || hit_outcome.hit_result == Hit_result::miss)
    {
        rage -= 6;
    }
    else
    {
        rage -= 30;
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges);
    }
    time_keeper_.mortal_strike_cd = 6.0;
    time_keeper_.global_cd = 1.5;
    manage_flurry(hit_outcome.hit_result, special_stats, flurry_charges, true);
    damage_sources.add_damage(Damage_source::mortal_strike, hit_outcome.damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::bloodthirst(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                                   Damage_sources& damage_sources, int& flurry_charges)
{
    if (config.dpr_settings.compute_dpr_bt_)
    {
        get_uniform_random(100) < hit_table_yellow_[1] ? rage -= 6 : rage -= 30;
        time_keeper_.blood_thirst_cd = 6.0;
        time_keeper_.global_cd = 1.5;
        return;
    }
    simulator_cout("Bloodthirst!");
    double damage = special_stats.attack_power * 0.45;
    auto hit_outcome =
        generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats, damage_sources);
    if (hit_outcome.hit_result == Hit_result::dodge || hit_outcome.hit_result == Hit_result::miss)
    {
        rage -= 6;
    }
    else
    {
        rage -= 30;
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges);
    }
    time_keeper_.blood_thirst_cd = 6.0;
    time_keeper_.global_cd = 1.5;
    manage_flurry(hit_outcome.hit_result, special_stats, flurry_charges, true);
    damage_sources.add_damage(Damage_source::bloodthirst, hit_outcome.damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::overpower(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                                 Damage_sources& damage_sources, int& flurry_charges)
{
    if (config.dpr_settings.compute_dpr_op_)
    {
        if (rage > tactical_mastery_rage_)
        {
            rage = tactical_mastery_rage_;
        }
        rage -= 5;
        buff_manager_.add("battle_stance", {-3.0, 0, 0}, 1.5);
        time_keeper_.overpower_cd = 5.0;
        time_keeper_.global_cd = 1.5;
        return;
    }
    simulator_cout("Changed stance: Battle Stance.");
    simulator_cout("Overpower!");
    buff_manager_.add("battle_stance", {-3.0, 0, 0}, 1.5);
    double damage = main_hand_weapon.normalized_swing(special_stats.attack_power) + 35;
    auto hit_outcome = generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats,
                                    damage_sources, true, true);
    if (rage > tactical_mastery_rage_)
    {
        rage_lost_stance_swap_ += rage - tactical_mastery_rage_;
        rage = tactical_mastery_rage_;
    }
    rage -= 5;
    if (hit_outcome.hit_result != Hit_result::miss)
    {
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges);
    }
    time_keeper_.overpower_cd = 5.0;
    time_keeper_.global_cd = 1.5;
    manage_flurry(hit_outcome.hit_result, special_stats, flurry_charges, true);
    damage_sources.add_damage(Damage_source::overpower, hit_outcome.damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::whirlwind(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                                 Damage_sources& damage_sources, int& flurry_charges)
{
    if (config.dpr_settings.compute_dpr_ww_)
    {
        rage -= 25;
        time_keeper_.whirlwind_cd = 10;
        time_keeper_.global_cd = 1.5;
        return;
    }
    simulator_cout("Whirlwind! #targets = boss + ", number_of_extra_targets_, " adds");
    simulator_cout("Whirlwind hits: ", std::min(number_of_extra_targets_ + 1, 4), " targets");
    double damage = main_hand_weapon.normalized_swing(special_stats.attack_power);
    std::vector<Hit_outcome> hit_outcomes{};
    hit_outcomes.reserve(4);
    for (int i = 0; i < std::min(number_of_extra_targets_ + 1, 4); i++)
    {
        hit_outcomes.emplace_back(generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand,
                                               special_stats, damage_sources, i == 0, false, i == 0));
        if (hit_outcomes.back().hit_result != Hit_result::dodge && hit_outcomes.back().hit_result != Hit_result::miss)
        {
            hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges);
        }
    }
    rage -= 25;
    time_keeper_.whirlwind_cd = 10;
    time_keeper_.global_cd = 1.5;
    Hit_result result_used_for_flurry = Hit_result::TBD;
    double total_damage = 0;
    for (const auto& hit_outcome : hit_outcomes)
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

void Combat_simulator::execute(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                               Damage_sources& damage_sources, int& flurry_charges)
{
    if (config.dpr_settings.compute_dpr_ex_)
    {
        get_uniform_random(100) < hit_table_yellow_[1] ? rage *= 0.8 : rage -= execute_rage_cost_;
        double next_server_batch = std::fmod(time_keeper_.time + init_server_time, 0.4);
        buff_manager_.add("execute_rage_batch", {}, 0.4 + next_server_batch);
        time_keeper_.global_cd = 1.5;
        return;
    }
    simulator_cout("Execute!");
    double damage = 600 + (rage - execute_rage_cost_) * 15;
    auto hit_outcome =
        generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats, damage_sources);
    if (hit_outcome.hit_result == Hit_result::dodge || hit_outcome.hit_result == Hit_result::miss)
    {
        rage *= 0.8;
    }
    else
    {
        rage -= execute_rage_cost_;
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges);
    }
    double next_server_batch = std::fmod(time_keeper_.time + init_server_time, 0.4);
    buff_manager_.add("execute_rage_batch", {}, 0.4 + next_server_batch);
    buff_manager_.rage_before_execute = rage;
    time_keeper_.global_cd = 1.5;
    manage_flurry(hit_outcome.hit_result, special_stats, flurry_charges, true);
    damage_sources.add_damage(Damage_source::execute, hit_outcome.damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::hamstring(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                                 Damage_sources& damage_sources, int& flurry_charges)
{
    if (config.dpr_settings.compute_dpr_ha_)
    {
        get_uniform_random(100) < hit_table_yellow_[1] ? rage -= 2 : rage -= 10;
        time_keeper_.global_cd = 1.5;
        return;
    }
    simulator_cout("Hamstring!");
    double damage = 45;
    auto hit_outcome =
        generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats, damage_sources);
    time_keeper_.global_cd = 1.5;
    if (hit_outcome.hit_result == Hit_result::dodge || hit_outcome.hit_result == Hit_result::miss)
    {
        rage -= 2;
    }
    else
    {
        rage -= 10;
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges);
    }
    manage_flurry(hit_outcome.hit_result, special_stats, flurry_charges, true);
    damage_sources.add_damage(Damage_source::hamstring, hit_outcome.damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::hit_effects(Weapon_sim& weapon, Weapon_sim& main_hand_weapon, Special_stats& special_stats,
                                   double& rage, Damage_sources& damage_sources, int& flurry_charges,
                                   bool is_extra_attack)
{
    for (const auto& hit_effect : weapon.hit_effects)
    {
        double r = get_uniform_random(1);
        if (r < hit_effect.probability)
        {
            proc_data_[hit_effect.name]++;
            switch (hit_effect.type)
            {
            case Hit_effect::Type::extra_hit:
                if (!is_extra_attack)
                {
                    simulator_cout("PROC: extra hit from: ", hit_effect.name);
                    swing_weapon(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources,
                                 flurry_charges, hit_effect.attack_power_boost, true);
                }
                break;
            case Hit_effect::Type::damage_magic: {
                // * 0.83 Assumes a static 17% chance to resist.
                // (100 + special_stats.spell_crit / 2) / 100 is the average damage gained from a x1.5 spell crit
                double effect_damage = hit_effect.damage * 0.83 * (100 + special_stats.spell_crit / 2) / 100 *
                                       (1 + special_stats.damage_mod_spell);
                damage_sources.add_damage(Damage_source::item_hit_effects, effect_damage, time_keeper_.time);
                simulator_cout("PROC: ", hit_effect.name, " does ", effect_damage, " magic damage.");
                break;
            }
            case Hit_effect::Type::damage_physical: {
                auto hit = generate_hit(main_hand_weapon, hit_effect.damage, Hit_type::yellow, Socket::main_hand,
                                        special_stats, damage_sources);
                damage_sources.add_damage(Damage_source::item_hit_effects, hit.damage, time_keeper_.time);
                if (config.display_combat_debug)
                {
                    simulator_cout("PROC: ", hit_effect.name, hit_result_to_string(hit.hit_result), " does ",
                                   int(hit.damage), " physical damage");
                }
                hit_effects(weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges,
                            is_extra_attack);
            }
            break;
            case Hit_effect::Type::stat_boost:
                simulator_cout("PROC: ", hit_effect.name, " stats increased for ", hit_effect.duration, "s");
                buff_manager_.add(weapon.socket_name + "_" + hit_effect.name,
                                  hit_effect.get_special_stat_equivalent(special_stats), hit_effect.duration);
                break;
            case Hit_effect::Type::reduce_armor: {
                if (current_armor_red_stacks_ < hit_effect.max_stacks)
                {
                    recompute_mitigation_ = true;
                    current_armor_red_stacks_++;
                    armor_penetration_ = current_armor_red_stacks_ * hit_effect.armor_reduction;
                    simulator_cout("PROC: ", hit_effect.name, ", current stacks: ", current_armor_red_stacks_);
                }
                else
                {
                    simulator_cout("PROC: ", hit_effect.name,
                                   ". At max stacks. Current stacks: ", current_armor_red_stacks_);
                }
            }
            break;
            default:
                std::cout << ":::::::::::FAULTY HIT EFFECT IN SIMULATION!!!:::::::::";
                break;
            }
        }
    }
}

void Combat_simulator::swing_weapon(Weapon_sim& weapon, Weapon_sim& main_hand_weapon, Special_stats& special_stats,
                                    double& rage, Damage_sources& damage_sources, int& flurry_charges,
                                    double attack_power_bonus, bool is_extra_attack)
{
    std::vector<Hit_outcome> hit_outcomes{};
    hit_outcomes.reserve(2);
    double swing_damage = weapon.swing(special_stats.attack_power + attack_power_bonus);
    if (weapon.socket == Socket::off_hand)
    {
        swing_damage *= dual_wield_damage_factor_;
    }
    weapon.internal_swing_timer = weapon.swing_speed / (1 + special_stats.haste);

    // Check if heroic strike should be performed
    if (ability_queue_manager.heroic_strike_queued && weapon.socket == Socket::main_hand &&
        rage >= heroic_strike_rage_cost)
    {
        simulator_cout("Performing Heroic Strike");
        swing_damage += config.combat.heroic_strike_damage;
        hit_outcomes.emplace_back(generate_hit(main_hand_weapon, swing_damage, Hit_type::yellow, weapon.socket,
                                               special_stats, damage_sources));
        ability_queue_manager.heroic_strike_queued = false;
        if (config.ability_queue_ && rage > config.ability_queue_rage_thresh_)
        {
            simulator_cout("Re-queue Heroic Strike!");
            ability_queue_manager.queue_heroic_strike();
        }
        if (hit_outcomes[0].hit_result == Hit_result::dodge || hit_outcomes[0].hit_result == Hit_result::miss)
        {
            rage -= 0.2 * heroic_strike_rage_cost; // Refund rage for missed/dodged heroic strikes.
        }
        else
        {
            rage -= heroic_strike_rage_cost;
        }
        damage_sources.add_damage(Damage_source::heroic_strike, hit_outcomes[0].damage, time_keeper_.time);
        simulator_cout("Current rage: ", int(rage));
    }
    else if (ability_queue_manager.cleave_queued && weapon.socket == Socket::main_hand && rage >= 20)
    {
        simulator_cout("Performing cleave! #targets = boss + ", number_of_extra_targets_, " adds");
        simulator_cout("Cleave hits: ", std::min(number_of_extra_targets_ + 1, 2), " targets");
        swing_damage += cleave_bonus_damage_;

        for (int i = 0; i < std::min(number_of_extra_targets_ + 1, 2); i++)
        {
            hit_outcomes.emplace_back(generate_hit(main_hand_weapon, swing_damage, Hit_type::yellow, weapon.socket,
                                                   special_stats, damage_sources, i == 0));
        }
        ability_queue_manager.cleave_queued = false;
        if (config.ability_queue_ && rage > config.ability_queue_rage_thresh_)
        {
            simulator_cout("Re-queue Cleave!");
            ability_queue_manager.queue_cleave();
        }
        rage -= 20;
        double total_damage = 0;
        for (const auto& hit_outcome : hit_outcomes)
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
        hit_outcomes.emplace_back(generate_hit(main_hand_weapon, swing_damage, Hit_type::white, weapon.socket,
                                               special_stats, damage_sources));
        if (dpr_heroic_strike_queued_)
        {
            rage -= heroic_strike_rage_cost;
            dpr_heroic_strike_queued_ = false;
        }
        else if (dpr_cleave_queued_)
        {
            rage -= 20;
            dpr_cleave_queued_ = false;
        }
        else
        {
            rage += rage_generation(hit_outcomes[0].damage);
        }
        if (hit_outcomes[0].hit_result == Hit_result::dodge)
        {
            simulator_cout("Rage gained since the enemy dodged.");
            rage += 0.75 *
                    rage_generation(swing_damage * armor_reduction_factor_ * (1 + special_stats.damage_mod_physical));
        }
        if (rage > 100.0)
        {
            rage_lost_capped_ += rage - 100.0;
            rage = 100.0;
        }
        simulator_cout("Current rage: ", int(rage));
        if (weapon.socket == Socket::main_hand)
        {
            damage_sources.add_damage(Damage_source::white_mh, hit_outcomes[0].damage, time_keeper_.time);
        }
        else
        {
            damage_sources.add_damage(Damage_source::white_oh, hit_outcomes[0].damage, time_keeper_.time);
        }
    }

    Hit_result result_used_for_flurry = Hit_result::TBD;
    for (const auto& hit_outcome : hit_outcomes)
    {
        if (hit_outcome.hit_result == Hit_result::crit)
        {
            result_used_for_flurry = Hit_result::crit;
            break;
        }
    }
    manage_flurry(result_used_for_flurry, special_stats, flurry_charges);

    for (const auto& hit_outcome : hit_outcomes)
    {
        if (hit_outcome.hit_result != Hit_result::miss && hit_outcome.hit_result != Hit_result::dodge)
        {
            hit_effects(weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges, is_extra_attack);
        }
    }

    // Unbridled wrath
    if (get_uniform_random(1) < p_unbridled_wrath_)
    {
        rage += 1;
        if (rage > 100.0)
        {
            rage_lost_capped_ += rage - 100.0;
            rage = 100.0;
        }
        simulator_cout("Unbridled wrath. Current rage: ", int(rage));
    }
}

void Combat_simulator::simulate(const Character& character, size_t n_simulations, double init_mean,
                                double init_variance, size_t init_simulations)
{
    dps_distribution_.mean_ = init_mean;
    dps_distribution_.variance_ = init_variance;
    config.n_batches = n_simulations;
    simulate(character, init_simulations);
}

void Combat_simulator::simulate(const Character& character, int init_iteration, bool log_data)
{
    int n_damage_batches = config.n_batches;
    if (config.display_combat_debug)
    {
        debug_topic_ = "";
        n_damage_batches = 1;
    }
    if (log_data)
    {
        reset_time_lapse();
        init_histogram();
    }
    buff_manager_.aura_uptime.clear();
    damage_distribution_ = Damage_sources{};
    flurry_uptime_mh_ = 0;
    flurry_uptime_oh_ = 0;
    rage_lost_execute_batch_ = 0;
    rage_lost_stance_swap_ = 0;
    rage_lost_capped_ = 0;
    heroic_strike_uptime_ = 0;
    const auto starting_special_stats = character.total_special_stats;
    std::vector<Weapon_sim> weapons;
    for (const auto& wep : character.weapons)
    {
        weapons.emplace_back(wep.swing_speed, wep.min_damage, wep.max_damage, wep.socket, wep.type, wep.weapon_socket,
                             wep.hit_effects);
        weapons.back().compute_weapon_damage(wep.buff.bonus_damage + starting_special_stats.bonus_damage);
        compute_hit_table(get_weapon_skill(starting_special_stats, wep.type, wep.weapon_socket), starting_special_stats,
                          wep.socket, wep.weapon_socket);
    }

    // TODO can move this to armory::compute_total_stats method
    if (character.is_dual_wield())
    {
        for (size_t i = 0; i < 2; i++)
        {
            size_t j = (i == 0) ? 1 : 0;
            for (const auto& hit_effect : character.weapons[i].hit_effects)
            {
                if (hit_effect.affects_both_weapons)
                {
                    auto new_hit_effect = hit_effect;
                    if (hit_effect.ppm != 0)
                    {
                        new_hit_effect.probability = hit_effect.ppm / (60.0 / weapons[j].swing_speed);
                    }
                    else
                    {
                        std::cout << "missing PPM for hit effect that affects both weapons!\n";
                    }
                    weapons[j].hit_effects.emplace_back(new_hit_effect);
                }
            }
        }
    }

    const bool is_dual_wield = character.is_dual_wield();
    const auto hit_effects_mh_copy = weapons[0].hit_effects;
    auto hit_effects_oh_copy = is_dual_wield ? weapons[1].hit_effects : std::vector<Hit_effect>{};

    double sim_time = config.sim_time;
    const double averaging_interval = 2.0; // Duration of the uniform interval that is evaluated
    sim_time -= averaging_interval;

    // Configure use effects
    std::vector<Use_effect> use_effects_all = use_effects_all_;
    for (const auto& use_effect : character.use_effects)
    {
        use_effects_all.push_back(use_effect);
    }

    if (config.enable_blood_fury)
    {
        // TODO need strength multiplier to make this more accurate
        double ap_boost =
            character.total_attributes.convert_to_special_stats(character.total_special_stats).attack_power * 0.25;
        use_effects_all.emplace_back(
            Use_effect{"Blood_fury", Use_effect::Effect_socket::unique, {}, {0, 0, ap_boost}, 0, 15, 120, true});
    }

    double ap_equiv{};
    if (is_dual_wield)
    {
        ap_equiv = get_character_ap_equivalent(starting_special_stats, character.weapons[0], character.weapons[1],
                                               sim_time, {});
    }
    else
    {
        ap_equiv = get_character_ap_equivalent(starting_special_stats, character.weapons[0], sim_time, {});
    }

    auto use_effect_order =
        compute_use_effect_order(use_effects_all, starting_special_stats, sim_time + averaging_interval / 2, ap_equiv,
                                 0, 0, config.combat.initial_rage);

    for (int iter = init_iteration; iter < n_damage_batches + init_iteration; iter++)
    {
        time_keeper_.reset(); // Class variable that keeps track of the time spent, cooldowns, iteration number
        ability_queue_manager.reset();
        slam_manager.reset();
        auto special_stats = starting_special_stats;
        Damage_sources damage_sources{};
        double rage = config.combat.initial_rage;

        // Reset hit effects
        weapons[0].hit_effects = hit_effects_mh_copy;
        if (is_dual_wield)
        {
            weapons[1].hit_effects = hit_effects_oh_copy;
            buff_manager_.initialize(special_stats, use_effect_order, weapons[0].hit_effects, weapons[1].hit_effects,
                                     tactical_mastery_rage_, config.performance_mode);
        }
        else
        {
            buff_manager_.initialize(special_stats, use_effect_order, weapons[0].hit_effects, hit_effects_oh_copy,
                                     tactical_mastery_rage_, config.performance_mode);
        }

        recompute_mitigation_ = true;
        current_armor_red_stacks_ = 0;
        armor_penetration_ = 0;

        int flurry_charges = 0;
        bool apply_delayed_armor_reduction = false;
        bool execute_phase = false;

        int mh_hits = 0;
        int mh_hits_w_flurry = 0;
        int oh_hits = 0;
        int oh_hits_w_flurry = 0;
        int oh_hits_w_heroic = 0;

        for (auto& wep : weapons)
        {
            wep.internal_swing_timer = 0.0;
        }

        // To avoid local max/min results from running a specific run time
        sim_time += averaging_interval / n_damage_batches;
        double time_execute_phase = sim_time * (100.0 - config.execute_phase_percentage_) / 100.0;
        init_server_time = 50.0 * iter / n_damage_batches;

        // Combat configuration
        if (!config.multi_target_mode_)
        {
            number_of_extra_targets_ = 0;
        }
        else
        {
            number_of_extra_targets_ = config.number_of_extra_targets;
        }

        // Check if the simulator should use any use effects before the fight
        if (!use_effect_order.empty() && use_effect_order.back().first < 0.0)
        {
            time_keeper_.time = use_effect_order.back().first;
            while (time_keeper_.time < 0.0)
            {
                double buff_dt = buff_manager_.get_dt(time_keeper_.time);
                double use_effect_dt = buff_manager_.use_effect_order.back().first - time_keeper_.time;
                if (use_effect_dt <= 0.0)
                {
                    use_effect_dt = 1e-5;
                    if (buff_manager_.use_effect_order.back().second.triggers_gcd)
                    {
                        use_effect_dt = (time_keeper_.global_cd > 0.0) ? time_keeper_.global_cd : use_effect_dt;
                    }
                }
                double dt =
                    time_keeper_.get_dynamic_time_step(100.0, 100.0, buff_dt, 0.0 - time_keeper_.time, use_effect_dt);
                time_keeper_.increment(dt);
                std::vector<std::string> debug_msg{};
                buff_manager_.increment(dt, time_keeper_.time, rage, rage_lost_stance_swap_, rage_lost_execute_batch_,
                                        time_keeper_.global_cd, config.display_combat_debug, debug_msg);
                for (const auto& msg : debug_msg)
                {
                    simulator_cout(msg);
                }
            }
        }

        for (const auto& over_time_effect : over_time_effects_)
        {
            buff_manager_.add_over_time_effect(over_time_effect, 0);
        }

        // First global sunder
        bool first_global_sunder = config.first_global_sunder_;

        if (config.combat.first_hit_heroic_strike && rage > heroic_strike_rage_cost)
        {
            ability_queue_manager.queue_heroic_strike();
        }

        while (time_keeper_.time < sim_time)
        {
            double mh_dt = weapons[0].internal_swing_timer;
            double oh_dt = is_dual_wield ? weapons[1].internal_swing_timer : 1000.0;
            double buff_dt = buff_manager_.get_dt(time_keeper_.time);
            double slam_dt = slam_manager.time_left(time_keeper_.time);
            double dt =
                time_keeper_.get_dynamic_time_step(mh_dt, oh_dt, buff_dt, sim_time - time_keeper_.time, slam_dt);
            time_keeper_.increment(dt);
            std::vector<std::string> debug_msg{};
            buff_manager_.increment(dt, time_keeper_.time, rage, rage_lost_stance_swap_, rage_lost_execute_batch_,
                                    time_keeper_.global_cd, config.display_combat_debug, debug_msg);
            for (const auto& msg : debug_msg)
            {
                simulator_cout(msg);
            }

            if (sim_time - time_keeper_.time < 0.0)
            {
                break;
            }

            if (buff_manager_.need_to_recompute_hittables)
            {
                for (const auto& weapon : weapons)
                {
                    compute_hit_table(get_weapon_skill(special_stats, weapon.weapon_type, weapon.weapon_socket),
                                      special_stats, weapon.socket, weapon.weapon_socket);
                }
                buff_manager_.need_to_recompute_hittables = false;
            }

            if (buff_manager_.reset_armor_reduction)
            {
                recompute_mitigation_ = true;
                current_armor_red_stacks_ = 0;
                armor_penetration_ = 0;
                buff_manager_.reset_armor_reduction = false;
            }

            if (!apply_delayed_armor_reduction && time_keeper_.time > 6.0 && config.exposed_armor)
            {
                apply_delayed_armor_reduction = true;
                recompute_mitigation_ = true;
                simulator_cout("Applying improved exposed armor!");
            }

            if (recompute_mitigation_)
            {
                int target_armor =
                    config.main_target_initial_armor_ - armor_reduction_from_spells_ - armor_penetration_;
                if (apply_delayed_armor_reduction)
                {
                    target_armor -= armor_reduction_delayed_;
                }
                target_armor = std::max(target_armor, 0);
                armor_reduction_factor_ = 1 - armor_mitigation(target_armor, config.main_target_level);
                simulator_cout("Target armor: ", target_armor, ". Mitigation factor: ", 1 - armor_reduction_factor_,
                               "%.");
                if (config.multi_target_mode_)
                {
                    int extra_target_armor = config.extra_target_initial_armor_ - armor_penetration_;
                    extra_target_armor = std::max(extra_target_armor, 0);
                    armor_reduction_factor_add = 1 - armor_mitigation(extra_target_armor, config.extra_target_level);

                    simulator_cout("Extra targets armor: ", extra_target_armor,
                                   ". Mitigation factor: ", 1 - armor_reduction_factor_add, "%.");
                }
                recompute_mitigation_ = false;
            }

            if (config.multi_target_mode_ && number_of_extra_targets_ > 0 &&
                time_keeper_.time / sim_time > config.extra_target_duration)
            {
                simulator_cout("Extra targets die.");
                number_of_extra_targets_ = 0;
            }

            bool mh_swing = weapons[0].time_for_swing(dt);
            bool oh_swing = is_dual_wield ? weapons[1].time_for_swing(dt) : false;

            if (mh_swing)
            {
                mh_hits++;
                if (flurry_charges > 0)
                {
                    mh_hits_w_flurry++;
                }
                swing_weapon(weapons[0], weapons[0], special_stats, rage, damage_sources, flurry_charges);
            }

            if (oh_swing)
            {
                oh_hits++;
                if (flurry_charges > 0)
                {
                    oh_hits_w_flurry++;
                }
                if (ability_queue_manager.heroic_strike_queued || ability_queue_manager.cleave_queued)
                {
                    oh_hits_w_heroic++;
                }
                swing_weapon(weapons[1], weapons[0], special_stats, rage, damage_sources, flurry_charges);
            }

            if (!execute_phase)
            {
                if (time_keeper_.time > time_execute_phase)
                {
                    simulator_cout("------------ Execute phase! ------------");
                    execute_phase = true;
                }
            }

            if (use_sweeping_strikes_)
            {
                if (rage > 30.0 && time_keeper_.global_cd < 0.0 && time_keeper_.sweeping_strikes_cd < 0.0)
                {
                    simulator_cout("Changed stance: Battle Stance.");
                    simulator_cout("Sweeping strkes!");
                    buff_manager_.add("battle_stance", {-3.0, 0, 0}, 1.5);
                    sweeping_strikes_charges_ = 5;
                    time_keeper_.sweeping_strikes_cd = 30;
                    time_keeper_.global_cd = 1.5;
                }
            }

            if (first_global_sunder)
            {
                if (time_keeper_.global_cd < 0 && rage > 15)
                {
                    simulator_cout("Sunder Armor!");
                    time_keeper_.global_cd = 1.5;
                    rage -= 15;
                    first_global_sunder = false;
                }
            }
            else if (execute_phase)
            {
                if (rage > heroic_strike_rage_cost && !ability_queue_manager.heroic_strike_queued &&
                    config.combat.use_hs_in_exec_phase)
                {
                    if (config.combat.use_heroic_strike)
                    {
                        if (config.dpr_settings.compute_dpr_hs_)
                        {
                            if (mh_swing)
                            {
                                rage -= config.combat.heroic_strike_rage_thresh;
                            }
                        }
                        else
                        {
                            ability_queue_manager.queue_heroic_strike();
                            simulator_cout("Heroic strike activated");
                        }
                    }
                }
                if (use_bloodthirst_ && config.combat.use_bt_in_exec_phase)
                {
                    if (time_keeper_.blood_thirst_cd < 0.0 && time_keeper_.global_cd < 0 && rage > 30)
                    {
                        bloodthirst(weapons[0], special_stats, rage, damage_sources, flurry_charges);
                    }
                }
                if (use_mortal_strike_ && config.combat.use_ms_in_exec_phase)
                {
                    if (time_keeper_.mortal_strike_cd < 0.0 && time_keeper_.global_cd < 0 && rage > 30)
                    {
                        mortal_strike(weapons[0], special_stats, rage, damage_sources, flurry_charges);
                    }
                }
                if (time_keeper_.global_cd < 0 && rage > execute_rage_cost_)
                {
                    execute(weapons[0], special_stats, rage, damage_sources, flurry_charges);
                }
            }
            else
            {
                if (weapons[0].weapon_socket == Weapon_socket::two_hand && config.combat.use_slam)
                {
                    if (!slam_manager.is_slam_queued() && time_keeper_.global_cd < 0.0)
                    {
                        if (start_cast_slam(mh_swing, rage, weapons[0].internal_swing_timer))
                        {
                            continue;
                        }
                    }
                    else if (slam_manager.time_left(time_keeper_.time) <= 0.0)
                    {
                        slam(weapons[0], special_stats, rage, damage_sources, flurry_charges);
                        slam_manager.un_queue_slam();
                        weapons[0].internal_swing_timer = weapons[0].swing_speed / (1 + special_stats.haste);
                        if (time_keeper_.global_cd < 0.0)
                        {
                            if (start_cast_slam(mh_swing, rage, weapons[0].internal_swing_timer))
                            {
                                continue;
                            }
                        }
                    }
                    else
                    {
                        continue;
                    }
                }

                if (use_bloodthirst_)
                {
                    if (time_keeper_.blood_thirst_cd < 0.0 && time_keeper_.global_cd < 0 && rage > 30)
                    {
                        bloodthirst(weapons[0], special_stats, rage, damage_sources, flurry_charges);
                    }
                }

                if (use_mortal_strike_)
                {
                    if (time_keeper_.mortal_strike_cd < 0.0 && time_keeper_.global_cd < 0 && rage > 30)
                    {
                        mortal_strike(weapons[0], special_stats, rage, damage_sources, flurry_charges);
                    }
                }

                if (config.combat.use_whirlwind)
                {
                    bool use_ww = true;
                    if (use_bloodthirst_)
                    {
                        use_ww = time_keeper_.blood_thirst_cd > config.combat.whirlwind_bt_cooldown_thresh;
                    }
                    if (use_mortal_strike_)
                    {
                        use_ww &= time_keeper_.mortal_strike_cd > config.combat.whirlwind_bt_cooldown_thresh;
                    }
                    if (time_keeper_.whirlwind_cd < 0.0 && rage > config.combat.whirlwind_rage_thresh && rage > 25 &&
                        time_keeper_.global_cd < 0 && use_ww)
                    {
                        whirlwind(weapons[0], special_stats, rage, damage_sources, flurry_charges);
                    }
                }

                if (config.combat.use_overpower)
                {
                    bool use_op = true;
                    if (use_bloodthirst_)
                    {
                        use_op &= time_keeper_.blood_thirst_cd > config.combat.overpower_bt_cooldown_thresh;
                    }
                    if (use_mortal_strike_)
                    {
                        use_op &= time_keeper_.mortal_strike_cd > config.combat.overpower_bt_cooldown_thresh;
                    }
                    if (config.combat.use_whirlwind)
                    {
                        use_op &= time_keeper_.whirlwind_cd > config.combat.overpower_ww_cooldown_thresh;
                    }
                    if (time_keeper_.overpower_cd < 0.0 && rage < config.combat.overpower_rage_thresh && rage > 5 &&
                        time_keeper_.global_cd < 0 && buff_manager_.can_do_overpower() && use_op)
                    {
                        overpower(weapons[0], special_stats, rage, damage_sources, flurry_charges);
                    }
                }

                if (config.combat.use_hamstring)
                {
                    bool use_ham = true;
                    if (config.combat.dont_use_hm_when_ss && sweeping_strikes_charges_ > 0)
                    {
                        use_ham = false;
                    }
                    if (use_bloodthirst_)
                    {
                        use_ham &= time_keeper_.blood_thirst_cd > config.combat.hamstring_cd_thresh;
                    }
                    if (use_mortal_strike_)
                    {
                        use_ham &= time_keeper_.mortal_strike_cd > config.combat.hamstring_cd_thresh;
                    }
                    if (config.combat.use_whirlwind)
                    {
                        use_ham &= time_keeper_.whirlwind_cd > config.combat.hamstring_cd_thresh;
                    }
                    if (rage > config.combat.hamstring_thresh_dd && time_keeper_.global_cd < 0 && use_ham)
                    {
                        hamstring(weapons[0], special_stats, rage, damage_sources, flurry_charges);
                    }
                }

                // Heroic strike or Cleave
                // TODO move to the top of sim loop, enables 'continue' after casting spells for performance
                if (number_of_extra_targets_ > 0 && config.combat.cleave_if_adds)
                {
                    if (rage > config.combat.cleave_rage_thresh && !ability_queue_manager.cleave_queued)
                    {
                        if (config.dpr_settings.compute_dpr_cl_)
                        {
                            dpr_cleave_queued_ = true;
                        }
                        else
                        {
                            ability_queue_manager.queue_cleave();
                            simulator_cout("Cleave activated");
                        }
                    }
                }
                else
                {
                    if (rage > config.combat.heroic_strike_rage_thresh && !ability_queue_manager.heroic_strike_queued &&
                        rage > heroic_strike_rage_cost)
                    {
                        if (config.combat.use_heroic_strike)
                        {
                            if (config.dpr_settings.compute_dpr_hs_)
                            {
                                dpr_heroic_strike_queued_ = true;
                            }
                            else
                            {
                                ability_queue_manager.queue_heroic_strike();
                                simulator_cout("Heroic strike activated");
                            }
                        }
                    }
                }
            }
        }
        if (deep_wounds_)
        {
            double dw_average_damage = buff_manager_.deep_wounds_damage / buff_manager_.deep_wounds_timestamps.size();
            for (double deep_wounds_timestamp : buff_manager_.deep_wounds_timestamps)
            {
                damage_sources.add_damage(Damage_source::deep_wounds, dw_average_damage, deep_wounds_timestamp);
            }
        }
        double new_sample = damage_sources.sum_damage_sources() / sim_time;
        dps_distribution_.add_sample(new_sample);
        damage_distribution_ = damage_distribution_ + damage_sources;
        flurry_uptime_mh_ = Statistics::update_mean(flurry_uptime_mh_, iter + 1, double(mh_hits_w_flurry) / mh_hits);
        if (weapons.size() > 1)
        {
            flurry_uptime_oh_ =
                Statistics::update_mean(flurry_uptime_oh_, iter + 1, double(oh_hits_w_flurry) / oh_hits);
            heroic_strike_uptime_ =
                Statistics::update_mean(heroic_strike_uptime_, iter + 1, double(oh_hits_w_heroic) / oh_hits);
        }
        avg_rage_spent_executing_ =
            Statistics::update_mean(avg_rage_spent_executing_, iter + 1, buff_manager_.rage_spent_executing);
        if (log_data)
        {
            add_damage_source_to_time_lapse(damage_sources.damage_instances);
            hist_y[new_sample / 20]++;
        }
    }
    if (log_data)
    {
        normalize_timelapse();
        prune_histogram();
    }
}

std::vector<std::pair<double, Use_effect>> Combat_simulator::get_use_effect_order(const Character& character)
{
    // Copy paste from the simulate function
    std::vector<Use_effect> use_effects_all = use_effects_all_;
    for (const auto& use_effect : character.use_effects)
    {
        use_effects_all.push_back(use_effect);
    }
    if (config.enable_blood_fury)
    {
        double ap_boost =
            character.total_attributes.convert_to_special_stats(character.total_special_stats).attack_power * 0.25;
        use_effects_all.emplace_back(
            Use_effect{"Blood_fury", Use_effect::Effect_socket::unique, {}, {0, 0, ap_boost}, 0, 15, 120, true});
    }
    double ap_equiv{};
    if (character.is_dual_wield())
    {
        ap_equiv = get_character_ap_equivalent(character.total_special_stats, character.weapons[0],
                                               character.weapons[1], config.sim_time, {});
    }
    else
    {
        ap_equiv =
            get_character_ap_equivalent(character.total_special_stats, character.weapons[0], config.sim_time, {});
    }
    return compute_use_effect_order(use_effects_all, character.total_special_stats, config.sim_time, ap_equiv, 0, 0,
                                    config.combat.initial_rage);
}

void Combat_simulator::init_histogram()
{
    double res = 20;
    for (int i = 0; i < 1000; i++)
    {
        hist_x.push_back(i * res);
        hist_y.push_back(0);
    }
}

void Combat_simulator::normalize_timelapse()
{
    for (auto& damage_time_lapse_i : damage_time_lapse)
    {
        for (auto& singe_damage_instance : damage_time_lapse_i)
        {
            singe_damage_instance /= config.n_batches;
        }
    }
}

void Combat_simulator::prune_histogram()
{
    int start_idx{};
    int end_idx{};
    for (size_t i = 0; i < hist_x.size(); i++)
    {
        if (hist_y[i] != 0 && !start_idx)
        {
            start_idx = i;
            break;
        }
    }
    for (size_t i = hist_x.size() - 1; i > 0; i--)
    {
        if (hist_y[i] != 0 && !end_idx)
        {
            end_idx = i;
            break;
        }
    }
    {
        auto first = hist_x.begin() + start_idx;
        auto last = hist_x.begin() + end_idx + 1;
        hist_x = std::vector<int>(first, last);
    }
    {
        auto first = hist_y.begin() + start_idx;
        auto last = hist_y.begin() + end_idx + 1;
        hist_y = std::vector<int>(first, last);
    }
}

const std::vector<double>& Combat_simulator::get_hit_probabilities_white_mh() const
{
    return hit_table_white_mh_;
}

const std::vector<double>& Combat_simulator::get_hit_probabilities_white_oh() const
{
    return hit_table_white_oh_;
}

const std::vector<double>& Combat_simulator::get_hit_probabilities_white_2h() const
{
    return hit_table_two_hand_;
}

const std::vector<double>& Combat_simulator::get_hit_probabilities_yellow() const
{
    return hit_table_yellow_;
}

double Combat_simulator::get_glancing_penalty_mh() const
{
    return damage_multipliers_white_mh_[2];
}

double Combat_simulator::get_glancing_penalty_oh() const
{
    return damage_multipliers_white_oh_[2];
}

std::vector<std::string> Combat_simulator::get_aura_uptimes() const
{
    std::vector<std::string> aura_uptimes;
    double total_sim_time = config.n_batches * (config.sim_time - 1);
    for (const auto& aura : buff_manager_.aura_uptime)
    {
        double uptime = aura.second / total_sim_time;
        aura_uptimes.emplace_back(aura.first + " " + std::to_string(100 * uptime));
    }
    if (flurry_uptime_mh_ != 0.0)
    {
        aura_uptimes.emplace_back("Flurry_main_hand " + std::to_string(100 * flurry_uptime_mh_));
    }
    if (flurry_uptime_oh_ != 0.0)
    {
        aura_uptimes.emplace_back("Flurry_off_hand " + std::to_string(100 * flurry_uptime_oh_));
    }
    if (heroic_strike_uptime_ != 0.0)
    {
        aura_uptimes.emplace_back("'Heroic_strike_bug' " + std::to_string(100 * heroic_strike_uptime_));
    }
    return aura_uptimes;
}

std::vector<std::string> Combat_simulator::get_proc_statistics() const
{
    std::vector<std::string> proc_counter;
    for (const auto& proc : proc_data_)
    {
        double counter = static_cast<double>(proc.second) / config.n_batches;
        proc_counter.emplace_back(proc.first + " " + std::to_string(counter));
    }
    return proc_counter;
}

void Combat_simulator::reset_time_lapse()
{
    double resolution = .50;
    std::vector<double> history;
    history.reserve(config.sim_time / resolution);
    for (double t = 0; t < config.sim_time; t += resolution)
    {
        history.push_back(0);
    }
    history.push_back(0); // This extra bin might be used for last second dot effects
    if (!(damage_time_lapse.empty()))
    {
        damage_time_lapse.clear();
    }
    for (size_t i = 0; i < source_map.size(); i++)
    {
        damage_time_lapse.push_back(history);
    }
}

void Combat_simulator::add_damage_source_to_time_lapse(std::vector<Damage_instance>& damage_instances)
{
    double resolution = .50;
    for (const auto& damage_instance : damage_instances)
    {
        size_t first_idx = source_map.at(damage_instance.damage_source);
        size_t second_idx = damage_instance.time_stamp / resolution; // automatically floored
        damage_time_lapse[first_idx][second_idx] += damage_instance.damage;
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
