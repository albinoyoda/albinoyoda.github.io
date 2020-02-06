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
        bonus_skill_{Skill_type::none, 0} {};

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

void Item::set_bonus_skill(Extra_skill bonus_skill)
{
    bonus_skill_ = bonus_skill;
}

const Extra_skill &Item::get_bonus_skill() const
{
    return bonus_skill_;
}

/**
WEAPON
 */
Weapon::Weapon(double swing_speed, std::pair<double, double> damage_interval, Stats stats, Special_stats special_stats,
               Socket socket, Skill_type skill_type)
        : Item{stats, special_stats},
        swing_speed_{swing_speed},
        internal_swing_timer_{0.0},
        damage_interval_{std::move(damage_interval)},
        average_damage_{0.0},
        socket_{socket},
        weapon_type_{skill_type},
        hand_{} {}

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
        if (get_hand() == Hand::off_hand)
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

Skill_type Weapon::get_weapon_type() const
{
    return weapon_type_;
}

void Weapon::set_weapon_type(Skill_type weapon_type)
{
    weapon_type_ = weapon_type;
}

void Weapon::set_hand(Hand hand)
{
    hand_ = hand;
}

void Weapon::set_internal_swing_timer(double internal_swing_timer)
{
    internal_swing_timer_ = internal_swing_timer;
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
