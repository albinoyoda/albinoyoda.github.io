#include "../include/Item.hpp"

#include <cassert>

int get_weapon_skill(const Special_stats &special_stats, Weapon_type weapon_type)
{
    switch (weapon_type)
    {
        case Weapon_type::sword:
            return special_stats.sword_skill;
        case Weapon_type::axe:
            return special_stats.axe_skill;
        case Weapon_type::dagger:
            return special_stats.dagger_skill;
        case Weapon_type::mace:
            return special_stats.mace_skill;
        case Weapon_type::unarmed:
            //TODO
            return 300;
        default:
            assert(true);
            return 0;
    }
}

std::ostream &operator<<(std::ostream &os, const Socket &socket)
{
    os << "Item slot ";
    switch (socket)
    {
        case Socket::head:
            os << "head." << "\n";
            break;
        case Socket::neck:
            os << "neck." << "\n";
            break;
        case Socket::shoulder:
            os << "shoulder." << "\n";
            break;
        case Socket::back:
            os << "back." << "\n";
            break;
        case Socket::chest:
            os << "chest." << "\n";
            break;
        case Socket::wrists:
            os << "wrists." << "\n";
            break;
        case Socket::hands:
            os << "hands." << "\n";
            break;
        case Socket::belt:
            os << "belt." << "\n";
            break;
        case Socket::legs:
            os << "legs." << "\n";
            break;
        case Socket::boots:
            os << "boots." << "\n";
            break;
        case Socket::ring:
            os << "ring." << "\n";
            break;
        case Socket::trinket:
            os << "trinket." << "\n";
            break;
        case Socket::ranged:
            os << "ranged." << "\n";
            break;
        case Socket::main_hand:
            os << "main hand." << "\n";
            break;
        case Socket::off_hand:
            os << "off hand." << "\n";
            break;
        case Socket::none:
            os << "none." << "\n";
            break;
    }
    return os;
}
