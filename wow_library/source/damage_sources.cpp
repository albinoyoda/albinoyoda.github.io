#include <iomanip>
#include <iostream>
#include "damage_sources.hpp"

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

