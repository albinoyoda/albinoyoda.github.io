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
        case Socket::wrist :
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
                case Enchant::Type::greater_agility:
                    return {0, 15};
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
        case Socket::shoulder:
        {
            switch (type)
            {
                case Enchant::Type::attack_power:
                    return {0, 0, 30};
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
        return {"crusader", Hit_effect::Type::stat_boost, {100, 0}, {0, 0, 0}, 0, 15, weapon_speed / 60};
    }
    return {"none", Hit_effect::Type::none, {}, {}, 0, 0, 0};
}

std::vector<Set_bonus> Armory::get_set_bonuses() const
{
    return set_bonuses;
}

void Armory::clean_weapon(Weapon &weapon) const
{
    if (!weapon.hit_effects.empty())
    {
        auto temp = weapon.hit_effects;
        weapon.hit_effects = {};
        for (const auto &hit_effect : temp)
        {
            if (hit_effect.name == weapon.name)
            {
                weapon.hit_effects.emplace_back(hit_effect);
            }
        }
    }
}

void Armory::compute_total_stats(Character &character) const
{
    if (!check_if_weapons_valid(character.weapons))
    {
        std::cout << "invalid weapon setup";
        assert(false);
    }
    if (!check_if_armor_valid(character.armor))
    {
        std::cout << "invalid armor setup";
        assert(false);
    }
    character.total_attributes = {};
    character.total_special_stats = {};
    character.set_bonuses = {};
    clean_weapon(character.weapons[0]);
    clean_weapon(character.weapons[1]);
    Attributes total_attributes{};
    Special_stats total_special_stats{};

    total_attributes += character.base_attributes;
    total_special_stats += character.base_special_stats;
    std::vector<Set> set_names{};
    std::vector<Use_effect> use_effects{};
    for (const Armor &armor : character.armor)
    {
        total_attributes += armor.attributes;
        total_special_stats += armor.special_stats;

        total_attributes += get_enchant_attributes(armor.socket, armor.enchant.type);
        total_special_stats += get_enchant_special_stats(armor.socket, armor.enchant.type);

        set_names.emplace_back(armor.set_name);
        for (const auto &use_effect : armor.use_effects)
        {
            use_effects.emplace_back(use_effect);
        }
        for (const auto &hit_effect : armor.hit_effects)
        {
            for (Weapon &weapon : character.weapons)
            {
                weapon.hit_effects.emplace_back(hit_effect);
            }
        }
    }

    for (Weapon &weapon : character.weapons)
    {
        total_attributes += weapon.attributes;
        total_special_stats += weapon.special_stats;

        total_attributes += get_enchant_attributes(weapon.socket, weapon.enchant.type);
        total_special_stats += get_enchant_special_stats(weapon.socket, weapon.enchant.type);

        auto hit_effect = enchant_hit_effect(weapon.swing_speed, weapon.enchant.type);
        if (hit_effect.type != Hit_effect::Type::none)
        {
            weapon.hit_effects.emplace_back(enchant_hit_effect(weapon.swing_speed, weapon.enchant.type));
        }

        set_names.emplace_back(weapon.set_name);
    }

    std::vector<Set> unique_set_names{};
    for (const Set &set_name : set_names)
    {
        if (set_name != Set::none)
        {
            bool unique = true;
            for (const Set &set_name_unique : unique_set_names)
            {
                if (set_name == set_name_unique)
                {
                    unique = false;
                }
            }
            if (unique)
            {
                unique_set_names.emplace_back(set_name);
            }
        }
    }

    for (const Set &unique_set_name : unique_set_names)
    {
        int count = 0;
        for (const Set &set_name : set_names)
        {
            if (set_name == unique_set_name)
            {
                count++;
            }
        }
        for (const Set_bonus &set_bonus: set_bonuses)
        {
            if (set_bonus.set == unique_set_name && count >= set_bonus.pieces)
            {
                total_attributes += set_bonus.attributes;
                total_special_stats += set_bonus.special_stats;
                character.set_bonuses.emplace_back(set_bonus);
            }
        }
    }

    for (const auto &buff : character.buffs)
    {
        total_attributes += buff.attributes;
        total_special_stats += buff.special_stats;

        for (const auto &use_effect : buff.use_effects)
        {
            use_effects.emplace_back(use_effect);
        }

        for (const auto &hit_effect : buff.hit_effects)
        {
            for (Weapon &weapon : character.weapons)
            {
                weapon.hit_effects.emplace_back(hit_effect);
            }
        }
    }

    total_special_stats.critical_strike += 5; // crit from talent
    total_special_stats.critical_strike += 3; // crit from berserker stance

    total_special_stats += total_attributes.convert_to_special_stats(total_special_stats);

    character.total_attributes = total_attributes.multiply(total_special_stats);
    character.total_special_stats = total_special_stats;
    character.use_effects = use_effects;
}

