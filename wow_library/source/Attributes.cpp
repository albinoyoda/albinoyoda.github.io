#include "Attributes.hpp"

double multiplicative_addition(double val1, double val2)
{
    return (1 + val1) * (1 + val2) - 1;
}

double multiplicative_subtraction(double val1, double val2)
{
    return (1 + val1) / (1 + val2) - 1;
}

std::ostream& operator<<(std::ostream& os, Special_stats const& special_stats)
{
    os << "<b>Special stats:</b><br>";
    os << "Hit: " << special_stats.hit << "<br>";
    os << "Crit: " << special_stats.critical_strike << "<br>";
    os << "Attack power: " << special_stats.attack_power << "<br>";
    return os;
}