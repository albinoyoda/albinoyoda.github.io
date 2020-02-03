#ifndef WOW_SIMULATOR_STATS_HPP
#define WOW_SIMULATOR_STATS_HPP

#include <ostream>

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

    void clear();
};

class Stats
{
public:
    Stats() = default;

    Stats(double strength, double agility) : strength{strength}, agility{agility} {};
    double strength;
    double agility;

    Stats operator+(const Stats &rhs);

    Stats &operator+=(const Stats &rhs);

    double compute_attack_power();

    Special_stats convert_to_special_stats();
};

Stats operator+(const Stats &lhs, const Stats &rhs);

std::ostream &operator<<(std::ostream &os, Special_stats const &special_stats);

#endif //WOW_SIMULATOR_STATS_HPP
