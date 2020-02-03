#include "Combat_simulator.hpp"

Combat_simulator::Outcome Combat_simulator::generate_hit(double damage)
{
    double random_var = 100 * rand() / static_cast<double>(RAND_MAX);

    int outcome = std::lower_bound(hit_probabilities_.begin(), hit_probabilities_.end(), random_var) -
                  hit_probabilities_.begin();

    switch (outcome)
    {
        case 0:
            return {0.0, Combat_simulator::Hit_type::miss};
        case 1:
            return {0.0, Combat_simulator::Hit_type::dodge};
        case 2:
            return {damage * 0.85, Combat_simulator::Hit_type::glancing};
        case 3:
            return {damage * 2.2, Combat_simulator::Hit_type::crit};
        case 4:
            return {damage, Combat_simulator::Hit_type::hit};
        default:
            assert(false);
            return {0.0, Combat_simulator::Hit_type::miss};
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
    hit_probabilities_.clear();
    hit_probabilities_.push_back(miss_chance);
    hit_probabilities_.push_back(hit_probabilities_.back() + dodge_chance);
    hit_probabilities_.push_back(hit_probabilities_.back() + glancing_chance);
    hit_probabilities_.push_back(hit_probabilities_.back() + crit_chance);

    std::cout << 100 - hit_probabilities_.back() << "% chance to white hit. Negative value = crit capped \n";
}

double Combat_simulator::simulate(const Character &character, double sim_time, double dt, int opponent_level)
{
    double time = 0;
    double total_damage = 0;
    double rage = 0;
    double blood_thirst_cd = 0;
    double whirlwind_cd = 0;

    auto special_stats = character.get_total_special_stats();
    auto weapons = character.get_weapons();

    compute_hit_table(opponent_level, character.get_weapon_skill(), special_stats);
    while (time < sim_time)
    {
        for (auto &weapon : weapons)
        {
            auto damage = weapon.step(dt, special_stats.attack_power);
//                if (heroic_strike_)
//                {
//                    damage += 157;
//                }
            auto filtered_damage = generate_hit(damage);
            total_damage += filtered_damage.damage;
            rage += filtered_damage.damage * 7.5 / 230.6;
            rage = std::min(100.0, rage);
        }

        if (spell_rotation_)
        {
            if (blood_thirst_cd < 0.0 && rage > 30)
            {
                total_damage += special_stats.attack_power * 0.45;
                rage -= 30;
                blood_thirst_cd = 6.0;
//                std::cout << "BT" << "\n";
            }
            if (whirlwind_cd < 0.0 && rage > 25 && blood_thirst_cd > 0)
            {
                total_damage += weapons[0].get_average_damage() +
                                special_stats.attack_power * weapons[0].get_swing_speed() / 14;
                rage -= 25;
                whirlwind_cd = 10;
//                std::cout << "WW" << "\n";
            }
            //            std::cout << rage << "\n";
//            if (rage > 75)
//            {
//                weapons[0].queue_heroic();
//                std::cout << "heroic" << "\n";
//            }
            blood_thirst_cd -= dt;
            whirlwind_cd -= dt;
        }
//            overpower = baseDamageMH + 35 + attackPower / 14 * normalizedSpeed
//            execute = 600 + (rage - 15 + impExecuteRage) * 15
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
