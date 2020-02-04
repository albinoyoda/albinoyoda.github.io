#include "Stats.hpp"

Stats Stats::operator+(const Stats &rhs)
{
    return {this->strength + rhs.strength, this->agility + rhs.agility,};
}

Stats &Stats::operator+=(const Stats &rhs)
{
    *this = *this + rhs;
    return *this;
}

Stats operator+(const Stats &lhs, const Stats &rhs)
{
    return {lhs.strength + rhs.strength, lhs.agility + rhs.agility};
}

std::ostream &operator<<(std::ostream &os, Special_stats const &special_stats)
{
    os << "Characters special stats: " << "\n";
    os << "hit: " << special_stats.hit << "\n";
    os << "crit: " << special_stats.critical_strike << "\n";
    os << "attack power: " << special_stats.attack_power << "\n";
    return os;
}

std::ostream &operator<<(std::ostream &os, Stats const &stats)
{
    os << "Characters stats: " << "\n";
    os << "strength: " << stats.strength << "\n";
    os << "agility: " << stats.agility << "\n";
    return os;
}

Special_stats Stats::convert_to_special_stats() const
{
    return {this->agility / 20, 0, this->strength * 2};
}

void Stats::clear()
{
    strength = 0;
    agility = 0;
}

void Special_stats::clear()
{
    hit = 0;
    critical_strike = 0;
    attack_power = 0;
}
