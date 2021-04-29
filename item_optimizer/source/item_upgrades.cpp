#include "item_upgrades.hpp"

std::vector<Item_upgrade_result> Item_upgrades::get_armor_upgrades(const Character& character, Socket socket,
                                                                   bool first_item)
{
    Character character_copy = character;
    const auto& special_stats = character_copy.total_special_stats;
    auto items = armory.get_items_in_socket(socket);
    if (socket != Socket::trinket)
    {
        std::string debug_string;
        items =
            item_optimizer.remove_weaker_items(items, special_stats, debug_string, config.number_of_items_to_consider);
    }
    {
        size_t i = 0;
        while (i < items.size())
        {
            if (character_copy.has_item(items[i].name))
            {
                items.erase(items.begin() + i);
            }
            else
            {
                i++;
            }
        }
    }

    return get_item_upgrades(character_copy, items, socket, first_item);
}
std::vector<Item_upgrade_result> Item_upgrades::get_weapon_upgrades(const Character& character,
                                                                    Weapon_socket weapon_socket)
{
    Character character_copy = character;
    Socket socket = ((weapon_socket == Weapon_socket::main_hand) || (weapon_socket == Weapon_socket::two_hand)) ?
                        Socket::main_hand :
                        Socket::off_hand;
    std::string dummy;
    auto items = armory.get_weapon_in_socket(weapon_socket);
    items = item_optimizer.remove_weaker_weapons(weapon_socket, items, character.total_special_stats, dummy, 10);
    {
        size_t i = 0;
        while (i < items.size())
        {
            if (character_copy.has_item(items[i].name))
            {
                items.erase(items.begin() + i);
            }
            else
            {
                i++;
            }
        }
    }

    return get_item_upgrades(character_copy, items, socket);
}
