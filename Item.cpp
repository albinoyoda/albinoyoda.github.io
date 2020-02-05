#include "Item.hpp"

/**
ITEM
 */
const Stats &Item::get_stats() const
{
    return stats_;
}

Item::Item(Stats stats, Special_stats special_stats) : stats_{stats},
        special_stats_{special_stats},
        chance_for_extra_hit{0.0},
        extra_skill_{0} {};

const Special_stats &Item::get_special_stats() const
{
    return special_stats_;
}

void Item::set_chance_for_extra_hit(int chance_for_extra_hit_input)
{
    chance_for_extra_hit = chance_for_extra_hit_input;
}

double Item::get_chance_for_extra_hit() const
{
    return chance_for_extra_hit;
}

int Item::get_extra_skill() const
{
    return extra_skill_;
}

void Item::set_extra_skill(double extra_skill)
{
    extra_skill_ = extra_skill;
}

/**
WEAPON
 */
Weapon::Weapon(double swing_speed, std::pair<double, double> damage_interval, Stats stats, Special_stats special_stats,
               Socket socket)
        : Item{stats, special_stats},
        swing_speed_{swing_speed},
        internal_swing_timer_{0},
        damage_interval_{std::move(damage_interval)},
        average_damage_{0.0},
        socket_{socket} {}

double Weapon::swing(double attack_power)
{
    // TODO random damage?
    return average_damage_ + attack_power * swing_speed_ / 14;
}

void Weapon::reset_timer()
{
    internal_swing_timer_ = swing_speed_;
}

double Weapon::step(double dt, double attack_power)
{
    internal_swing_timer_ -= dt;
    if (internal_swing_timer_ < 0.0)
    {
        internal_swing_timer_ += swing_speed_;
        double damage = swing(attack_power);
        if (socket_ == Weapon::Socket::off_hand)
        {
            damage *= 0.625;
        }
        return damage;
    }
    return 0.0;
}

Weapon::Socket Weapon::get_socket() const
{
    return socket_;
}

/**
ARMOR
 */
Armor::Armor(Stats stats, Special_stats special_stats, Socket socket) : Item{stats, special_stats},
        socket_{socket} {}

Armor::Socket Armor::get_socket() const
{
    return socket_;
}

std::ostream &operator<<(std::ostream &os, Armor::Socket const &socket)
{
    os << "Item slot:" << "\n";
    switch (socket)
    {
        case Armor::Socket::head:
            os << "head." << "\n";
            break;
        case Armor::Socket::neck:
            os << "neck." << "\n";
            break;
        case Armor::Socket::shoulder:
            os << "shoulder." << "\n";
            break;
        case Armor::Socket::back:
            os << "back." << "\n";
            break;
        case Armor::Socket::chest:
            os << "chest." << "\n";
            break;
        case Armor::Socket::wrists:
            os << "wrists." << "\n";
            break;
        case Armor::Socket::hands:
            os << "hands." << "\n";
            break;
        case Armor::Socket::belt:
            os << "belt." << "\n";
            break;
        case Armor::Socket::legs:
            os << "legs." << "\n";
            break;
        case Armor::Socket::boots:
            os << "boots." << "\n";
            break;
        case Armor::Socket::ring:
            os << "ring." << "\n";
            break;
        case Armor::Socket::trinket:
            os << "trinket." << "\n";
            break;
        case Armor::Socket::ranged:
            os << "ranged." << "\n";
            break;
    }
    return os;
}
