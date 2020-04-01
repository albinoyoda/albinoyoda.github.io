#ifndef WOW_SIMULATOR_WEAPON_SIM_HPP
#define WOW_SIMULATOR_WEAPON_SIM_HPP

#include "Item.hpp"

class Weapon_sim
{
public:
    Weapon_sim(double swing_speed, std::pair<double, double> damage_interval,
               Socket socket, Weapon_type skill_type);

    double step(double time, double attack_power, bool is_random);

    constexpr double swing(double attack_power)
    {
        return average_damage_ + attack_power * swing_speed_ / 14;
    }

    double random_swing(double attack_power)
    {
        double damage = damage_interval_.first + (damage_interval_.second - damage_interval_
                .first) * static_cast<double>(rand()) / RAND_MAX
                        + attack_power * swing_speed_ / 14;
        return damage;
    }

    double random_normalized_swing(double attack_power)
    {
        return damage_interval_.first + (damage_interval_.second - damage_interval_
                .first) * static_cast<double>(rand()) / RAND_MAX
               + attack_power * normalized_swing_speed_ / 14;
    }

    constexpr double normalized_swing(double attack_power)
    {
        // TODO random damage?
        return average_damage_ + attack_power * normalized_swing_speed_ / 14;
    }

    void reset_timer();

    constexpr void compute_weapon_damage(double bonus_damage)
    {
        damage_interval_.first += bonus_damage;
        damage_interval_.second += bonus_damage;
        average_damage_ = (damage_interval_.second + damage_interval_.first) / 2;
    }

    constexpr double get_average_damage()
    {
        return average_damage_;
    }

    constexpr double get_swing_speed()
    {
        return swing_speed_;
    }

    constexpr double get_internal_swing_timer()
    {
        return internal_swing_timer_;
    }

    Socket get_socket() const;

    Weapon_type get_weapon_type() const
    {
        return weapon_type_;
    }

    void set_internal_swing_timer(double internal_swing_timer);

private:
    double swing_speed_;
    double normalized_swing_speed_;
    double internal_swing_timer_;
    std::pair<double, double> damage_interval_;
    double average_damage_;
    Socket socket_;
    Weapon_type weapon_type_;
};

#endif //WOW_SIMULATOR_WEAPON_SIM_HPP
