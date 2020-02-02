#include "Stats.hpp"

Stats Stats::operator+(const Stats &rhs)
{
    return {this->agility + rhs.agility, this->strength + rhs.strength};
}

Stats &Stats::operator+=(const Stats &rhs)
{
    *this = *this + rhs;
    return *this;
}

Stats operator+(const Stats &lhs, const Stats &rhs)
{
    return {lhs.agility + rhs.agility, lhs.strength + rhs.strength};
}

double Stats::compute_attack_power()
{
    return strength * 2 + agility;
}

Special_stats Stats::convert_to_special_stats()
{
    return {this->agility / 20, 0, this->agility * 1 + this->strength * 2};
}