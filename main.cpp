#include <iostream>
#include <cstdlib>
#include <vector>
#include <cassert>

class Hit_table
{
public:
    Hit_table() = delete;

    Hit_table(std::vector<double> hit_probabilities) : hit_probabilities_{std::move(hit_probabilities)} {}

    double generate_hit(double damage)
    {
        double random_var = 100 * rand() / static_cast<double>(RAND_MAX);

        int outcome = std::lower_bound(hit_probabilities_.begin(), hit_probabilities_.end(), random_var) -
                      hit_probabilities_.begin();

        switch (outcome)
        {
            case 0:
                std::cout << "miss" << "\n";
                return 0.0;
            case 1:
                std::cout << "dodge" << "\n";
                return 0.0;
            case 2:
                std::cout << "glancing" << "\n";
                return damage * 0.85;
            case 3:
                std::cout << "crit" << "\n";
                return damage * 2.2;
            case 4:
                std::cout << "hit" << "\n";
                return damage;
            default:
                std::cout << "BUGGGGGGG" << "\n";
        }
        assert(false);
        return 0.0;
    }

private:
    std::vector<double> hit_probabilities_;
    std::vector<double> damage_weights;
};

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

    Special_stats &operator+=(const Special_stats &rhs)
    {
        *this = *this + rhs;
        return *this;
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

    Stats &operator+=(const Stats &rhs)
    {
        *this = *this + rhs;
        return *this;
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

    Item(Stats stats, Special_stats special_stats) : stats_{stats}, special_stats_{special_stats} {};

    const Stats &get_stats() const
    {
        return stats_;
    }

    const Special_stats &get_special_stats() const
    {
        return special_stats_;
    }

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
        off_hand
    };

    Weapon(double swing_speed, std::pair<double, double> damage_interval, Stats stats, Special_stats special_stats,
           Socket socket)
            : swing_speed_{swing_speed},
              internal_swing_timer_{0},
              damage_interval_{std::move(damage_interval)},
              Item{stats, special_stats},
              socket_{socket} {};

    double step(double time, double attack_power, bool is_main_hand)
    {
        internal_swing_timer_ -= time;
        if (internal_swing_timer_ < 0.0)
        {
            internal_swing_timer_ += swing_speed_;
            double damage = (damage_interval_.second - damage_interval_.first) + attack_power * swing_speed_ / 14;
            if (!is_main_hand)
            {
                damage *= 0.625;
            }
            return damage;
        }
        return 0.0;
    }

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
    };

    Armor() = delete;

    Armor(Stats stats, Special_stats special_stats, Socket socket) : Item{stats, special_stats},
                                                                     socket_{socket} {};
private:
    Socket socket_;
};

enum class Race
{
    human,
};


class Character
{
public:
    explicit Character(const Race &race) : base_stats_{}, total_special_stats_{}, weapon_skill_{300}
    {
        switch (race)
        {
            case Race::human:
                base_stats_ += Stats{120, 80};
                total_special_stats_ += Special_stats{5, 0, 0};
                weapon_skill_ += 5;
                return;
            default:
                assert(false);
        }
    }

//    void

    Stats compute_total_stats()
    {
        Stats total_stats = base_stats_;
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
        // TODO do buffs here
        auto total_stats = compute_total_stats();
        for (const Item &armor : gear)
        {
            total_special_stats_ += armor.get_special_stats();
        }
        for (const Item &weapon : gear)
        {
            total_special_stats_ += weapon.get_special_stats();
        }
        total_special_stats_ += total_stats.convert_to_special_stats();
    };

    Hit_table compute_hit_table(double level)
    {
        // TODO level based glancing/hit/crit

        double target_defence = level * 5;
        double skill_diff = target_defence - weapon_skill_;

        // Crit chance
        double crit_chance = total_special_stats_.critical_strike - skill_diff * 0.2;

        // Miss chance
        double base_miss_chance = (5 + skill_diff * 0.1);
        double dw_miss_chance = (base_miss_chance * 0.8 + 20);
        double miss_chance = dw_miss_chance - total_special_stats_.hit;

        // Dodge chance
        double dodge_chance = (5 + skill_diff * 0.1);

        // Glancing blows
        double glancing_chance = 40;
//        double glancing_damage = 15;

        // Order -> Miss, parry, dodge, block, glancing, crit, hit.
        std::vector<double> hit_table = {miss_chance};
        hit_table.push_back(hit_table.back() + dodge_chance);
        hit_table.push_back(hit_table.back() + glancing_chance);
        hit_table.push_back(hit_table.back() + crit_chance);
        if (hit_table.back() > 100)
        {
            std::cout << "crit capped!!" << "\n";
        }

        return {hit_table};
    };

    double simulate(double sim_time, double dt, int level, int iterations)
    {
        double time = 0;

        Hit_table hit_table = compute_hit_table(63);

        double total_damage = 0;

        while (time < sim_time)
        {
            auto damage_main = weapons[0].step(dt, total_special_stats_.attack_power, true);
            auto filtered_damage = hit_table.generate_hit(damage_main);
            total_damage += filtered_damage;

            auto damage_off = weapons[1].step(dt, total_special_stats_.attack_power, false);
            filtered_damage = hit_table.generate_hit(damage_main);
            total_damage += filtered_damage;

            time += dt;
        }

        return total_damage / sim_time;
    }

    template<typename T, typename ...Ts>
    void equip_armor(T piece, Ts ...pieces)
    {
        equip_armor(piece);
        equip_armor(pieces...);
    }

    template<typename T>
    void equip_armor(T piece)
    {
        gear.emplace_back(piece);
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
        weapons.emplace_back(piece);
    }

private:
    Stats base_stats_;
    Special_stats total_special_stats_;
    double weapon_skill_;

    std::vector<Armor> gear;
    std::vector<Weapon> weapons;
};


int main()
{
    Character character{Race::human};

//    std::vector<std::string> gear{"Lionheart_helm", "Onyxia_tooth_pendant"};
//    std::vector<std::string> weapons{"Brutality_blade", "Mirahs_song"};

    Armor Lionheart_helm = Armor{Stats{0, 18}, Special_stats{2, 2, 0}, Armor::Socket::head};
    Armor Onyxia_tooth_pendant = Armor{Stats{13, 0}, Special_stats{1, 1, 0}, Armor::Socket::neck};

    Weapon Brutality_blade = Weapon{2.5, {100, 150}, Stats{9, 9}, Special_stats{1, 0, 0}, Weapon::Socket::main_hand};
    Weapon Mirahs_song = Weapon{1.6, {50, 100}, Stats{9, 9}, Special_stats{0, 0, 0}, Weapon::Socket::off_hand};

    character.equip_armor(Lionheart_helm, Onyxia_tooth_pendant);
    character.equip_weapon(Brutality_blade, Mirahs_song);


    character.compute_special_stats();
    double dps = character.simulate(60, 0.1, 63, 1);

    std::cout << "Dps from simulation: " << dps << "\n";

    return 0;
}

















