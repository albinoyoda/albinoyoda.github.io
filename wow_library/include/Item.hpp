#ifndef WOW_SIMULATOR_ITEM_HPP
#define WOW_SIMULATOR_ITEM_HPP

#include "Attributes.hpp"

#include <cassert>
#include <utility>

enum class Socket
{
    none,
    head,
    neck,
    shoulder,
    back,
    chest,
    wrist,
    hands,
    belt,
    legs,
    boots,
    ring,
    trinket,
    main_hand,
    off_hand,
    ranged,
};

enum class Weapon_socket
{
    main_hand,
    one_hand,
    off_hand,
    two_hand
};

enum class Weapon_type
{
    sword,
    axe,
    dagger,
    mace,
    unarmed
};

enum class Set
{
    none,
    devilsaur,
    valor,
    black_dragonscale,
    rare_pvp_set,
    epic_pvp_set,
    dal_rends,
    warblade_of_the_hakkari,
    major_mojo_infusion,
    battlegear_of_heroism,
    the_defilers_resolution
};

struct Over_time_effect
{
    Over_time_effect() = default;

    ~Over_time_effect() = default;

    Over_time_effect(std::string name, Special_stats special_stats, double rage_gain, double damage, int interval,
                     double duration)
        : name(std::move(name))
        , special_stats(special_stats)
        , rage_gain(rage_gain)
        , damage(damage)
        , interval(interval)
        , duration(duration){};

    std::string name;
    Special_stats special_stats;
    double rage_gain;
    double damage;
    int interval;
    double duration;
};

class Hit_effect
{
public:
    enum class Type
    {
        none,
        extra_hit,
        stat_boost,
        damage_physical,
        damage_magic,
        damage_magic_guaranteed,
        reduce_armor
    };

    Hit_effect() = default;

    Hit_effect(std::string name, Type type, Attributes attribute_boost, Special_stats special_stats_boost,
               double damage, double duration, double probability, double attack_power_boost = 0, int n_targets = 1,
               double armor_reduction = 0, int max_stacks = 0)
        : name(std::move(name))
        , type(type)
        , attribute_boost(attribute_boost)
        , special_stats_boost(special_stats_boost)
        , damage(damage)
        , duration(duration)
        , probability(probability)
        , attack_power_boost(attack_power_boost)
        , n_targets(n_targets)
        , armor_reduction(armor_reduction)
        , max_stacks(max_stacks){};

    inline Special_stats get_special_stat_equivalent(const Special_stats& special_stats) const
    {
        return attribute_boost.convert_to_special_stats(special_stats) + special_stats_boost;
    }

    std::string name;
    Type type;
    Attributes attribute_boost;
    Special_stats special_stats_boost;
    double damage;
    double duration;
    double probability;
    double attack_power_boost;
    int n_targets;
    double armor_reduction;
    int max_stacks;
};

class Use_effect
{
public:
    enum class Effect_socket
    {
        shared,
        unique,
    };

    Use_effect() = default;

    ~Use_effect() = default;

    Use_effect(std::string name, Effect_socket effect_socket, Attributes attribute_boost,
               Special_stats special_stats_boost, double rage_boost, double duration, double cooldown,
               bool triggers_gcd, std::vector<Hit_effect> hit_effects = std::vector<Hit_effect>(),
               std::vector<Over_time_effect> over_time_effects = std::vector<Over_time_effect>())
        : name(std::move(name))
        , effect_socket(effect_socket)
        , attribute_boost(attribute_boost)
        , special_stats_boost(special_stats_boost)
        , rage_boost(rage_boost)
        , duration(duration)
        , cooldown(cooldown)
        , triggers_gcd(triggers_gcd)
        , hit_effects(hit_effects)
        , over_time_effects(over_time_effects){};

    inline Special_stats get_special_stat_equivalent(const Special_stats& special_stats) const
    {
        return attribute_boost.convert_to_special_stats(special_stats) + special_stats_boost;
    }

