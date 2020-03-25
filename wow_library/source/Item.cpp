#include "../include/Item.hpp"

#include <cassert>

Attributes get_enchant_attributes(Socket socket, Enchant::Type type)
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
                    assert(false);
            }
            break;
        }
        case Socket::back:
        {
            switch (type)
            {
                case Enchant::Type::agility:
                    return {0, 3};
                default:
                    assert(false);
            }
            break;
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
                    assert(false);
            }
            break;
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
                    assert(false);
            }
            break;
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
                    assert(false);
            }
            break;
        }
        case Socket::legs :
        {
            switch (type)
            {
                case Enchant::Type::agility:
                    return {0, 8};
                    break;
                case Enchant::Type::strength:
                    return {8, 0};
                    break;
                default:
                    assert(false);
            }
            break;
        }
        case Socket::boots :
        {
            switch (type)
            {
                case Enchant::Type::agility:
                    return {0, 7};
                    break;
                default:
                    assert(false);
            }
            break;
        }
        default:
            break;
    }
    return {0, 0};
}

Special_stats get_enchant_special_stats(Socket socket, Enchant::Type type)
{
    switch (socket)
    {
        case Socket::head:
        {
            switch (type)
            {
                case Enchant::Type::haste:
                    return {0, 0, 0, 0, 1};
                default:
                    assert(false);
            }
            break;
        }
        case Socket::hands :
        {
            switch (type)
            {
                case Enchant::Type::haste:
                    return {0, 0, 0, 0, 1};
                default:
                    assert(false);
            }
            break;
        }
        case Socket::legs:
        {
            switch (type)
            {
                case Enchant::Type::haste:
                    return {0, 0, 0, 0, 1};
                default:
                    assert(false);
            }
            break;
        }
        default:
            break;
    }
    return {0, 0, 0};
}

Hit_effect enchant_hit_effect(Socket socket, Enchant::Type type)
{
    switch (socket)
    {
        case Socket::main_hand:
        {
            switch (type)
            {
                case Enchant::Type::crusader:
                    return {Hit_effect::Type::stat_boost, {100, 0}, {0, 0, 0}, 0, 15};
                default:
                    assert(false);
            }
            break;
        }
        case Socket::off_hand:
        {
            switch (type)
            {
                case Enchant::Type::crusader:
                    return {Hit_effect::Type::stat_boost, {100, 0}, {0, 0, 0}, 0, 15};
                default:
                    assert(false);
            }
            break;
        }
        default:
            return {Hit_effect::Type::none, {}, {}, 0, 0};
    }
    return {Hit_effect::Type::none, {}, {}, 0, 0};
}

Enchant::Enchant(Socket socket, Enchant::Type type) :
        Stat_base{get_enchant_attributes(socket, type), get_enchant_special_stats(socket, type)},
        socket_{socket},
        type_{type},
        hit_effect_(enchant_hit_effect(socket, type)) {}

