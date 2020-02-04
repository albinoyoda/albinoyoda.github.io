#include <algorithm>
#include "Character.hpp"

Character::Character(const Race &race)
        : stats_{},
          total_special_stats_{},
          haste_{1.0},
          weapon_skill_{300},
          chance_for_extra_hit{0.0}
{
    stats_.clear();
    switch (race)
    {
        case Race::human:
            stats_ += Stats{120, 80};
            total_special_stats_ += Special_stats{5, 0, 0};
            weapon_skill_ += 5;
            break;
        case Race::dwarf:
            stats_ += Stats{122, 76};
            total_special_stats_ += Special_stats{5, 0, 0};
            break;
        case Race::night_elf:
            stats_ += Stats{117, 85};
            total_special_stats_ += Special_stats{5, 0, 0};
            break;
        case Race::gnome:
            stats_ += Stats{115, 83};
            total_special_stats_ += Special_stats{5, 0, 0};
            break;
        default:
            assert(false);
    }
}

void Character::compute_all_stats(Talent talent)
{
    total_special_stats_.clear();
    for (const Item &armor : armor_)
    {
        stats_ += armor.get_stats();
        total_special_stats_ += armor.get_special_stats();
        chance_for_extra_hit += armor.get_chance_for_extra_hit();
        weapon_skill_ += armor.get_extra_skill();
    }

    for (const Item &weapon : weapons_)
    {
        stats_ += weapon.get_stats();
        total_special_stats_ += weapon.get_special_stats();
        chance_for_extra_hit += weapon.get_chance_for_extra_hit();
        weapon_skill_ += weapon.get_extra_skill();
    }

    for (const Enchant &ench : enchants_)
    {
        stats_ += ench.get_stats();
//        total_special_stats_ += ench.convert_to_special_stats();
        haste_ *= ench.get_haste();
    }

    if (talent == Talent::fury)
    {
        total_special_stats_.attack_power += 231;  // battle shout
        total_special_stats_.critical_strike += 5; // crit from talent
        total_special_stats_.critical_strike += 3; // crit from talent
    }
    total_special_stats_ += stats_.convert_to_special_stats();
}

const Special_stats &Character::get_total_special_stats() const
{
    return total_special_stats_;
}

int Character::get_weapon_skill() const
{
    return weapon_skill_;
}

const std::vector<Weapon> &Character::get_weapons() const
{
    return weapons_;
}

const std::vector<Armor> &Character::get_gear() const
{
    return armor_;
}

bool Character::check_if_gear_valid()
{
    bool is_unique{true};
    {
        std::vector<Armor::Socket> sockets;
        for (auto const &armor_piece : armor_)
        {
            sockets.emplace_back(armor_piece.get_socket());
        }
        auto it = std::unique(sockets.begin(), sockets.end());
        is_unique &= (it == sockets.end());
    }
    {
        std::vector<Weapon::Socket> sockets;
        for (auto const &weapon : weapons_)
        {
            sockets.emplace_back(weapon.get_socket());
        }
        auto it = std::unique(sockets.begin(), sockets.end());
        is_unique &= (it == sockets.end());
    }
    return is_unique;
}

double Character::get_haste() const
{
    return haste_;
}

const Stats &Character::get_stats() const
{
    return stats_;
}

double Character::get_chance_for_extra_hit() const
{
    return chance_for_extra_hit;
}







