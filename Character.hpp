#ifndef WOW_SIMULATOR_CHARACTER_HPP
#define WOW_SIMULATOR_CHARACTER_HPP

#include <cassert>
#include <vector>
#include "Stats.hpp"
#include "Item.hpp"

enum class Race
{
    human,
};

class Character
{
public:
    enum class Talent
    {
        fury,
        none,
    };

    explicit Character(const Race &race);

    Stats compute_total_stats();

    void compute_special_stats(Talent talent);

    template<typename T, typename ...Ts>
    void equip_armor(T piece, Ts ...pieces)
    {
        equip_armor(piece);
        equip_armor(pieces...);
    }

    template<typename T>
    void equip_armor(T piece)
    {
        armor.emplace_back(piece);
    }

    template<typename T, typename ...Ts>
    void equip_weapon(T piece, Ts ...pieces)
    {
        equip_weapon(piece);
        equip_weapon(pieces...);
    }

    template<typename T>
    void equip_weapon(T piece)
    {
        weapons.emplace_back(piece);
    }

    bool check_if_gear_valid();

    const Special_stats &get_total_special_stats() const;

    double get_weapon_skill() const;

    const std::vector<Armor> &get_gear() const;

    const std::vector<Weapon> &get_weapons() const;

private:
    Stats base_stats_;
    Special_stats total_special_stats_;
    double weapon_skill_;
    std::vector<Armor> armor;
    std::vector<Weapon> weapons;
};

#endif //WOW_SIMULATOR_CHARACTER_HPP
