#include "Combat_simulator.hpp"
#include "sim_interface.hpp"
#include "string_helpers.hpp"

#include "gtest/gtest.h"

using extra_option = std::pair<std::string, double>;

namespace
{
std::vector<std::string> get_buff_vec()
{
    std::vector<std::string> buff_vec{};
    buff_vec.emplace_back("rallying_cry");
    buff_vec.emplace_back("dire_maul");
    buff_vec.emplace_back("spirit_of_zandalar");
    buff_vec.emplace_back("blessing_of_kings");
    buff_vec.emplace_back("dense_stone_off_hand");
    buff_vec.emplace_back("elemental_stone_main_hand");
    buff_vec.emplace_back("battle_shout_aq");
    return buff_vec;
}

std::vector<std::string> get_enchant_vec()
{
    std::vector<std::string> ench_vec{};
    ench_vec.emplace_back("e+8 strength");
    ench_vec.emplace_back("s+30 attack power");
    ench_vec.emplace_back("m+15 strength");
    ench_vec.emplace_back("mcrusader");
    ench_vec.emplace_back("ocrusader");
    return ench_vec;
}

std::vector<std::string> get_simulator_options()
{
    return {"faerie_fire",     "curse_of_recklessness", "death_wish",        "mighty_rage_potion", "use_hamstring",
            "use_bloodthirst", "use_whirlwind",         "use_heroic_strike", "use_hamstring",      "use_overpower"};
}

std::vector<std::string> get_float_options_string()
{
    return {"heroic_strike_rage_thresh_dd",
            "cleave_rage_thresh_dd",
            "whirlwind_rage_thresh_dd",
            "hamstring_cd_thresh_dd",
            "hamstring_thresh_dd",
            "initial_rage_dd",
            "whirlwind_bt_cooldown_thresh_dd",
            "overpower_rage_thresh_dd",
            "overpower_bt_cooldown_thresh_dd",
            "overpower_ww_cooldown_thresh_dd",
            "fight_time_dd",
            "opponent_level_dd",
            "boss_armor_dd",
            "sunder_armor_dd",
            "extra_target_level_dd",
            "execute_phase_percentage_dd",
            "re_queue_abilities_dd",
            "max_optimize_time_dd"};
}

std::vector<double> get_float_options_val()
{
    return {50.0, 50.0, 10.0, 2.0, 50.0, 0.0, 2.0, 25.0, 2.0, 2.0, 60.0, 63.0, 3731, 5, 63, 15, 70, 90};
}

std::vector<std::string> get_talents_string()
{
    return {"improved_heroic_strike_talent",
            "improved_rend_talent",
            "tactical_mastery_talent",
            "anger_management_talent",
            "deep_wounds_talent",
            "impale_talent",
            "cruelty_talent",
            "unbridled_wrath_talent",
            "improved_battle_shout_talent",
            "dual_wield_specialization_talent",
            "improved_execute_talent",
            "enrage_talent",
            "death_wish_talent",
            "flurry_talent",
            "bloodthirst_talent",
            "sweeping_strikes_talent",
            "improved_slam_talent"};
}

std::vector<int> get_talents_val()
{
    return {3, 3, 5, 1, 3, 2, 5, 5, 5, 5, 2, 5, 1, 5, 1, 1, 5};
}
} // namespace

class Optimizer_fixture : public ::testing::Test
{
public:
    std::string race;
    std::vector<std::string> buff_vec;
    std::vector<std::string> ench_vec;
    std::vector<std::string> sim_options;

    std::vector<extra_option> extra_options;

    std::vector<std::string> talents_string;
    std::vector<int> talents_val{};

    std::vector<std::string> float_options_string_base;
    std::vector<double> float_options_val_base;

    std::vector<std::string> mult_armor_vec;
    std::vector<std::string> mult_weapons_vec;
    std::vector<std::string> target_items{};

    Optimizer_fixture() = default;

    void SetUp() override
    {
        race = "orc";

        buff_vec = get_buff_vec();

        ench_vec = get_enchant_vec();

        sim_options = get_simulator_options();

        float_options_string_base = get_float_options_string();

        float_options_val_base = get_float_options_val();

        talents_string = get_talents_string();

        talents_val = get_talents_val();
    }

    void TearDown() override
    {
        // Nothing to do since no memory was allocated
    }

    void set_multiselect_fixture(const std::vector<std::string>& armor_vec, const std::vector<std::string>& weapons_vec,
                                 const std::vector<extra_option>& options = {})
    {
        mult_armor_vec = armor_vec;
        mult_weapons_vec = weapons_vec;
        extra_options = options;
    }

    void set_target_items(const std::vector<std::string>& target) { target_items = target; }

    double run_multiselect(int n_interations, const bool use_seed = true)
    {
        int start_seed = 110000;
        int successful_runs = 0;
        for (int iter = 0; iter < n_interations; iter++)
        {
            std::vector<std::string> float_options_string = float_options_string_base;
            std::vector<double> float_options_val = float_options_val_base;
            for (const auto& option : extra_options)
            {
                float_options_string.emplace_back(option.first);
                float_options_val.emplace_back(option.second);
            }

            if (use_seed)
            {
                float_options_string.emplace_back("seed");
                float_options_val.emplace_back(start_seed++);
            }

            Sim_input_mult sim_input_mult{{race},         mult_armor_vec, mult_weapons_vec,     buff_vec,
                                          ench_vec,       sim_options,    float_options_string, float_options_val,
                                          talents_string, talents_val};
            Sim_interface sim_interface{};

            auto sim_output = sim_interface.simulate_mult(sim_input_mult);

            if (is_run_successful(sim_output.keepers[0]))
            {
                successful_runs++;
            }
        }
        return double(successful_runs) / double(n_interations);
    }

    bool is_run_successful(const std::vector<std::string>& best_item_set)
    {
        bool success = true;
        for (const auto& item : target_items)
        {
            success &= String_helpers::find_string(best_item_set, item);
        }
        return success;
    }
};