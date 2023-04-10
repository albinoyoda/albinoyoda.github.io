#ifndef WOW_SIMULATOR_HIT_TABLES_HPP
#define WOW_SIMULATOR_HIT_TABLES_HPP

#include "Attributes.hpp"
#include "Item.hpp"
#include "statistics/uniform.hpp"
#include "weapon_sim.hpp"

#include <common/math.hpp>
#include <vector>

enum class Melee_result
{
    miss,
    dodge,
    glancing,
    crit,
    hit,
};

enum class Hit_color
{
    white,
    yellow,
};

struct Hit_table_result
{
    Hit_table_result() = delete;
    Hit_table_result(double damage_multiplier, Melee_result melee_result)
        : damage_multiplier{damage_multiplier}, melee_result{melee_result} {};

    double damage_multiplier;
    Melee_result melee_result;
};

struct Hit_outcome
{
    Hit_outcome() = delete;
    Hit_outcome(const Hit_table_result& hit_table_result, double damage, double armor_reduction,
                double physical_damage_mod)
        : hit_table_result{hit_table_result}
        , damage{damage * hit_table_result.damage_multiplier * armor_reduction * (1 + physical_damage_mod)} {};

    Hit_table_result hit_table_result;
    double damage;
};

class Hit_tables
{
public:
    Hit_tables() = default;

    Hit_tables(int target_level, int overpower_talents, int impale_talents);

    void compute_hit_table(int weapon_skill, const Special_stats& special_stats, Socket weapon_hand,
                           Weapon_socket weapon_socket);

    [[nodiscard]] Hit_table_result generate_hit(Hit_color hit_type, Socket weapon_hand, bool is_overpower,
                                                bool is_spell, bool is_ability_queued) const;

    [[nodiscard]] Hit_table_result generate_hit_mh(Hit_color hit_type, bool is_overpower, bool is_spell) const;

    [[nodiscard]] Hit_table_result generate_hit_oh(bool is_ability_queued) const;

    [[nodiscard]] const std::vector<double>& get_hit_probabilities_white_mh() const { return white_mh_; }

    [[nodiscard]] const std::vector<double>& get_hit_probabilities_white_oh() const { return white_oh_; }

    [[nodiscard]] const std::vector<double>& get_hit_probabilities_white_2h() const { return two_hand_; }

    [[nodiscard]] const std::vector<double>& get_hit_probabilities_yellow() const { return yellow_; }

    [[nodiscard]] double get_glancing_penalty_mh() const { return damage_multipliers_white_mh_[2]; }

    [[nodiscard]] double get_glancing_penalty_oh() const { return damage_multipliers_white_oh_[2]; }

    [[nodiscard]] double get_probability(Weapon_socket weapon_socket, Hit_color hit_type, Melee_result hit_result,
                                         bool is_spell = false) const;

    [[nodiscard]] std::vector<double> get_table(Weapon_socket weapon_socket, Hit_color hit_type, bool is_spell) const;
    int target_level{};
    int overpower_talents{};
    int impale_talents{};

    std::vector<double> white_mh_;
    std::vector<double> damage_multipliers_white_mh_;
    std::vector<double> white_oh_;
    std::vector<double> damage_multipliers_white_oh_;
    std::vector<double> yellow_;
    std::vector<double> damage_multipliers_yellow_;
    std::vector<double> yellow_spell_;
    std::vector<double> overpower_;
    std::vector<double> two_hand_;
};

void cout_damage_parse(Hit_color hit_type, Socket weapon_hand, Hit_table_result hit_outcome);

std::string hit_result_to_string(Melee_result hit_result);

#endif // WOW_SIMULATOR_HIT_TABLES_HPP
