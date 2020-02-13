#ifndef WOW_SIMULATOR_CHARACTER_HPP
#define WOW_SIMULATOR_CHARACTER_HPP

#include <cassert>
#include <vector>
#include "Stats.hpp"
#include "Item.hpp"
#include "Enchant.hpp"

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

class Character
{
public:
    enum class Talent
    {
        fury,
        none,
    };

    explicit Character(const Race &race);

    void set_base_stats(const Race &race);

    void compute_all_stats(Talent talent, std::vector<Set_bonus> set_bonuses);

    void clean_all();

    bool check_if_armor_valid();

    bool check_if_weapons_valid();

    const Special_stats &get_total_special_stats() const;

    int get_weapon_skill_oh() const;

    int get_weapon_skill_mh() const;

    const std::vector<Armor> &get_gear() const;

    const std::vector<Weapon> &get_weapons() const;

    double get_haste() const;

    const Stats &get_stats() const;


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

    double get_chance_for_extra_hit() const;

    void set_stats(const Stats &stats);

    void set_total_special_stats(const Special_stats &total_special_stats);

    void set_haste(double haste);

    void set_extra_haste(double extra_haste_percent)
    {
        double extra_haste_factor = 1.0 + std::abs(extra_haste_percent) / 100.0;
        if (extra_haste_percent > 0.0)
        {
            haste_ *= extra_haste_factor;
        }
        else
        {
            haste_ /= extra_haste_factor;
        }
    }

    void increase_chance_for_extra_hit(double extra_chance_on_hit)
    {
        chance_for_extra_hit_ += extra_chance_on_hit;
    }

    void increase_extra_skill(double extra_chance_on_hit)
    {
        chance_for_extra_hit_ += extra_chance_on_hit;
    }

    void set_chance_for_extra_hit(double chance_for_extra_hit);

    void set_extra_weapon_skill_sword(int weapon_skill)
    {
        Extra_skill extra_skill{Skill_type::sword, weapon_skill};
        extra_skills_.push_back(extra_skill);
    }

    void set_extra_weapon_skill_axe(int weapon_skill)
    {
        Extra_skill extra_skill{Skill_type::axe, weapon_skill};
        extra_skills_.push_back(extra_skill);
    }

    void set_extra_weapon_skill_dagger(int weapon_skill)
    {
        Extra_skill extra_skill{Skill_type::dagger, weapon_skill};
        extra_skills_.push_back(extra_skill);
    }

    void set_extra_weapon_skill_mace(int weapon_skill)
    {
        Extra_skill extra_skill{Skill_type::mace, weapon_skill};
        extra_skills_.push_back(extra_skill);
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
        permutated_stats_ = Stats{};
        permutated_special_stats_ = Special_stats{};
    }

    double get_oh_bonus_damage() const;

    double get_mh_bonus_damage() const;

    // Used to compute
    Stats permutated_stats_;
    Special_stats permutated_special_stats_;

private:
    Stats base_stats_;
    Special_stats base_special_stats_;
    Stats total_stats_;
    Special_stats total_special_stats_;
    double haste_;
    bool crusader_mh_;
    bool crusader_oh_;
    int weapon_skill_mh;
    int weapon_skill_oh;
    std::vector<Extra_skill> extra_skills_;
    double chance_for_extra_hit_;
    double stat_multipliers_;
    double oh_bonus_damage_;
    double mh_bonus_damage_;
    std::vector<Armor> armor_;
    std::vector<Weapon> weapons_;
    std::vector<Enchant> enchants_;
    std::vector<Buff> buffs_;
    Race race_;
};

#endif //WOW_SIMULATOR_CHARACTER_HPP
