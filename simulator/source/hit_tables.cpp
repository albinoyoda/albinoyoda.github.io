#include "simulator/hit_tables.hpp"

namespace
{
double compute_crit_chance(const Special_stats& special_stats, int level_difference, int base_skill_diff)
{
    double crit_chance;
    if (level_difference == 3)
    {
        crit_chance = special_stats.critical_strike - base_skill_diff * 0.2 - 1.8; // 1.8 flat aura modifier
    }
    else if (level_difference > 0)
    {
        crit_chance = special_stats.critical_strike - base_skill_diff * 0.2;
    }
    else
    {
        crit_chance = special_stats.critical_strike + base_skill_diff * 0.04;
    }
    crit_chance = std::max(crit_chance, 0.0);
    return crit_chance;
}
} // namespace

std::vector<double> create_hit_table(double miss, double dodge, double glancing, double crit)
{
    // Order -> Miss, parry, dodge, block, glancing, crit, hit.
    return {miss, miss + dodge, miss + dodge + glancing, miss + dodge + glancing + crit};
}

std::vector<double> create_hit_table_yellow(double miss, double dodge, double crit, bool double_roll)
{
    double double_roll_factor = double_roll ? (100.0 - miss - dodge) / 100.0 : 1.0;
    // Order -> Miss, parry, dodge, block, glancing, crit, hit.
    // double_roll_factor compensates for the crit suppression caused by ability double roll
    return {miss, miss + dodge, miss + dodge, miss + dodge + double_roll_factor * crit};
}

std::vector<double> create_multipliers(double glancing_factor, double crit_damage_bonus, double bonus_crit_multiplier)
{
    // Order -> Miss, parry, dodge, block, glancing, crit, hit.
    return {0.0, 0.0, glancing_factor, 1.0 + multiplicative_addition(1.0 + crit_damage_bonus, bonus_crit_multiplier),
            1.0};
}

Hit_tables::Hit_tables(int target_level, int overpower_talents, int impale_talents)
    : target_level{target_level}, overpower_talents{overpower_talents}, impale_talents{impale_talents}
{
}

void Hit_tables::compute_hit_table(int weapon_skill, const Special_stats& special_stats, Socket weapon_hand,
                                   Weapon_socket weapon_socket)
{
    int level_difference = target_level - 60;
    int target_defence_level = target_level * 5;
    int skill_diff = target_defence_level - weapon_skill;
    int base_skill_diff = level_difference * 5;

    double crit_chance = compute_crit_chance(special_stats, level_difference, base_skill_diff);

    // Miss chance
    double base_miss_chance;
    int hit_penalty = 0;
    if (skill_diff > 10)
    {
        base_miss_chance = 5.0 + skill_diff * 0.2;
        hit_penalty = 1;
    }
    else if (skill_diff > 0)
    {
        base_miss_chance = 5.0 + skill_diff * 0.1;
    }
    else
    {
        base_miss_chance = 5.0;
    }
    double dw_miss_chance =
        (weapon_socket == Weapon_socket::two_hand) ? base_miss_chance : (base_miss_chance * 0.8 + 20.0);
    double corrected_hit = special_stats.hit - hit_penalty;
    double miss_chance = dw_miss_chance - std::max(corrected_hit, 0.0);
    miss_chance = std::max(miss_chance, 0.0);
    double two_hand_miss_chance = std::max(base_miss_chance - corrected_hit, 0.0);

    // Dodge chance
    double dodge_chance;
    if (level_difference > 0)
    {
        dodge_chance = std::max(5 + skill_diff * 0.1, 5.0);
    }
    else
    {
        dodge_chance = std::max(5 - base_skill_diff * 0.04, 0.0);
    }

    // Glancing blows
    double glancing_chance = 0.0;
    if (level_difference > 0)
    {
        glancing_chance = 10 + level_difference * 10;
    }

    double glancing_penalty;
    if (skill_diff >= 8)
    {
        glancing_penalty = 35.0 - (15.0 - skill_diff) * 4.0;
    }
    else
    {
        glancing_penalty = 5.0;
    }

    if (weapon_hand == Socket::main_hand)
    {
        white_mh_ = create_hit_table(miss_chance, dodge_chance, glancing_chance, crit_chance);
        damage_multipliers_white_mh_ =
            create_multipliers((100.0 - glancing_penalty) / 100.0, 0.0, special_stats.crit_multiplier);

        yellow_spell_ = create_hit_table_yellow(two_hand_miss_chance, dodge_chance, crit_chance, true);
        yellow_ = create_hit_table_yellow(two_hand_miss_chance, dodge_chance, crit_chance, false);
        overpower_ =
            create_hit_table_yellow(two_hand_miss_chance, 0, crit_chance + 25 * overpower_talents - 3.0, false);
        damage_multipliers_yellow_ = create_multipliers(1.0, 0.1 * impale_talents, special_stats.crit_multiplier);
    }
    else
    {
        white_oh_ = create_hit_table(miss_chance, dodge_chance, glancing_chance, crit_chance);
        damage_multipliers_white_oh_ =
            create_multipliers((100.0 - glancing_penalty) / 100.0, 0.0, special_stats.crit_multiplier);

        two_hand_ = create_hit_table(two_hand_miss_chance, dodge_chance, glancing_chance, crit_chance);
    }
}

