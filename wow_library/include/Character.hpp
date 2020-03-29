#ifndef WOW_SIMULATOR_CHARACTER_HPP
#define WOW_SIMULATOR_CHARACTER_HPP

#include <cassert>
#include <vector>
#include "Attributes.hpp"
#include "Item.hpp"

enum class Race
{
    human,
    dwarf,
    night_elf,
    gnome,
    orc,
    tauren,
    troll,
    undead
};

enum class Talent
{
    fury,
    arms,
    none,
};

class Character
{
public:
    Character(const Race &race, size_t level);

    void equip_armor(const Armor &piece)
    {
        armor.emplace_back(piece);
    }

    void equip_weapon(Weapon weapon)
    {
        if (weapon.weapon_socket != Weapon_socket::two_hand)
        {
            std::cout << "WARN: Wielding single weapon that is not two handed." << "\n";
        }
        weapon.socket = Socket::main_hand;
        weapons.emplace_back(weapon);
    }

    void equip_weapon(Weapon &weapon1, Weapon &weapon2)
    {
        weapon1.socket = Socket::main_hand;
        weapon2.socket = Socket::off_hand;
        weapons.emplace_back(weapon1);
        weapons.emplace_back(weapon2);
    }

    void add_enchant(const Socket socket, const Enchant::Type type)
    {
        for (auto &wep : weapons)
        {
            if (socket == wep.socket)
            {
                wep.enchant = Enchant{type};
            }
        }
        for (auto &item : armor)
        {
            if (socket == item.socket)
            {
                item.enchant = Enchant{type};
                return;
            }
        }
    }

    void add_buff(const Buff &buff)
    {
        buffs.emplace_back(buff);
    }

    void add_weapon_buff(const Socket socket, const Weapon_buff &buff)
    {
        if (socket == Socket::main_hand)
        {
            weapons[0].buff = buff;
        }
        else if (socket == Socket::off_hand)
        {
            if (weapons.size() <= 1)
            {
                std::cout << "cant buff offhand with only 1 weapon equipped \n";
                assert(true);
            }
            weapons[1].buff = buff;
        }
    }

    Attributes base_attributes;
    Attributes total_attributes;
    Special_stats base_special_stats;
    Special_stats total_special_stats;
    std::vector<Armor> armor;
    std::vector<Weapon> weapons;
    std::vector<Buff> buffs;
    Race race;
    size_t level;
};

std::ostream &operator<<(std::ostream &os, const Character &socket);

#endif //WOW_SIMULATOR_CHARACTER_HPP
