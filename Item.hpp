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

private:
    Stats stats_;
    Special_stats special_stats_;
};

class Weapon : public Item
{
public:
    enum class Socket
    {
        main_hand,
        off_hand,
    };

    Weapon(double swing_speed, std::pair<double, double> damage_interval, Stats stats, Special_stats special_stats,
           Socket socket);

    double step(double time, double attack_power);

    double get_average_damage()
    {
        return (damage_interval_.second + damage_interval_.first) / 2;
    }

    double get_swing_speed()
    {
        return swing_speed_;
    }

    void queue_heroic()
    {
        heroic_strike_ = true;
    }

private:
    double swing_speed_;
    double internal_swing_timer_;
    std::pair<double, double> damage_interval_;
    Socket socket_;
    bool heroic_strike_{false};
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
    };

    Armor() = delete;

    Armor(Stats stats, Special_stats special_stats, Socket socket);

private:
    Socket socket_;
};

class Buff : public Item
{
public:
    enum class Socket
    {
        strength,
        agility,
    };

    Buff() = delete;

    Buff(Stats stats, Special_stats special_stats, Socket socket);

private:
    Socket socket_;
};


#endif //WOW_SIMULATOR_ITEM_HPP

