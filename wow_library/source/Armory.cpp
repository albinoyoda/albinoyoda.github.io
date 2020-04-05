#include "../include/Armory.hpp"

Attributes Armory::get_enchant_attributes(Socket socket, Enchant::Type type) const
{
    switch (socket)
    {
        case Socket::head:
        {
            switch (type)
            {
                case Enchant::Type::agility:
                    return {0, 8};
                case Enchant::Type::strength:
                    return {8, 0};
                default:
                    return {0, 0};
            }
        }
        case Socket::back:
        {
            switch (type)
            {
                case Enchant::Type::agility:
                    return {0, 3};
                default:
                    return {0, 0};
            }
        }
        case Socket::chest:
        {
            switch (type)
            {
                case Enchant::Type::minor_stats:
                    return {3, 3};
                case Enchant::Type::major_stats:
                    return {4, 4};
                default:
                    return {0, 0};
            }
        }
        case Socket::wrists :
        {
            switch (type)
            {
                case Enchant::Type::strength7:
                    return {7, 0};
                case Enchant::Type::strength9:
                    return {9, 0};
                default:
                    return {0, 0};
            }
        }
        case Socket::hands :
        {
            switch (type)
            {
                case Enchant::Type::agility:
                    return {0, 7};
                case Enchant::Type::strength:
                    return {7, 0};
                default:
                    return {0, 0};
            }
        }
        case Socket::legs :
        {
            switch (type)
            {
                case Enchant::Type::agility:
                    return {0, 8};
                case Enchant::Type::strength:
                    return {8, 0};
                default:
                    return {0, 0};
            }
        }
        case Socket::boots :
        {
            switch (type)
            {
                case Enchant::Type::agility:
                    return {0, 7};
                default:
                    return {0, 0};
            }
        }
        default:
            return {0, 0};
    }
}

Special_stats Armory::get_enchant_special_stats(Socket socket, Enchant::Type type) const
{
    switch (socket)
    {
        case Socket::head:
        {
            switch (type)
            {
                case Enchant::Type::haste:
                    return {0, 0, 0, 0, .01};
                default:
                    return {0, 0, 0};
            }
        }
        case Socket::hands :
        {
            switch (type)
            {
                case Enchant::Type::haste:
                    return {0, 0, 0, 0, .01};
                default:
                    return {0, 0, 0};
            }
        }
        case Socket::legs:
        {
            switch (type)
            {
                case Enchant::Type::haste:
                    return {0, 0, 0, 0, .01};
                default:
                    return {0, 0, 0};
            }
        }
        default:
            return {0, 0, 0};
    }
}

Hit_effect Armory::enchant_hit_effect(double weapon_speed, Enchant::Type type) const
{
    if (type == Enchant::Type::crusader)
    {
        return {"crusader", Hit_effect::Type::stat_boost, {100, 0}, {0, 0, 0}, 0, 15, weapon_speed / 40};
    }
    return {"none", Hit_effect::Type::none, {}, {}, 0, 0, 0};
}