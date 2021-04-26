#include "item_upgrades_fixture.cpp"
#include "string_helpers.hpp"

TEST_F(Item_upgrades_fixture, test_multi_select)
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

    Character character{};

    double success_rate = get_item_upgrades(10, true);

    // Allow two failed runs. This is to make it very unlikely to fail if it is working.
    EXPECT_GE(success_rate, 0.8);
}
