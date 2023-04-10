#ifndef WOW_SIMULATOR_ITEM_HPP
#define WOW_SIMULATOR_ITEM_HPP

#include "Attributes.hpp"
#include "common/sim_time.hpp"

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
    the_defilers_resolution,
    battlegear_of_wrath,
    dreadnaughts_battlegear,
    battlegear_of_undead_slaying
};

struct Over_time_effect
{
    Over_time_effect(std::string name, Special_stats special_stats, double rage_gain, double damage, Sim_time interval,
                     Sim_time duration)
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
    Sim_time interval;
    Sim_time duration;
};

class Hit_effect
{
public:
    enum class Type
    {
        extra_hit,
        stat_boost,
        damage_physical,
        damage_magic,
        reduce_armor
    };

    Hit_effect(std::string name, Type type, Attributes attribute_boost, Special_stats special_stats_boost,
               double damage, Sim_time duration, double probability, double attack_power_boost = 0, int n_targets = 1,
               int armor_reduction = 0, int max_stacks = 0, double ppm = 0.0, bool affects_both_weapons = false)
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
        , ppm(ppm)
        , affects_both_weapons(affects_both_weapons)
        , max_stacks(max_stacks){};

    [[nodiscard]] inline Special_stats get_special_stat_equivalent(const Special_stats& special_stats) const
    {
        return attribute_boost.convert_to_special_stats(special_stats) + special_stats_boost;
    }

    std::string name;
    Type type;
    Attributes attribute_boost;
    Special_stats special_stats_boost;
    double damage;
    Sim_time duration;
    double probability;
    double attack_power_boost;
    int n_targets;
    int armor_reduction;
    double ppm;
    bool affects_both_weapons;
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
               Special_stats special_stats_boost, double rage_boost, Sim_time duration, Sim_time cooldown,
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
        , hit_effects(std::move(hit_effects))
        , over_time_effects(std::move(over_time_effects)){};

    [[nodiscard]] inline Special_stats get_special_stat_equivalent(const Special_stats& special_stats) const
    {
        return attribute_boost.convert_to_special_stats(special_stats) + special_stats_boost;
    }

    std::string name;
    Effect_socket effect_socket{};
    Attributes attribute_boost{};
    Special_stats special_stats_boost{};
    double rage_boost{};
    Sim_time duration{};
    Sim_time cooldown{};
    bool triggers_gcd{false};
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
        attack_power,
        naxxramas
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
           std::vector<Hit_effect> hit_effects = std::vector<Hit_effect>(), Set set_name = Set::none,
           std::vector<Use_effect> use_effects = std::vector<Use_effect>())
        : name(std::move(name))
        , attributes(attributes)
        , special_stats(special_stats)
        , swing_speed(Sim_time::from_seconds(swing_speed))
        , min_damage(min_damage)
        , max_damage(max_damage)
        , weapon_socket(weapon_socket)
        , type(weapon_type)
        , hit_effects(std::move(hit_effects))
        , set_name(set_name)
        , use_effects(std::move(use_effects)){};

    std::string name;
    Attributes attributes;
    Special_stats special_stats;
    Sim_time swing_speed;
    double min_damage;
    double max_damage;
    Weapon_socket weapon_socket;
    Weapon_type type;
    std::vector<Hit_effect> hit_effects;
    Set set_name;
    std::vector<Use_effect> use_effects;
    Socket socket;
    Enchant enchant;
    Weapon_buff buff;
};

std::ostream& operator<<(std::ostream& os, const Socket& socket);

std::string operator+(std::string& string, const Socket& socket);

std::string operator+(std::string& string, const Weapon_socket& socket);

#endif // WOW_SIMULATOR_ITEM_HPP
