#ifndef WOW_SIMULATOR_ITEM_HPP
#define WOW_SIMULATOR_ITEM_HPP

#include <utility>

#include "Stats.hpp"

enum class Hand
{
    main_hand,
    off_hand
};

enum class Skill_type
{
    sword,
    axe,
    mace,
    dagger,
    all,
    none
};

struct Extra_skill
{
    Extra_skill(Skill_type type, int amount) : type(type), amount(amount) {}

    Skill_type type;
    int amount;
};

class Item
{
public:
    Item() = delete;

    Item(Stats stats, Special_stats special_stats);

    const Stats &get_stats() const;

    const Special_stats &get_special_stats() const;

    void set_chance_for_extra_hit(int chance_for_extra_hit_input);

    double get_chance_for_extra_hit() const;

    const Extra_skill &get_bonus_skill() const;

    void set_bonus_skill(Extra_skill bonus_skill);

private:
    Stats stats_;
    Special_stats special_stats_;
    double chance_for_extra_hit;
    Extra_skill bonus_skill_;
};

class Weapon : public Item
{
public:
    enum class Socket
    {
        main_hand,
        one_hand,
        off_hand,
    };

    Weapon(double swing_speed, std::pair<double, double> damage_interval, Stats stats, Special_stats special_stats,
           Socket socket, Skill_type skill_type);

    double step(double time, double attack_power, bool is_random);

    constexpr double swing(double attack_power)
    {
        return average_damage_ + attack_power * swing_speed_ / 14;
    }

    double random_swing(double attack_power)
    {
        double damage = damage_interval_.first + (damage_interval_.second - damage_interval_
                .first) * static_cast<double>(rand()) / RAND_MAX
                        + attack_power * swing_speed_ / 14;
        return damage;
    }

    double random_normalized_swing(double attack_power)
    {
        return damage_interval_.first + (damage_interval_.second - damage_interval_
                .first) * static_cast<double>(rand()) / RAND_MAX
               + attack_power * normalized_swing_speed_ / 14;
    }

    constexpr double normalized_swing(double attack_power)
    {
        // TODO random damage?
        return average_damage_ + attack_power * normalized_swing_speed_ / 14;
    }

    void reset_timer();

    constexpr void compute_average_damage(double bonus_damage)
    {
        damage_interval_.first += bonus_damage;
        damage_interval_.second += bonus_damage;
        average_damage_ = (damage_interval_.second + damage_interval_.first) / 2;
    }

    constexpr double get_average_damage()
    {
        return average_damage_;
    }

    constexpr double get_swing_speed()
    {
        return swing_speed_;
    }

    constexpr double get_internal_swing_timer()
    {
        return internal_swing_timer_;
    }

    Socket get_socket() const;

    void set_weapon_type(Skill_type weapon_type);

    Skill_type get_weapon_type() const;

    constexpr Hand get_hand() const
    {
        return hand_;
    }

    void set_hand(Hand hand);

    void set_internal_swing_timer(double internal_swing_timer);

private:
    double swing_speed_;
    double normalized_swing_speed_;
    double internal_swing_timer_;
    std::pair<double, double> damage_interval_;
    double average_damage_;
    Socket socket_;
    Skill_type weapon_type_;
    Hand hand_;
};

enum class Set_name
{
    none,
    devilsaur,
    black_dragonscale
};

class Armor : public Item
{
public:
    enum class Socket
    {
        head,
        neck,
        shoulder,
        back,
        chest,
        wrists,
        hands,
        belt,
        legs,
        boots,
        ring,
        trinket,
        ranged
    };

    Armor() = delete;

    Armor(Stats stats, Special_stats special_stats, Socket socket, Set_name set_name = Set_name::none);

    Socket get_socket() const;

    Set_name get_set() const { return set_; }

private:
    Socket socket_;
    Set_name set_;
};

class Set_bonus : public Item
{
public:
    Set_bonus() = delete;

    Set_bonus(Stats stats, Special_stats special_stats, size_t pieces, Set_name set_name) : Item(stats, special_stats),
            pieces_(pieces)
    {
        set_name_ = set_name;
    };

    Set_name get_set_name()
    {
        return set_name_;
    }

private:
    Set_name set_name_;
    size_t pieces_;
};

std::ostream &operator<<(std::ostream &os, Armor::Socket const &socket);

class Buff : public Item
{
public:
    Buff() = delete;

    Buff(Stats stats, Special_stats special_stats) : Item(stats, special_stats),
            stat_multiplier_{1.0},
            oh_bonus_damage_{0.0},
            mh_bonus_damage_{0.0} {};

    void set_stat_multiplier(double stat_multiplier)
    {
        stat_multiplier_ = stat_multiplier;
    }

    void set_oh_bonus_damage(double oh_bonus_damage)
    {
        oh_bonus_damage_ = oh_bonus_damage;
    }

    void set_mh_bonus_damage(double mh_bonus_damage)
    {
        mh_bonus_damage_ = mh_bonus_damage;
    }

    double get_stat_multiplier() const
    {
        return stat_multiplier_;
    }

    double get_oh_bonus_damage() const
    {
        return oh_bonus_damage_;
    }

    double get_mh_bonus_damage() const
    {
        return mh_bonus_damage_;
    }

private:
    double stat_multiplier_;
    double oh_bonus_damage_;
    double mh_bonus_damage_;
};

class World_buff : public Buff
{
public:
    enum class Id
    {
        rallying_cry,
        dire_maul,
        songflower,
    };

    World_buff() = delete;

    World_buff(Stats stats, Special_stats special_stats, Id name) : Buff(stats, special_stats),
            name_{name}
    {
        name_ = name;
    };

private:
    Id name_;
};

class Player_buff : public Buff
{
public:

    enum class Id
    {
        blessing_of_kings,
        blessing_of_might,
        mark_of_the_wild,
        trueshot_aura,
    };

    Player_buff() = delete;

    Player_buff(Stats stats, Special_stats special_stats, Id name) : Buff(stats, special_stats)
    {
        name_ = name;
    };

private:
    Id name_;
};

class Consumable : public Buff
{
public:
    enum class Id
    {
        elixir_mongoose,
        dense_stone_mh,
        dense_stone_oh,
        elemental_stone_mh,
        elemental_stone_oh,
        blessed_sunfruit,
        juju_power,
        juju_might,
        roids,
    };


    Consumable() = delete;

    Consumable(Stats stats, Special_stats special_stats, Id name) : Buff(stats, special_stats),
            name_{name}
    {
        name_ = name;
    };

private:
    Id name_;
};

#endif //WOW_SIMULATOR_ITEM_HPP

