#ifndef WOW_SIMULATOR_DAMAGE_SOURCES_HPP
#define WOW_SIMULATOR_DAMAGE_SOURCES_HPP

#include <vector>
#include "Statistics.hpp"

enum class Damage_source
{
    white_mh,
    white_oh,
    bloodthirst,
    execute,
    heroic_strike,
    whirlwind,
    item_hit_effects
};

struct Damage_sources
{
    Damage_sources() = default;

    Damage_sources &operator+(const Damage_sources &rhs)
    {
        whirlwind_damage = whirlwind_damage + rhs.whirlwind_damage;
        bloodthirst_damage = bloodthirst_damage + rhs.bloodthirst_damage;
        execute_damage = execute_damage + rhs.execute_damage;
        white_mh_damage = white_mh_damage + rhs.white_mh_damage;
        white_oh_damage = white_oh_damage + rhs.white_oh_damage;
        heroic_strike_damage = heroic_strike_damage + rhs.heroic_strike_damage;
        item_hit_effects_damage = item_hit_effects_damage + rhs.item_hit_effects_damage;

        whirlwind_count = whirlwind_count + rhs.whirlwind_count;
        bloodthirst_count = bloodthirst_count + rhs.bloodthirst_count;
        execute_count = execute_count + rhs.execute_count;
        white_mh_count = white_mh_count + rhs.white_mh_count;
        white_oh_count = white_oh_count + rhs.white_oh_count;
        heroic_strike_count = heroic_strike_count + rhs.heroic_strike_count;
        item_hit_effects_count = item_hit_effects_count + rhs.item_hit_effects_count;
        return *(this);
    }

    constexpr double sum_damage_sources() const
    {
        return white_mh_damage + white_oh_damage + bloodthirst_damage + heroic_strike_damage + whirlwind_damage + execute_damage + item_hit_effects_damage;
    }

    constexpr double sum_counts() const
    {
        return white_mh_count + white_oh_count + bloodthirst_count + heroic_strike_count + whirlwind_count + execute_count + item_hit_effects_count;
    }

    void add_damage(Damage_source source, double damage)
    {
        switch (source)
        {
            case Damage_source::white_mh:
                white_mh_damage += damage;
                white_mh_count++;
                break;
            case Damage_source::white_oh:
                white_oh_damage += damage;
                white_oh_count++;
                break;
            case Damage_source::bloodthirst:
                bloodthirst_damage += damage;
                bloodthirst_count++;
                break;
            case Damage_source::execute:
                execute_damage += damage;
                execute_count++;
                break;
            case Damage_source::heroic_strike:
                heroic_strike_damage += damage;
                heroic_strike_count++;
                break;
            case Damage_source::whirlwind:
                whirlwind_damage += damage;
                whirlwind_count++;
                break;
            case Damage_source::item_hit_effects:
                item_hit_effects_damage += damage;
                item_hit_effects_count++;
                break;
        }
    }

    double white_mh_damage{};
    double white_oh_damage{};
    double bloodthirst_damage{};
    double execute_damage{};
    double heroic_strike_damage{};
    double whirlwind_damage{};
    double item_hit_effects_damage{};

    long int white_mh_count{};
    long int white_oh_count{};
    long int bloodthirst_count{};
    long int execute_count{};
    long int heroic_strike_count{};
    long int whirlwind_count{};
    long int item_hit_effects_count{};
};

template<typename T>
std::vector<double> extract_percentage(const std::vector<Damage_sources> &damage_sources_vector, T field_ptr)
{
    std::vector<double> vector_of_t;
    vector_of_t.reserve(damage_sources_vector.size());
    for (const auto &damage_source : damage_sources_vector)
    {
        vector_of_t.emplace_back(damage_source.*field_ptr / damage_source.sum_damage_sources());
    }
    return vector_of_t;
}

template<typename T>
std::vector<double> extract_field(const std::vector<Damage_sources> &damage_sources_vector, T field_ptr)
{
    std::vector<double> vector_of_t;
    vector_of_t.reserve(damage_sources_vector.size());
    for (const auto &damage_source : damage_sources_vector)
    {
        vector_of_t.emplace_back(damage_source.*field_ptr);
    }
    return vector_of_t;
}

template<typename T1, typename T2>
std::vector<double>
extract_field_average(const std::vector<Damage_sources> &damage_sources_vector, T1 field_damage_ptr, T2 field_count_ptr)
{
    std::vector<double> vector_of_t;
    vector_of_t.reserve(damage_sources_vector.size());
    for (const auto &damage_source : damage_sources_vector)
    {
        if (damage_source.*field_count_ptr != 0)
        {
            vector_of_t.emplace_back(damage_source.*field_damage_ptr / damage_source.*field_count_ptr);
        }
        else
        {
            vector_of_t.emplace_back(0);
        }
    }
    return vector_of_t;
}

void print_damage_sources(const std::string &source_name, double source_percent,
                          double source_std, double source_count, double avg_value)
{
    if (source_count > 0)
    {
        std::cout << std::setw(15) << source_name << std::left << std::setprecision(3)
                  << std::setw(5) << 100 * source_percent << " +- " << std::setw(5) << 100 * source_std << ", casts: "
                  << source_count << ", average damage per cast: " << std::setprecision(6) << avg_value << "\n";
    }
}

template<typename T1, typename T2>
void extract_damage_source_data(const std::vector<Damage_sources> &damage_sources_vector, const std::string &name,
                                T1 source_damage_field, T2 source_count_field)
{
    auto source_percentage_vec = extract_percentage(damage_sources_vector, source_damage_field);
    auto source_percentage_avg = Statistics::average(source_percentage_vec);
    auto source_percentage_std = Statistics::standard_deviation(source_percentage_vec, source_percentage_avg);

    auto source_count_vec = extract_field(damage_sources_vector, source_count_field);
    auto source_count_avg = Statistics::average(source_count_vec);

    auto source_damage_vec = extract_field_average(damage_sources_vector, source_damage_field, source_count_field);
    auto source_damage_avg = Statistics::average(source_damage_vec);

    print_damage_sources(name + ":", source_percentage_avg, source_percentage_std, source_count_avg, source_damage_avg);
}

void print_damage_source_vector(const std::vector<Damage_sources> &damage_sources_vector)
{
    std::cout << "Damage_sources (%):\n";
    extract_damage_source_data(damage_sources_vector, "White MH", &Damage_sources::white_mh_damage,
                               &Damage_sources::white_mh_count);
    extract_damage_source_data(damage_sources_vector, "White OH", &Damage_sources::white_oh_damage,
                               &Damage_sources::white_oh_count);
    extract_damage_source_data(damage_sources_vector, "Bloodthirst", &Damage_sources::bloodthirst_damage,
                               &Damage_sources::bloodthirst_count);
    extract_damage_source_data(damage_sources_vector, "Execute", &Damage_sources::execute_damage,
                               &Damage_sources::execute_count);
    extract_damage_source_data(damage_sources_vector, "Heroic strike", &Damage_sources::heroic_strike_damage,
                               &Damage_sources::heroic_strike_count);
    extract_damage_source_data(damage_sources_vector, "Whirlwind", &Damage_sources::whirlwind_damage,
                               &Damage_sources::whirlwind_count);
    extract_damage_source_data(damage_sources_vector, "Item effects", &Damage_sources::item_hit_effects_damage,
                               &Damage_sources::item_hit_effects_count);
    std::cout << "\n";

}

#endif //WOW_SIMULATOR_DAMAGE_SOURCES_HPP
