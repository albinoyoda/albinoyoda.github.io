#include "sim_interface.hpp"

#include <ctime>
#include <iostream>

int main()
{
    clock_t startTime = clock();
    Sim_interface sim_interface;

    std::vector<std::string> armor_vec;
    armor_vec.emplace_back("lionheart_helm");
    armor_vec.emplace_back("stormrages_talisman_of_seething");
    armor_vec.emplace_back("conquerors_spaulders");
    armor_vec.emplace_back("shroud_of_dominion");
    armor_vec.emplace_back("breastplate_of_undead_slaying");
    armor_vec.emplace_back("bracers_of_undead_slaying");
    armor_vec.emplace_back("gauntlets_of_undead_slaying");
    armor_vec.emplace_back("girdle_of_the_mentor");
    armor_vec.emplace_back("leggings_of_carnage");
    armor_vec.emplace_back("chromatic_boots");
    armor_vec.emplace_back("quick_strike_ring");
    armor_vec.emplace_back("band_of_unnatural_forces");
    armor_vec.emplace_back("kiss_of_the_spider");
    armor_vec.emplace_back("badge_of_the_swarmguard");
    armor_vec.emplace_back("nerubian_slavemaker");

    std::vector<std::string> weapons_vec;
    weapons_vec.emplace_back("thunderfury_blessed_blade");
    weapons_vec.emplace_back("argent_avenger");
    //    weapons_vec.emplace_back("manual_crowd_pummeler_1charge");

    std::vector<std::string> compare_armor_vec;
    //    compare_armor_vec.emplace_back("lionheart_helm");
    //    compare_armor_vec.emplace_back("onyxia_tooth_pendant");
    //    compare_armor_vec.emplace_back("truestrike_shoulders");
    //    compare_armor_vec.emplace_back("cape_of_the_black_baron");
    //    compare_armor_vec.emplace_back("savage_gladiator_chain");
    //    compare_armor_vec.emplace_back("wristguards_of_stability");
    //    compare_armor_vec.emplace_back("flameguard_gauntlets");
    //    compare_armor_vec.emplace_back("onslaught_girdle");
    //    compare_armor_vec.emplace_back("cloudkeeper_legplates");
    //    compare_armor_vec.emplace_back("chromatic_boots");
    //    compare_armor_vec.emplace_back("might_of_cenarius");
    //    compare_armor_vec.emplace_back("master_dragonslayers_ring");
    //    compare_armor_vec.emplace_back("badge_of_the_swarmguard");
    //    compare_armor_vec.emplace_back("diamond_flask");
    //    compare_armor_vec.emplace_back("blastershot");

    std::vector<std::string> compare_weapons_vec;
    //    compare_weapons_vec.emplace_back("thrash_blade");
    //    compare_weapons_vec.emplace_back("assassination_blade");

    std::vector<std::string> buff_vec;
    buff_vec.emplace_back("rallying_cry");
    buff_vec.emplace_back("dire_maul");
    buff_vec.emplace_back("songflower");
    buff_vec.emplace_back("warchiefs_blessing");
    buff_vec.emplace_back("spirit_of_zandalar");
    buff_vec.emplace_back("sayges_fortune");
    buff_vec.emplace_back("windfury_totem");
    buff_vec.emplace_back("blessing_of_kings");
    buff_vec.emplace_back("dense_stone_off_hand");
    buff_vec.emplace_back("elemental_stone_main_hand");
    buff_vec.emplace_back("battle_shout");
    //    buff_vec.emplace_back("battle_shout_aq");
    //    buff_vec.emplace_back("strength_of_earth_totem");
    //    buff_vec.emplace_back("grace_of_air_totem");

    std::vector<std::string> ench_vec;
    ench_vec.emplace_back("e+8 strength");
    ench_vec.emplace_back("s+30 attack power");
    ench_vec.emplace_back("m+15 strength");
    //    ench_vec.emplace_back("mcrusader");
    ench_vec.emplace_back("ocrusader");

    std::string race = "orc";

    std::vector<std::string> sim_options = {
        "faerie_fire", "recklessness", "enable_blood_fury", "curse_of_recklessness", "talents_stat_weights", "golemagg",
        "death_wish", "mighty_rage_potion", "debug_on", "exposed_armor", "talents_stat_weights",
        //        "cleave_if_adds",
        "use_hamstring", "compute_dpr", "use_bloodthirst", "use_whirlwind", "use_heroic_strike", "use_hamstring",
        "use_overpower", "use_slam", "use_sweeping_strikes", "multi_target_mode",
        //        "wep_strengths",
        //        "fungal_bloom",
        "full_polarity",
        //        "item_strengths",
        "wep_strengths", "battle_squawk"};

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
        "number_of_extra_targets_dd",
        "extra_target_armor_dd",
        "extra_target_level_dd",
        "periodic_damage_interval_dd",
        "periodic_damage_amount_dd",
        "execute_phase_percentage_dd",
        "re_queue_abilities_dd",
        "stat_weight_mh_speed_dd",
        "stat_weight_oh_speed_dd",
        "n_simulations_dd",
        "n_simulations_stat_dd",
        "n_simulations_talent_dd",
        "slam_cd_thresh_dd",
        "slam_spam_max_time_dd",
        "slam_spam_rage_dd",
        "slam_rage_dd",
        "full_polarity_dd",
        "battle_squawk_dd",
        "max_optimize_time_dd",
    };

    std::vector<double> float_options_val{50.0, 50.0, 10.0, 2.0,  50.0, 0.0, 2.0,  25.0, 2.0, 2.0, 60.0,
                                          63.0, 3731, 5,    0,    3731, 63,  1.0,  300,  15,  70,  0.5,
                                          0.5,  2000, 1000, 1000, 1.0,  1.5, 60.0, 15,   190, 20,  90};

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

    Sim_input sim_input{{race},
                        armor_vec,
                        weapons_vec,
                        buff_vec,
                        ench_vec,
                        {"mh_speed", "oh_speed", "crit", "hit"},
                        sim_options,
                        float_options_string,
                        float_options_val,
                        talents_string,
                        talents_val,
                        compare_armor_vec,
                        compare_weapons_vec};

    auto sim_output = sim_interface.simulate(sim_input);

    for (const auto& dmg_batch : sim_output.stat_weights)
    {
        std::cout << dmg_batch << "\n";
    }

    for (double dmg_source : sim_output.dmg_sources)
    {
        std::cout << "source = " << dmg_source << "\n";
    }

    for (const auto& message : sim_output.extra_stats)
    {
        std::cout << message << "\n";
    }

    for (const auto& dps : sim_output.mean_dps)
    {
        std::cout << "DPS = " << dps << "\n";
    }

    for (const auto& message : sim_output.messages)
    {
        std::cout << message << "\n";
    }

    std::cout << "Code executed in: " << double(clock() - startTime) / (double)CLOCKS_PER_SEC << " seconds."
              << std::endl;

    return 0;
}
