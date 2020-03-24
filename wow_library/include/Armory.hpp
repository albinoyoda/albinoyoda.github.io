#ifndef WOW_SIMULATOR_ARMORY_HPP
#define WOW_SIMULATOR_ARMORY_HPP

#include "Item.hpp"

struct Buffs
{
    Buffs()
    {
        set_extra_properties();
    }

    // World buffs
    World_buff rallying_cry{"rallying_cry", Stats{0.0, 0.0}, Special_stats{5.0, 0.0, 140}};
    World_buff dire_maul{"dire_maul", Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 200}};
    World_buff songflower{"songflower", Stats{15, 15}, Special_stats{5, 0, 0}};

    // Player_buffs
    Player_buff blessing_of_kings{"blessing_of_kings", Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Player_buff blessing_of_might{"blessing_of_might", Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 155}};
    Player_buff gift_of_the_wild{"gift_of_the_wild", Stats{12.0, 12.0}, Special_stats{0.0, 0.0, 0.0}};
    Player_buff trueshot_aura{"trueshot_aura", Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 100}};

    // Consumables
    Consumable elixir_mongoose{"elixir_mongoose", Stats{0.0, 25.0}, Special_stats{2.0, 0.0, 0.0}};
    Consumable dense_stone_mh{"dense_stone_mh", Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Consumable dense_stone_oh{"dense_stone_oh", Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Consumable elemental_stone_mh{"elemental_stone_mh", Stats{0.0, 0.0}, Special_stats{2.0, 0.0, 0.0}};
    Consumable elemental_stone_oh{"elemental_stone_oh", Stats{0.0, 0.0}, Special_stats{2.0, 0.0, 0.0}};
    Consumable blessed_sunfruit{"blessed_sunfruit", Stats{10.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Consumable juju_power{"juju_power", Stats{30.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Consumable juju_might{"juju_might", Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 40}};
    Consumable roids{"roids", Stats{25.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};

    void set_extra_properties()
    {
        blessing_of_kings.set_stat_multiplier(1.1);
        dense_stone_mh.set_mh_bonus_damage(8.0);
        dense_stone_oh.set_oh_bonus_damage(8.0);
    }
};

struct Armory
{
    Armory()
    {
        set_extra_item_properties();
    }

    struct helmet_t
    {
        Armor lionheart_helm{"lionheart_helm", Stats{18, 0}, Special_stats{2, 2, 0}, Armor::Socket::head};
        Armor expert_goldminers_head{"expert_goldminers_head", Stats{0, 5}, Special_stats{0, 0, 0},
                                     Armor::Socket::head};
        Armor mask_of_the_unforgiven{"mask_of_the_unforgiven", Stats{0, 0}, Special_stats{1, 2, 0},
                                     Armor::Socket::head};
        Armor crown_of_destruction{"crown_of_destruction", Stats{0, 0}, Special_stats{2, 0, 44}, Armor::Socket::head};
        Armor helm_of_endless_rage{"helm_of_endless_rage", Stats{26, 26}, Special_stats{0, 0, 0}, Armor::Socket::head};


    } helmet;

    struct neck_t
    {
        Armor onyxia_tooth_pendant{"onyxia_tooth_pendant", Stats{0, 13}, Special_stats{1, 1, 0}, Armor::Socket::neck};
        Armor prestors_talisman_of_connivery{"prestors_talisman_of_connivery", Stats{0, 30}, Special_stats{0, 1, 0},
                                             Armor::Socket::neck};
        Armor aq_barbed_choker{"aq_barbed_choker", Stats{0, 0}, Special_stats{1, 0, 44},
                               Armor::Socket::neck};
        Armor mark_of_fordring{"mark_of_fordring", Stats{0, 0}, Special_stats{1, 0, 26},
                               Armor::Socket::neck};
    } neck;

    struct shoulder_t
    {
        Armor black_dragonscale_shoulders{"black_dragonscale_shoulders", Stats{0, 0}, Special_stats{0, 0, 40},
                                          Armor::Socket::shoulder, Set_name::black_dragonscale};
        Armor truestrike_shoulders{"truestrike_shoulders", Stats{0, 0}, Special_stats{0, 2, 24},
                                   Armor::Socket::shoulder};
        Armor drake_talon_pauldrons{"drake_talon_pauldrons", Stats{20, 20}, Special_stats{0, 0, 0},
                                    Armor::Socket::shoulder};
        Armor leutenant_shoulders{"leutenant_shoulders", Stats{17, 0}, Special_stats{1, 0, 0}, Armor::Socket::shoulder,
                                  Set_name::rare_pvp_set};
        Armor spaulders_of_valor{"spaulders_of_valor", Stats{11, 9}, Special_stats{0, 0, 0}, Armor::Socket::shoulder};
        Armor aq_chitinous_shoulderguards{"aq_chitinous_shoulderguards", Stats{11, 25}, Special_stats{0, 0, 0},
                                          Armor::Socket::shoulder};
        Armor aq_mantle_of_wicked_revenge{"aq_mantle_of_wicked_revenge", Stats{16, 30}, Special_stats{0, 0, 0},
                                          Armor::Socket::shoulder};
        Armor aq_conquerors_spaulders{"aq_conquerors_spaulders", Stats{20, 16}, Special_stats{0, 1, 0},
                                      Armor::Socket::shoulder};
        Armor big_bad_pauldrons{"big_bad_pauldrons", Stats{12, 0}, Special_stats{0, 0, 0},
                                Armor::Socket::shoulder};
    } shoulder;

    struct back_t
    {
        Armor shadewood_cloak{"shadewood_cloak", Stats{13, 0}, Special_stats{0, 0, 0},
                              Armor::Socket::back};
        Armor cape_of_the_black_baron{"cape_of_the_black_baron", Stats{0, 15}, Special_stats{0, 0, 20},
                                      Armor::Socket::back};
        Armor cloak_of_firemaw{"cloak_of_firemaw", Stats{0, 0}, Special_stats{0, 0, 50}, Armor::Socket::back};
        Armor cloak_of_draconic_might{"cloak_of_draconic_might", Stats{16, 16}, Special_stats{0, 0, 0},
                                      Armor::Socket::back};
        Armor aq_cloak_of_concentrated_hate{"aq_cloak_of_concentrated_hate", Stats{11, 16}, Special_stats{0, 1, 0},
                                            Armor::Socket::back};
    } back;

    struct chest_t
    {
        Armor malfurions_blessed_bulwark{"malfurions_blessed_bulwark", Stats{40, 0}, Special_stats{0, 0, 0},
                                         Armor::Socket::chest};
        Armor savage_gladiator_chain{"savage_gladiator_chain", Stats{13, 14}, Special_stats{2, 0, 0},
                                     Armor::Socket::chest};
        Armor knight_captains_plate_hauberk{"knight_captains_plate_hauberk", Stats{21, 0}, Special_stats{1, 0, 0},
                                            Armor::Socket::chest, Set_name::rare_pvp_set};
        Armor tombstone_breastplate{"tombstone_breastplate", Stats{10, 0}, Special_stats{2, 0, 0},
                                    Armor::Socket::chest};
        Armor breastplate_of_bloodthirts{"breastplate_of_bloodthirts", Stats{10, 0}, Special_stats{2, 0, 0},
                                         Armor::Socket::chest};
        Armor cadaverous_armor{"cadaverous_armor", Stats{8, 8}, Special_stats{0, 0, 60}, Armor::Socket::chest};
        Armor aq_breatplate_of_annihilation{"aq_breatplate_of_annihilation", Stats{37, 0}, Special_stats{1, 1, 0},
                                            Armor::Socket::chest};
        Armor aq_vest_of_swift_execution{"aq_vest_of_swift_execution", Stats{21, 41}, Special_stats{0, 0, 0},
                                         Armor::Socket::chest};
        Armor aq_conquerors_breastplate{"aq_conquerors_breastplate", Stats{34, 24}, Special_stats{0, 0, 0},
                                        Armor::Socket::chest};
        Armor carapace_of_anub{"carapace_of_anub", Stats{11, 11}, Special_stats{0, 0, 0},
                               Armor::Socket::chest};
        Armor black_dragonscale_chest{"black_dragonscale_chest", Stats{0, 0}, Special_stats{0, 0, 50},
                                      Armor::Socket::chest, Set_name::black_dragonscale};
    } chest;

    struct wrists_t
    {
        Armor wristguards_of_stability{"wristguards_of_stability", Stats{24, 0}, Special_stats{0, 0, 0},
                                       Armor::Socket::wrists};
        Armor vambraces_of_the_sadist{"vambraces_of_the_sadist", Stats{6, 0}, Special_stats{1, 0, 0},
                                      Armor::Socket::wrists};
        Armor wristhuards_of_true_flight{"wristhuards_of_true_flight", Stats{0, 19}, Special_stats{0, 1, 0},
                                         Armor::Socket::wrists};
        Armor battleborn_armbraces{"battleborn_armbraces", Stats{0, 0}, Special_stats{1, 1, 0}, Armor::Socket::wrists};
        Armor aq_bracers_of_brutality{"aq_bracers_of_brutality", Stats{21, 12}, Special_stats{0, 0, 0},
                                      Armor::Socket::wrists};
        Armor aq_hive_defiler_wristguards{"aq_hive_defiler_wristguards", Stats{23, 18}, Special_stats{0, 0, 0},
                                          Armor::Socket::wrists};
        Armor aq_quiraji_execution_bracers{"aq_quiraji_execution_bracers", Stats{15, 16}, Special_stats{0, 1, 0},
                                           Armor::Socket::wrists};
        Armor slashclaw_bracers{"slashclaw_bracers", Stats{0, 7}, Special_stats{0, 1, 0},
                                Armor::Socket::wrists};
    } wrist;

    struct hands_t
    {
        Armor devilsaur_gauntlets{"devilsaur_gauntlets", Stats{0, 0}, Special_stats{1, 0, 28}, Armor::Socket::hands,
                                  Set_name::devilsaur};
        Armor edgemasters_handguards{"edgemasters_handguards", Stats{0, 0}, Special_stats{0, 0, 0},
                                     Armor::Socket::hands};
        Armor flameguard_gauntlets{"flameguard_gauntlets", Stats{0, 0}, Special_stats{1, 0, 54}, Armor::Socket::hands};
        Armor aq_gloves_of_enforcement{"aq_gloves_of_enforcement", Stats{28, 20}, Special_stats{0, 1, 0},
                                       Armor::Socket::hands};
        Armor aged_core_leather_gloves{"aged_core_leather_gloves", Stats{15, 0}, Special_stats{1, 0, 0},
                                       Armor::Socket::hands};
        Armor aq_gauntlets_of_annihilation{"aq_gauntlets_of_annihilation", Stats{35, 0}, Special_stats{1, 1, 0},
                                           Armor::Socket::hands};
    } hands;

    struct belt_t
    {
        Armor onslaught_girdle{"onslaught_girdle", Stats{31, 0}, Special_stats{1, 1, 0}, Armor::Socket::belt};
        Armor mugglers_belt{"mugglers_belt", Stats{0, 0}, Special_stats{1, 0, 0}, Armor::Socket::belt};
        Armor omokks_girth{"omokks_girth", Stats{15, 0}, Special_stats{1, 0, 0}, Armor::Socket::belt};
        Armor girdle_of_the_fallen_crusader{"girdle_of_the_fallen_crusader", Stats{20, 10}, Special_stats{0, 0, 0},
                                            Armor::Socket::belt};
    } belt;

    struct legs_t
    {
        Armor devilsaur_leggings{"devilsaur_leggings", Stats{0, 0}, Special_stats{1, 0, 46}, Armor::Socket::legs,
                                 Set_name::devilsaur};
        Armor black_dragonscale_leggings{"black_dragonscale_leggings", Stats{0, 0}, Special_stats{0, 0, 54},
                                         Armor::Socket::legs, Set_name::black_dragonscale};
        Armor eldritch_legplates{"eldritch_legplates", Stats{15, 9}, Special_stats{1, 0, 0}, Armor::Socket::legs};
        Armor ubrs_legs{"ubrs_legs", Stats{29, 0}, Special_stats{0, 0, 0}, Armor::Socket::legs};
        Armor cloudkeeper_legplaters{"cloudkeeper_legplaters", Stats{20, 20}, Special_stats{0, 0, 100},
                                     Armor::Socket::legs};
        Armor legguards_of_the_fallen_crusader{"legguards_of_the_fallen_crusader", Stats{28, 22},
                                               Special_stats{0, 0, 0}, Armor::Socket::legs};
        Armor knight_captain_plate_leggings{"knight_captain_plate_leggings", Stats{12, 0},
                                            Special_stats{2, 0, 0}, Armor::Socket::legs, Set_name::rare_pvp_set};

        Armor aq_legplates_of_the_qiraji_command{"aq_legplates_of_the_qiraji_command", Stats{20, 0},
                                                 Special_stats{2, 0, 0}, Armor::Socket::legs};
        Armor aq_scaled_sand_reaver_leggings{"aq_scaled_sand_reaver_leggings", Stats{0, 0},
                                             Special_stats{2, 0, 62}, Armor::Socket::legs};
        Armor aq_conquerors_legguards{"aq_conquerors_legguards", Stats{33, 21}, Special_stats{0, 1, 0},
                                      Armor::Socket::legs};
        Armor titanic_leggings{"titanic_leggings", Stats{30, 0}, Special_stats{1, 2, 0},
                               Armor::Socket::legs};
    } legs;

    struct boots_t
    {
        Armor windreaver_greaves{"windreaver_greaves", Stats{0, 20}, Special_stats{0, 1, 0}, Armor::Socket::boots};
        Armor bloodmail_boots{"bloodmail_boots", Stats{9, 9}, Special_stats{0, 1, 0}, Armor::Socket::boots};
        Armor black_dragonscale_boots{"black_dragonscale_boots", Stats{0, 0}, Special_stats{0, 0, 28},
                                      Armor::Socket::boots, Set_name::black_dragonscale};
        Armor knight_leutenants_greaves{"knight_leutenants_greaves", Stats{10, 9}, Special_stats{0, 0, 40},
                                        Armor::Socket::boots, Set_name::rare_pvp_set};
        Armor battlechasers{"battlechasers", Stats{14, 13}, Special_stats{0, 0, 0}, Armor::Socket::boots};
        Armor chromatic_boots{"chromatic_boots", Stats{20, 20}, Special_stats{0, 1, 0}, Armor::Socket::boots};
        Armor boots_of_shadow_flame{"boots_of_shadow_flame", Stats{0, 0}, Special_stats{0, 2, 44},
                                    Armor::Socket::boots};
        Armor aq_boots_of_the_vanguard{"aq_boots_of_the_vanguard", Stats{22, 22}, Special_stats{0, 0, 0},
                                       Armor::Socket::boots};
    } boots;

    struct rings_t
    {
        Armor don_julios_band{"don_julios_band", Stats{0, 0}, Special_stats{1, 1, 16}, Armor::Socket::ring};
        Armor magnis_will{"magnis_will", Stats{6, 0}, Special_stats{1, 0, 0}, Armor::Socket::ring};
        Armor quick_strike_ring{"quick_strike_ring", Stats{8, 0}, Special_stats{1, 0, 30}, Armor::Socket::ring};
        Armor master_dragonslayers_ring{"master_dragonslayers_ring", Stats{0, 0}, Special_stats{0, 1, 48},
                                        Armor::Socket::ring};
        Armor circle_of_applied_force{"circle_of_applied_force", Stats{12, 22}, Special_stats{0, 0, 0},
                                      Armor::Socket::ring};
        Armor blackstone_ring{"blackstone_ring", Stats{0, 0}, Special_stats{0, 1, 20}, Armor::Socket::ring};
        Armor tarnished_elven{"tarnished_elven", Stats{0, 15}, Special_stats{0, 1, 0}, Armor::Socket::ring};
    } rings;

    struct trinket_t
    {
        Armor hand_of_justice{"hand_of_justice", Stats{0, 0}, Special_stats{0, 0, 20}, Armor::Socket::trinket};
        Armor diamond_flask{"diamond_flask", Stats{75, 0}, Special_stats{0, 0, 0}, Armor::Socket::trinket};
        Armor blackhands_breadth{"blackhands_breadth", Stats{0, 0}, Special_stats{2, 0, 0}, Armor::Socket::trinket};
        Armor drake_fang_talisman{"drake_fang_talisman", Stats{0, 0}, Special_stats{0, 2, 56}, Armor::Socket::trinket};
    } trinket;

    struct ranged_t
    {
        Armor satyrs_bow{"satyrs_bow", Stats{0, 3}, Special_stats{0, 1, 0}, Armor::Socket::ranged};
        Armor precisely_calibrated_boomstick{"precisely_calibrated_boomstick", Stats{0, 14}, Special_stats{0, 0, 0},
                                             Armor::Socket::ranged};
        Armor strikers_mark{"strikers_mark", Stats{0, 0}, Special_stats{0, 1, 22}, Armor::Socket::ranged};
        Armor heartstriker{"heartstriker", Stats{0, 0}, Special_stats{0, 0, 24}, Armor::Socket::ranged};
        Armor blastershot{"blastershot", Stats{0, 0}, Special_stats{1, 0, 0}, Armor::Socket::ranged};
        Armor dragonbreath_hand_cannon{"dragonbreath_hand_cannon", Stats{0, 14}, Special_stats{0, 0, 0},
                                       Armor::Socket::ranged};
        Armor aq_crossbow_of_imminent_doom{"aq_crossbow_of_imminent_doom", Stats{5, 7}, Special_stats{0, 1, 0},
                                           Armor::Socket::ranged};
        Armor aq_larvae_of_the_great_worm{"aq_larvae_of_the_great_worm", Stats{0, 0}, Special_stats{1, 0, 18},
                                          Armor::Socket::ranged};
        Armor bloodseeker{"bloodseeker", Stats{8, 7}, Special_stats{0, 0, 0},
                          Armor::Socket::ranged};
        Armor riphook{"riphook", Stats{0, 0}, Special_stats{0, 0, 22}, Armor::Socket::ranged};
    } ranged;

    // ---------------- Weapons ------------------
    struct swords_t
    {
        Weapon maladath{"maladath", 2.2, {86.0, 162.0}, Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0},
                        Weapon::Socket::one_hand, Skill_type::sword};
        Weapon chromatically_tempered_sword{"chromatically_tempered_sword", 2.6, {106.0, 198.0}, Stats{14.0, 14.0},
                                            Special_stats{0.0, 0.0, 0.0}, Weapon::Socket::one_hand, Skill_type::sword};
        Weapon brutality_blade{"brutality_blade", 2.5, {90, 168}, Stats{9, 9}, Special_stats{1, 0, 0},
                               Weapon::Socket::one_hand, Skill_type::sword};
        Weapon assassination_blade{"assassination_blade", 2.7, {71, 132}, Stats{3, 0}, Special_stats{1, 0, 0},
                                   Weapon::Socket::one_hand, Skill_type::sword};
        Weapon mirahs_song{"mirahs_song", 1.8, {57, 87}, Stats{9, 9}, Special_stats{0, 0, 0}, Weapon::Socket::one_hand,
                           Skill_type::sword};
        Weapon viskag{"viskag", 2.6, {100, 187}, Stats{0, 0}, Special_stats{0, 0, 0}, Weapon::Socket::one_hand,
                      Skill_type::sword};
        Weapon quel_serrar{"quel_serrar", 2.0, {84, 126}, Stats{0, 0}, Special_stats{0, 0, 0}, Weapon::Socket::one_hand,
                           Skill_type::sword};
        Weapon aq_ancient_quiraj_ripper{"aq_ancient_quiraj_ripper", 2.8, {114, 213}, Stats{0, 0},
                                        Special_stats{1, 0, 20}, Weapon::Socket::one_hand, Skill_type::sword};
    } swords;

    struct axes_t
    {
        Weapon deathbringer{"deathbringer", 2.9, {114, 213}, Stats{0, 0}, Special_stats{0, 0, 0},
                            Weapon::Socket::one_hand, Skill_type::axe};
        Weapon crul_shorukh_edge_of_chaos{"crul_shorukh_edge_of_chaos", 2.3, {101.0, 188.0}, Stats{0.0, 0.0},
                                          Special_stats{0.0, 0.0, 36.0}, Weapon::Socket::one_hand, Skill_type::axe};
        Weapon dooms_edge{"dooms_edge", 2.3, {83.0, 154.0}, Stats{9.0, 16.0}, Special_stats{0.0, 0.0, 0.0},
                          Weapon::Socket::one_hand, Skill_type::axe};
    } axes;

    struct daggers_t
    {
        Weapon core_hound_tooth{"core_hound_tooth", 1.6, {57, 107}, Stats{0, 0}, Special_stats{1, 0, 20},
                                Weapon::Socket::one_hand, Skill_type::dagger};
        Weapon aq_deaths_sting{"aq_deaths_sting", 1.8, {95, 144}, Stats{0, 0}, Special_stats{0, 0, 38},
                               Weapon::Socket::one_hand, Skill_type::dagger};
    } daggers;

    struct maces_t
    {
        Weapon spineshatter{"spineshatter", 2.5, {99.0, 184.0}, Stats{9.0, 0.0}, Special_stats{0.0, 0.0, 0.0},
                            Weapon::Socket::main_hand, Skill_type::mace};
        Weapon stormstike_hammer{"stormstike_hammer", 2.7, {80, 150}, Stats{15, 0}, Special_stats{0, 0, 0},
                                 Weapon::Socket::one_hand, Skill_type::mace};
        Weapon aq_sand_polished_hammer{"aq_sand_polished_hammer", 2.6, {97, 181}, Stats{0, 0}, Special_stats{1, 0, 20},
                                       Weapon::Socket::one_hand, Skill_type::mace};
        Weapon aq_anubisath_warhammer{"aq_anubisath_warhammer", 1.8, {66, 123}, Stats{0, 0}, Special_stats{0, 0, 32},
                                      Weapon::Socket::one_hand, Skill_type::mace};
    } maces;

    struct fists_t
    {
        Weapon claw_of_the_black_drake{"claw_of_the_black_drake", 2.6, {102, 191}, Stats{13, 0}, Special_stats{1, 0, 0},
                                       Weapon::Socket::main_hand, Skill_type::none};
    } fists;

    struct set_bonuses_t
    {
        Set_bonus devilsaur_set_bonus{"devilsaur_set_bonus", Stats{0, 0}, Special_stats{0, 2, 0}, 2,
                                      Set_name::devilsaur};
        Set_bonus black_dragonscale_bonus2{"black_dragonscale_bonus2", Stats{0, 0}, Special_stats{0, 1, 0}, 2,
                                           Set_name::black_dragonscale};
        Set_bonus black_dragonscale_bonus3{"black_dragonscale_bonus3", Stats{0, 0}, Special_stats{2, 0, 0}, 3,
                                           Set_name::black_dragonscale};
        Set_bonus rare_pvp_set_bonus_1{"rare_pvp_set_bonus_1", Stats{0, 0}, Special_stats{0, 0, 40}, 2,
                                       Set_name::rare_pvp_set};
    } set_bonuses;

    void set_extra_item_properties()
    {
        trinket.hand_of_justice.set_chance_for_extra_hit(2);
        hands.edgemasters_handguards.set_bonus_skill(Extra_skill{Skill_type::all, 7});
        swords.maladath.set_bonus_skill(Extra_skill{Skill_type::sword, 4});
        helmet.expert_goldminers_head.set_bonus_skill(Extra_skill{Skill_type::axe, 7});
        belt.mugglers_belt.set_bonus_skill(Extra_skill{Skill_type::dagger, 5});
        daggers.aq_deaths_sting.set_bonus_skill(Extra_skill{Skill_type::dagger, 3});
        maces.aq_anubisath_warhammer.set_bonus_skill(Extra_skill{Skill_type::mace, 4});
        hands.aged_core_leather_gloves.set_bonus_skill(Extra_skill{Skill_type::dagger, 5});


    }

    auto get_set_bonuses() const
    {
        return set_bonuses;
    }

};

#endif //WOW_SIMULATOR_ARMORY_HPP