bool Armory::check_if_armor_valid(const std::vector<Armor> &armor) const
{
    std::vector<Socket> sockets;
    bool one_ring{false};
    bool one_trinket{false};
    for (auto const &armor_piece : armor)
    {
        for (auto const &socket : sockets)
        {
            if (armor_piece.socket == socket)
            {
                if (armor_piece.socket == Socket::ring)
                {
                    if (armor_piece.socket == Socket::ring && one_ring)
                    {
                        std::cout << "extra copy of " << armor_piece.socket << "\n";
                        return false;
                    }
                    one_ring = true;
                }
                else if (armor_piece.socket == Socket::trinket)
                {
                    if (armor_piece.socket == Socket::trinket && one_trinket)
                    {
                        std::cout << "extra copy of " << armor_piece.socket << "\n";
                        return false;
                    }
                    one_trinket = true;
                }
                else
                {
                    std::cout << "extra copy of " << armor_piece.socket << "\n";
                    return false;
                }
            }
        }
        sockets.emplace_back(armor_piece.socket);
    }
    return true;
}

bool Armory::check_if_weapons_valid(std::vector<Weapon> &weapons) const
{
    bool is_valid{true};
    is_valid &= weapons.size() <= 2;
    is_valid &= (weapons[0].socket != Socket::off_hand);
    if (weapons.size() == 2)
    {
        is_valid &= (weapons[1].socket != Socket::main_hand);
    }
    if (weapons.size() == 1)
    {
        is_valid &= (weapons[0].weapon_socket == Weapon_socket::two_hand);
    }
    return is_valid;
}

void
Armory::change_weapon(std::vector<Weapon> &current_weapons, const Weapon &equip_weapon, const Socket &socket) const
{
    // TODO fix twohanded -> dual wield item swap!
    assert(socket == Socket::main_hand || socket == Socket::off_hand);
    if (equip_weapon.weapon_socket == Weapon_socket::two_hand)
    {
        Weapon &current_wep = current_weapons[0];
        current_weapons.erase(current_weapons.begin() + 1);
        Weapon weapon_copy = equip_weapon;
        weapon_copy.buff = current_wep.buff;
        weapon_copy.enchant = current_wep.enchant;
        weapon_copy.socket = socket;
        current_wep = weapon_copy;
    }
    else
    {
        Weapon &current_wep = (socket == Socket::main_hand) ? current_weapons[0] : current_weapons[1];
        Weapon weapon_copy = equip_weapon;
        weapon_copy.buff = current_wep.buff;
        weapon_copy.enchant = current_wep.enchant;
        weapon_copy.socket = socket;
        current_wep = weapon_copy;
    }
}

void Armory::change_armor(std::vector<Armor> &armor_vec, const Armor &armor, bool first_misc_slot) const
{
    auto socket = armor.socket;
    for (auto &armor_piece : armor_vec)
    {
        if (armor_piece.socket == socket)
        {
            if (socket == Socket::ring || socket == Socket::trinket)
            {
                if (first_misc_slot)
                {
                    armor_piece = armor;
                    return;
                }
                first_misc_slot = true; // Will trigger on the second hit instead
            }
            else
            {
                // Reuse the same enchant
                auto enchant = armor_piece.enchant;
                armor_piece = armor;
                armor_piece.enchant = enchant;
                return;
            }
        }
    }
}

Armor Armory::find_armor(const Socket socket, const std::string &name) const
{
    std::vector<Armor> items;
    switch (socket)
    {
        case Socket::head:
            items = helmet_t;
            break;
        case Socket::neck:
            items = neck_t;
            break;
        case Socket::shoulder:
            items = shoulder_t;
            break;
        case Socket::back:
            items = back_t;
            break;
        case Socket::chest:
            items = chest_t;
            break;
        case Socket::wrist:
            items = wrists_t;
            break;
        case Socket::hands:
            items = hands_t;
            break;
        case Socket::belt:
            items = belt_t;
            break;
        case Socket::legs:
            items = legs_t;
            break;
        case Socket::boots:
            items = boots_t;
            break;
        case Socket::ring:
            items = ring_t;
            break;
        case Socket::trinket:
            items = trinket_t;
            break;
        case Socket::ranged:
            items = ranged_t;
            break;
        default:
            std::cout << "ERROR: incorrect item socket provided: " << socket << "\n";
            assert(false);
            break;
    }
    for (const auto &item : items)
    {
        if (item.name == name)
        {
            return item;
        }
    }
    std::cout << "ERROR: Could not find item: " << name << "\n";
    return {"item_not_found: " + name, {}, {}, socket};
}

Weapon Armory::find_weapon(const std::string &name) const
{
    for (const auto &item : swords_t)
    {
        if (item.name == name)
        {
            return item;
        }
    }
    for (const auto &item : maces_t)
    {
        if (item.name == name)
        {
            return item;
        }
    }
    for (const auto &item : axes_t)
    {
        if (item.name == name)
        {
            return item;
        }
    }
    for (const auto &item : daggers_t)
    {
        if (item.name == name)
        {
            return item;
        }
    }
    for (const auto &item : fists_t)
    {
        if (item.name == name)
        {
            return item;
        }
    }
    std::cout << "ERROR: Could not find item: " << name << "\n";
    return {"item_not_found: " + name, {}, {}, 1.0, 0, 0, Weapon_socket::one_hand, Weapon_type::dagger};
}

