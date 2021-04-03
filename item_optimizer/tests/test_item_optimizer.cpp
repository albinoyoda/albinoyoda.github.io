#include "sim_interface.hpp"
#include "string_helpers.hpp"

#include "gtest/gtest.h"

using extra_option = std::pair<std::string, double>;

namespace
{
std::vector<std::vector<std::string>> run_multiselect(const std::vector<std::string>& mult_armor_vec,
                                                      const std::vector<std::string>& mult_weapons_vec,
                                                      const std::vector<extra_option>& extra_options = {})
{
    std::vector<std::string> buff_vec;
    buff_vec.emplace_back("rallying_cry");
    buff_vec.emplace_back("dire_maul");
    buff_vec.emplace_back("spirit_of_zandalar");
    buff_vec.emplace_back("blessing_of_kings");
    buff_vec.emplace_back("dense_stone_off_hand");
    buff_vec.emplace_back("elemental_stone_main_hand");
    buff_vec.emplace_back("battle_shout_aq");

    std::vector<std::string> ench_vec;
    ench_vec.emplace_back("e+8 strength");
    ench_vec.emplace_back("s+30 attack power");
    ench_vec.emplace_back("m+15 strength");
    ench_vec.emplace_back("mcrusader");
    ench_vec.emplace_back("ocrusader");

    std::string race = "orc";

    std::vector<std::string> sim_options{
        "faerie_fire",     "curse_of_recklessness", "death_wish",        "mighty_rage_potion", "use_hamstring",
        "use_bloodthirst", "use_whirlwind",         "use_heroic_strike", "use_hamstring",      "use_overpower",
    };

    std::vector<std::string> float_options_string{
        "heroic_strike_rage_thresh_dd",
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
        "max_optimize_time_dd",
    };

    std::vector<double> float_options_val{50.0, 50.0, 10.0, 2.0,  50.0, 0.0, 2.0, 25.0, 2.0,
                                          2.0,  60.0, 63.0, 3731, 5,    63,  15,  70,   90};

    for (const auto& option : extra_options)
    {
        float_options_string.emplace_back(option.first);
        float_options_val.emplace_back(option.second);
    }

    std::vector<std::string> talents_string{
        "improved_heroic_strike_talent",
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
        "improved_slam_talent",
    };

    std::vector<int> talents_val{3, 3, 5, 1, 3, 2, 5, 5, 5, 5, 2, 5, 1, 5, 1, 1, 5};

    Sim_input_mult sim_input_mult{{race},      mult_armor_vec,       mult_weapons_vec,  buff_vec,       ench_vec,
                                  sim_options, float_options_string, float_options_val, talents_string, talents_val};

    Sim_interface sim_interface{};
    auto sim_output = sim_interface.simulate_mult(sim_input_mult);
    return sim_output.keepers;
}

} // namespace

TEST(TestSuite, test_multi_select)
{
    std::vector<std::string> mult_armor_vec{};

    mult_armor_vec.emplace_back("lionheart_helm");
    mult_armor_vec.emplace_back("mask_of_the_unforgiven");

    mult_armor_vec.emplace_back("stormrages_talisman_of_seething");
    mult_armor_vec.emplace_back("mark_of_fordring");

    mult_armor_vec.emplace_back("drake_talon_pauldrons");
    mult_armor_vec.emplace_back("spaulders_of_valor");

    mult_armor_vec.emplace_back("cloak_of_draconic_might");
    mult_armor_vec.emplace_back("cape_of_the_black_baron");

    mult_armor_vec.emplace_back("plated_abomination_ribcage");
    mult_armor_vec.emplace_back("breastplate_of_annihilation");

    mult_armor_vec.emplace_back("hive_defiler_wristguards");
    mult_armor_vec.emplace_back("bracers_of_brutality");

    mult_armor_vec.emplace_back("gauntlets_of_heroism");
    mult_armor_vec.emplace_back("gauntlets_of_valor");

    mult_armor_vec.emplace_back("onslaught_girdle");
    mult_armor_vec.emplace_back("brigam_girdle");

    std::vector<std::string> mult_weapon_vec{};
    mult_weapon_vec.emplace_back("mirahs_song");
    mult_weapon_vec.emplace_back("chromatically_tempered_sword");

    std::vector<extra_option> extra_options{};
    extra_options.emplace_back("item_filter_threshold", 2500);
    extra_options.emplace_back("item_filter_timeout", 1);
    extra_options.emplace_back("min_iterations", 15);

    int total_runs = 10;
    int successful_runs = 0;
    for (int i = 0; i < total_runs; ++i)
    {
        auto best_item_sets = run_multiselect(mult_armor_vec, mult_weapon_vec, extra_options);
        auto best_item_set = best_item_sets[0];

        bool success = String_helpers::find_string(best_item_set, "lionheart_helm") &&
                       String_helpers::find_string(best_item_set, "stormrages_talisman_of_seething") &&
                       String_helpers::find_string(best_item_set, "drake_talon_pauldrons") &&
                       String_helpers::find_string(best_item_set, "cloak_of_draconic_might") &&
                       String_helpers::find_string(best_item_set, "plated_abomination_ribcage") &&
                       String_helpers::find_string(best_item_set, "hive_defiler_wristguards") &&
                       String_helpers::find_string(best_item_set, "gauntlets_of_heroism");
        if (success)
        {
            successful_runs++;
        }
    }
    // Allow two failed runs. This is to make it very unlikely to fail if it is working.
    EXPECT_GE(successful_runs, total_runs - 2);
}

