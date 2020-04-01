#ifndef WOW_SIMULATOR_STATS_HPP
#define WOW_SIMULATOR_STATS_HPP

#include <ostream>
#include <vector>

struct Special_stats
{
    Special_stats() = default;

    Special_stats(double critical_strike, double hit, double attack_power, double chance_for_extra_hit = 0.0,
                  double haste = 0.0, size_t sword_skill = 0, size_t axe_skill = 0, size_t dagger_skill = 0,
                  size_t mace_skill = 0) :
            critical_strike{critical_strike},
            hit{hit},
            attack_power{attack_power},
            chance_for_extra_hit(chance_for_extra_hit),
            haste(haste),
            sword_skill(sword_skill),
            axe_skill(axe_skill),
            dagger_skill(dagger_skill),
            mace_skill(mace_skill) {}

    void clear()
    {
        critical_strike = 0;
        hit = 0;
        attack_power = 0;
        chance_for_extra_hit = 0;
        haste = 0;
        sword_skill = 0;
        axe_skill = 0;
        dagger_skill = 0;
        mace_skill = 0;
    }

    Special_stats operator+(const Special_stats &rhs)
    {
        return {critical_strike + rhs.critical_strike,
                hit + rhs.hit,
                attack_power + rhs.attack_power,
                chance_for_extra_hit + rhs.chance_for_extra_hit,
                haste + rhs.haste,
                sword_skill + rhs.sword_skill,
                axe_skill + rhs.axe_skill,
                dagger_skill + rhs.dagger_skill,
                mace_skill + rhs.mace_skill
        };
    }

    Special_stats &operator+=(const Special_stats &rhs)
    {
        *this = *this + rhs;
        return *this;
    }

    double critical_strike{};
    double hit{};
    double attack_power{};
    double chance_for_extra_hit{};
    double haste{};
    size_t sword_skill{};
    size_t axe_skill{};
    size_t dagger_skill{};
    size_t mace_skill{};
};

class Attributes
{
public:
    Attributes() = default;

    Attributes(double strength, double agility) : strength{strength}, agility{agility} {};

    void clear()
    {
        strength = 0;
        agility = 0;
    }

    Special_stats convert_to_special_stats() const
    {
        return {agility / 20, 0, strength * 2, 0, 0};
    }

    Attributes operator+(const Attributes &rhs)
    {
        return {strength + rhs.strength, agility + rhs.agility};
    }

    Attributes &operator+=(const Attributes &rhs)
    {
        *this = *this + rhs;
        return *this;
    }

    Attributes operator*(double rhs)
    {
        return {this->strength * rhs, this->agility * rhs};
    }

    Attributes &operator*=(double rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    double strength;
    double agility;
};

std::ostream &operator<<(std::ostream &os, Special_stats const &special_stats);
//{
//    os << "Characters special stats: " << "\n";
//    os << "hit: " << special_stats.hit << "\n";
//    os << "crit: " << special_stats.critical_strike << "\n";
//    os << "attack power: " << special_stats.attack_power << "\n";
//    return os;
//}

std::ostream &operator<<(std::ostream &os, Attributes const &stats);
//{
//    os << "Characters stats: " << "\n";
//    os << "strength: " << stats.strength << "\n";
//    os << "agility: " << stats.agility << "\n";
//    return os;
//}

#endif //WOW_SIMULATOR_STATS_HPP