Hit_table_result Hit_tables::generate_hit(Hit_color hit_type, Socket weapon_hand, bool is_overpower, bool is_spell,
                                          bool is_ability_queued) const
{
    if (weapon_hand == Socket::main_hand)
    {
        return generate_hit_mh(hit_type, is_overpower, is_spell);
        //            cout_damage_parse(hit_type, weapon_hand, hit_outcome);
    }
    else
    {
        return generate_hit_oh(is_ability_queued);
        // cout_damage_parse(hit_type, weapon_hand, hit_outcome);
    }
}

Hit_table_result Hit_tables::generate_hit_mh(Hit_color hit_type, bool is_overpower, bool is_spell) const
{
    double random_var = get_uniform_random(100);
    if (hit_type == Hit_color::white)
    {
        //            simulator_cout("Drawing outcome from MH hit table");
        size_t outcome = std::lower_bound(white_mh_.begin(), white_mh_.end(), random_var) - white_mh_.begin();
        return {damage_multipliers_white_mh_[outcome], Melee_result(outcome)};
    }
    else
    {
        //            simulator_cout("Drawing outcome from yellow table");
        if (is_spell)
        {
            size_t outcome =
                std::lower_bound(yellow_spell_.begin(), yellow_spell_.end(), random_var) - yellow_spell_.begin();
            return {damage_multipliers_yellow_[outcome], Melee_result(outcome)};
        }
        else if (is_overpower)
        {
            size_t outcome = std::lower_bound(overpower_.begin(), overpower_.end(), random_var) - overpower_.begin();
            return {damage_multipliers_yellow_[outcome], Melee_result(outcome)};
        }
        else
        {
            size_t outcome = std::lower_bound(yellow_.begin(), yellow_.end(), random_var) - yellow_.begin();
            return {damage_multipliers_yellow_[outcome], Melee_result(outcome)};
        }
    }
}

Hit_table_result Hit_tables::generate_hit_oh(bool is_ability_queued) const
{
    if (is_ability_queued)
    {
        //            simulator_cout("Drawing outcome from OH hit table (without DW penalty)");
        double random_var = get_uniform_random(100);
        size_t outcome = std::lower_bound(two_hand_.begin(), two_hand_.end(), random_var) - two_hand_.begin();
        return {damage_multipliers_white_oh_[outcome], Melee_result(outcome)};
    }
    else
    {
        //            simulator_cout("Drawing outcome from OH hit table");
        double random_var = get_uniform_random(100);
        size_t outcome = std::lower_bound(white_oh_.begin(), white_oh_.end(), random_var) - white_oh_.begin();
        return {damage_multipliers_white_oh_[outcome], Melee_result(outcome)};
    }
}

