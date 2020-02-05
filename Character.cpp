#include <algorithm>
#include <iostream>
#include "Character.hpp"

Character::Character(const Race &race)
        : permutated_stats_{},
        permutated_special_stats_{},
        base_stats_{},
        total_stats_{},
        total_special_stats_{},
        haste_{1.0},
        crusader_mh_{false},
        crusader_oh_{false},
        weapon_skill_{300},
        chance_for_extra_hit_{0.0},
        stat_multipliers_{1.0},
        oh_bonus_damage_{0.0},
        mh_bonus_damage_{0.0},
        race_{race}
{
    set_base_stats(race_);
}

void Character::set_base_stats(const Race &race)
{
    switch (race)
    {
        case Race::human:
            base_stats_ = Stats{120, 80};
            base_special_stats_ = Special_stats{0, 0, 160};
            weapon_skill_ = 305;
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
    clean_all();
    total_stats_ += base_stats_;
    total_stats_ += permutated_stats_;
    total_special_stats_ += base_special_stats_;
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
        haste_ *= ench.get_haste();
        crusader_mh_ += ench.is_crusader_mh();
        crusader_oh_ += ench.is_crusader_oh();
    }

    for (const auto &buff : buffs_)
    {
        total_stats_ += buff.get_stats();
        total_special_stats_ += buff.get_special_stats();
        stat_multipliers_ *= buff.get_stat_multiplier();
        mh_bonus_damage_ += buff.get_mh_bonus_damage();
        oh_bonus_damage_ += buff.get_oh_bonus_damage();
    }

    if (talent == Talent::fury)
    {
        total_special_stats_.attack_power += 241;  // battle shout
        total_special_stats_.critical_strike += 5; // crit from talent
        total_special_stats_.critical_strike += 3; // crit from talent
    }

    total_stats_ *= stat_multipliers_;

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

bool Character::check_if_armor_valid()
{
    bool is_unique{true};
    std::vector<Armor::Socket> sockets;
    for (auto const &armor_piece : armor_)
    {
        sockets.emplace_back(armor_piece.get_socket());
    }
    auto it = std::unique(sockets.begin(), sockets.end());
    is_unique &= (it == sockets.end());
    return is_unique;
}


bool Character::check_if_weapons_valid()
{
    bool is_unique{true};
    is_unique &= weapons_.size() <= 2;
    is_unique &= !(weapons_[0].get_socket() == Weapon::Socket::off_hand);
    is_unique &= !(weapons_[1].get_socket() == Weapon::Socket::main_hand);
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

bool Character::is_crusader_mh() const
{
    return crusader_mh_;
}

bool Character::is_crusader_oh() const
{
    return crusader_oh_;
}

double Character::get_oh_bonus_damage() const
{
    return oh_bonus_damage_;
}

double Character::get_mh_bonus_damage() const
{
    return mh_bonus_damage_;
}

void Character::clean_all()
{
    total_special_stats_.clear();
    total_stats_.clear();
    set_base_stats(race_);
    haste_ = 1.0;
    crusader_mh_ = false;
    crusader_oh_ = false;
    chance_for_extra_hit_ = 0.0;
    stat_multipliers_ = 1.0;
    oh_bonus_damage_ = 0.0;
    mh_bonus_damage_ = 0.0;
}