    std::string name;
    Effect_socket effect_socket;
    Attributes attribute_boost;
    Special_stats special_stats_boost;
    double rage_boost{};
    double duration{};
    double cooldown{};
    double triggers_gcd{false};
    std::vector<Hit_effect> hit_effects{};
    std::vector<Over_time_effect> over_time_effects{};
};

struct Enchant
{
    enum class Type
    {
        none,
        strength,
        strength7,
        strength9,
        agility,
        greater_agility,
        haste,
        crusader,
        minor_stats,
        major_stats,
        attack_power
    };

    Enchant() = default;

    explicit Enchant(Type type) : type(type){};

    Type type{};
    Attributes attributes{};
    Special_stats special_stats{};
};

struct Set_bonus
{
    Set_bonus(std::string name, Attributes attributes, Special_stats special_stats, int pieces, Set set)
        : name(std::move(name)), attributes(attributes), special_stats(special_stats), pieces(pieces), set(set){};

    std::string name;
    Attributes attributes;
    Special_stats special_stats;
    int pieces;
    Set set;
};

struct Buff
{
    Buff(std::string name, Attributes attributes, Special_stats special_stats, double bonus_damage = 0,
         std::vector<Hit_effect> hit_effects = std::vector<Hit_effect>(),
         std::vector<Use_effect> use_effects = std::vector<Use_effect>())
        : name(std::move(name))
        , attributes(attributes)
        , special_stats(special_stats)
        , bonus_damage(bonus_damage)
        , hit_effects(std::move(hit_effects))
        , use_effects(std::move(use_effects)){};

    std::string name;
    Attributes attributes;
    Special_stats special_stats;
    double bonus_damage;
    std::vector<Hit_effect> hit_effects{};
    std::vector<Use_effect> use_effects{};
};

struct Weapon_buff
{
    Weapon_buff() = default;

    Weapon_buff(std::string name, Attributes attributes, Special_stats special_stats, double bonus_damage = 0)
        : name(std::move(name)), attributes(attributes), special_stats(special_stats), bonus_damage(bonus_damage){};

    std::string name{};
    Attributes attributes{};
    Special_stats special_stats{};
    double bonus_damage{};
};

struct Armor
{
    Armor(std::string name, Attributes attributes, Special_stats special_stats, Socket socket, Set set_name = Set::none,
          std::vector<Hit_effect> hit_effects = std::vector<Hit_effect>(),
          std::vector<Use_effect> use_effects = std::vector<Use_effect>())
        : name(std::move(name))
        , attributes(attributes)
        , special_stats(special_stats)
        , socket(socket)
        , set_name(set_name)
        , hit_effects(std::move(hit_effects))
        , use_effects(std::move(use_effects)){};
    std::string name;
    Attributes attributes;
    Special_stats special_stats;
    Socket socket;
    Set set_name;
    Enchant enchant{};
    std::vector<Hit_effect> hit_effects{};
    std::vector<Use_effect> use_effects{};
};

struct Weapon
{
    Weapon(std::string name, Attributes attributes, Special_stats special_stats, double swing_speed, double min_damage,
           double max_damage, Weapon_socket weapon_socket, Weapon_type weapon_type,
           std::vector<Hit_effect> hit_effects = std::vector<Hit_effect>(), Set set_name = Set::none)
        : name(std::move(name))
        , attributes(attributes)
        , special_stats(special_stats)
        , swing_speed(swing_speed)
        , min_damage(min_damage)
        , max_damage(max_damage)
        , weapon_socket(weapon_socket)
        , type(weapon_type)
        , hit_effects(std::move(hit_effects))
        , set_name(set_name){};

    std::string name;
    Attributes attributes;
    Special_stats special_stats;
    double swing_speed;
    double min_damage;
    double max_damage;
    Weapon_socket weapon_socket;
    Weapon_type type;
    std::vector<Hit_effect> hit_effects;
    Set set_name;
    Socket socket;
    Enchant enchant;
    Weapon_buff buff;
};

std::ostream& operator<<(std::ostream& os, const Socket& socket);

int get_weapon_skill(const Special_stats& special_stats, Weapon_type weapon_type);

#endif // WOW_SIMULATOR_ITEM_HPP
