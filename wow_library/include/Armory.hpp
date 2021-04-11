#ifndef WOW_SIMULATOR_ARMORY_HPP
#define WOW_SIMULATOR_ARMORY_HPP

#include "Item.hpp"

class Character;

struct Buffs
{
    // World buffs
    Buff rallying_cry{"rallying_cry", Attributes{0.0, 0.0}, Special_stats{5.0, 0.0, 140, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10}};
    Buff dire_maul{"dire_maul", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 200}};
    Buff slipkiks_savy{"slipkiks_savy", Attributes{0.0, 0.0}, Special_stats{0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3}};
    Buff songflower{"songflower", Attributes{15, 15}, Special_stats{5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5}};
    Buff warchiefs_blessing{"warchiefs_blessing", Attributes{0, 0}, Special_stats{0, 0, 0, 0, .15}};
    Buff spirit_of_zandalar{"spirit_of_zandalar", Attributes{0, 0}, Special_stats{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, .15}};
    Buff sayges_fortune{"sayges_fortune", Attributes{0, 0}, Special_stats{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, .1, 0, 0, 0, 0, 0, 0, 0, .1}};
    Buff traces_of_silithyst{"traces_of_silithyst", Attributes{0, 0}, Special_stats{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, .05, 0, 0, 0, 0, 0, 0, 0, .05}};
    Buff fungal_bloom{"fungal_bloom", Attributes{0, 0}, Special_stats{50, 0, 0}};

    // these are set in "sim_interface.cpp" or "sim_interface_mult.cpp" depending on whats specified
    Buff full_polarity{"full_polarity", {}, {}};
    Buff battle_squawk{"battle_squawk", {}, {}};

    // Player_buffs
    Buff battle_shout{"battle_shout", {}, {}, 0, {}, {{"battle_shout", Use_effect::Effect_socket::unique, {}, {0, 0, 193}, 0, 120, 120, true}}};
    Buff battle_shout_aq{"battle_shout_aq", {}, {}, 0, {}, {{"battle_shout_aq", Use_effect::Effect_socket::unique, {}, {0, 0, 232}, 0, 120, 120, true}}};
    Buff blessing_of_kings{"blessing_of_kings", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 0.1}};
    Buff blessing_of_might{"blessing_of_might", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 186}};
    Buff blessing_of_might_aq{"blessing_of_might_aq", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 222}};
    Buff gift_of_the_wild{"gift_of_the_wild", Attributes{16.0, 16.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff leader_of_the_pack{"leader_of_the_pack", Attributes{0.0, 0.0}, Special_stats{3.0, 0.0, 0.0}};
    Buff trueshot_aura{"trueshot_aura", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 100}};
    Buff windfury_totem{"windfury_totem", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0}, 0,
                        {{"windfury_totem", Hit_effect::Type::extra_hit, {}, {}, 0, 0, 0.2, 315}}};
    Buff strength_of_earth_totem{"strength_of_earth_totem", Attributes{61.0, 0.0}, Special_stats{0.0, 0.0, 0}};
    Buff strength_of_earth_totem_aq{"strength_of_earth_totem_aq", Attributes{77.0, 0.0}, Special_stats{0.0, 0.0, 0}};
    Buff grace_of_air_totem{"grace_of_air_totem", Attributes{0.0, 67.0}, Special_stats{0.0, 0.0, 0}};
    Buff grace_of_air_totem_aq{"grace_of_air_totem_aq", Attributes{0.0, 77.0}, Special_stats{0.0, 0.0, 0}};

    // Consumables
    Buff elixir_mongoose{"elixir_mongoose", Attributes{0.0, 25.0}, Special_stats{2.0, 0.0, 0.0}};
    Buff blessed_sunfruit{"blessed_sunfruit", Attributes{10.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff smoked_dessert_dumplings{"smoked_dessert_dumplings", Attributes{20.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff juju_power{"juju_power", Attributes{30.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff elixir_of_giants{"elixir_of_giants", Attributes{25.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff juju_might{"juju_might", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 40}};
    Buff winterfall_firewater{"winterfall_firewater", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 35}};
    Buff roids{"roids", Attributes{25.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff fire_toasted_bun{"fire_toasted_bun", Attributes{0.0, 0.0}, Special_stats{0.0, 2.0, 0.0}};
    Buff mighty_rage_potion{"mighty_rage_potion", {}, {}, 0, {},
                            {{"mighty_rage_potion", Use_effect::Effect_socket::unique, {60, 0}, {0, 0, 0}, 60, 20,
                              120, false}}};

    Weapon_buff dense_stone{"dense_stone", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, 8};
    Buff elemental_stone{"elemental_stone", Attributes{0.0, 0.0}, Special_stats{2.0, 0.0, 0.0}};
    Buff consecrated_sharpening_stone{"consecrated_sharpening_stone", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 100.0}};
};

struct Armory
{
    std::vector<Armor> helmet_t
            {
                    {"lionheart_helm",             Attributes{18, 0},  Special_stats{2, 2, 0}, Socket::head},
                    {"icebane_helmet",             Attributes{12, 0},  Special_stats{0, 0, 0}, Socket::head},
                    {"icy_scale_coif",             Attributes{0, 18},  Special_stats{0, 0, 0}, Socket::head},
                    {"expert_goldminers_head",     Attributes{0, 5},   Special_stats{0, 0, 0, 0, 0, 0,
                                                                                     7},       Socket::head},
                    {"mask_of_the_unforgiven",     Attributes{0, 0},   Special_stats{1, 2,
                                                                                     0},       Socket::head},
                    {"crown_of_destruction",       Attributes{0, 0},   Special_stats{2, 0,
                                                                                     44},      Socket::head},
                    {"helm_of_endless_rage",       Attributes{26, 26}, Special_stats{0, 0,
                                                                                     0},       Socket::head},
                    {"r13_plate_helm",             Attributes{28, 0},  Special_stats{1, 0,
                                                                                     0},       Socket::head, Set::epic_pvp_set},
                    {"eye_of_rend",                Attributes{13, 0},  Special_stats{2, 0,
                                                                                     0},       Socket::head},
                    {"fury_visor",                 Attributes{18, 0},  Special_stats{1, 1,
                                                                                     0},       Socket::head},
                    {"helm_of_the_executioner",    Attributes{14, 0},  Special_stats{0, 2,
                                                                                     0},       Socket::head},
                    {"helm_of_valor",              Attributes{15, 9},  Special_stats{0, 0,
                                                                                     0},       Socket::head, Set::valor},
                    {"r10_commanders_plate_helm",  Attributes{24, 0},  Special_stats{1, 1,
                                                                                     0},       Socket::head, Set::rare_pvp_set},
                    {"circlet_of_restless_dreams", Attributes{0, 21},  Special_stats{0, 0, 0, 0, 0, 0,
                                                                                     0, 6},    Socket::head},
                    {"conquerors_crown",           Attributes{29, 18}, Special_stats{0, 0, 0}, Socket::head},
                    {"helm_of_domination",         Attributes{21, 11}, Special_stats{0, 0, 0}, Socket::head},
                    {"helm_of_heroism",            Attributes{21, 11}, Special_stats{0, 0,
                                                                                     0},       Socket::head, Set::battlegear_of_heroism},
                    {"helm_of_wrath",            Attributes{17, 0}, Special_stats{0, 0,
                                                                                     0},       Socket::head, Set::battlegear_of_wrath},
                    {"dreadnaught_helmet",            Attributes{21, 0}, Special_stats{0, 0,
                                                                                  0},       Socket::head, Set::dreadnaughts_battlegear},
                    {"raging_berserkers_helm",            Attributes{13, 0}, Special_stats{1, 0,
                                                                                       0},       Socket::head},
            };

    std::vector<Armor> neck_t
            {
                    {"onyxia_tooth_pendant",           Attributes{0, 12},  Special_stats{1, 1, 0},  Socket::neck},
                    {"prestors_talisman_of_connivery", Attributes{0, 30},  Special_stats{0, 1, 0},  Socket::neck},
                    {"barbed_choker",                  Attributes{0, 0},   Special_stats{1, 0, 44}, Socket::neck},
                    {"the_eye_of_hakkar",              Attributes{0, 0},   Special_stats{1, 0, 40}, Socket::neck},
                    {"mark_of_fordring",               Attributes{0, 0},   Special_stats{1, 0, 26}, Socket::neck},
                    {"amulet_of_the_darkmoon",         Attributes{10, 19}, Special_stats{0, 0, 0},  Socket::neck},
                    {"imperial_jewel",                 Attributes{0, 0},   Special_stats{0, 0, 32}, Socket::neck},
                    {"beads_of_ogre_might",            Attributes{0, 0},   Special_stats{0, 1, 24}, Socket::neck},
                    {"choker_of_the_shifting_sands",   Attributes{0, 0},   Special_stats{0, 0, 42}, Socket::neck},
                    {"fury_of_the_forgotten_swarm",    Attributes{8, 0},   Special_stats{1, 1, 0},  Socket::neck},
                    {"pendant_of_the_qiraji_guardian",    Attributes{12, 11},   Special_stats{0, 0, 0},  Socket::neck},
                    {"sadists_collar",    Attributes{0, 0},   Special_stats{1, 0, 20},  Socket::neck},
                    {"stormrages_talisman_of_seething",    Attributes{0, 0},   Special_stats{2, 0, 26},  Socket::neck},
                    {"emberfury_talisman",    Attributes{0, 0},   Special_stats{1, 0, 0},  Socket::neck},
            };

    std::vector<Armor> shoulder_t
            {
                    {"black_dragonscale_shoulders",    Attributes{0, 0},   Special_stats{0, 0,
                                                                                         40}, Socket::shoulder, Set::black_dragonscale},
                    {"truestrike_shoulders",           Attributes{0, 0},   Special_stats{0, 2,
                                                                                         24}, Socket::shoulder},
                    {"wyrmhide_spaulders",             Attributes{0, 0},   Special_stats{0, 2,
                                                                                         0},  Socket::shoulder},
                    {"drake_talon_pauldrons",          Attributes{20, 20}, Special_stats{0, 0,
                                                                                         0},  Socket::shoulder},
                    {"r10_commanders_plate_shoulders", Attributes{17, 0},  Special_stats{1, 0,
                                                                                         0},  Socket::shoulder, Set::rare_pvp_set},
                    {"spaulders_of_valor",             Attributes{11, 9},  Special_stats{0, 0,
                                                                                         0},  Socket::shoulder, Set::valor},
                    {"chitinous_shoulderguards",       Attributes{11, 25}, Special_stats{0, 0,
                                                                                         0},  Socket::shoulder},
                    {"mantle_of_wicked_revenge",       Attributes{16, 30}, Special_stats{0, 0,
                                                                                         0},  Socket::shoulder},
                    {"conquerors_spaulders",           Attributes{20, 16}, Special_stats{0, 1,
                                                                                         0},  Socket::shoulder},
                    {"r13_plate_shoulderguards",       Attributes{18, 16}, Special_stats{0, 1,
                                                                                         0},  Socket::shoulder, Set::epic_pvp_set},
                    {"razorsteel_shoulders",           Attributes{10, 10}, Special_stats{0, 1,
                                                                                         0},  Socket::shoulder},
                    {"flamescarred_shoulders",         Attributes{12, 12}, Special_stats{0, 0,
                                                                                         0},  Socket::shoulder},
                    {"abyssal_plate_epaulets",         Attributes{13, 13}, Special_stats{0, 1,
                                                                                         0},  Socket::shoulder},
                    {"bloadsoaked_pauldrons",          Attributes{16, 11}, Special_stats{0, 0,
                                                                                         0},  Socket::shoulder},
                    {"pauldrons_of_the_unrelenting",   Attributes{11, 0},  Special_stats{0, 0,
                                                                                         0},  Socket::shoulder},
                    {"spaulders_of_heroism",           Attributes{12, 12}, Special_stats{0, 0,
                                                                                         0},  Socket::shoulder, Set::battlegear_of_heroism},
                    {"defilers_plate_spaulders",       Attributes{18, 17}, Special_stats{0, 0,
                                                                                         0},  Socket::shoulder, Set::the_defilers_resolution},
                    {"pauldrons_of_wrath",       Attributes{13, 0}, Special_stats{0, 0,
                                                                                         0},  Socket::shoulder, Set::battlegear_of_wrath},
                    {"pauldrons_of_might",       Attributes{15, 0}, Special_stats{0, 0,
                                                                                  0},  Socket::shoulder},
                    {"taut_dragonhide_shoulderpads",       Attributes{0, 0}, Special_stats{0, 0,
                                                                                         46},  Socket::shoulder},
                    {"dreadnaught_pauldrons",       Attributes{16, 0}, Special_stats{0, 1,
                                                                                  0},  Socket::shoulder, Set::dreadnaughts_battlegear},
                    {"icebane_pauldrons",       Attributes{9, 0}, Special_stats{0, 0, 0},  Socket::shoulder},
            };

    std::vector<Armor> back_t
            {
                    {"cape_of_the_black_baron",      Attributes{0, 15},  Special_stats{0, 0, 20}, Socket::back},
                    {"cloak_of_firemaw",             Attributes{0, 0},   Special_stats{0, 0, 50}, Socket::back},
                    {"cloak_of_draconic_might",      Attributes{16, 16}, Special_stats{0, 0, 0},  Socket::back},
                    {"cloak_of_concentrated_hate",   Attributes{11, 16}, Special_stats{0, 1, 0},  Socket::back},
                    {"blackveil_cloak",              Attributes{6, 14},  Special_stats{0, 0, 0},  Socket::back},
                    {"puissant_cape",                Attributes{0, 0},   Special_stats{0, 1, 40}, Socket::back},
                    {"cloak_of_the_shrouded_mists",  Attributes{0, 22},  Special_stats{0, 0, 0},  Socket::back},
                    {"stoneskin_gargoyle_cape",      Attributes{7, 8},   Special_stats{0, 0, 0},  Socket::back},
                    {"deathguards_cloak",            Attributes{0, 5},   Special_stats{0, 0, 34}, Socket::back},
                    {"windshear_cape",               Attributes{8, 15},  Special_stats{0, 0, 0},  Socket::back},
                    {"zulian_tigerhide_cloak",       Attributes{0, 13},  Special_stats{0, 1, 0},  Socket::back},
                    {"cloak_of_the_fallen_god",      Attributes{11, 26}, Special_stats{0, 0, 0},  Socket::back},
                    {"cloak_of_the_golden_hive",     Attributes{13, 10}, Special_stats{0, 0, 0},  Socket::back},
                    {"drape_of_unyielding_strength", Attributes{15, 9},  Special_stats{0, 1, 0},  Socket::back},
                    {"might_of_the_tribe", Attributes{0, 0},  Special_stats{0, 0, 28},  Socket::back},
                    {"cloak_of_the_scourge", Attributes{0, 0},  Special_stats{0, 1, 30},  Socket::back},
                    {"shifting_cloak", Attributes{0, 17},  Special_stats{0, 0, 0},  Socket::back},
                    {"shroud_of_dominion", Attributes{0, 0},  Special_stats{1, 0, 50},  Socket::back},
            };

    std::vector<Armor> chest_t
            {
                    {"malfurions_blessed_bulwark",          Attributes{40, 0},  Special_stats{0, 0, 0},  Socket::chest},
                    {"savage_gladiator_chain",              Attributes{13, 14}, Special_stats{2, 0, 0},  Socket::chest},
                    {"r8_plate_hauberk",                    Attributes{21, 0},  Special_stats{1, 0,
                                                                                              0},        Socket::chest, Set::rare_pvp_set},
                    {"tombstone_breastplate",               Attributes{10, 0},  Special_stats{2, 0, 0},  Socket::chest},
                    {"deathdealer_breastplate",             Attributes{8, 0},   Special_stats{2, 0, 0},  Socket::chest},
                    {"cadaverous_armor",                    Attributes{8, 8},   Special_stats{0, 0, 60}, Socket::chest},
                    {"breastplate_of_annihilation",         Attributes{37, 0},  Special_stats{1, 1, 0},  Socket::chest},
                    {"vest_of_swift_execution",             Attributes{21, 41}, Special_stats{0, 0, 0},  Socket::chest},
                    {"conquerors_breastplate",              Attributes{34, 24}, Special_stats{0, 0, 0},  Socket::chest},
                    {"black_dragonscale_chest",             Attributes{0, 0},   Special_stats{0, 0,
                                                                                              50},       Socket::chest, Set::black_dragonscale},
                    {"breastplate_of_bloodthirst",          Attributes{13, 0},  Special_stats{2, 0, 0},  Socket::chest},
                    {"runed_bloodstaind_hauberk",           Attributes{0, 0},   Special_stats{1, 0, 58}, Socket::chest},
                    {"ogre_forged_hauberk",                 Attributes{8, 20},  Special_stats{1, 0, 0},  Socket::chest},
                    {"breastplate_of_the_chromatic_flight", Attributes{20, 10}, Special_stats{0, 0, 0},  Socket::chest},
                    {"breastplate_of_valor",                Attributes{15, 10}, Special_stats{0, 0,
                                                                                              0},        Socket::chest, Set::valor},
                    {"r13_plate_armor",                     Attributes{16, 14}, Special_stats{1, 0,
                                                                                              0},        Socket::chest, Set::epic_pvp_set},
                    {"bloodsoul_breastplate",               Attributes{0, 9},   Special_stats{2, 0,
                                                                                              0},        Socket::chest},
                    {"zandalar_vindicators_breastplate",    Attributes{23, 15}, Special_stats{0, 0,
                                                                                              0},        Socket::chest},
                    {"obsidian_mail_tunic",                 Attributes{0, 0},   Special_stats{1, 0, 76}, Socket::chest},
                    {"breastplate_of_heroism",              Attributes{21, 13}, Special_stats{0, 1,
                                                                                              0},        Socket::chest, Set::battlegear_of_heroism},
                    {"breastplate_of_wrath",              Attributes{17, 0}, Special_stats{0, 0,
                                                                                              0},        Socket::chest, Set::battlegear_of_wrath},
                    {"breastplate_of_undead_slaying",              Attributes{0, 0}, Special_stats{0, 0,
                                                                                           81},        Socket::chest, Set::battlegear_of_undead_slaying},
                    {"ghoul_skin_tunic",              Attributes{40, 0}, Special_stats{2, 0,
                                                                                           0},        Socket::chest},
                    {"plated_abomination_ribcage",              Attributes{45, 0}, Special_stats{1, 1,
                                                                                       0},        Socket::chest},
                    {"dreadnaught_breastplate",              Attributes{21, 0}, Special_stats{0, 2,
                                                                                                 0},        Socket::chest, Set::dreadnaughts_battlegear},
                    {"icebane_breastplate",              Attributes{12, 0}, Special_stats{0, 0, 0},        Socket::chest},
                    {"icy_scale_breastplate",              Attributes{0, 0}, Special_stats{0, 0, 40},        Socket::chest},
                    {"polar_tunic",              Attributes{0, 18}, Special_stats{0, 0, 0},        Socket::chest},
            };

    std::vector<Armor> wrists_t
            {
                    {"wristguards_of_stability",   Attributes{24, 0},  Special_stats{0, 0, 0}, Socket::wrist},
                    {"vambraces_of_the_sadist",    Attributes{6, 0},   Special_stats{1, 0, 0}, Socket::wrist},
                    {"wristguards_of_true_flight", Attributes{0, 19},  Special_stats{0, 1, 0}, Socket::wrist},
                    {"battleborn_armbraces",       Attributes{0, 0},   Special_stats{1, 1, 0}, Socket::wrist},
                    {"zandalar_armguards",         Attributes{13, 13}, Special_stats{0, 0, 0}, Socket::wrist},
                    {"blinkstrike",                Attributes{3, 0},   Special_stats{1, 0, 0}, Socket::wrist},
                    {"bracers_of_brutality",       Attributes{21, 12}, Special_stats{0, 0, 0}, Socket::wrist},
                    {"hive_defiler_wristguards",   Attributes{23, 18}, Special_stats{0, 0, 0}, Socket::wrist},
                    {"qiraji_execution_bracers",  Attributes{15, 16}, Special_stats{0, 1, 0}, Socket::wrist},
                    {"berserker_bracers",          Attributes{19, 8},  Special_stats{0, 0, 0}, Socket::wrist},
                    {"abyssal_plate_vambraces",    Attributes{11, 11}, Special_stats{0, 0, 0}, Socket::wrist},
                    {"bracers_of_heroism",         Attributes{9, 5},   Special_stats{0, 0,
                                                                                     0},       Socket::wrist, Set::battlegear_of_heroism},
                    {"deeprock_bracers",           Attributes{19, 10}, Special_stats{0, 0, 0}, Socket::wrist},
                    {"bracelets_of_wrath",           Attributes{13, 0}, Special_stats{0, 0, 0}, Socket::wrist, Set::battlegear_of_wrath},
                    {"bracers_of_undead_slaying",           Attributes{0, 0}, Special_stats{0, 0, 45}, Socket::wrist, Set::battlegear_of_undead_slaying},
                    {"wristguards_of_vengeance",           Attributes{24, 0}, Special_stats{1, 0, 0}, Socket::wrist},
                    {"dreadnaught_bracers",           Attributes{14, 0}, Special_stats{0, 0, 0}, Socket::wrist},
                    {"icebane_bracers",           Attributes{6, 0}, Special_stats{0, 0, 0}, Socket::wrist},
                    {"icy_scale_bracers",           Attributes{0, 0}, Special_stats{0, 0, 32}, Socket::wrist},
                    {"polar_bracers",           Attributes{0, 12}, Special_stats{0, 0, 0}, Socket::wrist},
            };

    std::vector<Armor> hands_t
            {
                    {"devilsaur_gauntlets",       Attributes{0, 0},   Special_stats{1, 0,
                                                                                    28},      Socket::hands, Set::devilsaur},
                    {"edgemasters_handguards",    Attributes{0, 0},   Special_stats{0, 0, 0, 0, 0, 7, 7,
                                                                                    7},       Socket::hands},
                    {"flameguard_gauntlets",      Attributes{0, 0},   Special_stats{1, 0,
                                                                                    54},      Socket::hands},
                    {"gloves_of_enforcement",     Attributes{28, 20}, Special_stats{0, 1,
                                                                                    0},       Socket::hands},
                    {"aged_core_leather_gloves",  Attributes{15, 0},  Special_stats{1, 0, 0, 0, 0, 0, 0,
                                                                                    5},       Socket::hands},
                    {"gauntlets_of_annihilation", Attributes{35, 0},  Special_stats{1, 1,
                                                                                    0},       Socket::hands},
                    {"bloodmail_gauntlets",       Attributes{9, 0},   Special_stats{1, 0,
                                                                                    0},       Socket::hands},
                    {"sacrificial_gauntlets",     Attributes{19, 0},  Special_stats{1, 1,
                                                                                    0},       Socket::hands},
                    {"r12_plate_gauntlets",       Attributes{20, 0},  Special_stats{1, 0,
                                                                                    0},       Socket::hands, Set::epic_pvp_set},
                    {"gauntlets_of_might",        Attributes{22, 0},  Special_stats{0, 1,
                                                                                    0},       Socket::hands},
                    {"reiver_claws",              Attributes{9, 0},   Special_stats{1, 0,
                                                                                    0},       Socket::hands},
                    {"gauntlets_of_valor",        Attributes{17, 3},  Special_stats{0, 0,
                                                                                    0},       Socket::hands, Set::valor},
                    {"r7_plate_gauntlets",        Attributes{17, 0},  Special_stats{0, 0,
                                                                                    0},       Socket::hands, Set::valor},
                    {"gauntlets_of_heroism",      Attributes{18, 0},  Special_stats{1, 0,
                                                                                    0},       Socket::hands, Set::battlegear_of_heroism},
                    {"gauntlets_of_steadfast_determination",     Attributes{19, 18}, Special_stats{0, 0, 0}, Socket::hands},
                    {"gauntlets_of_wrath",     Attributes{15, 0}, Special_stats{0, 0, 0}, Socket::hands, Set::battlegear_of_wrath},
                    {"gauntlets_of_undead_slaying",     Attributes{0, 0}, Special_stats{0, 0, 60}, Socket::hands, Set::battlegear_of_undead_slaying},
                    {"dreadnaught_gauntlets",     Attributes{17, 0}, Special_stats{0, 0, 0}, Socket::hands, Set::dreadnaughts_battlegear},
                    {"icebane_gauntlets",     Attributes{9, 0}, Special_stats{0, 0, 0}, Socket::hands},
                    {"icy_scale_gauntlets",     Attributes{0, 0}, Special_stats{0, 0, 22}, Socket::hands},
                    {"polar_gloves",     Attributes{0, 18}, Special_stats{0, 0, 0}, Socket::hands},
            };

    std::vector<Armor> belt_t
            {
                    {"onslaught_girdle",              Attributes{31, 0},  Special_stats{1, 1, 0},  Socket::belt},
                    {"muggers_belt",                  Attributes{0, 0},   Special_stats{1, 0, 0, 0, 0, 0, 0,
                                                                                        5},        Socket::belt},
                    {"omokks_girth_restrainer",       Attributes{15, 0},  Special_stats{1, 0, 0},  Socket::belt},
                    {"brigam_girdle",                 Attributes{15, 0},  Special_stats{0, 1, 0},  Socket::belt},
                    {"zandalar_vindicators_belt",     Attributes{25, 0},  Special_stats{1, 0, 0},  Socket::belt},
                    {"therazanes_link",               Attributes{0, 0},   Special_stats{1, 0, 44}, Socket::belt},
                    {"girdle_of_the_fallen_crusader", Attributes{20, 10}, Special_stats{0, 0, 0},  Socket::belt},
                    {"defilers_plate_girdle",         Attributes{17, 0},  Special_stats{1, 0,
                                                                                        0},        Socket::belt, Set::the_defilers_resolution},
                    {"frostwolf_plate_belt",          Attributes{18, 7},  Special_stats{0, 0, 0},  Socket::belt},
                    {"belt_of_heroism",               Attributes{15, 9},  Special_stats{0, 0,
                                                                                        0},        Socket::belt, Set::battlegear_of_heroism},
                    {"belt_of_never_ending_agony",    Attributes{0, 0},   Special_stats{1, 1, 64}, Socket::belt},
                    {"triad_girdle",                  Attributes{26, 19}, Special_stats{0, 0, 0},  Socket::belt},
                    {"belt_of_preserved_heads",                  Attributes{14, 15}, Special_stats{0, 1, 0},  Socket::belt},
                    {"belt_of_wrath",                 Attributes{20, 0},  Special_stats{0, 0,
                                                                                        0},        Socket::belt, Set::battlegear_of_wrath},
                    {"girdle_of_the_mentor",                 Attributes{21, 20},  Special_stats{1, 1, 0},        Socket::belt},
                    {"dreadnaught_waistguard",                 Attributes{20, 0},  Special_stats{0, 0, 0},        Socket::belt, Set::dreadnaughts_battlegear},
                    {"frostbite_girdle",                 Attributes{5, 15},  Special_stats{0, 0, 0},        Socket::belt},
                    {"unmelting_ice_girdle",                 Attributes{14, 14},  Special_stats{0, 0, 0},        Socket::belt},
            };

    std::vector<Armor> legs_t
            {
                    {"devilsaur_leggings",               Attributes{0, 0},   Special_stats{1, 0,
                                                                                           46},       Socket::legs, Set::devilsaur},
                    {"black_dragonscale_leggings",       Attributes{0, 0},   Special_stats{0, 0,
                                                                                           54},       Socket::legs, Set::black_dragonscale},
                    {"eldritch_legplates",               Attributes{15, 9},  Special_stats{1, 0, 0},  Socket::legs},
                    {"handcrafted_mastersmith_leggings", Attributes{29, 0},  Special_stats{0, 0, 0},  Socket::legs},
                    {"cloudkeeper_legplates",            Attributes{20, 20}, Special_stats{0, 0,
                                                                                           0},        Socket::legs, Set::none, {},
                            {{"cloudkeeper_legplates", Use_effect::Effect_socket::shared, {}, {0, 0, 100}, 0, 30, 900, true}}},
                    {"cloudkeeper_legplates_no_use",     Attributes{20, 20}, Special_stats{0, 0,
                                                                                           0},        Socket::legs},
                    {"legguards_of_the_fallen_crusader", Attributes{28, 22}, Special_stats{0, 0, 0},  Socket::legs},
                    {"r8_plate_leggings",                Attributes{12, 0},  Special_stats{2, 0,
                                                                                           0},        Socket::legs, Set::rare_pvp_set},
                    {"legplates_of_the_qiraji_command",  Attributes{20, 0},  Special_stats{2, 0, 0},  Socket::legs},
                    {"scaled_sand_reaver_leggings",      Attributes{0, 0},   Special_stats{2, 0, 62}, Socket::legs},
                    {"conquerors_legguards",             Attributes{33, 21}, Special_stats{0, 1, 0},  Socket::legs},
                    {"titanic_leggings",                 Attributes{30, 0},  Special_stats{1, 2, 0},  Socket::legs},
                    {"bloodsoaked_legplates",            Attributes{36, 0},  Special_stats{0, 0, 0},  Socket::legs},
                    {"r12_plate_legguards",              Attributes{20, 0},  Special_stats{2, 1,
                                                                                           0},        Socket::legs, Set::epic_pvp_set},
                    {"blademaster_leggings",             Attributes{5, 0},   Special_stats{1, 1, 0},  Socket::legs},
                    {"legplates_of_valor",               Attributes{23, 11}, Special_stats{0, 0, 0},  Socket::legs},
                    {"abyssal_plate_legplates",          Attributes{15, 15}, Special_stats{1, 0, 0},  Socket::legs},
                    {"dark_heart_pants",                 Attributes{0, 0},   Special_stats{2, 0, 48}, Socket::legs},
                    {"legplates_of_heroism",             Attributes{25, 11}, Special_stats{0, 0,
                                                                                           0},        Socket::legs, Set::battlegear_of_heroism},
                    {"legplates_of_wrath",             Attributes{19, 0}, Special_stats{0, 0, 0},        Socket::legs, Set::battlegear_of_wrath},
                    {"leggings_of_apocalypse",             Attributes{15, 31}, Special_stats{2, 0, 0},        Socket::legs},
                    {"leggings_of_carnage",             Attributes{42, 0}, Special_stats{2, 0, 0},        Socket::legs},
                    {"dreadnaughts_legplates",             Attributes{23, 0}, Special_stats{0, 0, 0},        Socket::legs, Set::dreadnaughts_battlegear},
                    {"WSG_plate_legguards",             Attributes{28, 0}, Special_stats{1, 1, 0},        Socket::legs},
                    {"icebane_leggings",             Attributes{10, 0}, Special_stats{0, 0, 0},        Socket::legs},
                    {"icy_scale_leggings",             Attributes{0, 15}, Special_stats{0, 0, 0},        Socket::legs},
            };

    std::vector<Armor> boots_t
            {
                    {"windreaver_greaves",        Attributes{0, 20},  Special_stats{0, 1, 0},  Socket::boots},
                    {"bloodmail_boots",           Attributes{9, 9},   Special_stats{0, 1, 0},  Socket::boots},
                    {"black_dragonscale_boots",   Attributes{0, 0},   Special_stats{0, 0,
                                                                                    28},       Socket::boots, Set::black_dragonscale},
                    {"r7_greaves",                Attributes{10, 9},  Special_stats{0, 0,
                                                                                    0},        Socket::boots, Set::rare_pvp_set},
                    {"battlechasers_greaves",     Attributes{14, 13}, Special_stats{0, 0, 0},  Socket::boots},
                    {"chromatic_boots",           Attributes{20, 20}, Special_stats{0, 1, 0},  Socket::boots},
                    {"boots_of_the_shadow_flame", Attributes{0, 0},   Special_stats{0, 2, 44}, Socket::boots},
                    {"boots_of_the_vanguard",     Attributes{22, 22}, Special_stats{0, 0, 0},  Socket::boots},
                    {"r12_plate_boots",           Attributes{18, 12}, Special_stats{0, 1,
                                                                                    0},        Socket::boots, Set::epic_pvp_set},
                    {"sapphirons_scale_boots",    Attributes{14, 9},  Special_stats{0, 0, 0},  Socket::boots},
                    {"boots_of_heroism",          Attributes{20, 0},  Special_stats{0, 1,
                                                                                    0},        Socket::boots, Set::battlegear_of_heroism},
                    {"boots_of_the_fallen_hero",  Attributes{20, 14}, Special_stats{0, 1, 0},  Socket::boots},
                    {"conquerors_greaves",        Attributes{21, 17}, Special_stats{0, 0, 0},  Socket::boots},
                    {"slime_kickers",             Attributes{18, 12}, Special_stats{0, 1, 0},  Socket::boots},
                    {"blooddrenched_footpads",    Attributes{0, 21},  Special_stats{0, 1, 0},  Socket::boots},
                    {"dreadnaughts_sabatons",    Attributes{15, 0},  Special_stats{0, 0, 0},  Socket::boots, Set::dreadnaughts_battlegear},
                    {"defilers_plate_greaves",    Attributes{14, 12}, Special_stats{0, 0,
                                                                                    0},        Socket::boots, Set::the_defilers_resolution},
                    {"sabatons_of_wrath",         Attributes{13, 0}, Special_stats{0, 0,
                                                                                    0},        Socket::boots, Set::battlegear_of_wrath},
            };

    std::vector<Armor> ring_t
            {
                    {"don_julios_band",                   Attributes{0, 0},   Special_stats{1, 1, 16},   Socket::ring},
                    {"magnis_will",                       Attributes{6, 0},   Special_stats{1, 0, 0},    Socket::ring},
                    {"quick_strike_ring",                 Attributes{5, 0},   Special_stats{1, 0, 30},   Socket::ring},
                    {"master_dragonslayers_ring",         Attributes{0, 0},   Special_stats{0, 1, 48},   Socket::ring},
                    {"circle_of_applied_force",           Attributes{12, 22}, Special_stats{0, 0, 0},    Socket::ring},
                    {"blackstone_ring",                   Attributes{0, 0},   Special_stats{0, 1, 20},   Socket::ring},
                    {"tarnished_elven_ring",              Attributes{0, 15},  Special_stats{0, 1, 0},    Socket::ring},
                    {"seal_of_the_gurubashi_berserker",   Attributes{0, 0},   Special_stats{0, 0, 40},   Socket::ring},
                    {"band_of_accuria",                   Attributes{0, 16},  Special_stats{0, 2, 0},    Socket::ring},
                    {"band_of_the_ogre_king",             Attributes{14, 0},  Special_stats{0, 0, 0},    Socket::ring},
                    {"painweaver_band",                   Attributes{0, 0},   Special_stats{1, 0, 16},   Socket::ring},
                    {"band_of_jin",                       Attributes{0, 14},  Special_stats{0, 1,
                                                                                            0},          Socket::ring, Set::major_mojo_infusion},
                    {"seal_of_jin",                       Attributes{0, 0},   Special_stats{1, 0,
                                                                                            20},         Socket::ring, Set::major_mojo_infusion},
                    {"band_of_earthen_might",             Attributes{1, 0},   Special_stats{1, 1, 0},    Socket::ring},
                    {"might_of_cenarius",                 Attributes{0, 0},   Special_stats{0, 1, 0, 0, 0, 0, 0, 0, 0,
                                                                                            0, 0, 0, 4}, Socket::ring},
                    {"ring_of_the_god_slayer",            Attributes{0, 27},  Special_stats{0, 0, 0},    Socket::ring},
                    {"ring_of_the_qiraji_fury",           Attributes{0, 0},   Special_stats{1, 0, 40},   Socket::ring},
                    {"signet_of_unyielding_strength",     Attributes{11, 0},  Special_stats{1, 0, 0},    Socket::ring},
                    {"signet_of_the_bronze_dragonflight", Attributes{0, 24},  Special_stats{0, 1, 0},    Socket::ring},
                    {"band_of_reanimation", Attributes{0, 34},  Special_stats{0, 0, 0},    Socket::ring},
                    {"band_of_unnatural_forces", Attributes{0, 0},  Special_stats{1, 1, 52},    Socket::ring},
                    {"signet_of_the_fallen_defender", Attributes{0, 0},  Special_stats{0, 1, 0},    Socket::ring},
                    {"ring_of_the_dreadnaught", Attributes{0, 0},  Special_stats{0, 0, 0},    Socket::ring, Set::dreadnaughts_battlegear},
                    {"ring_of_fury", Attributes{0, 0},  Special_stats{0, 1, 30},    Socket::ring},
                    {"masons_fraternity_ring", Attributes{0, 13},  Special_stats{0, 0, 0},    Socket::ring},
            };

    std::vector<Armor> trinket_t
            {
                    {"hand_of_justice",           Attributes{0, 0}, Special_stats{0, 0,
                                                                                  20},       Socket::trinket, Set::none, {{"hand_of_justice",         Hit_effect::Type::extra_hit,    {}, {}, 0,   0, 0.02}}},
                    {"diamond_flask",             Attributes{0, 0}, Special_stats{0, 0,
                                                                                  0},        Socket::trinket, Set::none, {}, {{"diamond_flask",           Use_effect::Effect_socket::shared, {75, 0}, {},          0, 60, 360, true}}},
                    {"blackhands_breadth",        Attributes{0, 0}, Special_stats{2, 0, 0},  Socket::trinket},
                    {"drake_fang_talisman",       Attributes{0, 0}, Special_stats{0, 2, 56}, Socket::trinket},
                    {"counterattack_lodestone",   Attributes{0, 0}, Special_stats{0, 0, 22}, Socket::trinket},
                    {"darkmoon_card_maelstrom",   Attributes{0, 0}, Special_stats{0, 0,
                                                                                  0},        Socket::trinket, Set::none, {{"darkmoon_card_maelstrom", Hit_effect::Type::damage_magic, {}, {}, 250, 0, 0.02}}},
                    {"rune_of_the_guard_captain", Attributes{0, 0}, Special_stats{0, 1, 20}, Socket::trinket},
                    {"jom_gabbar",                Attributes{0, 0}, Special_stats{0, 0,
                                                                                  0},        Socket::trinket, Set::none, {}, {{"jom_gabbar",              Use_effect::Effect_socket::shared, {0,  0}, {0, 0, 358}, 0, 20, 120, true}}},
                    {"earthstrike",               Attributes{0, 0}, Special_stats{0, 0,
                                                                                  0},        Socket::trinket, Set::none, {}, {{"earthstrike",             Use_effect::Effect_socket::shared, {0,  0}, {0, 0, 280}, 0, 20, 120, true}}},
                    {"badge_of_the_swarmguard",   Attributes{0, 0}, Special_stats{0, 0,
                                                                                  0},        Socket::trinket, Set::none, {}, {{"badge_of_the_swarmguard", Use_effect::Effect_socket::unique, {},      {},          0, 30, 180, false, {{"badge_of_the_swarmguard", Hit_effect::Type::reduce_armor, {}, {}, 0, 30, 0.5, 0, 1, 200, 6}}}}},
                    {"kiss_of_the_spider",               Attributes{0, 0}, Special_stats{1, 1,
                                                                                  0},        Socket::trinket, Set::none, {}, {{"kiss_of_the_spider",             Use_effect::Effect_socket::shared, {0,  0}, {0, 0, 0, 0, .2}, 0, 15, 120, true}}},
                    {"mark_of_the_champion",       Attributes{0, 0}, Special_stats{0, 0, 150}, Socket::trinket},
                    {"seal_of_the_dawn",           Attributes{0, 0}, Special_stats{0, 0, 81}, Socket::trinket},
                    {"slayers_crest",              Attributes{0, 0}, Special_stats{0, 0,
                                                                                  64},        Socket::trinket, Set::none, {}, {{"slayers_crest",             Use_effect::Effect_socket::shared, {0,  0}, {0, 0, 260}, 0, 20, 120, true}}},
            };

    std::vector<Armor> ranged_t
            {
                    {"satyrs_bow",                     Attributes{0, 3},  Special_stats{0, 1, 0},  Socket::ranged},
                    {"precisely_calibrated_boomstick", Attributes{0, 14}, Special_stats{0, 0, 0},  Socket::ranged},
                    {"dragonbreath_hand_cannon",       Attributes{0, 14}, Special_stats{0, 0, 0},  Socket::ranged},
                    {"strikers_mark",                  Attributes{0, 0},  Special_stats{0, 1, 22}, Socket::ranged},
                    {"heartstriker",                   Attributes{0, 0},  Special_stats{0, 0, 24}, Socket::ranged},
                    {"blastershot",                    Attributes{0, 0},  Special_stats{1, 0, 0},  Socket::ranged},
                    {"dragonbreath_hand_cannon",       Attributes{0, 14}, Special_stats{0, 0, 0},  Socket::ranged},
                    {"crossbow_of_imminent_doom",      Attributes{5, 7},  Special_stats{0, 1, 0},  Socket::ranged},
                    {"larvae_of_the_great_worm",       Attributes{0, 0},  Special_stats{1, 0, 18}, Socket::ranged},
                    {"bloodseeker",                    Attributes{8, 7},  Special_stats{0, 0, 0},  Socket::ranged},
                    {"riphook",                        Attributes{0, 0},  Special_stats{0, 0, 22}, Socket::ranged},
                    {"gurubashi_dwarf_destroyer",      Attributes{0, 0},  Special_stats{0, 0, 30}, Socket::ranged},
                    {"huhurans_stinger",               Attributes{0, 18}, Special_stats{0, 0, 0},  Socket::ranged},
                    {"bow_of_taut_sinew",               Attributes{0, 0}, Special_stats{0, 0, 22},  Socket::ranged},
                    {"nerubian_slavemaker",               Attributes{0, 0}, Special_stats{1, 0, 24},  Socket::ranged},
                    {"soulstring",               Attributes{0, 0}, Special_stats{1, 0, 16},  Socket::ranged},
                    {"toxin_injector",               Attributes{0, 0}, Special_stats{0, 0, 28},  Socket::ranged},
                    {"heartseeking_crossbow",               Attributes{0, 9}, Special_stats{0, 0, 0},  Socket::ranged},
            };

    std::vector<Weapon> swords_t
            {
                    {"maladath",                     Attributes{0.0, 0.0},   Special_stats{0.0, 0.0, 0.0, 0, 0,
                                                                                           4},   2.2, 86.0,  162.0, Weapon_socket::one_hand,  Weapon_type::sword},
                    {"chromatically_tempered_sword", Attributes{14.0, 14.0}, Special_stats{0.0, 0.0,
                                                                                           0.0}, 2.6, 106.0, 198.0, Weapon_socket::one_hand,  Weapon_type::sword},
                    {"brutality_blade",              Attributes{9, 9},       Special_stats{1, 0,
                                                                                           0},   2.5, 90,    168,   Weapon_socket::one_hand,  Weapon_type::sword},
                    {"thrash_blade",                 Attributes{0, 0},       Special_stats{0, 0,
                                                                                           0},   2.7, 66,    124,   Weapon_socket::one_hand,  Weapon_type::sword, {{"thrash_blade",              Hit_effect::Type::extra_hit,               {}, {}, 0,   0, 0.045}}},
                    {"assassination_blade",          Attributes{3, 0},       Special_stats{1, 0,
                                                                                           0},   2.7, 71,    132,   Weapon_socket::one_hand,  Weapon_type::sword},
                    {"mirahs_song",                  Attributes{9, 9},       Special_stats{0, 0,
                                                                                           0},   1.8, 57,    87,    Weapon_socket::one_hand,  Weapon_type::sword},
                    {"viskag",                       Attributes{0, 0},       Special_stats{0, 0,
                                                                                           0},   2.6, 100,   187,   Weapon_socket::one_hand,  Weapon_type::sword, {{"viskag",                    Hit_effect::Type::damage_physical,         {}, {}, 240, 0, 0.026}}},
                    {"quel_serrar",                  Attributes{0, 0},       Special_stats{0, 0,
                                                                                           0},   2.0, 84,    126,   Weapon_socket::main_hand, Weapon_type::sword},
                    {"ancient_qiraji_ripper",       Attributes{0, 0},       Special_stats{1, 0,
                                                                                           20},  2.8, 114,   213,   Weapon_socket::one_hand,  Weapon_type::sword},
                    {"dal_rends_sacred_charge",      Attributes{4, 0},       Special_stats{1, 0,
                                                                                           0},   2.8, 81,    151,   Weapon_socket::main_hand, Weapon_type::sword, std::vector<Hit_effect>(), Set::dal_rends},
                    {"dal_rends_tribal_guardian",    Attributes{0, 0},       Special_stats{0, 0,
                                                                                           0},   1.8, 52,    97,    Weapon_socket::off_hand,  Weapon_type::sword, std::vector<Hit_effect>(), Set::dal_rends},
                    {"warblade_hakkari_mh",          Attributes{0, 0},       Special_stats{1, 0,
                                                                                           28},  1.7, 59,    110,   Weapon_socket::main_hand, Weapon_type::sword, std::vector<Hit_effect>(), Set::warblade_of_the_hakkari},
                    {"warblade_hakkari_oh",          Attributes{0, 0},       Special_stats{0, 0,
                                                                                           40},  1.7, 57,    106,   Weapon_socket::off_hand,  Weapon_type::sword, std::vector<Hit_effect>(), Set::warblade_of_the_hakkari},
                    {"r14_swiftblade",               Attributes{0, 0},       Special_stats{1, 0,
                                                                                           28},  1.8, 85,    129,   Weapon_socket::one_hand,  Weapon_type::sword},
                    {"r14_longsword",                Attributes{0, 0},       Special_stats{1, 0,
                                                                                           28},  2.9, 138,   207,   Weapon_socket::one_hand,  Weapon_type::sword},
                    {"thunderfury_blessed_blade",    Attributes{0, 5},       Special_stats{0, 0, 0},
                                                                                                 1.9, 60,    145,   Weapon_socket::one_hand,  Weapon_type::sword,
                                                                                                                                                                  {{"thunderfury_blessed_blade", Hit_effect::Type::damage_magic, {}, {}, 300, 0, 0.19}}},
                    {"teebus_blazing_longsword",     Attributes{0, 0},       Special_stats{0, 0,
                                                                                           0},   2.9, 96,    178,   Weapon_socket::main_hand, Weapon_type::sword, {{"teebus_blazing_longsword",  Hit_effect::Type::damage_magic,            {}, {}, 150, 0, 0.048}}},
                    {"protectors_sword",             Attributes{13, 0},      Special_stats{0, 0,
                                                                                           0},   2.7, 78,    146,   Weapon_socket::one_hand,  Weapon_type::sword},
                    {"nightmare_blade",              Attributes{0, 0},       Special_stats{0, 0,
                                                                                           32},  2.7, 99,    185,   Weapon_socket::one_hand,  Weapon_type::sword},
                    {"zulian_slicer",                Attributes{0, 0},       Special_stats{0, 0,
                                                                                           12},  2.5, 78,    146,   Weapon_socket::one_hand,  Weapon_type::sword, {{"zulian_slicer",             Hit_effect::Type::damage_magic,            {}, {}, 84,  0, .041}}},
                    {"ravencrests_legacy",           Attributes{13, 9},      Special_stats{0, 0,
                                                                                           0},  2.1, 84,    157,   Weapon_socket::one_hand,  Weapon_type::sword},
                    {"ravenholdt_slicer",            Attributes{0, 0},       Special_stats{0, 0,
                                                                                           26},  2.8, 83,    156,   Weapon_socket::one_hand,  Weapon_type::sword},
                    {"blackguard",                   Attributes{0, 0},       Special_stats{0, 0,
                                                                                           0},   1.8, 65,    121,   Weapon_socket::one_hand,  Weapon_type::sword},
                    {"gressil_dawn_of_ruin",                   Attributes{0, 0},       Special_stats{0, 0,
                                                                                           40},   2.7, 138,    257,   Weapon_socket::one_hand,  Weapon_type::sword},
                    {"iblis_blade_of_the_fallen_seraph",                   Attributes{0, 0},       Special_stats{1, 1,
                                                                                                     26},   1.6, 70,    131,   Weapon_socket::one_hand,  Weapon_type::sword},
                    {"the_hungering_cold",                   Attributes{0, 0},       Special_stats{0, 0,
                                                                                                     0, 0, 0, 6},   1.5, 76,    143,   Weapon_socket::one_hand,  Weapon_type::sword},
                    {"widows_remorse",                   Attributes{0, 0},       Special_stats{0, 1,
                                                                                                     0},   1.6, 70,    131,   Weapon_socket::one_hand,  Weapon_type::sword},
                    {"argent_avenger",                   Attributes{0, 0},       Special_stats{0, 0,
                                                                                               0},   2.2, 71,    108,   Weapon_socket::one_hand,  Weapon_type::sword,
                     {{"argent_avenger", Hit_effect::Type::stat_boost,   {0, 0}, Special_stats{0, 0, 200}, 0, 10, 2.2 / 60}}},
                    {"krol_blade",              Attributes{7, 0},       Special_stats{1, 0,
                                                                                           0},  2.8, 80,    149,   Weapon_socket::main_hand,  Weapon_type::sword},
            };


    std::vector<Weapon> two_handed_swords_t
            {
                    {"ashkandi,_greatsword_of_the_brotherhood", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0,
                                                                     86.0}, 3.5, 229.0, 344.0, Weapon_socket::two_hand, Weapon_type::sword},
                    {"the_untamed_blade", Attributes{0.0, 22.0}, Special_stats{0.0, 0.0,
                     0.0}, 3.4, 192.0, 289.0, Weapon_socket::two_hand, Weapon_type::sword, {{"the_untamed_blade", Hit_effect::Type::stat_boost, Attributes{300, 0}, {}, 0, 8, 3.4/60.0,0}}},
                    {"corrupted_ashbringer", Attributes{0.0, 0.0}, Special_stats{2.0, 1.0,
                                                                                                    0.0}, 3.6, 259.0, 389.0, Weapon_socket::two_hand, Weapon_type::sword, {{"corrupted_ashbringer", Hit_effect::Type::damage_magic, {}, {}, 200, 0, 3.6/60.0,0}}},
                    {"kalimdors_revenge", Attributes{24.0, 0.0}, Special_stats{0.0, 0.0,
                                                                                                    0.0}, 3.2, 209.0, 315.0, Weapon_socket::two_hand, Weapon_type::sword, {{"kalimdors_revenge", Hit_effect::Type::damage_magic, {}, {}, 258, 0, 3.2/60.0,0}}},
                    {"claymore_of_unholy_might", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0,
                                                                                                    98.0}, 3.6, 235.0, 354.0, Weapon_socket::two_hand, Weapon_type::sword},
                    {"obsidian_edged_blade", Attributes{42.0, 0.0}, Special_stats{0.0, 0.0,
                                                                                     0.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8}, 3.4, 176.0, 264.0, Weapon_socket::two_hand, Weapon_type::sword},
                    {"bonereavers_edge", Attributes{0.0, 0.0}, Special_stats{1.0, 0.0,
                                                                                  0.0},3.4, 206.0, 310.0, Weapon_socket::two_hand, Weapon_type::sword, {{"bonereavers_edge", Hit_effect::Type::reduce_armor, {}, {}, 0, 10, 2.0*3.4/60.0,0, 1, 700, 3}}},
                    {"manslayer_of_the_qiraji", Attributes{35.0, 15.0}, Special_stats{0.0, 0.0,
                                                                                  0.0}, 3.6, 180.0, 270.0, Weapon_socket::two_hand, Weapon_type::sword},
                    {"zinrokh,_destroyer_of_worlds", Attributes{.0, .0}, Special_stats{0.0, 0.0,
                                                                                      72.0}, 3.8, 196.0, 295.0, Weapon_socket::two_hand, Weapon_type::sword},
                    {"destiny", Attributes{.0, .0}, Special_stats{0.0, 0.0,
                                                                                       .0}, 2.6, 112.0, 168.0, Weapon_socket::two_hand, Weapon_type::sword, {{"destiny", Hit_effect::Type::stat_boost, Attributes{200, 0}, {}, 0, 10, 2.6/60.0,0}}},
                    {"zinrokh,_destroyer_of_worlds", Attributes{.0, .0}, Special_stats{0.0, 0.0,
                                                                                       72.0}, 3.8, 196.0, 295.0, Weapon_socket::two_hand, Weapon_type::sword},

            };

    std::vector<Weapon> two_handed_axes_t
    {
        {"severance", Attributes{43.0, 0.0}, Special_stats{2.0, 0.0, 0.0}, 3.6, 235.0, 354.0, Weapon_socket::two_hand,
         Weapon_type::axe},
        {"dark_edge_of_insanity", Attributes{35.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, 3.5, 242.0, 364.0, Weapon_socket::two_hand,
         Weapon_type::axe},
        {"drake_talon_cleaver", Attributes{22.0, 0.0}, Special_stats{2.0, 0.0, 0.0}, 3.4, 199.0, 300.0, Weapon_socket::two_hand,
            Weapon_type::axe, {{"drake_talon_cleaver", Hit_effect::Type::damage_physical, {}, {}, 240, 0, 3.4/60.0,0}}},
        {"spinal_reaper", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 34.0}, 3.4, 203.0, 305.0, Weapon_socket::two_hand,
            Weapon_type::axe},
        {"treants_bane", Attributes{25.0, 0.0}, Special_stats{2.0, 0.0, 0.0}, 2.7, 128.0, 193.0, Weapon_socket::two_hand,
            Weapon_type::axe},
        {"neretzek,_the_blood_drinker", Attributes{21.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, 3.7, 202.0, 303.0, Weapon_socket::two_hand,
            Weapon_type::axe, {{"neretzek,_the_blood_drinker", Hit_effect::Type::damage_magic, {}, {}, 152, 0, 3.7/60.0,0}}},
        {"draconic_avenger", Attributes{21.0, 0.0}, Special_stats{.0, 0.0, 0.0}, 3.2, 174.0, 262.0, Weapon_socket::two_hand, Weapon_type::axe},
    };

    std::vector<Weapon> two_handed_maces_t
    {
        {"might_of_menethil", Attributes{20.0, 0.0}, Special_stats{2.0, 0.0, 0.0}, 3.8, 289.0, 435.0, Weapon_socket::two_hand,
         Weapon_type::mace},
        {"sulfuras,_hand_of_ragnaros", Attributes{12.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, 3.7, 223.0, 372.0, Weapon_socket::two_hand,
            Weapon_type::mace, {{"sulfuras,_hand_of_ragnaros", Hit_effect::Type::damage_magic, {}, {}, 378, 0, 0.7*3.4/60.0,0}}},
        {"herald_of_woe", Attributes{31.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, 3.3, 199.0, 300.0, Weapon_socket::two_hand,Weapon_type::mace},
        {"draconic_maul", Attributes{27.0, 0.0}, Special_stats{2.0, 0.0, 0.0}, 3.5, 187.0, 282.0, Weapon_socket::two_hand,Weapon_type::mace},
        {"jekliks_crusher", Attributes{.0, 0.0}, Special_stats{.0, 0.0, 0.0}, 3.6, 159.0, 248.0, Weapon_socket::two_hand,Weapon_type::mace,
         {{"jekliks_crusher", Hit_effect::Type::damage_physical, {}, {}, 210, 0, 4*3.6/60.0,0}}},
        {"manual_crowd_pummeler_1charge", Attributes{16.0, 5.0}, Special_stats{.0, 0.0, 0.0},
         2.0, 46.0, 70.0, Weapon_socket::two_hand,Weapon_type::mace, {}, Set::none, {{"manual_crowd_pummeler", Use_effect::Effect_socket::unique, {}, {0,0,0,0,0.5}, 0, 30, 1000,false}}},
        {"manual_crowd_pummeler_2charge", Attributes{16.0, 5.0}, Special_stats{.0, 0.0, 0.0},
            2.0, 46.0, 70.0, Weapon_socket::two_hand,Weapon_type::mace, {}, Set::none, {{"manual_crowd_pummeler", Use_effect::Effect_socket::unique, {}, {0,0,0,0,0.5}, 0, 60, 1000,false}}},
        {"manual_crowd_pummeler_3charge", Attributes{16.0, 5.0}, Special_stats{.0, 0.0, 0.0},
            2.0, 46.0, 70.0, Weapon_socket::two_hand,Weapon_type::mace, {}, Set::none, {{"manual_crowd_pummeler", Use_effect::Effect_socket::unique, {}, {0,0,0,0,0.5}, 0, 90, 1000,false}}},
    };

    std::vector<Weapon> axes_t
            {
                    {"deathbringer",                  Attributes{0, 0},      Special_stats{0, 0,
                                                                                           0},    2.9, 114,   213,   Weapon_socket::one_hand, Weapon_type::axe, {{"deathbringer",              Hit_effect::Type::damage_magic, {}, {}, 125, 0, 0.08}}},
                    {"crul_shorukh_edge_of_chaos",    Attributes{0.0, 0.0},  Special_stats{0.0, 0.0,
                                                                                           36.0}, 2.3, 101.0, 188.0, Weapon_socket::one_hand, Weapon_type::axe},
                    {"dooms_edge",                    Attributes{9.0, 16.0}, Special_stats{0.0, 0.0,
                                                                                           0.0},  2.3, 83.0,  154.0, Weapon_socket::one_hand, Weapon_type::axe},
                    {"axe_of_the_deep_woods",         Attributes{0.0, 0.0},  Special_stats{0.0, 0.0,
                                                                                           0.0},  2.7, 78.0,  146.0, Weapon_socket::one_hand, Weapon_type::axe, {{"axe_of_the_deep_woods",     Hit_effect::Type::damage_magic, {}, {}, 108, 0, 0.036}}},
                    {"ancient_hakkari_manslayer",     Attributes{0.0, 0.0},  Special_stats{0.0, 0.0,
                                                                                           0.0},  2.0, 69.0,  130.0, Weapon_socket::one_hand, Weapon_type::axe, {{"ancient_hakkari_manslayer", Hit_effect::Type::damage_magic, {}, {}, 51,  0, 0.036}}},
                    {"r14_handaxe",                   Attributes{0, 0},      Special_stats{1, 0,
                                                                                           28},   2.9, 138,   207,   Weapon_socket::one_hand, Weapon_type::axe},
                    {"frostbite",                     Attributes{15, 0},     Special_stats{0, 0,
                                                                                           0},    2.7, 80,    150,   Weapon_socket::one_hand, Weapon_type::axe},
                    {"flurry_axe",                    Attributes{0, 0},      Special_stats{0, 0,
                                                                                           0},    1.5, 37,    69,    Weapon_socket::one_hand, Weapon_type::axe, {{"flurry_axe",                Hit_effect::Type::extra_hit,    {}, {}, 0,   0, 0.04625}}},
                    {"bone_slicing_hatchet",          Attributes{0, 13},     Special_stats{0, 0,
                                                                                           0},    1.7, 48,    90,    Weapon_socket::one_hand, Weapon_type::axe},
                    {"zulian_hacker_of_strength",     Attributes{12, 0},     Special_stats{0, 0,
                                                                                           0, 0, 0, 0,
                                                                                           2},    2.4, 71,    134,   Weapon_socket::one_hand, Weapon_type::axe},
                    {"zulian_hacker_of_the_tiger",    Attributes{8, 8},      Special_stats{0, 0,
                                                                                           0, 0, 0, 0,
                                                                                           2},    2.4, 71,    134,   Weapon_socket::one_hand, Weapon_type::axe},
                    {"blessed_qiraji_war_axe",        Attributes{0, 0},      Special_stats{1, 0,
                                                                                           14},   2.6, 110,   205,   Weapon_socket::one_hand, Weapon_type::axe},
                    {"sickle_on_unyielding_strength", Attributes{15, 6},     Special_stats{0, 0,
                                                                                           0},    2.1, 75,    141,   Weapon_socket::one_hand, Weapon_type::axe},
                    {"hatchet_of_sundered_bone", Attributes{0, 0},     Special_stats{1, 0,
                                                                                           36},    2.6, 119,    221,   Weapon_socket::one_hand, Weapon_type::axe},
                    {"annihilator", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0,
                                                                             0.0},1.7, 49.0, 92.0, Weapon_socket::main_hand, Weapon_type::axe, {{"annihilator", Hit_effect::Type::reduce_armor, {}, {}, 0, 10, 1.7/60.0,0, 1, 200, 3}}},
                    {"rivenspike", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0,
                                                                        0.0},2.9, 77.0, 144.0, Weapon_socket::one_hand, Weapon_type::axe, {{"rivenspike", Hit_effect::Type::reduce_armor, {}, {}, 0, 10, 2.9/60.0,0, 1, 200, 3}}},
            };

    std::vector<Weapon> daggers_t
            {
                    {"distracting_dagger",     Attributes{0, 0},  Special_stats{0, 0,
                                                                              0, 0, 0, 0, 0, 6}, 1.3, 42, 64, Weapon_socket::off_hand,  Weapon_type::dagger},
                    {"core_hound_tooth",     Attributes{0, 0},  Special_stats{1, 0,
                                                                              20}, 1.6, 57, 107, Weapon_socket::one_hand,  Weapon_type::dagger},
                    {"deaths_sting",         Attributes{0, 0},  Special_stats{0, 0, 38, 0, 0, 0, 0,
                                                                              3},  1.8, 95, 144, Weapon_socket::one_hand,  Weapon_type::dagger},
                    {"r14_dagger",           Attributes{0, 0},  Special_stats{1, 0,
                                                                              28}, 2.0, 95, 143, Weapon_socket::one_hand,  Weapon_type::dagger},
                    {"dragonfang_blade",     Attributes{0, 16}, Special_stats{0, 0,
                                                                              0},  1.8, 69, 130, Weapon_socket::one_hand,  Weapon_type::dagger},
                    {"perditions_blade",     Attributes{0, 0},  Special_stats{0, 0,
                                                                              0},  1.8, 73, 137, Weapon_socket::one_hand,  Weapon_type::dagger, {{"perditions_blade", Hit_effect::Type::damage_magic, {}, {}, 48, 0, 0.03}}},
                    {"alcors_sunrazor",      Attributes{0, 0},  Special_stats{0, 0,
                                                                              0},  1.3, 41, 77,  Weapon_socket::one_hand,  Weapon_type::dagger, {{"alcors_sunrazor",  Hit_effect::Type::damage_magic, {}, {}, 90, 0, 0.021}}},
                    {"finkles_skinner",      Attributes{0, 0},  Special_stats{0, 0,
                                                                              0},  1.3, 37, 70,  Weapon_socket::main_hand, Weapon_type::dagger},
                    {"blessed_qiraji_pugio", Attributes{0, 0},  Special_stats{1, 1,
                                                                              18}, 1.7, 72, 134, Weapon_socket::one_hand,  Weapon_type::dagger},
                    {"fang_of_the_faceless", Attributes{0, 0},  Special_stats{1, 0,
                                                                              28}, 1.9, 66, 123, Weapon_socket::one_hand,  Weapon_type::dagger},
                    {"harbinger_of_doom", Attributes{0, 8},  Special_stats{1, 1,
                                                                              0}, 1.6, 83, 126, Weapon_socket::one_hand,  Weapon_type::dagger},
                    {"kingsfall", Attributes{0, 16},  Special_stats{1, 1,
                                                                           0}, 1.8, 105, 158, Weapon_socket::one_hand,  Weapon_type::dagger},
                    {"maexxnas_fang", Attributes{0, 0},  Special_stats{0, 1,
                                                                    36}, 1.8, 94, 141, Weapon_socket::one_hand,  Weapon_type::dagger},
                    {"felstriker", Attributes{0, 0},  Special_stats{0, 0,
                                                                       0}, 1.7, 54, 101, Weapon_socket::one_hand,  Weapon_type::dagger,
                        {{"felstriker", Hit_effect::Type::stat_boost,   {0, 0}, Special_stats{100, 100, 0}, 0, 3, 1.7 / 60}}},
                    {"gutgore_ripper",                  Attributes{0, 0},      Special_stats{0, 0,
                                                                                           0},    1.8, 63,   119,   Weapon_socket::one_hand, Weapon_type::dagger, {{"gutgore_ripper",              Hit_effect::Type::damage_magic, {}, {}, 75, 0, 1.8/60}}},
            };

    std::vector<Weapon> maces_t
            {
                    {"ebon_hand",                 Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, 2.3, 83.0, 154.0,
                                                                                                                      Weapon_socket::one_hand,  Weapon_type::mace,
                            {{"ebon_hand",           Hit_effect::Type::damage_magic, {},     {},                200, 0,  0.08}}},
                    {"spineshatter",              Attributes{9.0, 0.0}, Special_stats{0.0, 0.0,
                                                                                      0.0},           2.6, 99.0, 184.0,
                                                                                                                      Weapon_socket::main_hand, Weapon_type::mace},
                    {"stormstike_hammer",         Attributes{15, 0},    Special_stats{0, 0,
                                                                                      0},             2.7, 80,   150,
                                                                                                                      Weapon_socket::one_hand,  Weapon_type::mace},
                    {"sand_polished_hammer",      Attributes{0, 0},     Special_stats{1, 0,
                                                                                      20},            2.6, 97,   181,
                                                                                                                      Weapon_socket::one_hand,  Weapon_type::mace},
                    {"empyrean_demolisher",       Attributes{0, 0},     Special_stats{0, 0,
                                                                                      0},             2.8, 94,   175,
                                                                                                                      Weapon_socket::main_hand, Weapon_type::mace,
                            {{"empyrean_demolisher", Hit_effect::Type::stat_boost,   {0, 0}, Special_stats{0, 0, 0, 0,
                                                                                                           .2}, 0,   10, 2.8 / 60}}},
                    {"anubisath_warhammer",       Attributes{0, 0},     Special_stats{0, 0, 32, 0, 0, 0, 0, 0,
                                                                                      4},             1.8, 66,
                                                                                                                 123, Weapon_socket::one_hand,  Weapon_type::mace},
                    {"r14_mace",                  Attributes{0, 0},     Special_stats{1, 0,
                                                                                      28},            2.9, 138,  207, Weapon_socket::one_hand,  Weapon_type::mace},
                    {"ironfoe",                   Attributes{0, 0},     Special_stats{0, 0,
                                                                                      0},             2.4, 73,   136, Weapon_socket::main_hand, Weapon_type::mace,
                            {{"ironfoe",             Hit_effect::Type::extra_hit,    {},     {},                0,   0,  0.064}}},
                    {"mass_of_mcgowan",           Attributes{10, 0},    Special_stats{0, 0,
                                                                                      0},             2.8, 80,   150, Weapon_socket::one_hand,  Weapon_type::mace},
                    {"sceptre_of_smiting",        Attributes{10, 0},    Special_stats{0, 0,
                                                                                      0, 0, 0, 0, 0, 0,
                                                                                      2},             2.6, 77,   145, Weapon_socket::one_hand,  Weapon_type::mace},
                    {"blessed_qiraji_warhammer", Attributes{10, 0},    Special_stats{0, 0,
                                                                                      0},             2.1, 89,   166, Weapon_socket::one_hand,  Weapon_type::mace},
                    {"persuader",                 Attributes{0, 0},     Special_stats{1, 1,
                                                                                      0},             2.7, 86,   161, Weapon_socket::main_hand, Weapon_type::mace},
                    {"misplaced_servo_arm",                 Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, 2.8, 128.0, 238.0,
                        Weapon_socket::one_hand,  Weapon_type::mace,
                        {{"misplaced_servo_arm",           Hit_effect::Type::damage_magic, {},     {},                125, 0,  0.10, 0, 1, 0, 0, 2, true}}},
                    {"the_castigator",                 Attributes{0, 0},     Special_stats{1, 1,
                                                                                      16},             2.6, 119,   221, Weapon_socket::one_hand, Weapon_type::mace},

            };

    std::vector<Weapon> fists_t
            {
                    {"claw_of_the_black_drake", Attributes{13, 0}, Special_stats{1, 0, 0},  2.6, 102, 191,
                            Weapon_socket::main_hand, Weapon_type::unarmed},
                    {"silithid_claw",           Attributes{0, 0},  Special_stats{1, 0, 30}, 1.6, 64,  120,
                            Weapon_socket::main_hand, Weapon_type::unarmed},
                    {"claw_of_the_frost_wyrm",           Attributes{0, 0},  Special_stats{1, 1, 22}, 1.5, 75,  140,
                        Weapon_socket::off_hand, Weapon_type::unarmed},
                    {"eskhadars_right_claw",           Attributes{0, 4},  Special_stats{0, 0, 0}, 1.5, 50,  94,
                        Weapon_socket::main_hand, Weapon_type::unarmed, {{"eskhadars_right_claw", Hit_effect::Type::stat_boost,   {0, 0}, Special_stats{0, 0, 0, 0,
                                                                                                                                                       .3}, 0,   5, 1.5 / 60}}},
            };

    std::vector<Set_bonus> set_bonuses{
            {"devilsaur",               Attributes{0, 0}, Special_stats{0, 2, 0},  2, Set::devilsaur},
            {"black_dragonscale",       Attributes{0, 0}, Special_stats{0, 1, 0},  2, Set::black_dragonscale},
            {"black_dragonscale",       Attributes{0, 0}, Special_stats{2, 0, 0},  3, Set::black_dragonscale},
            {"rare_pvp_set",            Attributes{0, 0}, Special_stats{0, 0, 40}, 2, Set::rare_pvp_set},
            {"epic_pvp_set",            Attributes{0, 0}, Special_stats{0, 0, 40}, 6, Set::epic_pvp_set},
            {"dal_rends",               Attributes{0, 0}, Special_stats{0, 0, 50}, 2, Set::dal_rends},
            {"warblade_of_the_hakkari", Attributes{0, 0}, Special_stats{0, 0, 0, 0, 0,
                                                                        6},        2, Set::warblade_of_the_hakkari},
            {"valor",                   Attributes{0, 0}, Special_stats{0, 0, 40}, 4, Set::valor},
            {"major_mojo_infusion",     Attributes{0, 0}, Special_stats{0, 0, 30}, 2, Set::major_mojo_infusion},
            {"battlegear_of_heroism",   Attributes{0, 0}, Special_stats{0, 0, 40}, 6, Set::battlegear_of_heroism},
            {"the_defilers_resolution", Attributes{0, 0}, Special_stats{1, 0, 0},  3, Set::the_defilers_resolution},
            {"battlegear_of_wrath",     Attributes{0, 0}, Special_stats{0, 0, 30},  3, Set::battlegear_of_wrath},
            {"battlegear_of_undead_slaying",     Attributes{0, 0}, Special_stats{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.02, 0, 0, 0, 0, 0, 0.02}, 3, Set::battlegear_of_undead_slaying},
    };

//    {"Thekals_grasp",           Attributes{0, 0},  Special_stats{1, 0, 0}, 2.2, 72,  135,
//            Weapon_socket::main_hand, Weapon_type::unarmed, std::vector<Hit_effect>(), Set::primal_blessing},
//    {"Arlokks_grasp",           Attributes{0, 0},  Special_stats{1, 0, 0}, 2.2, 72,  135,
//            Weapon_socket::main_hand, Weapon_type::unarmed, std::vector<Hit_effect>(), Set::primal_blessing},

    std::vector<Armor> get_items_in_socket(Socket socket) const;

    std::vector<Weapon> get_weapon_in_socket(Weapon_socket socket) const;

    Armor find_armor(Socket socket, const std::string &name) const;

    Weapon find_weapon(Weapon_socket weapon_socket, const std::string &name) const;

    Attributes get_enchant_attributes(Socket socket, Enchant::Type type) const;

    Special_stats get_enchant_special_stats(Socket socket, Enchant::Type type) const;

    Hit_effect enchant_hit_effect(double weapon_speed, Enchant::Type type) const;

    void clean_weapon(Weapon &weapon) const;

    void compute_total_stats(Character &character) const;

    bool check_if_armor_valid(const std::vector<Armor> &armor) const;

    bool check_if_weapons_valid(std::vector<Weapon> &weapons) const;

    void change_weapon(std::vector<Weapon> &current_weapons, const Weapon &equip_weapon, const Socket &socket) const;

    void change_armor(std::vector<Armor> &armor_vec, const Armor &armor, bool first_misc_slot = true) const;

    void add_enchants_to_character(Character& character, const std::vector<std::string>& ench_vec) const;

    void add_buffs_to_character(Character& character, const std::vector<std::string>& buffs_vec) const;

    void add_talents_to_character(Character& character, const std::vector<std::string>& talent_string,
                                  const std::vector<int>& talent_val) const;

    Buffs buffs;
};

#endif //WOW_SIMULATOR_ARMORY_HPP
