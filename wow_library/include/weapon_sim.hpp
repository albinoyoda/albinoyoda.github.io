#ifndef WOW_SIMULATOR_WEAPON_SIM_HPP
#define WOW_SIMULATOR_WEAPON_SIM_HPP

#include "Item.hpp"

class Weapon_sim
{
public:
    Weapon_sim(const Weapon &weapon, const Special_stats &special_stats, int level_difference) :
            swing_speed(weapon.swing_speed),
            socket(weapon.socket),
            type(weapon.type),
            internal_swing_timer(0.0)
    {
        compute_hit_tables(level_difference, special_stats, weapon.type);
        average_damage = (weapon.min_damage + weapon.max_damage) / 2 + weapon.buff.bonus_damage;
        if (weapon.type == Weapon_type::dagger)
        {
            normalized_swing_speed = 1.7;
        }
        else
        {
            normalized_swing_speed = 2.5;
        }

        for (const auto &hit_effect : weapon.hit_effects)
        {
            if (hit_effect.type != Hit_effect::Type::none)
            {
                hit_effects.emplace_back(hit_effect);
            }
        }
    }

    static std::vector<double> create_hit_table_vector(double miss, double dodge, double glancing, double crit)
    {
        // Order -> Miss, parry, dodge, block, glancing, crit, hit.
        return {miss, miss + dodge, miss + dodge + glancing, miss + dodge + glancing + crit};
    }

    void compute_hit_tables(int level_difference, Special_stats special_stats, Weapon_type weapon_type)
    {
        size_t weapon_skill = special_stats.get_weapon_skill(weapon_type);
        size_t target_defence_level = (60 + level_difference) * 5;
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
        dw_miss_chance -= std::max(special_stats.hit - hit_penalty, 0.0);
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

        glancing_factor = (100.0 - glancing_penalty) / 100.0;

        hit_table_white = create_hit_table_vector(two_hand_miss_chance, dodge_chance, glancing_chance, crit_chance);
        hit_table_white_reck = create_hit_table_vector(two_hand_miss_chance, dodge_chance, glancing_chance, 100);
        hit_table_white_dw = create_hit_table_vector(dw_miss_chance, dodge_chance, glancing_chance, crit_chance);
        hit_table_white_dw_reck = create_hit_table_vector(dw_miss_chance, dodge_chance, glancing_chance, 100);
        hit_table_yellow = create_hit_table_vector(two_hand_miss_chance, dodge_chance, 0, crit_chance);
        hit_table_yellow_reck = create_hit_table_vector(two_hand_miss_chance, dodge_chance, 0, 100);
    }


//    double random_var = get_random_100();
//        if (recklessness_active)
//        {
//            simulator_cout("Drawing outcome from MH recklessness table");
//            int outcome = std::lower_bound(hit_probabilities_recklessness_mh_.begin(),
//                                           hit_probabilities_recklessness_mh_.end(),
//                                           random_var) - hit_probabilities_recklessness_mh_.begin();
//            return {damage * lookup_outcome_mh_white(outcome), Hit_result(outcome)};
//        }
//        else
//        {
//            simulator_cout("Drawing outcome from MH hit table");
//            int outcome = std::lower_bound(hit_probabilities_white_mh_.begin(), hit_probabilities_white_mh_.end(),
//                                           random_var) - hit_probabilities_white_mh_.begin();
//            return {damage * lookup_outcome_mh_white(outcome), Hit_result(outcome)};

    constexpr double step(double dt, double attack_power, double &rage)
    {
        internal_swing_timer -= dt;
        if (internal_swing_timer < 0.0)
        {
            internal_swing_timer += swing_speed;
            double damage = swing(attack_power);
            if (heroic_strike)
            {
                simulator_cout("Performing heroic strike");
                damage += 138;
                
                hit_outcome = generate_hit(swing_damage, Hit_type::yellow, weapon.socket, heroic_strike,
                                           deathwish_active, recklessness_active);
                heroic_strike = false;
                rage -= heroic_strike_rage_cost;
                damage_sources.heroic_strike += hit_outcome.damage;
                damage_sources.heroic_strike_count++;
                simulator_cout(rage, " rage");
            }
            if (socket == Socket::off_hand)
            {
                damage *= 0.625;
            }
            return damage;
        }
        return 0.0;
    }

    constexpr double swing(double attack_power) const
    {
        return average_damage + attack_power * swing_speed / 14;
    }

    constexpr double normalized_swing(double attack_power)
    {
        return average_damage + attack_power * normalized_swing_speed / 14;
    }

    constexpr double lookup_outcome_mh_white(int case_id)
    {
        switch (case_id)
        {
            case 0:
                return 0.0;
            case 1:
                return 0.0;
            case 2:
                return glancing_factor;
            case 3:
                // TODO talents here
                return 2.2;
            case 4:
                return 1.0;
            default:
                assert(false);
                return 0.0;
        }
    }

    template<typename T>
    void print_statement(T t)
    {
        std::cout << std::setprecision(4) << t;
    }

    template<typename... Args>
    void simulator_cout(Args &&... args)
    {
        if (debug_mode)
        {
            std::cout << "Time: " << std::setw(8) << std::left << time_keeper_.time + time_keeper_.dt
                      << "s. Loop idx:" << std::setw(4) << time_keeper_.step_index << "Event: ";
            __attribute__((unused)) int dummy[] = {0, ((void) print_statement(std::forward<Args>(args)), 0)...};
            std::cout << "\n";
        }
    }

    double swing_speed;
    double average_damage;
    Socket socket;
    Weapon_type type;
    double normalized_swing_speed;
    double internal_swing_timer;
    std::vector<Hit_effect> hit_effects;
    double glancing_factor;
    std::vector<double> hit_table_white{};
    std::vector<double> hit_table_white_reck{};
    std::vector<double> hit_table_white_dw{};
    std::vector<double> hit_table_white_dw_reck{};
    std::vector<double> hit_table_yellow{};
    std::vector<double> hit_table_yellow_reck{};

    Time_keeper &time_keeper_;
    bool debug_mode;

    bool heroic_strike;
    bool cleave;
};

#endif //WOW_SIMULATOR_WEAPON_SIM_HPP


//double random_swing(double attack_power)
//{
//    double damage = damage_interval_.first + (damage_interval_.second - damage_interval_
//            .first) * static_cast<double>(rand()) / RAND_MAX
//                    + attack_power * swing_speed_ / 14;
//    return damage;
//}
//
//double random_normalized_swing(double attack_power)
//{
//    return damage_interval_.first + (damage_interval_.second - damage_interval_
//            .first) * static_cast<double>(rand()) / RAND_MAX
//           + attack_power * normalized_swing_speed_ / 14;
//}
