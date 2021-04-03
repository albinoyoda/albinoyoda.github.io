#ifndef WOW_SIMULATOR_WEAPON_SIM_HPP
#define WOW_SIMULATOR_WEAPON_SIM_HPP

#include "include/Item.hpp"

class Weapon_sim
{
public:
    Weapon_sim(double swing_speed, double min_damage, double max_damage, Socket socket, Weapon_type skill_type,
               Weapon_socket weapon_socket, std::vector<Hit_effect> hit_effects);

    constexpr bool time_for_swing(double dt)
    {
        internal_swing_timer -= dt;
        return internal_swing_timer < 0.0;
    }

    constexpr double swing(double attack_power) const { return average_damage + attack_power * swing_speed / 14; }

    constexpr double normalized_swing(double attack_power)
    {
        return average_damage + attack_power * normalized_swing_speed / 14;
    }

    constexpr void compute_weapon_damage(double bonus_damage)
    {
        min_damage += bonus_damage;
        max_damage += bonus_damage;
        average_damage = (min_damage + max_damage) / 2;
    }

    const double swing_speed;
    double normalized_swing_speed;
    double internal_swing_timer;
    double min_damage;
    double max_damage;
    double average_damage;
    Socket socket;
    Weapon_type weapon_type;
    Weapon_socket weapon_socket;
    std::vector<Hit_effect> hit_effects;
    std::string socket_name;
};

#endif // WOW_SIMULATOR_WEAPON_SIM_HPP
