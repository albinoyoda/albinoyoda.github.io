#include "../include/Character.hpp"

#include <iostream>

Character::Character(const Race& race, int level)
    : base_attributes{}, total_attributes{}, base_special_stats{}, total_special_stats{}, race{race}, level(level)
{
    int base_skill = level * 5;
    base_special_stats = Special_stats{0,          0,          160, 0,          0,          base_skill,          base_skill,                      base_skill,
                                       base_skill, base_skill, 0,   0,          0,          base_skill,          base_skill,                      base_skill, 
                                       0,          0,          0,   0,          static_cast<double>(base_skill), static_cast<double>(base_skill), static_cast<double>(base_skill)};
    switch (race)
    {
    case Race::human:
        base_attributes = Attributes{145, 96};
        base_special_stats.sword_expertise = 1.25;
        base_special_stats.mace_expertise = 1.25;
        break;
    case Race::dwarf:
        base_attributes = Attributes{147, 92};
        break;
    case Race::night_elf:
        base_attributes = Attributes{142, 101};
        break;
    case Race::gnome:
        base_attributes = Attributes{140, 99};
        break;
    case Race::draenei:
        base_attributes = Attributes{146, 93};
        break;
    case Race::orc:
        base_attributes = Attributes{148, 93};
        base_special_stats.axe_expertise = 1.25;
        break;
    case Race::tauren:
        base_attributes = Attributes{150, 91};
        break;
    case Race::troll:
        base_attributes = Attributes{146, 98};
        break;
    case Race::undead:
        base_attributes = Attributes{144, 94};
        break;
    default:
        assert(false);
    }
}

std::ostream& operator<<(std::ostream& os, const Character& character)
{
    os << "Character items: <br>";
    for (const auto& item : character.armor)
    {
        os << item.name << "<br>";
    }
    os << "Main hand:<br>";
    os << character.weapons[0].name << "<br>";
    if (character.is_dual_wield())
    {
        os << "Off hand:<br>";
        os << character.weapons[1].name << "<br>";
    }
    return os;
}
