#include <iostream>
#include "../include/Character.hpp"

Character::Character(const Race &race, size_t level)
        :
        base_attributes{},
        total_attributes{},
        base_special_stats{},
        total_special_stats{},
        race{race},
        level(level)
{
    size_t base_skill = level * 5;
    base_special_stats = Special_stats{0, 0, 160, 0, 0, base_skill, base_skill, base_skill, base_skill};
    switch (race)
    {
        case Race::human:
            base_attributes = Attributes{120, 80};
            base_special_stats += Special_stats{0, 0, 0, 0, 0, 5, 0, 0, 5};
            break;
        case Race::dwarf:
            base_attributes = Attributes{122, 76};
            break;
        case Race::night_elf:
            base_attributes = Attributes{117, 85};
            break;
        case Race::gnome:
            base_attributes = Attributes{115, 83};
            break;
        case Race::orc:
            base_attributes = Attributes{123, 77};
            base_special_stats = Special_stats{0, 0, 0, 0, 0, 0, 5, 0, 0};
            break;
        case Race::tauren:
            base_attributes = Attributes{125, 75};
            break;
        case Race::troll:
            base_attributes = Attributes{121, 82};
            break;
        case Race::undead:
            base_attributes = Attributes{119, 78};
            break;
        default:
            assert(false);
    }
}








