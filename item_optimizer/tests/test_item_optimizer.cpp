#include "item_optimizer_fixture.cpp"
#include "string_helpers.hpp"

TEST_F(Optimizer_fixture, test_multi_select)
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

    set_multiselect_fixture(mult_armor_vec, mult_weapon_vec, extra_options);
    set_target_items({"lionheart_helm", "stormrages_talisman_of_seething", "drake_talon_pauldrons",
                      "cloak_of_draconic_might", "plated_abomination_ribcage", "hive_defiler_wristguards",
                      "gauntlets_of_heroism"});

    double success_rate = run_multiselect(10, true);

    // Allow two failed runs. This is to make it very unlikely to fail if it is working.
    EXPECT_GE(success_rate, 0.8);
}

TEST_F(Optimizer_fixture, test_multi_select_item_filter)
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

    set_multiselect_fixture(mult_armor_vec, mult_weapon_vec, extra_options);
    set_target_items({"lionheart_helm", "stormrages_talisman_of_seething", "drake_talon_pauldrons",
                      "cloak_of_draconic_might", "plated_abomination_ribcage", "hive_defiler_wristguards",
                      "gauntlets_of_heroism", "onslaught_girdle", "cloudkeeper_legplates", "chromatic_boots",
                      "quick_strike_ring", "master_dragonslayers_ring", "mark_of_the_champion", "kiss_of_the_spider",
                      "larvae_of_the_great_worm", "crul_shorukh_edge_of_chaos", "dooms_edge"});

    double success_rate = run_multiselect(10, true);

    // Allow two failed runs. This is to make it very unlikely to fail if it is working.
    EXPECT_GE(success_rate, 0.8);
}