double Hit_tables::get_probability(Weapon_socket weapon_socket, Hit_color hit_type, Melee_result hit_result,
                                   bool is_spell) const
{
    auto table{get_table(weapon_socket, hit_type, is_spell)};
    // Order -> Miss, parry, dodge, block, glancing, crit, hit.
    switch (hit_result)
    {
    case Melee_result::miss:
        return table[0];
    case Melee_result::dodge:
        return table[1] - table[0];
    case Melee_result::glancing:
        return table[2] - table[1];
    case Melee_result::crit:
        return table[3] - table[2];
    case Melee_result::hit:
        return 1 - table[3];
    }
    throw std::runtime_error("Hit table probability not found!");
}

[[nodiscard]] std::vector<double> Hit_tables::get_table(Weapon_socket weapon_socket, Hit_color hit_type,
                                                        bool is_spell) const
{
    switch (hit_type)
    {
    case Hit_color::white:
        switch (weapon_socket)
        {
        case Weapon_socket::main_hand:
            return white_mh_;

        case Weapon_socket::off_hand:
            return white_oh_;

        case Weapon_socket::two_hand:
            return two_hand_;

        case Weapon_socket::one_hand:
            break;
        }
        break;
    case Hit_color::yellow:
        switch (weapon_socket)
        {
        case Weapon_socket::main_hand:
        case Weapon_socket::off_hand:
        case Weapon_socket::two_hand:
            if (is_spell)
            {
                return yellow_;
            }

            return yellow_spell_;
        case Weapon_socket::one_hand:
            break;
        }
    }
    throw std::runtime_error("Hit table not found!");
}

void cout_damage_parse(Hit_color hit_type, Socket weapon_hand, Hit_table_result hit_outcome)
{
    if (weapon_hand == Socket::main_hand)
    {
        if (hit_type == Hit_color::white)
        {
            switch (hit_outcome.melee_result)
            {
            case Melee_result::glancing:
                //                simulator_cout("Mainhand glancing hit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Melee_result::hit:
                //                simulator_cout("Mainhand white hit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Melee_result::crit:
                //                simulator_cout("Mainhand crit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Melee_result::dodge:
                // simulator_cout("Mainhand hit dodged");
                break;
            case Melee_result::miss:
                // simulator_cout("Mainhand hit missed");
                break;
            }
        }
        else
        {
            switch (hit_outcome.melee_result)
            {
            case Melee_result::glancing:
                // simulator_cout("BUG: Ability glanced for: ", int(hit_outcome.damage), " damage.");
                break;
            case Melee_result::hit:
                // simulator_cout("Ability hit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Melee_result::crit:
                // simulator_cout("Ability crit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Melee_result::dodge:
                // simulator_cout("Ability dodged");
                break;
            case Melee_result::miss:
                // simulator_cout("Ability missed");
                break;
            }
        }
    }
    else
    {
        switch (hit_outcome.melee_result)
        {
        case Melee_result::glancing:
            // simulator_cout("Offhand glancing hit for: ", int(hit_outcome.damage), " damage.");
            break;
        case Melee_result::hit:
            // simulator_cout("Offhand white hit for: ", int(hit_outcome.damage), " damage.");
            break;
        case Melee_result::crit:
            // simulator_cout("Offhand crit for: ", int(hit_outcome.damage), " damage.");
            break;
        case Melee_result::dodge:
            // simulator_cout("Offhand hit dodged");
            break;
        case Melee_result::miss:
            // simulator_cout("Offhand hit missed");
            break;
        }
    }
}

std::string hit_result_to_string(const Melee_result hit_result)
{
    switch (hit_result)
    {
    case Melee_result::hit:
        return "hit";
    case Melee_result::crit:
        return "crit";
    case Melee_result::dodge:
        return "dodge";
    case Melee_result::miss:
        return "miss";
    case Melee_result::glancing:
        return "glancing";
    }
    throw std::runtime_error("Enum case not covered!");
}
