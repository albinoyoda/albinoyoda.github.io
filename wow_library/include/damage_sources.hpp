#ifndef WOW_SIMULATOR_DAMAGE_SOURCES_HPP
#define WOW_SIMULATOR_DAMAGE_SOURCES_HPP

#include "Statistics.hpp"

#include <string>
#include <vector>

enum class Damage_source
{
    white_mh,
    white_oh,
    bloodthirst,
    execute,
    heroic_strike,
    cleave,
    whirlwind,
    hamstring,
    revenge,
    overpower,
    sunder_armor,
    deep_wounds,
    item_hit_effects,
    weapon_elemental_damage
};

struct Damage_instance
{
    Damage_instance(Damage_source source, double damage, double time_stamp)
        : damage_source(source), damage(damage), time_stamp(time_stamp){};
    Damage_source damage_source;
    double damage{};
    double time_stamp{};
};

struct Damage_sources
{
    Damage_sources() { damage_instances.reserve(500); };

    ~Damage_sources() = default;

    Damage_sources& operator+(const Damage_sources& rhs)
    {
        whirlwind_damage = whirlwind_damage + rhs.whirlwind_damage;
        bloodthirst_damage = bloodthirst_damage + rhs.bloodthirst_damage;
        execute_damage = execute_damage + rhs.execute_damage;
        white_mh_damage = white_mh_damage + rhs.white_mh_damage;
        white_oh_damage = white_oh_damage + rhs.white_oh_damage;
        heroic_strike_damage = heroic_strike_damage + rhs.heroic_strike_damage;
        cleave_damage = cleave_damage + rhs.cleave_damage;
        hamstring_damage = hamstring_damage + rhs.hamstring_damage;
        deep_wounds_damage = deep_wounds_damage + rhs.deep_wounds_damage;
        item_hit_effects_damage = item_hit_effects_damage + rhs.item_hit_effects_damage;

        whirlwind_count = whirlwind_count + rhs.whirlwind_count;
        bloodthirst_count = bloodthirst_count + rhs.bloodthirst_count;
        execute_count = execute_count + rhs.execute_count;
        white_mh_count = white_mh_count + rhs.white_mh_count;
        white_oh_count = white_oh_count + rhs.white_oh_count;
        heroic_strike_count = heroic_strike_count + rhs.heroic_strike_count;
        cleave_count = cleave_count + rhs.cleave_count;
        hamstring_count = hamstring_count + rhs.hamstring_count;
        deep_wounds_count = deep_wounds_count + rhs.deep_wounds_count;
        item_hit_effects_count = item_hit_effects_count + rhs.item_hit_effects_count;
        return *(this);
    }

    constexpr double sum_damage_sources() const
    {
        return white_mh_damage + white_oh_damage + bloodthirst_damage + heroic_strike_damage + cleave_damage +
               whirlwind_damage + hamstring_damage + execute_damage + deep_wounds_damage + item_hit_effects_damage;
    }

    constexpr double sum_counts() const
    {
        return white_mh_count + white_oh_count + bloodthirst_count + heroic_strike_count + cleave_count +
               whirlwind_count + hamstring_count + execute_count + deep_wounds_count + item_hit_effects_count;
    }

    void add_damage(Damage_source source, double damage, double time_stamp)
    {
        damage_instances.emplace_back(source, damage, time_stamp);
        switch (source)
        {
        case Damage_source::white_mh:
            white_mh_damage += damage;
            white_mh_count++;
            break;
        case Damage_source::white_oh:
            white_oh_damage += damage;
            white_oh_count++;
            break;
        case Damage_source::bloodthirst:
            bloodthirst_damage += damage;
            bloodthirst_count++;
            break;
        case Damage_source::execute:
            execute_damage += damage;
            execute_count++;
            break;
        case Damage_source::heroic_strike:
            heroic_strike_damage += damage;
            heroic_strike_count++;
            break;
        case Damage_source::cleave:
            cleave_damage += damage;
            cleave_count++;
            break;
        case Damage_source::whirlwind:
            whirlwind_damage += damage;
            whirlwind_count++;
            break;
        case Damage_source::hamstring:
            hamstring_damage += damage;
            hamstring_count++;
            break;
        case Damage_source::deep_wounds:
            deep_wounds_damage += damage;
            deep_wounds_count++;
            break;
        case Damage_source::item_hit_effects:
            item_hit_effects_damage += damage;
            item_hit_effects_count++;
            break;
        }
    }

    double white_mh_damage{};
    double white_oh_damage{};
    double bloodthirst_damage{};
    double execute_damage{};
    double heroic_strike_damage{};
    double cleave_damage{};
    double whirlwind_damage{};
    double hamstring_damage{};
    double deep_wounds_damage{};
    double item_hit_effects_damage{};

    int white_mh_count{};
    int white_oh_count{};
    int bloodthirst_count{};
    int execute_count{};
    int heroic_strike_count{};
    int cleave_count{};
    int whirlwind_count{};
    int hamstring_count{};
    int deep_wounds_count{};
    int item_hit_effects_count{};

    std::vector<Damage_instance> damage_instances;
};

#endif // WOW_SIMULATOR_DAMAGE_SOURCES_HPP
