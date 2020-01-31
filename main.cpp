#include <iostream>
#include <cstdlib>
#include <vector>
#include <cassert>

class Opponent
{
public:
    Opponent(double level) : level{level} {};
    double level;
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
};

Stats operator+(const Stats &lhs, const Stats &rhs)
{
    return {lhs.agility + rhs.agility, lhs.strength + rhs.strength};
}

struct Special_stats
{
    double critical_strike;
    double hit;
    double attack_power;

    double compute_attack_power()
    {
        return attack_power;
    }
    // add more
};

class Item
{
public:
    Stats stats;
    Special_stats special_stats;

    double compute_attack_power()
    {
        return stats.compute_attack_power() + special_stats.compute_attack_power();
    }
};

class Weapon : public Item
{
public:
    double swing_speed;
    std::pair<double, double> damage_interval;

    int socket; // TODO unique socket ID


};

class Armor : public Item
{
public:
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
        total_stats = Stats{};
        switch (race)
        {
            case Race::human:
                base_stats = Stats{120, 80};
                return;
            default:
                assert(false);
        }
    }

    std::vector<Armor> gear;
    std::vector<Weapon> weapons;
    Stats base_stats;
    Stats total_stats;

    void compute_total_stats()
    {
        total_stats = base_stats;
        for (const Item &armor : gear)
        {
            total_stats += armor.stats;
        }
        for (const Item &weapon : gear)
        {
            total_stats += weapon.stats;
        }

        // TODO do buffs here
    };

    void simulate(double time)
    {

    }
private:
};

int main()
{
    Character character{Race::human};
    Opponent opponent{63};

    character.compute_total_stats();
    character.simulate(60);



    return 0;
}

















