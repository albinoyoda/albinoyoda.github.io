#ifndef WOW_SIMULATOR_ITEM_HPP
#define WOW_SIMULATOR_ITEM_HPP

#include <utility>
#include "Stats.hpp"

class Item
{
public:
    Item() = delete;

    Item(Stats stats, Special_stats special_stats);

    const Stats &get_stats() const;

    const Special_stats &get_special_stats() const;

    void set_chance_for_extra_hit(int chance_for_extra_hit_input);

    double get_chance_for_extra_hit() const;

    int get_extra_skill() const;

    void set_extra_skill(double extra_skill);

private:
    Stats stats_;
    Special_stats special_stats_;
    double chance_for_extra_hit;
    int extra_skill_;
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
           Socket socket);

    double step(double time, double attack_power);

    double swing(double attack_power);

    void reset_timer();

    void compute_average_damage(double bonus_damage)
    {
        average_damage_ = (damage_interval_.second + damage_interval_.first) / 2 + bonus_damage / 2;
    }

    double get_average_damage()
    {
        return average_damage_;
    }

    double get_swing_speed()
    {
        return swing_speed_;
    }

    double get_internal_swing_timer()
    {
        return internal_swing_timer_;
    }

    Socket get_socket() const;

private:
    double swing_speed_;
    double internal_swing_timer_;
    std::pair<double, double> damage_interval_;
    double average_damage_;
    Socket socket_;
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
        ring1,
        ring2,
        trinket1,
        trinket2,
        ranged
    };

    Armor() = delete;

    Armor(Stats stats, Special_stats special_stats, Socket socket);

    Socket get_socket() const;

private:
    Socket socket_;
};

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
            name_{name} {};

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

    Player_buff(Stats stats, Special_stats special_stats, Id name) : Buff(stats, special_stats),
            name_{name} {};

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
            name_{name} {};

private:
    Id name_;
};

#endif //WOW_SIMULATOR_ITEM_HPP

