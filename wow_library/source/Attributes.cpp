#include "../include/Attributes.hpp"

std::ostream &operator<<(std::ostream &os, Special_stats const &special_stats)
{
    os << "Characters special stats: " << "\n";
    os << "hit: " << special_stats.hit << "\n";
    os << "crit: " << special_stats.critical_strike << "\n";
    os << "attack power: " << special_stats.attack_power << "\n";
    return os;
}