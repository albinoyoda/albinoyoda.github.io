#include "../include/Attributes.hpp"

double multiplicative_addition(double val1, double val2)
{
    return (1 + val1) * (1 + val2) - 1;
}

double multiplicative_subtraction(double val1, double val2)
{
    return (1 + val1) / (1 + val2) - 1;
}

std::ostream &operator<<(std::ostream &os, Special_stats const &special_stats)
{
    os << "Characters special stats: " << "\n";
    os << "hit: " << special_stats.hit << "\n";
    os << "crit: " << special_stats.critical_strike << "\n";
    os << "attack power: " << special_stats.attack_power << "\n";
    return os;
}