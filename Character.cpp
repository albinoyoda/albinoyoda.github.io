#include "Character.hpp"

Character::Character(const Race &race) : base_stats_{}, total_special_stats_{}, weapon_skill_{300}
{
    switch (race)
    {
        case Race::human:
            base_stats_ += Stats{120, 80};
            total_special_stats_ += Special_stats{5, 0, 0};
            weapon_skill_ += 5;
            return;
        default:
            assert(false);
    }
}

Stats Character::compute_total_stats()
{
    Stats total_stats = base_stats_;
    for (const Armor &armor : gear)
    {
        total_stats += armor.get_stats();
    }
    for (const Weapon &weapon : weapons)
    {
        total_stats += weapon.get_stats();
    }
    return total_stats;
}

void Character::compute_special_stats()
{
    // TODO do buffs here
    auto total_stats = compute_total_stats();
    for (const Item &armor : gear)
    {
        total_special_stats_ += armor.get_special_stats();
    }
    for (const Item &weapon : weapons)
    {
        total_special_stats_ += weapon.get_special_stats();
    }
    total_special_stats_ += total_stats.convert_to_special_stats();
}

const Special_stats &Character::get_total_special_stats() const
{
    return total_special_stats_;
}

double Character::get_weapon_skill() const
{
    return weapon_skill_;
}

const std::vector<Weapon> &Character::get_weapons() const
{
    return weapons;
}

const std::vector<Armor> &Character::get_gear() const
{
    return gear;
}
