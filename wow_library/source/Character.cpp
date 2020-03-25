#include <algorithm>
#include <iostream>
#include "../include/Character.hpp"

Character::Character(const Race &race)
        :
        total_attributes_{},
        total_special_stats_{},
        race_{race}
{
    set_base_stats(race_);
}

void Character::set_base_stats(const Race &race)
{
    switch (race)
    {
        case Race::human:
            base_attributes_ = Attributes{120, 80};
            base_special_stats_ = Special_stats{0, 0, 160, 0, 0, {{Skill_type::sword, 5}, {Skill_type::mace, 5}}};
            break;
        case Race::dwarf:
            base_attributes_ = Attributes{122, 76};
            base_special_stats_ = Special_stats{0, 0, 160, 0, 0};
            break;
        case Race::night_elf:
            base_attributes_ = Attributes{117, 85};
            base_special_stats_ = Special_stats{0, 0, 160, 0, 0};
            break;
        case Race::gnome:
            base_attributes_ = Attributes{115, 83};
            base_special_stats_ = Special_stats{0, 0, 160, 0, 0};
            break;
        case Race::orc:
            base_attributes_ = Attributes{123, 77};
            base_special_stats_ = Special_stats{0, 0, 160, 0, 0, {{Skill_type::axe, 5}}};
            break;
        case Race::tauren:
            base_attributes_ = Attributes{125, 75};
            base_special_stats_ = Special_stats{0, 0, 160, 0, 0};
            break;
        case Race::troll:
            base_attributes_ = Attributes{121, 82};
            base_special_stats_ = Special_stats{0, 0, 160, 0, 0};
            break;
        case Race::undead:
            base_attributes_ = Attributes{119, 78};
            base_special_stats_ = Special_stats{0, 0, 160, 0, 0};
            break;
        default:
            assert(false);
    }
}

void Character::compute_all_stats(Talent talent, Armory::set_bonuses_t set_bonuses)
{
    clean_all();
    double stat_multiplier_percent = 0;
    total_attributes_ += base_attributes_;
    total_special_stats_ += base_special_stats_;
    std::vector<Set> set_names{};
    for (const Armor &armor : armor_)
    {
        total_attributes_ += armor.get_stats();
        total_special_stats_ += armor.get_special_stats();
        set_names.emplace_back(armor.get_set());
    }

    for (const Weapon &weapon : weapons_)
    {
        total_attributes_ += weapon.get_stats();
        total_special_stats_ += weapon.get_special_stats();
    }

    // TODO fix this mess
    size_t set_pieces_devil = 0;
    size_t set_pieces_bds = 0;
    size_t set_pieces_pvp = 0;
    for (Set &set_name : set_names)
    {
        switch (set_name)
        {
            case Set::devilsaur:
                set_pieces_devil++;
                break;
            case Set::black_dragonscale:
                set_pieces_bds++;
                break;
            case Set::rare_pvp_set:
                set_pieces_pvp++;
                break;
            default:
                break;
        }
    }
    if (set_pieces_devil >= set_bonuses.devilsaur_set_bonus.get_pieces())
    {
        total_attributes_ += set_bonuses.devilsaur_set_bonus.get_stats();
        total_special_stats_ += set_bonuses.devilsaur_set_bonus.get_special_stats();
    }
    if (set_pieces_bds >= set_bonuses.black_dragonscale_bonus2.get_pieces())
    {
        total_attributes_ += set_bonuses.black_dragonscale_bonus2.get_stats();
        total_special_stats_ += set_bonuses.black_dragonscale_bonus2.get_special_stats();
    }
    if (set_pieces_bds >= set_bonuses.black_dragonscale_bonus3.get_pieces())
    {
        total_attributes_ += set_bonuses.black_dragonscale_bonus3.get_stats();
        total_special_stats_ += set_bonuses.black_dragonscale_bonus3.get_special_stats();
    }
    if (set_pieces_pvp >= set_bonuses.rare_pvp_set_bonus_1.get_pieces())
    {
        total_attributes_ += set_bonuses.rare_pvp_set_bonus_1.get_stats();
        total_special_stats_ += set_bonuses.rare_pvp_set_bonus_1.get_special_stats();
    }

    for (const Enchant &ench : enchants_)
    {
        total_attributes_ += ench.get_stats();
        total_special_stats_ += ench.get_special_stats();
    }

    for (const auto &buff : buffs_)
    {
        total_attributes_ += buff.get_stats();
        total_special_stats_ += buff.get_special_stats();
        stat_multiplier_percent += buff.get_stat_multiplier();
    }

    if (talent == Talent::fury)
    {
        // TODO implement shout in simulator instead
        total_special_stats_.attack_power += 241;  // battle shout
        total_special_stats_.critical_strike += 5; // crit from talent
        total_special_stats_.critical_strike += 3; // crit from berserker stance?
    }

    // TODO wep enchants and buffs
//    for (const Weapon &weapon : weapons_)
//    {
//        total_attributes_ += weapon.get_stats();
//        total_special_stats_ += weapon.get_special_stats();
//    }

    total_attributes_ *= (1.0 + stat_multiplier_percent / 100);

    total_special_stats_ += total_attributes_.convert_to_special_stats();
}

