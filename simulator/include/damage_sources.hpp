#ifndef WOW_SIMULATOR_DAMAGE_SOURCES_HPP
#define WOW_SIMULATOR_DAMAGE_SOURCES_HPP

#include <string>
#include <vector>

enum class Damage_source
{
    white_mh,
    white_oh,
    slam,
    mortal_strike,
    sweeping_strikes,
    overpower,
    bloodthirst,
    execute,
    heroic_strike,
    cleave,
    whirlwind,
    hamstring,
    deep_wounds,
    item_hit_effects
};

struct Damage_instance
{
    Damage_instance(Damage_source source, double damage, double time_stamp)
        : damage_source(source), damage(damage), time_stamp(time_stamp){};
    Damage_source damage_source;
    double damage{};
    double time_stamp{};
};

struct Damage_sources
{
    Damage_sources();

    ~Damage_sources() = default;

    Damage_sources& operator+(const Damage_sources& rhs);

    constexpr double sum_damage_sources() const
    {
        return white_mh_damage + white_oh_damage + bloodthirst_damage + mortal_strike_damage + slam_damage +
               overpower_damage + heroic_strike_damage + cleave_damage + whirlwind_damage + hamstring_damage +
               execute_damage + deep_wounds_damage + item_hit_effects_damage + sweeping_strikes_damage;
    }

    constexpr double sum_counts() const
    {
        return white_mh_count + white_oh_count + bloodthirst_count + mortal_strike_count + slam_count +
               overpower_count + heroic_strike_count + cleave_count + whirlwind_count + hamstring_count +
               execute_count + deep_wounds_count + item_hit_effects_count + sweeping_strikes_count;
    }

    void add_damage(Damage_source source, double damage, double time_stamp, bool increment_counter = true);

    double white_mh_damage{};
    double white_oh_damage{};
    double slam_damage{};
    double overpower_damage{};
    double bloodthirst_damage{};
    double mortal_strike_damage{};
    double sweeping_strikes_damage{};
    double execute_damage{};
    double heroic_strike_damage{};
    double cleave_damage{};
    double whirlwind_damage{};
    double hamstring_damage{};
    double deep_wounds_damage{};
    double item_hit_effects_damage{};

    long int white_mh_count{};
    long int white_oh_count{};
    long int slam_count{};
    long int overpower_count{};
    long int bloodthirst_count{};
    long int mortal_strike_count{};
    long int sweeping_strikes_count{};
    long int execute_count{};
    long int heroic_strike_count{};
    long int cleave_count{};
    long int whirlwind_count{};
    long int hamstring_count{};
    long int deep_wounds_count{};
    long int item_hit_effects_count{};

    std::vector<Damage_instance> damage_instances;
};

#endif // WOW_SIMULATOR_DAMAGE_SOURCES_HPP
