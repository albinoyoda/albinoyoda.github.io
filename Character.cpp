#include <algorithm>
#include "Character.hpp"

Character::Character(const Race &race)
        : permutated_stats_{},
          permutated_special_stats_{},
          base_stats_{},
          total_stats_{},
          total_special_stats_{},
          haste_{1.0},
          weapon_skill_{300},
          chance_for_extra_hit_{0.0}
{
    switch (race)
    {
        case Race::human:
            base_stats_ = Stats{120, 80};
            base_special_stats_ = Special_stats{0, 0, 160};
            weapon_skill_ += 5;
            break;
        case Race::dwarf:
            base_stats_ = Stats{122, 76};
            base_special_stats_ = Special_stats{0, 0, 160};
            break;
        case Race::night_elf:
            base_stats_ = Stats{117, 85};
            base_special_stats_ = Special_stats{0, 0, 160};
            break;
        case Race::gnome:
            base_stats_ = Stats{115, 83};
            base_special_stats_ = Special_stats{0, 0, 160};
            break;
        default:
            assert(false);
    }
}

void Character::compute_all_stats(Talent talent)
{
    total_special_stats_.clear();
    total_stats_.clear();
    haste_ = 1.0;
    chance_for_extra_hit_ = 0.0;
    total_special_stats_ += base_special_stats_;
    total_stats_ += base_stats_;
    total_stats_ += permutated_stats_;
    total_special_stats_ += permutated_special_stats_;
    for (const Item &armor : armor_)
    {
        total_stats_ += armor.get_stats();
        total_special_stats_ += armor.get_special_stats();
        chance_for_extra_hit_ += armor.get_chance_for_extra_hit();
        weapon_skill_ += armor.get_extra_skill();
    }

    for (const Item &weapon : weapons_)
    {
        total_stats_ += weapon.get_stats();
        total_special_stats_ += weapon.get_special_stats();
        chance_for_extra_hit_ += weapon.get_chance_for_extra_hit();
        weapon_skill_ += weapon.get_extra_skill();
    }

    for (const Enchant &ench : enchants_)
    {
        total_stats_ += ench.get_stats();
//        total_special_stats_ += ench.convert_to_special_stats();
        haste_ *= ench.get_haste();
    }

    if (talent == Talent::fury)
    {
        total_special_stats_.attack_power += 241;  // battle shout
        total_special_stats_.critical_strike += 5; // crit from talent
        total_special_stats_.critical_strike += 3; // crit from talent
    }
    total_special_stats_ += total_stats_.convert_to_special_stats();
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
    return total_stats_;
}

double Character::get_chance_for_extra_hit() const
{
    return chance_for_extra_hit_;
}

void Character::set_stats(const Stats &stats)
{
    total_stats_ = stats;
}

void Character::set_total_special_stats(const Special_stats &total_special_stats)
{
    total_special_stats_ = total_special_stats;
}

void Character::set_weapon_skill(int weapon_skill)
{
    weapon_skill_ = weapon_skill;
}

void Character::set_chance_for_extra_hit(double chance_for_extra_hit)
{
    Character::chance_for_extra_hit_ = chance_for_extra_hit;
}

void Character::set_haste(double haste)
{
    haste_ = haste;
}







