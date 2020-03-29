#ifndef WOW_SIMULATOR_DAMAGE_SOURCES_HPP
#define WOW_SIMULATOR_DAMAGE_SOURCES_HPP

#include <vector>

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
        whirlwind_count = whirlwind_count + rhs.whirlwind_count;
        bloodthirst_count = bloodthirst_count + rhs.bloodthirst_count;
        execute_count = execute_count + rhs.execute_count;
        white_mh_count = white_mh_count + rhs.white_mh_count;
        white_oh_count = white_oh_count + rhs.white_oh_count;
        heroic_strike_count = heroic_strike_count + rhs.heroic_strike_count;
        return *(this);
    }

    constexpr double sum_damage_sources() const
    {
        return white_mh_damage + white_oh_damage + bloodthirst_damage + heroic_strike_damage + whirlwind_damage + execute_damage;
    }

    constexpr double sum_counts() const
    {
        return white_mh_count + white_oh_count + bloodthirst_count + heroic_strike_count + whirlwind_count + execute_count;
    }

    enum class Damage_source
    {
        white_mh,
        white_oh,
        bloodthirst,
        execute,
        heroic_strike,
        whirlwind,
    };

    void add_damage(Damage_source source, double damage)
    {
        switch (source)
        {
            case Damage_source::white_mh:
                white_mh_damage += damage;
                white_mh_count++;
            case Damage_source::white_oh:
                white_oh_damage += damage;
                white_oh_count++;
            case Damage_source::bloodthirst:
                bloodthirst_damage += damage;
                bloodthirst_count++;
            case Damage_source::execute:
                execute_damage += damage;
                execute_count++;
            case Damage_source::heroic_strike:
                heroic_strike_damage += damage;
                heroic_strike_count++;
            case Damage_source::whirlwind:
                whirlwind_damage += damage;
                whirlwind_count++;
        }
    }

//    void print(const std::string &source_name, double source_percent,
//                              double source_std, double source_count) const
//    {
//        std::cout << source_name << std::setw(5) << std::left << std::setprecision(3)
//                  << 100 * source_percent << " +- " << std::setw(4) << 100 * source_std << ", casts: "
//                  << source_count << "\n";
//    }

    double white_mh_damage{};
    double white_oh_damage{};
    double bloodthirst_damage{};
    double execute_damage{};
    double heroic_strike_damage{};
    double whirlwind_damage{};
    size_t white_mh_count{};
    size_t white_oh_count{};
    size_t bloodthirst_count{};
    size_t execute_count{};
    size_t heroic_strike_count{};
    size_t whirlwind_count{};
};

#endif //WOW_SIMULATOR_DAMAGE_SOURCES_HPP
