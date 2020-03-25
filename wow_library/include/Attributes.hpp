#ifndef WOW_SIMULATOR_STATS_HPP
#define WOW_SIMULATOR_STATS_HPP

#include <ostream>
#include <vector>

enum class Skill_type
{
    sword,
    axe,
    mace,
    dagger,
    none
};

struct Bonus_skill
{
    Bonus_skill() = default;

    Bonus_skill(Skill_type type, size_t amount)
    {
        switch (type)
        {
            case Skill_type::sword :
                sword_skill += amount;
                break;
            case Skill_type::axe :
                axe_skill += amount;
                break;
            case Skill_type::mace :
                mace_skill += amount;
                break;
            case Skill_type::dagger :
                dagger_skill += amount;
                break;
            default:
                break;
        }
    };

    size_t sword_skill{};
    size_t axe_skill{};
    size_t mace_skill{};
    size_t dagger_skill{};

    Bonus_skill operator+(const Bonus_skill &rhs)
    {
        (*this).sword_skill += rhs.sword_skill;
        (*this).axe_skill += rhs.axe_skill;
        (*this).mace_skill += rhs.mace_skill;
        (*this).dagger_skill += rhs.dagger_skill;
        return (*this);
    }

    Bonus_skill &operator+=(const Bonus_skill &rhs)
    {
        *this = *this + rhs;
        return *this;
    }
};

struct Special_stats
{
    Special_stats() = default;

    Special_stats(double critical_strike, double hit, double attack_power, double chance_for_extra_hit = 0.0,
                  double haste = 0.0,
                  const std::vector<Bonus_skill> &bonus_skill = std::vector<Bonus_skill>()) :
            critical_strike{critical_strike},
            hit{hit},
            attack_power{attack_power},
            chance_for_extra_hit(chance_for_extra_hit),
            haste(haste)
    {
        for (const auto &skill : bonus_skill)
        {
            this->bonus_skill.emplace_back(skill);
        }
    };

    void clear()
    {
        critical_strike = 0;
        hit = 0;
        attack_power = 0;
        chance_for_extra_hit = 0;
        haste = 0;
    }

    Special_stats operator+(const Special_stats &rhs)
    {
        for (const auto &skill : rhs.bonus_skill)
        {
            this->bonus_skill.emplace_back(skill);
        }
        return {this->critical_strike + rhs.critical_strike, this->hit + rhs.hit,
                this->attack_power + rhs.attack_power,
                this->chance_for_extra_hit + rhs.chance_for_extra_hit,
                this->haste + rhs.haste,
                this->bonus_skill};
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
    std::vector<Bonus_skill> bonus_skill;
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
        return {this->agility / 20, 0, this->strength * 2, 0, 0};
    }

    Attributes operator+(const Attributes &rhs)
    {
        return {this->strength + rhs.strength, this->agility + rhs.agility};
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

std::ostream &operator<<(std::ostream &os, Special_stats const &special_stats)
{
    os << "Characters special stats: " << "\n";
    os << "hit: " << special_stats.hit << "\n";
    os << "crit: " << special_stats.critical_strike << "\n";
    os << "attack power: " << special_stats.attack_power << "\n";
    return os;
}

std::ostream &operator<<(std::ostream &os, Attributes const &stats)
{
    os << "Characters stats: " << "\n";
    os << "strength: " << stats.strength << "\n";
    os << "agility: " << stats.agility << "\n";
    return os;
}

#endif //WOW_SIMULATOR_STATS_HPP