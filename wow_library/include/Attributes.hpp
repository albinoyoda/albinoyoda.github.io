#ifndef WOW_SIMULATOR_STATS_HPP
#define WOW_SIMULATOR_STATS_HPP

#include <ostream>
#include <vector>

double multiplicative_addition(double val1, double val2);

double multiplicative_subtraction(double val1, double val2);

enum class Stat
{
    agi,
    str,
    sta,
    hit,
    crit,
    haste,
    ap,
    hp,
    armor,
    def,
    sword,
    axe,
    dagger,
    mace,
    fist,
    dam_mult,
    str_mult,
    agi_mult,
    sta_mult,
    bonus_damage,
    thr_mod,
    def_mod,
};

using Stat_list = std::vector<std::pair<Stat, int>>;

struct Total_stats
{
    Total_stats() = default;

    Total_stats operator+(const Total_stats& rhs) const;

    Total_stats operator-(const Total_stats& rhs) const;

    Total_stats& operator+=(const Total_stats& rhs);

    Total_stats& operator-=(const Total_stats& rhs);

    double critical_strike{};
    double hit{};
    double attack_power{};
    double hitpoints{};
    double armor{};
    double haste{};
    int defence_skill{};
    int sword_skill{};
    int axe_skill{};
    int dagger_skill{};
    int mace_skill{};
    int fist_skill{};
    double damage_multiplier{};
    double strength_multiplier{};
    double agility_multiplier{};
    double stamina_multiplier{};
    double bonus_damage{};
    double threat_modifier{};
    double damage_taken_mod{};
};

class Attributes
{
public:
    Attributes() = default;

    Attributes(double strength, double agility, double stamina);

    Total_stats convert_to_total_stats(const Total_stats& total_stats) const;

    Attributes operator+(const Attributes& rhs);

    Attributes& operator+=(const Attributes& rhs);

    double strength;
    double agility;
    double stamina;
};

Attributes create_attributes(const Stat_list& stat_list);

Total_stats create_stats(const Stat_list& stat_list);

std::ostream& operator<<(std::ostream& os, Total_stats const& total_stats);

std::ostream& operator<<(std::ostream& os, Attributes const& stats);

#endif // WOW_SIMULATOR_STATS_HPP