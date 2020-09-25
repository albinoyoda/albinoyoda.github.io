#include "../include/Attributes.hpp"

double multiplicative_addition(double val1, double val2)
{
    return val1 + val2 + val1 * val2;
}

double multiplicative_subtraction(double val1, double val2)
{
    return (1 + val1) / (1 + val2) - 1;
}

Total_stats Total_stats::operator+(const Total_stats& rhs) const
{
    return {critical_strike + rhs.critical_strike,
            hit + rhs.hit,
            attack_power + rhs.attack_power,
            hitpoints + rhs.hitpoints,
            armor + rhs.armor,
            multiplicative_addition(haste, rhs.haste),
            defence_skill + rhs.defence_skill,
            sword_skill + rhs.sword_skill,
            axe_skill + rhs.axe_skill,
            dagger_skill + rhs.dagger_skill,
            mace_skill + rhs.mace_skill,
            fist_skill + rhs.fist_skill,
            multiplicative_addition(damage_multiplier, rhs.damage_multiplier),
            multiplicative_addition(strength_multiplier, rhs.strength_multiplier),
            multiplicative_addition(agility_multiplier, rhs.agility_multiplier),
            multiplicative_addition(stamina_multiplier, rhs.stamina_multiplier),
            bonus_damage + rhs.bonus_damage,
            multiplicative_addition(threat_modifier, rhs.threat_modifier)};
}

Total_stats Total_stats::operator-(const Total_stats& rhs) const
{
    return {critical_strike - rhs.critical_strike,
            hit - rhs.hit,
            attack_power - rhs.attack_power,
            hitpoints - rhs.hitpoints,
            armor - rhs.armor,
            multiplicative_subtraction(haste, rhs.haste),
            defence_skill - rhs.defence_skill,
            sword_skill - rhs.sword_skill,
            axe_skill - rhs.axe_skill,
            dagger_skill - rhs.dagger_skill,
            mace_skill - rhs.mace_skill,
            fist_skill - rhs.fist_skill,
            multiplicative_subtraction(damage_multiplier, rhs.damage_multiplier),
            multiplicative_subtraction(strength_multiplier, rhs.strength_multiplier),
            multiplicative_subtraction(agility_multiplier, rhs.agility_multiplier),
            multiplicative_subtraction(stamina_multiplier, rhs.stamina_multiplier),
            bonus_damage - rhs.bonus_damage,
            multiplicative_subtraction(threat_modifier, rhs.threat_modifier)};
}

Total_stats& Total_stats::operator+=(const Total_stats& rhs)
{
    *this = *this + rhs;
    return *this;
}

Total_stats& Total_stats::operator-=(const Total_stats& rhs)
{
    *this = *this - rhs;
    return *this;
}

Attributes::Attributes(double strength, double agility, double stamina)
    : strength{strength}, agility{agility}, stamina{stamina}
{
}

Attributes Attributes::operator+(const Attributes& rhs)
{
    return {strength + rhs.strength, agility + rhs.agility, stamina + rhs.stamina};
}

Attributes& Attributes::operator+=(const Attributes& rhs)
{
    *this = *this + rhs;
    return *this;
}

Attributes create_attributes(const Stat_list& stat_list)
{
    Attributes attributes{};
    for (const auto& pair : stat_list)
    {
        switch (pair.first)
        {
        case Stat::agi:
        {
            attributes.agility = pair.second;
            break;
        }
        case Stat::str:
        {
            attributes.strength = pair.second;
            break;
        }
        case Stat::sta:
        {
            attributes.stamina = pair.second;
            break;
        }
        default:
            break;
        }
    }
    return attributes;
}

Total_stats create_stats(const Stat_list& stat_list)
{
    Total_stats total_stats{};
    for (const auto& pair : stat_list)
    {
        switch (pair.first)
        {
        case Stat::hit:
        {
            total_stats.hit = pair.second;
            break;
        }
        case Stat::crit:
        {
            total_stats.critical_strike = pair.second;
            break;
        }
        default:
            break;
        }
    }
    return total_stats;
}

Total_stats Attributes::convert_to_total_stats(const Total_stats& total_stats) const
{
    return create_stats({{Stat::agi, agility / 20 * total_stats.agility_multiplier},
                         {Stat::str, strength * 2 * total_stats.strength_multiplier},
                         {Stat::sta, stamina * total_stats.stamina_multiplier}});
}

std::ostream& operator<<(std::ostream& os, Total_stats const& total_stats)
{
    os << "<b>Special stats:</b><br>";
    os << "Hit: " << total_stats.hit << "<br>";
    os << "Crit: " << total_stats.critical_strike << "<br>";
    os << "Attack power: " << total_stats.attack_power << "<br>";
    return os;
}