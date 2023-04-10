#include "Attributes.hpp"

std::ostream& operator<<(std::ostream& os, Special_stats const& special_stats)
{
    os << "<b>Special stats:</b><br>";
    os << "Hit: " << special_stats.hit << "<br>";
    os << "Crit: " << special_stats.critical_strike << "<br>";
    os << "Attack power: " << special_stats.attack_power << "<br>";
    return os;
}