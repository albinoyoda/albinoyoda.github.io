#include "../include/Combat_simulator.hpp"

#include <algorithm>

namespace
{
constexpr double rage_factor = 15.0 / 230.6 / 2.0;

// constexpr double rage_from_damage_taken(double damage)
//{
//    return damage * 5 / 2 / 230.6;
//}

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

std::vector<double> create_hit_table_yellow(double miss, double dodge, double crit)
{
    double double_roll_factor = (100 - miss - dodge) / 100;
    // Order -> Miss, parry, dodge, block, glancing, crit, hit.
    // double_roll_factor compensates for the crit suppression caused by ability double roll
    return {miss, miss + dodge, miss + dodge, miss + dodge + double_roll_factor * crit};
}

std::vector<double> create_multipliers(double glancing_factor, double bonus_crit_multiplier)
{
    // Order -> Miss, parry, dodge, block, glancing, crit, hit.
    return {0.0, 0.0, glancing_factor, 2.0 + bonus_crit_multiplier, 1.0};
}
} // namespace

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
        simulator_cout("Drawing outcome from OH twohanded hit table");
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
                                                             const Special_stats& special_stats, bool boss_target,
                                                             bool is_overpower)
{
    Combat_simulator::Hit_outcome hit_outcome;
    if (weapon_hand == Socket::main_hand)
    {
        hit_outcome = generate_hit_mh(damage, hit_type, is_overpower);
        if (boss_target)
        {
            hit_outcome.damage *= armor_reduction_factor_ * (1 + special_stats.damage_multiplier);
        }
        else
        {
            hit_outcome.damage *= armor_reduction_factor_add * (1 + special_stats.damage_multiplier);
        }
        cout_damage_parse(hit_type, weapon_hand, hit_outcome);
    }
    else
    {
        hit_outcome = generate_hit_oh(damage);
        if (boss_target)
        {
            hit_outcome.damage *= armor_reduction_factor_ * (1 + special_stats.damage_multiplier);
        }
        else
        {
            hit_outcome.damage *= armor_reduction_factor_add * (1 + special_stats.damage_multiplier);
        }
        cout_damage_parse(hit_type, weapon_hand, hit_outcome);
    }
    if (config.combat.deep_wounds)
    {
        if (hit_outcome.hit_result == Combat_simulator::Hit_result::crit)
        {
            buff_manager_.add_over_time_effect(
                {"Deep_wounds",
                 {},
                 0,
                 (1 + special_stats.damage_multiplier) * weapon.swing(special_stats.attack_power) / 4,
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

void Combat_simulator::compute_hit_table(int level_difference, int weapon_skill, Special_stats special_stats,
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
    double dw_miss_chance = (base_miss_chance * 0.8 + 20.0);
    double corrected_hit = special_stats.hit - hit_penalty;
    double miss_chance = dw_miss_chance - std::max(corrected_hit, 0.0);
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
        damage_multipliers_white_mh_ = create_multipliers((100.0 - glancing_penalty) / 100.0, 0.0);

        hit_table_yellow_ = create_hit_table_yellow(two_hand_miss_chance, dodge_chance, crit_chance);
        hit_table_overpower_ =
            create_hit_table_yellow(two_hand_miss_chance, 0, crit_chance + 25 * config.talents.overpower - 3.0);
        damage_multipliers_yellow_ = create_multipliers(1.0, 0.1 * config.talents.impale);
    }
    else
    {
        hit_table_white_oh_ = create_hit_table(miss_chance, dodge_chance, glancing_chance, crit_chance);
        damage_multipliers_white_oh_ = create_multipliers((100.0 - glancing_penalty) / 100.0, 0.0);

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
                special_stats += {0, 0, 0, 0, 0.05 + 0.05 * config.talents.flurry};
            }
        }
        else if (flurry_active && flurry_charges == 0)
        {
            special_stats -= {0, 0, 0, 0, 0.05 + 0.05 * config.talents.flurry};
        }
        simulator_cout(flurry_charges, " flurry charges");
        assert(special_stats.haste > -.1);
        assert(special_stats.haste < 1.2);
    }
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
    auto hit_outcome = generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats);
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
        rage > 25 ? rage = 20 : rage -= 5;
        buff_manager_.add("battle_stance", {-3.0, 0, 0}, 1.5);
        time_keeper_.overpower_cd = 5.0;
        time_keeper_.global_cd = 1.5;
        return;
    }
    simulator_cout("Changed stance: Battle Stance.");
    simulator_cout("Overpower!");
    buff_manager_.add("battle_stance", {-3.0, 0, 0}, 1.5);
    double damage = main_hand_weapon.normalized_swing(special_stats.attack_power) + 35;
    auto hit_outcome =
        generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats, true, true);
    if (rage > 25)
    {
        rage_lost_stance_swap_ += rage - 25;
        rage = 25;
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
    simulator_cout("Whirlwind! #targets = boss + ", adds_in_melee_range, " adds");
    simulator_cout("Whirlwind hits: ", std::min(adds_in_melee_range + 1, 4), " targets");
    double damage = main_hand_weapon.normalized_swing(special_stats.attack_power);
    std::vector<Hit_outcome> hit_outcomes{};
    hit_outcomes.reserve(4);
    for (int i = 0; i < std::min(adds_in_melee_range + 1, 4); i++)
    {
        hit_outcomes.emplace_back(
            generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats, i == 0));
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
                               Damage_sources& damage_sources, int& flurry_charges, double execute_rage_cost)
{
    if (config.dpr_settings.compute_dpr_ex_)
    {
        get_uniform_random(100) < hit_table_yellow_[1] ? rage *= 0.85 : rage -= 30;
        double next_server_batch = std::fmod(time_keeper_.time, 0.4);
        buff_manager_.add("execute_rage_batch", {}, 0.4 + next_server_batch);
        time_keeper_.global_cd = 1.5;
        return;
    }
    simulator_cout("Execute!");
    double damage = 600 + (rage - execute_rage_cost) * 15;
    auto hit_outcome = generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats);
    if (hit_outcome.hit_result == Hit_result::dodge || hit_outcome.hit_result == Hit_result::miss)
    {
        rage *= 0.85;
    }
    else
    {
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges);
    }
    double next_server_batch = std::fmod(time_keeper_.time, 0.4);
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
    auto hit_outcome = generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats);
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
            if (hit_effect.type != Hit_effect::Type::damage_magic_guaranteed)
            {
                buff_manager_.increment_proc(hit_effect.name);
            }
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
            case Hit_effect::Type::damage_magic:
                damage_sources.add_damage(Damage_source::item_hit_effects, hit_effect.damage * 0.83 * 1.1,
                                          time_keeper_.time);
                simulator_cout("PROC: ", hit_effect.name, " does ", hit_effect.damage * 0.83 * 1.1, " magic damage.");
                break;
            case Hit_effect::Type::damage_magic_guaranteed:
                simulator_cout("Weapon swing with: ", hit_effect.name, " does ", hit_effect.damage * 0.83,
                               " magic damage.");
                damage_sources.add_damage(Damage_source::item_hit_effects, hit_effect.damage * 0.83, time_keeper_.time,
                                          false);
                break;
            case Hit_effect::Type::damage_physical:
            {
                auto hit = generate_hit(main_hand_weapon, hit_effect.damage, Hit_type::yellow, Socket::main_hand,
                                        special_stats);
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
                    simulator_cout("PROC: ", hit_effect.name, result, " does ", int(hit.damage), " physical damage");
                }
            }
            break;
            case Hit_effect::Type::stat_boost:
                simulator_cout("PROC: ", hit_effect.name, " stats increased for ", hit_effect.duration, "s");
                buff_manager_.add(weapon.socket_name + "_" + hit_effect.name,
                                  hit_effect.get_special_stat_equivalent(special_stats), hit_effect.duration);
                break;
            case Hit_effect::Type::reduce_armor:
            {
                if (current_armor_red_stacks_ < hit_effect.max_stacks)
                {
                    target_armor_ -= hit_effect.armor_reduction;
                    target_armor_ = std::max(target_armor_, 0.0);
                    current_armor_red_stacks_++;
                    double target_mitigation = armor_mitigation(target_armor_, 63);
                    armor_reduction_factor_ = 1 - target_mitigation;
                    simulator_cout("PROC: ", hit_effect.name, " armor reduced by ", int(hit_effect.armor_reduction),
                                   ". Target armor: ", int(target_armor_),
                                   ". New mitigation factor: ", target_mitigation,
                                   "%. Current stacks: ", int(current_armor_red_stacks_));
                }
                else
                {
                    simulator_cout("PROC: ", hit_effect.name,
                                   ". Cant add more stacks. Current stacks: ", int(current_armor_red_stacks_));
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
        swing_damage *= (0.5 + 0.025 * config.talents.dual_wield_specialization);
    }
    weapon.internal_swing_timer = weapon.swing_speed / (1 + special_stats.haste);

    // Check if heroic strike should be performed
    if (ability_queue_manager.heroic_strike_queued && weapon.socket == Socket::main_hand &&
        rage >= heroic_strike_rage_cost)
    {
        simulator_cout("Performing heroic strike");
        swing_damage += config.combat.heroic_strike_damage;
        hit_outcomes.emplace_back(
            generate_hit(main_hand_weapon, swing_damage, Hit_type::yellow, weapon.socket, special_stats));
        ability_queue_manager.heroic_strike_queued = false;
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
        simulator_cout("Performing cleave! #targets = boss + ", adds_in_melee_range, " adds");
        simulator_cout("Cleave hits: ", std::min(adds_in_melee_range + 1, 2), " targets");
        swing_damage += 50; // TODO talents

        for (int i = 0; i < std::min(adds_in_melee_range + 1, 2); i++)
        {
            hit_outcomes.emplace_back(
                generate_hit(main_hand_weapon, swing_damage, Hit_type::yellow, weapon.socket, special_stats, i == 0));
        }
        ability_queue_manager.cleave_queued = false;
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
        hit_outcomes.emplace_back(
            generate_hit(main_hand_weapon, swing_damage, Hit_type::white, weapon.socket, special_stats));
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
            simulator_cout("Rage gained from enemy dodging");
            rage += rage_generation(swing_damage * armor_reduction_factor_);
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
    dps_mean_ = init_mean;
    dps_variance_ = init_variance;
    config.n_batches = n_simulations;
    simulate(character, init_simulations, false, false);
}

void Combat_simulator::simulate(const Character& character, size_t n_simulations)
{
    config.n_batches = n_simulations;
    simulate(character, 0, false, false);
}

void Combat_simulator::simulate(const Character& character, int init_iteration, bool compute_time_lapse,
                                bool compute_histogram)
{
    int n_damage_batches = config.n_batches;
    if (config.display_combat_debug)
    {
        debug_topic_ = "";
        n_damage_batches = 1;
    }
    if (compute_time_lapse)
    {
        reset_time_lapse();
    }
    if (compute_histogram)
    {
        init_histogram();
    }
    buff_manager_.aura_uptime.auras.clear();
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
        compute_hit_table(config.opponent_level - character.level,
                          get_weapon_skill(character.total_special_stats, wep.type), starting_special_stats,
                          wep.socket);
    }

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
    auto hit_effects_mh = weapons[0].hit_effects;
    auto hit_effects_oh = weapons[1].hit_effects;

    heroic_strike_rage_cost = 15.0 - config.talents.improved_heroic_strike;
    p_unbridled_wrath_ = config.talents.unbridled_wrath * 0.08;
    double execute_rage_cost = 15 - static_cast<int>(2.51 * config.talents.improved_execute);

    double armor_reduction_from_spells = 0.0;
    double armor_reduction_delayed = 0.0;
    armor_reduction_from_spells += 450 * config.n_sunder_armor_stacks;
    armor_reduction_from_spells += 640 * config.curse_of_recklessness_active;
    armor_reduction_from_spells += 505 * config.faerie_fire_feral_active;
    if (config.exposed_armor)
    {
        armor_reduction_delayed = 1700 * 1.5 - 450 * config.n_sunder_armor_stacks;
    }

    double sim_time = config.sim_time;
    if (config.use_sim_time_ramp)
    {
        sim_time -= 2.0;
    }

    // Pick out the best use effect if there are several that shares cooldown
    std::vector<Use_effect> use_effects_all = character.use_effects;
    std::vector<Over_time_effect> over_time_effects{};
    std::vector<size_t> shared_items;
    size_t best_idx = 0;
    double best_value = 0;

    if (config.talents.death_wish)
    {
        use_effects_all.emplace_back(deathwish);
    }

    if (config.enable_recklessness)
    {
        use_effects_all.emplace_back(recklessness);
    }

    if (config.enable_bloodrage)
    {
        use_effects_all.emplace_back(bloodrage);
    }

    if (config.enable_blood_fury)
    {
        // TODO need strength multiplier to make this more accurate
        double ap_boost =
            character.total_attributes.convert_to_special_stats(character.total_special_stats).attack_power * 0.25;
        use_effects_all.emplace_back(
            Use_effect{"Blood_fury", Use_effect::Effect_socket::unique, {}, {0, 0, ap_boost}, 0, 15, 120, true, {}});
    }

    if (config.mode.vaelastrasz)
    {
        over_time_effects.push_back(essence_of_the_red);
    }

    if (config.talents.anger_management)
    {
        over_time_effects.push_back(anger_management);
    }

    for (size_t i = 0; i < use_effects_all.size(); i++)
    {
        if (use_effects_all[i].effect_socket == Use_effect::Effect_socket::shared)
        {
            shared_items.push_back(i);
            double value = use_effects_all[i].get_special_stat_equivalent(starting_special_stats).attack_power *
                           std::min(use_effects_all[i].duration, config.sim_time);
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
        if (!(std::find(shared_items.begin(), shared_items.end(), i) != shared_items.end())) // Add if not shared
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

    for (int iter = init_iteration; iter < n_damage_batches + init_iteration; iter++)
    {
        time_keeper_.reset(); // Class variable that keeps track of the time spent, cooldowns, iteration number
        ability_queue_manager.reset();
        auto special_stats = starting_special_stats;
        Damage_sources damage_sources{};
        double rage = config.combat.initial_rage;

        // Reset hit effects
        weapons[0].hit_effects = hit_effects_mh;
        weapons[1].hit_effects = hit_effects_oh;
        buff_manager_.initialize(special_stats, use_effects, weapons[0].hit_effects, weapons[1].hit_effects,
                                 config.performance_mode);

        for (const auto& over_time_effect : over_time_effects)
        {
            buff_manager_.add_over_time_effect(over_time_effect, 0.0);
        }

        // Reset, since these might change
        target_armor_ = 3731 - armor_reduction_from_spells; // Armor for Warrior class monsters
        double target_mitigation = armor_mitigation(target_armor_, 63);
        double add_mitigation = armor_mitigation(3000, 62);
        armor_reduction_factor_ = 1 - target_mitigation;
        armor_reduction_factor_add = 1 - add_mitigation;
        current_armor_red_stacks_ = 0;

        int flurry_charges = 0;
        bool apply_delayed_armor_reduction = true;
        bool execute_phase = false;
        //        int fuel_ticks = 0;

        double mh_hits = 0;
        double mh_hits_w_flurry = 0;
        double oh_hits = 0;
        double oh_hits_w_flurry = 0;
        double oh_hits_w_heroic = 0;

        for (auto& wep : weapons)
        {
            wep.internal_swing_timer = 0.0;
        }

        // To avoid local max/min results from running a specific run time
        if (config.use_sim_time_ramp)
        {
            sim_time += 2.0 / n_damage_batches;
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
            buff_manager_.add("sulfuron_demo_shout", {0, 0, -300}, 300);
        }

        while (time_keeper_.time < sim_time)
        {
            double mh_dt = weapons[0].internal_swing_timer;
            double oh_dt = (weapons.size() == 2) ? weapons[1].internal_swing_timer : 100.0;
            double buff_dt = buff_manager_.get_dt(time_keeper_.time);
            double dt = time_keeper_.get_dynamic_time_step(mh_dt, oh_dt, buff_dt, sim_time);
            time_keeper_.increment(dt);
            std::vector<std::string> debug_msg;
            buff_manager_.increment(dt, time_keeper_.time, sim_time - time_keeper_.time, rage, rage_lost_stance_swap_,
                                    rage_lost_execute_batch_, time_keeper_.global_cd, debug_msg,
                                    config.display_combat_debug);
            for (const auto& msg : debug_msg)
            {
                simulator_cout(msg);
            }

            if (buff_manager_.need_to_recompute_hittables)
            {
                for (const auto& weapon : weapons)
                {
                    compute_hit_table(config.opponent_level - character.level,
                                      get_weapon_skill(character.total_special_stats, weapon.weapon_type),
                                      special_stats, weapon.socket);
                }
                buff_manager_.need_to_recompute_hittables = false;
            }

            if (sim_time - time_keeper_.time < 0.0)
            {
                break;
            }

            if (time_keeper_.time > 6 && armor_reduction_delayed > 0 && apply_delayed_armor_reduction)
            {
                target_armor_ -= armor_reduction_delayed; // Armor for Warrior class monsters
                target_armor_ = std::max(target_armor_, 0.0);
                target_mitigation = armor_mitigation(target_armor_, 63);
                simulator_cout("Improved expose armor applied. Target armor: ", int(target_armor_),
                               ". New mitigation factor: ", target_mitigation, "%.");
                armor_reduction_factor_ = 1 - target_mitigation;
                apply_delayed_armor_reduction = false;
            }

            if (time_keeper_.time > remove_adds_timer * (1 + removed_adds))
            {
                if (adds_in_melee_range > 0)
                {
                    removed_adds++;
                    adds_in_melee_range--;
                    simulator_cout("Add #", removed_adds, " dies. Targets left: boss + ", adds_in_melee_range, "adds");
                }
            }

            bool mh_swing = weapons[0].time_for_swing(dt);
            bool oh_swing = (weapons.size() == 2) ? weapons[1].time_for_swing(dt) : false;

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
                if (config.combat.use_bt_in_exec_phase)
                {
                    if (time_keeper_.blood_thirst_cd < 0.0 && time_keeper_.global_cd < 0 && rage > 30)
                    {
                        bloodthirst(weapons[0], special_stats, rage, damage_sources, flurry_charges);
                    }
                }
                if (time_keeper_.global_cd < 0 && rage > execute_rage_cost)
                {
                    execute(weapons[0], special_stats, rage, damage_sources, flurry_charges, execute_rage_cost);
                }
            }
            else
            {
                if (config.combat.use_bloodthirst)
                {
                    if (time_keeper_.blood_thirst_cd < 0.0 && time_keeper_.global_cd < 0 && rage > 30)
                    {
                        bloodthirst(weapons[0], special_stats, rage, damage_sources, flurry_charges);
                    }
                }

                if (config.combat.use_whirlwind)
                {
                    bool use_ww = true;
                    if (config.combat.use_bloodthirst)
                    {
                        use_ww = time_keeper_.blood_thirst_cd > config.combat.whirlwind_bt_cooldown_thresh;
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
                    if (config.combat.use_bloodthirst)
                    {
                        use_op &= time_keeper_.blood_thirst_cd > config.combat.overpower_bt_cooldown_thresh;
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
                    if (config.combat.use_bloodthirst)
                    {
                        use_ham &= time_keeper_.blood_thirst_cd > config.combat.hamstring_cd_thresh;
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

                if (adds_in_melee_range > 0 && config.combat.cleave_if_adds)
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
        if (config.combat.deep_wounds)
        {
            double dw_average_damage = buff_manager_.deep_wounds_damage / buff_manager_.deep_wounds_timestamps.size();
            for (double deep_wounds_timestamp : buff_manager_.deep_wounds_timestamps)
            {
                damage_sources.add_damage(Damage_source::deep_wounds, dw_average_damage, deep_wounds_timestamp);
            }
        }
        double new_sample = damage_sources.sum_damage_sources() / sim_time;
        dps_mean_ = Statistics::update_mean(dps_mean_, iter + 1, new_sample);
        dps_variance_ = Statistics::update_variance(dps_variance_, dps_mean_, iter + 1, new_sample);
        damage_distribution_ = damage_distribution_ + damage_sources;
        flurry_uptime_mh_ = Statistics::update_mean(flurry_uptime_mh_, iter + 1, mh_hits_w_flurry / mh_hits);
        flurry_uptime_oh_ = Statistics::update_mean(flurry_uptime_oh_, iter + 1, oh_hits_w_flurry / oh_hits);
        heroic_strike_uptime_ = Statistics::update_mean(heroic_strike_uptime_, iter + 1, oh_hits_w_heroic / oh_hits);
        avg_rage_spent_executing_ =
            Statistics::update_mean(avg_rage_spent_executing_, iter + 1, buff_manager_.rage_spent_executing);
        if (compute_time_lapse)
        {
            add_damage_source_to_time_lapse(damage_sources.damage_instances);
        }
        if (compute_histogram)
        {
            hist_y[new_sample / 10.0]++;
        }
    }
    if (compute_time_lapse)
    {
        normalize_timelapse();
    }
    if (compute_histogram)
    {
        prune_histogram();
    }
}

void Combat_simulator::init_histogram()
{
    double res = 10.0;
    for (int i = 0; i < 500; i++)
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
        hist_x = std::vector<double>(first, last);
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
    double total_sim_time = config.n_batches * config.sim_time;
    for (const auto& aura : buff_manager_.aura_uptime.auras)
    {
        std::string aura_name = aura.id;
        double uptime = aura.duration / total_sim_time;
        aura_uptimes.emplace_back(aura.id + " " + std::to_string(100 * uptime));
    }
    aura_uptimes.emplace_back("Flurry_main_hand " + std::to_string(100 * flurry_uptime_mh_));
    aura_uptimes.emplace_back("Flurry_off_hand " + std::to_string(100 * flurry_uptime_oh_));
    aura_uptimes.emplace_back("'Heroic_strike_bug' " + std::to_string(100 * heroic_strike_uptime_));
    return aura_uptimes;
}

std::vector<std::string> Combat_simulator::get_proc_statistics() const
{
    std::vector<std::string> proc_counter;
    for (const auto& proc : buff_manager_.procs)
    {
        std::string aura_name = proc.id;
        double counter = static_cast<double>(proc.counter) / config.n_batches;
        proc_counter.emplace_back(proc.id + " " + std::to_string(counter));
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
