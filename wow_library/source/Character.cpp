#include "Character.hpp"

#include <iostream>

Character::Character(const Race& race, int level)
    : base_attributes{}, total_attributes{}, base_special_stats{}, total_special_stats{}, race{race}, level(level)
{
    int base_skill = level * 5;
    base_special_stats = Special_stats{0,          0,          160, 0, 0, base_skill, base_skill, base_skill,
                                       base_skill, base_skill, 0,   0, 0, base_skill, base_skill, base_skill};
    switch (race)
    {
    case Race::human:
        base_attributes = Attributes{120, 80};
        base_special_stats.sword_skill += 5;
        base_special_stats.two_hand_sword_skill += 5;
        base_special_stats.mace_skill += 5;
        base_special_stats.two_hand_mace_skill += 5;
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
        base_special_stats.axe_skill += 5;
        base_special_stats.two_hand_axe_skill += 5;
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

void Character::replace_armor(const Armor& piece_to_equip, bool first_misc_slot)
{
    auto socket = piece_to_equip.socket;
    for (auto& armor_piece : armor)
    {
        if (armor_piece.socket == socket)
        {
            if (socket == Socket::ring || socket == Socket::trinket)
            {
                if (first_misc_slot)
                {
                    armor_piece = piece_to_equip;
                    return;
                }
                first_misc_slot = true; // Will trigger on the second hit instead
            }
            else
            {
                // Reuse the same enchant
                auto enchant = armor_piece.enchant;
                armor_piece = piece_to_equip;
                armor_piece.enchant = enchant;
                return;
            }
        }
    }
}

Character character_setup(const Armory& armory, const std::string& race, const std::vector<std::string>& armor_vec,
                          const std::vector<std::string>& weapons_vec, const std::vector<std::string>& buffs_vec,
                          const std::vector<std::string>& talent_string, const std::vector<int>& talent_val,
                          const std::vector<std::string>& ench_vec)
{
    auto character = get_character_of_race(race);

    character.equip_armor(armory.find_armor(Socket::head, armor_vec[0]));
    character.equip_armor(armory.find_armor(Socket::neck, armor_vec[1]));
    character.equip_armor(armory.find_armor(Socket::shoulder, armor_vec[2]));
    character.equip_armor(armory.find_armor(Socket::back, armor_vec[3]));
    character.equip_armor(armory.find_armor(Socket::chest, armor_vec[4]));
    character.equip_armor(armory.find_armor(Socket::wrist, armor_vec[5]));
    character.equip_armor(armory.find_armor(Socket::hands, armor_vec[6]));
    character.equip_armor(armory.find_armor(Socket::belt, armor_vec[7]));
    character.equip_armor(armory.find_armor(Socket::legs, armor_vec[8]));
    character.equip_armor(armory.find_armor(Socket::boots, armor_vec[9]));
    character.equip_armor(armory.find_armor(Socket::ring, armor_vec[10]));
    character.equip_armor(armory.find_armor(Socket::ring, armor_vec[11]));
    character.equip_armor(armory.find_armor(Socket::trinket, armor_vec[12]));
    character.equip_armor(armory.find_armor(Socket::trinket, armor_vec[13]));
    character.equip_armor(armory.find_armor(Socket::ranged, armor_vec[14]));

    if (weapons_vec.size() > 1)
    {
        character.equip_weapon(armory.find_weapon(Weapon_socket::one_hand, weapons_vec[0]),
                               armory.find_weapon(Weapon_socket::one_hand, weapons_vec[1]));
    }
    else
    {
        character.equip_weapon(armory.find_weapon(Weapon_socket::two_hand, weapons_vec[0]));
    }

    armory.add_enchants_to_character(character, ench_vec);
    armory.add_buffs_to_character(character, buffs_vec);
    armory.add_talents_to_character(character, talent_string, talent_val);

    armory.compute_total_stats(character);

    return character;
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

Race get_race(const std::string& race)
{
    if (race == "human")
    {
        return Race::human;
    }
    else if (race == "gnome")
    {
        return Race::gnome;
    }
    else if (race == "dwarf")
    {
        return Race::dwarf;
    }
    else if (race == "night_elf")
    {
        return Race::night_elf;
    }
    else if (race == "orc")
    {
        return Race::orc;
    }
    else if (race == "troll")
    {
        return Race::troll;
    }
    else if (race == "undead")
    {
        return Race::undead;
    }
    else if (race == "tauren")
    {
        return Race::tauren;
    }
    else
    {
        std::cout << "Race not found!!! picking human"
                  << "\n";
        return Race::human;
    }
}

Character get_character_of_race(const std::string& race)
{
    if (race == "human")
    {
        return {Race::human, 60};
    }
    else if (race == "gnome")
    {
        return {Race::gnome, 60};
    }
    else if (race == "dwarf")
    {
        return {Race::dwarf, 60};
    }
    else if (race == "night_elf")
    {
        return {Race::night_elf, 60};
    }
    else if (race == "orc")
    {
        return {Race::orc, 60};
    }
    else if (race == "troll")
    {
        return {Race::troll, 60};
    }
    else if (race == "undead")
    {
        return {Race::undead, 60};
    }
    else if (race == "tauren")
    {
        return {Race::tauren, 60};
    }
    else
    {
        std::cout << "Race not found!!! picking human"
                  << "\n";
        return {Race::human, 60};
    }
}