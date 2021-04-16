#include "Armory.hpp"

#include "Character.hpp"
#include "Helper_functions.hpp"

Attributes Armory::get_enchant_attributes(Socket socket, Enchant::Type type) const
{
    switch (socket)
    {
    case Socket::head: {
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
    case Socket::back: {
        switch (type)
        {
        case Enchant::Type::agility:
            return {0, 3};
        default:
            return {0, 0};
        }
    }
    case Socket::chest: {
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
    case Socket::wrist: {
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
    case Socket::hands: {
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
    case Socket::legs: {
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
    case Socket::boots: {
        switch (type)
        {
        case Enchant::Type::agility:
            return {0, 7};
        default:
            return {0, 0};
        }
    }
    case Socket::main_hand:
    case Socket::off_hand: {
        switch (type)
        {
        case Enchant::Type::agility:
            return {0, 15};
        case Enchant::Type::strength:
            return {15, 0};
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
    case Socket::head: {
        switch (type)
        {
        case Enchant::Type::haste:
            return {0, 0, 0, 0, .01};
        default:
            return {0, 0, 0};
        }
    }
    case Socket::shoulder: {
        switch (type)
        {
        case Enchant::Type::attack_power:
            return {0, 0, 30};
        case Enchant::Type::naxxramas:
            return {1, 0, 26};
        default:
            return {0, 0, 0};
        }
    }
    case Socket::hands: {
        switch (type)
        {
        case Enchant::Type::haste:
            return {0, 0, 0, 0, .01};
        default:
            return {0, 0, 0};
        }
    }
    case Socket::legs: {
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

void Armory::clean_weapon(Weapon& weapon) const
{
    // TODO remove from armory.
    if (!weapon.hit_effects.empty())
    {
        auto temp = weapon.hit_effects;
        weapon.hit_effects = {};
        for (const auto& hit_effect : temp)
        {
            if (hit_effect.name == weapon.name)
            {
                weapon.hit_effects.emplace_back(hit_effect);
            }
        }
    }
}

void Armory::compute_total_stats(Character& character) const
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
    for (auto& wep : character.weapons)
    {
        clean_weapon(wep);
    }
    Attributes total_attributes{};
    Special_stats total_special_stats{};

    total_attributes += character.base_attributes;
    total_special_stats += character.base_special_stats;
    std::vector<Set> set_names{};
    std::vector<Use_effect> use_effects{};
    for (const Armor& armor : character.armor)
    {
        total_attributes += armor.attributes;
        total_special_stats += armor.special_stats;

        total_attributes += get_enchant_attributes(armor.socket, armor.enchant.type);
        total_special_stats += get_enchant_special_stats(armor.socket, armor.enchant.type);

        set_names.emplace_back(armor.set_name);
        for (const auto& use_effect : armor.use_effects)
        {
            use_effects.emplace_back(use_effect);
        }
        for (const auto& hit_effect : armor.hit_effects)
        {
            for (Weapon& weapon : character.weapons)
            {
                weapon.hit_effects.emplace_back(hit_effect);
            }
        }
    }

    for (Weapon& weapon : character.weapons)
    {
        total_attributes += weapon.attributes;
        total_special_stats += weapon.special_stats;

        total_attributes += get_enchant_attributes(weapon.socket, weapon.enchant.type);
        total_special_stats += get_enchant_special_stats(weapon.socket, weapon.enchant.type);

        for (const auto& use_effect : weapon.use_effects)
        {
            use_effects.emplace_back(use_effect);
        }
        auto hit_effect = enchant_hit_effect(weapon.swing_speed, weapon.enchant.type);
        if (hit_effect.type != Hit_effect::Type::none)
        {
            weapon.hit_effects.emplace_back(hit_effect);
        }

        set_names.emplace_back(weapon.set_name);
    }

    std::vector<Set> unique_set_names{};
    for (const Set& set_name : set_names)
    {
        if (set_name != Set::none)
        {
            bool unique = true;
            for (const Set& set_name_unique : unique_set_names)
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

    for (const Set& unique_set_name : unique_set_names)
    {
        int count = 0;
        for (const Set& set_name : set_names)
        {
            if (set_name == unique_set_name)
            {
                count++;
            }
        }
        for (const Set_bonus& set_bonus : set_bonuses)
        {
            if (set_bonus.set == unique_set_name && count >= set_bonus.pieces)
            {
                total_attributes += set_bonus.attributes;
                total_special_stats += set_bonus.special_stats;
                character.set_bonuses.emplace_back(set_bonus);
            }
        }
    }

    for (const auto& buff : character.buffs)
    {
        total_attributes += buff.attributes;
        total_special_stats += buff.special_stats;

        for (const auto& use_effect : buff.use_effects)
        {
            use_effects.emplace_back(use_effect);
        }

        for (const auto& hit_effect : buff.hit_effects)
        {
            if (hit_effect.name != "windfury_totem")
            {
                for (Weapon& weapon : character.weapons)
                {
                    weapon.hit_effects.emplace_back(hit_effect);
                }
            }
            else
            {
                character.weapons[0].hit_effects.emplace_back(hit_effect);
            }
        }
    }

    // Effects gained from talents
    for (auto& use_effect : use_effects)
    {
        if (use_effect.name == "battle_shout" || use_effect.name == "battle_shout_aq")
        {
            if (character.talents.booming_voice_talent)
            {
                use_effect.duration = 180.0;
            }
            if (character.talents.improved_battle_shout_talent > 0)
            {
                use_effect.special_stats_boost.attack_power *=
                    1.0 + 0.05 * character.talents.improved_battle_shout_talent;
            }
            break;
        }
    }

    if (character.talents.sword_specialization > 0)
    {
        for (auto& wep : character.weapons)
        {
            if (wep.type == Weapon_type::sword)
            {
                double prob = double(character.talents.sword_specialization) / 100.0;
                wep.hit_effects.push_back({"sword_specialization", Hit_effect::Type::extra_hit, {}, {}, 0, 0, prob});
            }
        }
    }

    if (character.talents.one_handed_weapon_specialization > 0 && character.is_dual_wield())
    {
        double multiplier = double(character.talents.one_handed_weapon_specialization) / 50.0;
        character.talent_special_stats += {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, multiplier};
    }

    // Cruelty etc.
    total_special_stats += character.talent_special_stats;
    total_special_stats.critical_strike += 3; // crit from berserker stance

    total_special_stats += total_attributes.convert_to_special_stats(total_special_stats);

    character.total_attributes = total_attributes.multiply(total_special_stats);
    character.total_special_stats = total_special_stats;
    character.use_effects = use_effects;
}

bool Armory::check_if_armor_valid(const std::vector<Armor>& armor) const
{
    std::vector<Socket> sockets;
    bool one_ring{false};
    bool one_trinket{false};
    for (auto const& armor_piece : armor)
    {
        for (auto const& socket : sockets)
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

bool Armory::check_if_weapons_valid(std::vector<Weapon>& weapons) const
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

void Armory::change_weapon(std::vector<Weapon>& current_weapons, const Weapon& equip_weapon, const Socket& socket) const
{
    // TODO fix twohanded -> dual wield item swap!
    if (equip_weapon.weapon_socket == Weapon_socket::two_hand)
    {
        Weapon& current_wep = current_weapons[0];
        Weapon weapon_copy = equip_weapon;
        weapon_copy.buff = current_wep.buff;
        weapon_copy.enchant = current_wep.enchant;
        weapon_copy.socket = socket;
        current_wep = weapon_copy;
    }
    else
    {
        Weapon& current_wep = (socket == Socket::main_hand) ? current_weapons[0] : current_weapons[1];
        Weapon weapon_copy = equip_weapon;
        weapon_copy.buff = current_wep.buff;
        weapon_copy.enchant = current_wep.enchant;
        weapon_copy.socket = socket;
        current_wep = weapon_copy;
    }
}

void Armory::change_armor(std::vector<Armor>& armor_vec, const Armor& armor, bool first_misc_slot) const
{
    auto socket = armor.socket;
    for (auto& armor_piece : armor_vec)
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

std::vector<Weapon> Armory::get_weapon_in_socket(const Weapon_socket socket) const
{
    switch (socket)
    {
    case Weapon_socket::main_hand: {
        std::vector<Weapon> mh_weapons{};
        for (const auto& wep : swords_t)
        {
            if (wep.weapon_socket == Weapon_socket::main_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                mh_weapons.emplace_back(wep);
            }
        }
        for (const auto& wep : axes_t)
        {
            if (wep.weapon_socket == Weapon_socket::main_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                mh_weapons.emplace_back(wep);
            }
        }
        for (const auto& wep : maces_t)
        {
            if (wep.weapon_socket == Weapon_socket::main_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                mh_weapons.emplace_back(wep);
            }
        }
        for (const auto& wep : daggers_t)
        {
            if (wep.weapon_socket == Weapon_socket::main_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                mh_weapons.emplace_back(wep);
            }
        }
        for (const auto& wep : fists_t)
        {
            if (wep.weapon_socket == Weapon_socket::main_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                mh_weapons.emplace_back(wep);
            }
        }
        return mh_weapons;
    }
    case Weapon_socket::off_hand: {
        std::vector<Weapon> oh_weapons{};
        for (const auto& wep : swords_t)
        {
            if (wep.weapon_socket == Weapon_socket::off_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                oh_weapons.emplace_back(wep);
            }
        }
        for (const auto& wep : axes_t)
        {
            if (wep.weapon_socket == Weapon_socket::off_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                oh_weapons.emplace_back(wep);
            }
        }
        for (const auto& wep : maces_t)
        {
            if (wep.weapon_socket == Weapon_socket::off_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                oh_weapons.emplace_back(wep);
            }
        }
        for (const auto& wep : daggers_t)
        {
            if (wep.weapon_socket == Weapon_socket::off_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                oh_weapons.emplace_back(wep);
            }
        }
        for (const auto& wep : fists_t)
        {
            if (wep.weapon_socket == Weapon_socket::off_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                oh_weapons.emplace_back(wep);
            }
        }
        return oh_weapons;
    }
    case Weapon_socket ::two_hand: {
        std::vector<Weapon> th_weapons{};
        for (const auto& wep : two_handed_swords_t)
        {
            th_weapons.emplace_back(wep);
        }
        for (const auto& wep : two_handed_maces_t)
        {
            th_weapons.emplace_back(wep);
        }
        for (const auto& wep : two_handed_axes_t)
        {
            th_weapons.emplace_back(wep);
        }
        return th_weapons;
    }
    default:
        std::cout << "ERROR: incorrect weapon socket provided!\n";
        return swords_t;
    }
}
std::vector<Armor> Armory::get_items_in_socket(const Socket socket) const
{
    switch (socket)
    {
    case Socket::head:
        return helmet_t;
    case Socket::neck:
        return neck_t;
    case Socket::shoulder:
        return shoulder_t;
    case Socket::back:
        return back_t;
    case Socket::chest:
        return chest_t;
    case Socket::wrist:
        return wrists_t;
    case Socket::hands:
        return hands_t;
    case Socket::belt:
        return belt_t;
    case Socket::legs:
        return legs_t;
    case Socket::boots:
        return boots_t;
    case Socket::ring:
        return ring_t;
    case Socket::trinket:
        return trinket_t;
    case Socket::ranged:
        return ranged_t;
    default:
        std::cout << "ERROR: incorrect item socket provided: " << socket << "\n";
        assert(false);
        return ranged_t;
    }
}

Armor Armory::find_armor(const Socket socket, const std::string& name) const
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
    for (const auto& item : items)
    {
        if (item.name == name)
        {
            return item;
        }
    }
    return {"item_not_found: " + name, {}, {}, socket};
}

Weapon Armory::find_weapon(Weapon_socket socket, const std::string& name) const
{
    if (socket == Weapon_socket::two_hand)
    {
        for (const auto& item : two_handed_swords_t)
        {
            if (item.name == name)
            {
                return item;
            }
        }
        for (const auto& item : two_handed_maces_t)
        {
            if (item.name == name)
            {
                return item;
            }
        }
        for (const auto& item : two_handed_axes_t)
        {
            if (item.name == name)
            {
                return item;
            }
        }
        return {"item_not_found: " + name, {}, {}, 3.0, 0, 0, Weapon_socket::two_hand, Weapon_type::sword};
    }
    for (const auto& item : swords_t)
    {
        if (item.name == name)
        {
            return item;
        }
    }
    for (const auto& item : maces_t)
    {
        if (item.name == name)
        {
            return item;
        }
    }
    for (const auto& item : axes_t)
    {
        if (item.name == name)
        {
            return item;
        }
    }
    for (const auto& item : daggers_t)
    {
        if (item.name == name)
        {
            return item;
        }
    }
    for (const auto& item : fists_t)
    {
        if (item.name == name)
        {
            return item;
        }
    }
    return {"item_not_found: " + name, {}, {}, 2.0, 0, 0, Weapon_socket::one_hand, Weapon_type::unarmed};
}

void Armory::add_enchants_to_character(Character& character, const std::vector<std::string>& ench_vec) const
{
    if (find_string(ench_vec, "e+8 strength"))
    {
        character.add_enchant(Socket::head, Enchant::Type::strength);
    }
    else if (find_string(ench_vec, "e+1 haste"))
    {
        character.add_enchant(Socket::head, Enchant::Type::haste);
    }

    if (find_string(ench_vec, "s+30 attack_power"))
    {
        character.add_enchant(Socket::shoulder, Enchant::Type::attack_power);
    }
    else if (find_string(ench_vec, "snaxxramas"))
    {
        character.add_enchant(Socket::shoulder, Enchant::Type::naxxramas);
    }

    if (find_string(ench_vec, "b+3 agility"))
    {
        character.add_enchant(Socket::back, Enchant::Type::agility);
    }

    if (find_string(ench_vec, "c+3 stats"))
    {
        character.add_enchant(Socket::chest, Enchant::Type::minor_stats);
    }
    else if (find_string(ench_vec, "c+4 stats"))
    {
        character.add_enchant(Socket::chest, Enchant::Type::major_stats);
    }

    if (find_string(ench_vec, "w+7 strength"))
    {
        character.add_enchant(Socket::wrist, Enchant::Type::strength7);
    }
    else if (find_string(ench_vec, "w+9 strength"))
    {
        character.add_enchant(Socket::wrist, Enchant::Type::strength9);
    }

    if (find_string(ench_vec, "h+7 strength"))
    {
        character.add_enchant(Socket::hands, Enchant::Type::strength);
    }
    else if (find_string(ench_vec, "h+7 agility"))
    {
        character.add_enchant(Socket::hands, Enchant::Type::agility);
    }
    else if (find_string(ench_vec, "h+15 agility"))
    {
        character.add_enchant(Socket::hands, Enchant::Type::greater_agility);
    }
    else if (find_string(ench_vec, "h+1 haste"))
    {
        character.add_enchant(Socket::hands, Enchant::Type::haste);
    }

    if (find_string(ench_vec, "l+8 strength"))
    {
        character.add_enchant(Socket::legs, Enchant::Type::strength);
    }
    else if (find_string(ench_vec, "l+1 haste"))
    {
        character.add_enchant(Socket::legs, Enchant::Type::haste);
    }

    if (find_string(ench_vec, "b+7 agility"))
    {
        character.add_enchant(Socket::boots, Enchant::Type::agility);
    }

    if (find_string(ench_vec, "mcrusader"))
    {
        character.add_enchant(Socket::main_hand, Enchant::Type::crusader);
    }
    else if (find_string(ench_vec, "m+15 agility"))
    {
        character.add_enchant(Socket::main_hand, Enchant::Type::agility);
    }
    else if (find_string(ench_vec, "m+15 strength"))
    {
        character.add_enchant(Socket::main_hand, Enchant::Type::strength);
    }

    if (find_string(ench_vec, "ocrusader"))
    {
        character.add_enchant(Socket::off_hand, Enchant::Type::crusader);
    }
    else if (find_string(ench_vec, "o+15 agility"))
    {
        character.add_enchant(Socket::off_hand, Enchant::Type::agility);
    }
    else if (find_string(ench_vec, "o+15 strength"))
    {
        character.add_enchant(Socket::off_hand, Enchant::Type::strength);
    }
}

void Armory::add_buffs_to_character(Character& character, const std::vector<std::string>& buffs_vec) const
{
    if (find_string(buffs_vec, "fungal_bloom"))
    {
        character.add_buff(buffs.fungal_bloom);
    }
    if (find_string(buffs_vec, "full_polarity"))
    {
        character.add_buff(buffs.full_polarity);
    }
    if (find_string(buffs_vec, "battle_squawk"))
    {
        character.add_buff(buffs.battle_squawk);
    }
    if (find_string(buffs_vec, "rallying_cry"))
    {
        character.add_buff(buffs.rallying_cry);
    }
    if (find_string(buffs_vec, "dire_maul"))
    {
        character.add_buff(buffs.dire_maul);
    }
    if (find_string(buffs_vec, "slipkiks_savy"))
    {
        character.add_buff(buffs.slipkiks_savy);
    }
    if (find_string(buffs_vec, "songflower"))
    {
        character.add_buff(buffs.songflower);
    }
    if (find_string(buffs_vec, "warchiefs_blessing"))
    {
        character.add_buff(buffs.warchiefs_blessing);
    }
    if (find_string(buffs_vec, "spirit_of_zandalar"))
    {
        character.add_buff(buffs.spirit_of_zandalar);
    }
    if (find_string(buffs_vec, "sayges_fortune"))
    {
        character.add_buff(buffs.sayges_fortune);
    }
    if (find_string(buffs_vec, "traces_of_silithyst"))
    {
        character.add_buff(buffs.traces_of_silithyst);
    }

    // Player buffs
    if (find_string(buffs_vec, "battle_shout"))
    {
        if (find_string(buffs_vec, "battle_shout_aq"))
        {
            character.add_buff(buffs.battle_shout_aq);
        }
        else
        {
            character.add_buff(buffs.battle_shout);
        }
    }
    if (find_string(buffs_vec, "blessing_of_kings"))
    {
        character.add_buff(buffs.blessing_of_kings);
    }
    if (find_string(buffs_vec, "blessing_of_might"))
    {
        if (find_string(buffs_vec, "blessing_of_might_aq"))
        {
            character.add_buff(buffs.blessing_of_might_aq);
        }
        else
        {
            character.add_buff(buffs.blessing_of_might);
        }
    }
    if (find_string(buffs_vec, "windfury_totem"))
    {
        Buff totem = buffs.windfury_totem;
        if (find_string(buffs_vec, "improved_weapon_totems"))
        {
            totem.hit_effects[0].attack_power_boost *= 1.3;
        }
        character.add_buff(totem);
    }
    if (find_string(buffs_vec, "strength_of_earth_totem"))
    {
        Buff totem = (find_string(buffs_vec, "strength_of_earth_totem_aq")) ? buffs.strength_of_earth_totem_aq :
                                                                              buffs.strength_of_earth_totem;
        if (find_string(buffs_vec, "enhancing_totems"))
        {
            totem.attributes.strength *= 1.15;
        }
        character.add_buff(totem);
    }
    if (find_string(buffs_vec, "grace_of_air_totem"))
    {
        Buff totem =
            (find_string(buffs_vec, "grace_of_air_totem_aq")) ? buffs.grace_of_air_totem_aq : buffs.grace_of_air_totem;
        if (find_string(buffs_vec, "enhancing_totems"))
        {
            totem.attributes.agility *= 1.15;
        }
        character.add_buff(totem);
    }
    if (find_string(buffs_vec, "gift_of_the_wild"))
    {
        character.add_buff(buffs.gift_of_the_wild);
    }
    if (find_string(buffs_vec, "leader_of_the_pack"))
    {
        character.add_buff(buffs.leader_of_the_pack);
    }
    if (find_string(buffs_vec, "trueshot_aura"))
    {
        character.add_buff(buffs.trueshot_aura);
    }
    if (find_string(buffs_vec, "elixir_mongoose"))
    {
        character.add_buff(buffs.elixir_mongoose);
    }
    if (find_string(buffs_vec, "blessed_sunfruit"))
    {
        character.add_buff(buffs.blessed_sunfruit);
    }
    if (find_string(buffs_vec, "smoked_dessert_dumplings"))
    {
        character.add_buff(buffs.smoked_dessert_dumplings);
    }
    if (find_string(buffs_vec, "juju_power"))
    {
        character.add_buff(buffs.juju_power);
    }
    if (find_string(buffs_vec, "elixir_of_giants"))
    {
        character.add_buff(buffs.elixir_of_giants);
    }
    if (find_string(buffs_vec, "juju_might"))
    {
        character.add_buff(buffs.juju_might);
    }
    if (find_string(buffs_vec, "winterfall_firewater"))
    {
        character.add_buff(buffs.winterfall_firewater);
    }
    if (find_string(buffs_vec, "roids"))
    {
        character.add_buff(buffs.roids);
    }
    if (find_string(buffs_vec, "fire_toasted_bun"))
    {
        character.add_buff(buffs.fire_toasted_bun);
    }
    if (find_string(buffs_vec, "mighty_rage_potion"))
    {
        character.add_buff(buffs.mighty_rage_potion);
    }

    if (find_string(buffs_vec, "dense_stone_main_hand"))
    {
        character.add_weapon_buff(Socket::main_hand, buffs.dense_stone);
    }
    else if (find_string(buffs_vec, "elemental_stone_main_hand"))
    {
        character.add_buff(buffs.elemental_stone);
    }
    else if (find_string(buffs_vec, "consecrated_sharpening_stone_main_hand"))
    {
        character.add_buff(buffs.consecrated_sharpening_stone);
    }

    if (find_string(buffs_vec, "dense_stone_off_hand"))
    {
        character.add_weapon_buff(Socket::off_hand, buffs.dense_stone);
    }
    else if (find_string(buffs_vec, "elemental_stone_off_hand"))
    {
        character.add_buff(buffs.elemental_stone);
    }
    else if (find_string(buffs_vec, "consecrated_sharpening_stone_off_hand"))
    {
        character.add_buff(buffs.consecrated_sharpening_stone);
    }
}

void Armory::add_talents_to_character(Character& character, const std::vector<std::string>& talent_string,
                                      const std::vector<int>& talent_val) const
{
    Find_values<int> fv{talent_string, talent_val};
    int val = fv.find("cruelty_talent");
    if (val > 0)
    {
        character.talent_special_stats.critical_strike += val;
    }
    val = fv.find("two_handed_weapon_specialization_talent");
    if (val > 0)
    {
        if (!character.is_dual_wield())
        {
            double amount = double(val) / 100.0;
            character.talent_special_stats += Special_stats{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, amount};
        }
    }
    val = fv.find("axe_specialization_talent");
    if (val > 0)
    {
        if (character.weapons[0].type == Weapon_type::axe)
        {
            character.talent_special_stats.critical_strike += val;
        }
    }
    val = fv.find("booming_voice_talent");
    if (val > 0)
    {
        character.talents.booming_voice_talent = true;
    }
    val = fv.find("improved_battle_shout_talent");
    if (val > 0)
    {
        character.talents.improved_battle_shout_talent = val;
    }

    val = fv.find("sword_specialization_talent");
    if (val > 0)
    {
        character.talents.sword_specialization = val;
    }

    val = fv.find("one_handed_weapon_specialization_talent");
    if (val > 0)
    {
        character.talents.one_handed_weapon_specialization = val;
    }
}