#include <iostream>
#include <cstdlib>
#include <vector>
#include <cassert>

struct Special_stats
{
    Special_stats() = default;

    Special_stats(double critical_strike, double hit, double attack_power) : critical_strike{critical_strike}, hit{hit},
                                                                             attack_power{attack_power} {};
    double critical_strike;
    double hit;
    double attack_power;

    Special_stats operator+(const Special_stats &rhs)
    {
        return {this->critical_strike + rhs.critical_strike, this->hit + rhs.hit,
                this->attack_power + rhs.attack_power};
    }

    Special_stats operator+=(const Special_stats &rhs)
    {
        return {this->critical_strike + rhs.critical_strike, this->hit + rhs.hit,
                this->attack_power + rhs.attack_power};
    }

    // add more
};

class Stats
{
public:
    Stats() = default;

    Stats(double agility, double strength) : agility{agility}, strength{strength} {};
    double agility;
    double strength;
    // add more

    Stats operator+(const Stats &rhs)
    {
        return {this->agility + rhs.agility, this->strength + rhs.strength};
    }

    Stats operator+=(const Stats &rhs)
    {
        return {this->agility + rhs.agility, this->strength + rhs.strength};
    }

    double compute_attack_power()
    {
        return strength * 2 + agility;
    }

    Special_stats convert_to_special_stats()
    {
        return {this->agility / 20, 0, this->agility * 1 + this->strength * 2};
    }

};

Stats operator+(const Stats &lhs, const Stats &rhs)
{
    return {lhs.agility + rhs.agility, lhs.strength + rhs.strength};
}

class Item
{
public:
    Item() = delete;

    Item(Stats stats, Special_stats special_stats) : stats{stats}, special_stats{special_stats} {};

    const Stats &get_stats() const
    {
        return stats;
    }

    const Special_stats &get_special_stats() const
    {
        return special_stats;
    }

private:
    Stats stats;
    Special_stats special_stats;
};

class Weapon : public Item
{
public:
    Weapon(double swing_speed, std::pair<double, double> damage_interval, Stats stats, Special_stats special_stats)
            : swing_speed{swing_speed},
              time_to_next_swing{swing_speed},
              damage_interval{
                      std::move(
                              damage_interval)},
              Item{stats,
                   special_stats} {};

    double step(double time, double attack_power, bool is_main_hand)
    {
        time_to_next_swing -= time;
        if (time_to_next_swing < 0.0)
        {
            time_to_next_swing += swing_speed;
            double damage = (damage_interval.second - damage_interval.first) + attack_power * swing_speed;
            if (!is_main_hand)
            {
                damage *= 0.625;
            }
            return damage;
        }
        return 0.0;
    }

private:
    double swing_speed;
    double time_to_next_swing;
    std::pair<double, double> damage_interval;
    int socket; // TODO unique socket ID
};

class Armor : public Item
{
public:
    Stats stats;
    Special_stats special_stats;
    int socket;
};

//using Head = Armor<std::string("head")>;
enum class Race
{
    human,
};


class Character
{
public:
    Character(const Race &race)
    {
        base_stats = Stats{};
        total_special_stats = Special_stats{};
        switch (race)
        {
            case Race::human:
                base_stats += Stats{120, 80};
                total_special_stats += Special_stats{};
                return;
            default:
                assert(false);
        }
    }

    std::vector<Armor> gear;
    std::vector<Weapon> weapons;

    Stats compute_total_stats()
    {
        Stats total_stats = base_stats;
        for (const Armor &armor : gear)
        {
            total_stats += armor.get_stats();
        }
        for (const Weapon &weapon : weapons)
        {
            total_stats += weapon.get_stats();
        }
        return total_stats;
    };

    void compute_special_stats()
    {
        auto total_stats = compute_total_stats();
        for (const Item &armor : gear)
        {
            total_special_stats += armor.get_special_stats();
        }
        for (const Item &weapon : gear)
        {
            total_special_stats += weapon.get_special_stats();
        }
        total_special_stats += total_stats.convert_to_special_stats();

        // TODO do buffs here
    };

    void simulate(double time, double dt, int level, int iterations)
    {
        
        for (const Weapon& wep : weapons)
        {
            wep.step(dt, total_special_stats.attack_power, )
        }

    }

private:
    Stats base_stats;
    Special_stats total_special_stats;
};

int main()
{
    Character character{Race::human};
//    Opponent opponent{63};

    character.compute_special_stats();
    character.simulate(60, 0.1, 63, 1);

    return 0;
}

















