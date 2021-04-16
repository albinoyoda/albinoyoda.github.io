#include "Item.hpp"

#include "iostream"

std::ostream& operator<<(std::ostream& os, const Socket& socket)
{
    os << "Item slot ";
    switch (socket)
    {
    case Socket::head:
        os << "head.\n";
        break;
    case Socket::neck:
        os << "neck.\n";
        break;
    case Socket::shoulder:
        os << "shoulder.\n";
        break;
    case Socket::back:
        os << "back.\n";
        break;
    case Socket::chest:
        os << "chest.\n";
        break;
    case Socket::wrist:
        os << "wrist.\n";
        break;
    case Socket::hands:
        os << "hands.\n";
        break;
    case Socket::belt:
        os << "belt.\n";
        break;
    case Socket::legs:
        os << "legs.\n";
        break;
    case Socket::boots:
        os << "boots.\n";
        break;
    case Socket::ring:
        os << "ring.\n";
        break;
    case Socket::trinket:
        os << "trinket.\n";
        break;
    case Socket::ranged:
        os << "ranged.\n";
        break;
    case Socket::main_hand:
        os << "main hand.\n";
        break;
    case Socket::off_hand:
        os << "off hand.\n";
        break;
    case Socket::none:
        os << "none.\n";
        break;
    }
    return os;
}

std::string operator+(std::string& string, const Socket& socket)
{
    switch (socket)
    {
    case Socket::head:
        string += "Helmet";
        break;
    case Socket::neck:
        string += "Neck";
        break;
    case Socket::shoulder:
        string += "Shoulder";
        break;
    case Socket::back:
        string += "Back";
        break;
    case Socket::chest:
        string += "Chest";
        break;
    case Socket::wrist:
        string += "Wrist";
        break;
    case Socket::hands:
        string += "Hands";
        break;
    case Socket::belt:
        string += "Belt";
        break;
    case Socket::legs:
        string += "Legs";
        break;
    case Socket::boots:
        string += "Boots";
        break;
    case Socket::ring:
        string += "Ring";
        break;
    case Socket::trinket:
        string += "Trinket";
        break;
    case Socket::ranged:
        string += "Ranged";
        break;
    case Socket::main_hand:
        string += "Main hand";
        break;
    case Socket::off_hand:
        string += "Off hand";
        break;
    case Socket::none:
        string += "None";
        break;
    }
    return string;
}

std::string operator+(std::string& string, const Weapon_socket& socket)
{
    switch (socket)
    {
    case Weapon_socket::main_hand:
        string += "main-hand";
        break;
    case Weapon_socket::off_hand:
        string += "off-hand";
        break;
    case Weapon_socket::one_hand:
        string += "one-hand";
        break;
    case Weapon_socket::two_hand:
        string += "two-hand";
        break;
    }
    return string;
}