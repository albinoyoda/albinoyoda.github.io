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
    struct Step_result
    {
        Step_result(double damage, bool did_swing) : damage{damage}, did_swing{did_swing} {}

        double damage;
        bool did_swing;

    };

    enum class Socket
    {
        main_hand,
        off_hand,
    };

    Weapon(double swing_speed, std::pair<double, double> damage_interval, Stats stats, Special_stats special_stats,
           Socket socket);

    Step_result step(double time, double attack_power);

    double swing(double attack_power);

    void reset_timer();

    double get_average_damage()
    {
        return (damage_interval_.second + damage_interval_.first) / 2;
    }

    double get_swing_speed()
    {
        return swing_speed_;
    }

    Socket get_socket() const;

private:
    double swing_speed_;
    double internal_swing_timer_;
    std::pair<double, double> damage_interval_;
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
    enum class Name
    {
        rallying_cry,
        dire_maul,
        blessing_of_kings,
        blessing_of_might,
    };

    Buff() = delete;

    Buff(Stats stats, Special_stats special_stats, Name name);

private:
    Name name_;
};


#endif //WOW_SIMULATOR_ITEM_HPP