TEST(TestSuite, test_multi_select_item_filter)
{
    std::vector<std::string> mult_armor_vec{};

    mult_armor_vec.emplace_back("lionheart_helm");
    mult_armor_vec.emplace_back("mask_of_the_unforgiven");

    mult_armor_vec.emplace_back("stormrages_talisman_of_seething");
    mult_armor_vec.emplace_back("mark_of_fordring");

    mult_armor_vec.emplace_back("drake_talon_pauldrons");
    mult_armor_vec.emplace_back("spaulders_of_valor");

    mult_armor_vec.emplace_back("cloak_of_draconic_might");
    mult_armor_vec.emplace_back("cape_of_the_black_baron");

    mult_armor_vec.emplace_back("plated_abomination_ribcage");
    mult_armor_vec.emplace_back("breastplate_of_annihilation");

    mult_armor_vec.emplace_back("hive_defiler_wristguards");
    mult_armor_vec.emplace_back("bracers_of_brutality");

    mult_armor_vec.emplace_back("gauntlets_of_heroism");
    mult_armor_vec.emplace_back("gauntlets_of_valor");

    mult_armor_vec.emplace_back("onslaught_girdle");
    mult_armor_vec.emplace_back("brigam_girdle");

    mult_armor_vec.emplace_back("cloudkeeper_legplates");
    mult_armor_vec.emplace_back("eldritch_legplates");

    mult_armor_vec.emplace_back("chromatic_boots");
    mult_armor_vec.emplace_back("slime_kickers");

    mult_armor_vec.emplace_back("quick_strike_ring");
    mult_armor_vec.emplace_back("master_dragonslayers_ring");
    mult_armor_vec.emplace_back("blackstone_ring");
    mult_armor_vec.emplace_back("tarnished_elven_ring");

    mult_armor_vec.emplace_back("mark_of_the_champion");
    mult_armor_vec.emplace_back("kiss_of_the_spider");
    mult_armor_vec.emplace_back("blackhands_breadth");

    mult_armor_vec.emplace_back("larvae_of_the_great_worm");
    mult_armor_vec.emplace_back("blastershot");

    std::vector<std::string> mult_weapon_vec{};
    mult_weapon_vec.emplace_back("mirahs_song");
    mult_weapon_vec.emplace_back("chromatically_tempered_sword");
    mult_weapon_vec.emplace_back("crul_shorukh_edge_of_chaos");
    mult_weapon_vec.emplace_back("dooms_edge");

    std::vector<extra_option> extra_options{};
    extra_options.emplace_back("item_filter_threshold", 200);
    extra_options.emplace_back("item_filter_timeout", 0.5);
    extra_options.emplace_back("min_iterations", 15);

    int total_runs = 10;
    int successful_runs = 0;
    for (int i = 0; i < total_runs; ++i)
    {
        auto best_item_sets = run_multiselect(mult_armor_vec, mult_weapon_vec, extra_options);
        auto best_item_set = best_item_sets[0];

        bool success = String_helpers::find_string(best_item_set, "lionheart_helm") &&
                       String_helpers::find_string(best_item_set, "stormrages_talisman_of_seething") &&
                       String_helpers::find_string(best_item_set, "drake_talon_pauldrons") &&
                       String_helpers::find_string(best_item_set, "cloak_of_draconic_might") &&
                       String_helpers::find_string(best_item_set, "plated_abomination_ribcage") &&
                       String_helpers::find_string(best_item_set, "hive_defiler_wristguards") &&
                       String_helpers::find_string(best_item_set, "gauntlets_of_heroism") &&
                       String_helpers::find_string(best_item_set, "onslaught_girdle") &&
                       String_helpers::find_string(best_item_set, "cloudkeeper_legplates") &&
                       String_helpers::find_string(best_item_set, "chromatic_boots") &&
                       String_helpers::find_string(best_item_set, "quick_strike_ring") &&
                       String_helpers::find_string(best_item_set, "master_dragonslayers_ring") &&
                       String_helpers::find_string(best_item_set, "mark_of_the_champion") &&
                       String_helpers::find_string(best_item_set, "kiss_of_the_spider") &&
                       String_helpers::find_string(best_item_set, "larvae_of_the_great_worm") &&
                       String_helpers::find_string(best_item_set, "crul_shorukh_edge_of_chaos") &&
                       String_helpers::find_string(best_item_set, "dooms_edge");

        if (success)
        {
            successful_runs++;
        }
    }
    // Allow two failed runs. This is to make it very unlikely to fail if it is working.
    EXPECT_GE(successful_runs, total_runs - 2);
}