const Special_stats &Character::get_total_special_stats() const
{
    return total_special_stats_;
}

const std::vector<Armor> &Character::get_armor() const
{
    return armor_;
}

const std::vector<Weapon> &Character::get_weapons() const
{
    return weapons_;
}

bool Character::check_if_armor_valid()
{
    std::vector<Socket> sockets;
    bool one_ring{false};
    bool one_trinket{false};
    for (auto const &armor_piece : armor_)
    {
        for (auto const &socket : sockets)
        {
            if (armor_piece.get_socket() == socket)
            {
                if (armor_piece.get_socket() == Socket::ring)
                {
                    if (armor_piece.get_socket() == Socket::ring && one_ring)
                    {
                        std::cout << "extra copy of " << armor_piece.get_socket() << "\n";
                        return false;
                    }
                    one_ring = true;
                }
                else if (armor_piece.get_socket() == Socket::trinket)
                {
                    if (armor_piece.get_socket() == Socket::trinket && one_trinket)
                    {
                        std::cout << "extra copy of " << armor_piece.get_socket() << "\n";
                        return false;
                    }
                    one_trinket = true;
                }
                else
                {
                    std::cout << "extra copy of " << armor_piece.get_socket() << "\n";
                    return false;
                }
            }
        }
        sockets.emplace_back(armor_piece.get_socket());
    }
    return true;
}

bool Character::check_if_weapons_valid()
{
    bool is_unique{true};
    is_unique &= weapons_.size() <= 2;
    is_unique &= !(weapons_[0].get_socket() == Socket::off_hand);
    is_unique &= !(weapons_[1].get_socket() == Socket::main_hand);
//    if (weapons_.size() == 2)
//    {
//        weapons_[0].set_hand(Hand::main_hand);
//        weapons_[1].set_hand(Hand::off_hand);
//    }
    return is_unique;
}

const Attributes &Character::get_stats() const
{
    return total_attributes_;
}

const std::vector<Buff> &Character::get_buffs() const
{
    return buffs_;
}

const std::vector<Enchant> &Character::get_enchants() const
{
    return enchants_;
}

void Character::clean_all()
{
    total_attributes_.clear();
    total_special_stats_.clear();
}

void Character::change_weapon(const Weapon &weapon, const Hand &hand)
{
    switch (hand)
    {
        case Hand::main_hand:
            weapons_[0] = weapon;
            break;
        case Hand::off_hand:
            weapons_[1] = weapon;
            break;
    }
}

void Character::change_armor(const Armor &armor, bool first_misc_slot)
{
    auto socket = armor.get_socket();
    for (auto &armor_piece : armor_)
    {
        if (armor_piece.get_socket() == socket)
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
                armor_piece = armor;
                return;
            }
        }
    }
}

std::ostream &operator<<(std::ostream &os, const Character &character)
{
    os << "Character items:" << "\n";
    for (const auto &item : character.get_armor())
    {
        os << item;
    }
    os << "Main hand:" << "\n";
    os << character.get_weapons()[0].get_name() << "\n";
    os << "Off hand:" << "\n";
    os << character.get_weapons()[1].get_name() << "\n";
    return os;
}






