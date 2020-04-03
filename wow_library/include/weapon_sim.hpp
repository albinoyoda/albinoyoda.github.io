#ifndef WOW_SIMULATOR_WEAPON_SIM_HPP
#define WOW_SIMULATOR_WEAPON_SIM_HPP

#include "Item.hpp"

class Weapon_sim
{
public:
    Weapon_sim(double swing_speed, std::pair<double, double> damage_interval, Socket socket, Weapon_type skill_type, std::vector<Hit_effect> hit_effects);

    constexpr bool time_for_swing(double dt)
    {
        internal_swing_timer -= dt;
        if (internal_swing_timer < 0.0)
        {
            internal_swing_timer += dt;
            return true;
        }
        return false;
    }

    constexpr double swing(double attack_power)
    {
        return average_damage + attack_power * swing_speed / 14;
    }

    constexpr double normalized_swing(double attack_power)
    {
        return average_damage + attack_power * normalized_swing_speed / 14;
    }

    constexpr void compute_weapon_damage(double bonus_damage)
    {
        damage_interval.first += bonus_damage;
        damage_interval.second += bonus_damage;
        average_damage = (damage_interval.second + damage_interval.first) / 2;
    }

    double random_swing(double attack_power);

    double random_normalized_swing(double attack_power);

    double swing_speed;
    double normalized_swing_speed;
    double internal_swing_timer;
    std::pair<double, double> damage_interval;
    double average_damage;
    Socket socket;
    Weapon_type weapon_type;
    std::vector<Hit_effect> hit_effects;
    std::string socket_name;
};

#endif //WOW_SIMULATOR_WEAPON_SIM_HPP
