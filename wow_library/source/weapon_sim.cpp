#include "../include/weapon_sim.hpp"

/**
WEAPON
 */
Weapon_sim::Weapon_sim(double swing_speed, std::pair<double, double> damage_interval,
                       Socket socket, Weapon_type skill_type) :
        swing_speed_{swing_speed},
        internal_swing_timer_{0.0},
        damage_interval_{std::move(damage_interval)},
        average_damage_{0.0},
        socket_{socket},
        weapon_type_{skill_type}
{
    if (weapon_type_ == Weapon_type::dagger)
    {
        normalized_swing_speed_ = 1.7;
    }
    else
    {
        normalized_swing_speed_ = 2.5;
    }
}

void Weapon_sim::reset_timer()
{
    internal_swing_timer_ = swing_speed_;
}

double Weapon_sim::step(double dt, double attack_power, bool is_random)
{
    internal_swing_timer_ -= dt;
    if (internal_swing_timer_ < 0.0)
    {
        internal_swing_timer_ += swing_speed_;
        double damage;
        if (is_random)
        {
            damage = random_swing(attack_power);
        }
        else
        {
            damage = swing(attack_power);
        }
        if (get_socket() == Socket::off_hand)
        {
            damage *= 0.625;
        }
        return damage;
    }
    return 0.0;
}

Socket Weapon_sim::get_socket() const
{
    return socket_;
}

void Weapon_sim::set_internal_swing_timer(double internal_swing_timer)
{
    internal_swing_timer_ = internal_swing_timer;
}