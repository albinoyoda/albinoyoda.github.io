#ifndef WOW_SIMULATOR_CHARACTER_HPP
#define WOW_SIMULATOR_CHARACTER_HPP

#include <cassert>
#include <vector>
#include "Attributes.hpp"
#include "Item.hpp"
#include "Enchant.hpp"
#include "Armory.hpp"

enum class Race
{
    human,
    dwarf,
    night_elf,
    gnome,
    orc,
    tauren,
    troll,
    undead
};

enum class Talent
{
    fury,
    arms,
    none,
};

class Character
{
public:
    explicit Character(const Race &race);

    void set_base_stats(const Race &race);

    void compute_all_stats(Talent talent, Armory::set_bonuses_t set_bonuses);

    void clean_all();

    bool check_if_armor_valid();

    bool check_if_weapons_valid();

    const Special_stats &get_total_special_stats() const;

    const std::vector<Armor> &get_armor() const;

    const std::vector<Weapon> &get_weapons() const;

    const Attributes &get_stats() const;

    template<typename T, typename ...Ts>
    void equip_armor(T piece, Ts ...pieces)
    {
        equip_armor(piece);
        equip_armor(pieces...);
    }

    template<typename T>
    void equip_armor(T piece)
    {
        armor_.emplace_back(piece);
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
        weapons_.emplace_back(piece);
    }

    template<typename T, typename ...Ts>
    void add_enchants(T en, Ts ...ens)
    {
        add_enchants(en);
        add_enchants(ens...);
    }

    template<typename T>
    void add_enchants(T piece)
    {
        enchants_.emplace_back(piece);
    }

    template<typename T, typename ...Ts>
    void add_buffs(T buff, Ts ...buffs)
    {
        add_buffs(buff);
        add_buffs(buffs...);
    }

    template<typename T>
    void add_buffs(T buff)
    {
        buffs_.emplace_back(buff);
    }

    void increase_weapon_damage(double weapon_damage)
    {
        mh_bonus_damage_ += weapon_damage;
        oh_bonus_damage_ += weapon_damage;
    }

    bool is_crusader_mh() const;

    bool is_crusader_oh() const;

    template<typename T>
    auto &get_field(T t)
    {
        return (*this).*t;
    }

    void clear_permutations()
    {
        permutated_stats_ = Attributes{};
        permutated_special_stats_ = Special_stats{};
    }

    const std::vector<Armor> &get_armor() const;

    void change_weapon(const Weapon &weapon, const Hand &hand);

    void change_armor(const Armor &armor, bool first_misc_slot = true);

    // Used to compute
    Attributes permutated_stats_;
    Special_stats permutated_special_stats_;

private:
    Attributes base_attributes_;
    Special_stats base_special_stats_;
    Attributes total_attributes_;
    Special_stats total_special_stats_;
    double chance_for_extra_hit_;
    double stat_multipliers_;
    std::vector<Armor> armor_;
    std::vector<Weapon> weapons_;
    std::vector<Enchant> enchants_;
    std::vector<Buff> buffs_;
    Race race_;
};

std::ostream &operator<<(std::ostream &os, const Character &socket);

#endif //WOW_SIMULATOR_CHARACTER_HPP
