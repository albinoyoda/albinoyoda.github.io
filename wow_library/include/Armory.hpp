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
    World_buff rallying_cry{"rallying_cry", Attributes{0.0, 0.0}, Special_stats{5.0, 0.0, 140}};
    World_buff dire_maul{"dire_maul", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 200}};
    World_buff songflower{"songflower", Attributes{15, 15}, Special_stats{5, 0, 0}};

    // Player_buffs
    Player_buff blessing_of_kings{"blessing_of_kings", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Player_buff blessing_of_might{"blessing_of_might", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 155}};
    Player_buff gift_of_the_wild{"gift_of_the_wild", Attributes{12.0, 12.0}, Special_stats{0.0, 0.0, 0.0}};
    Player_buff trueshot_aura{"trueshot_aura", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 100}};

    // Consumables
    Consumable elixir_mongoose{"elixir_mongoose", Attributes{0.0, 25.0}, Special_stats{2.0, 0.0, 0.0}};
    Consumable dense_stone_mh{"dense_stone_mh", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Consumable dense_stone_oh{"dense_stone_oh", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Consumable elemental_stone_mh{"elemental_stone_mh", Attributes{0.0, 0.0}, Special_stats{2.0, 0.0, 0.0}};
    Consumable elemental_stone_oh{"elemental_stone_oh", Attributes{0.0, 0.0}, Special_stats{2.0, 0.0, 0.0}};
    Consumable blessed_sunfruit{"blessed_sunfruit", Attributes{10.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Consumable juju_power{"juju_power", Attributes{30.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Consumable juju_might{"juju_might", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 40}};
    Consumable roids{"roids", Attributes{25.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};

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
        Armor lionheart_helm{"lionheart_helm", Attributes{18, 0}, Special_stats{2, 2, 0}, Armor::Socket::head};
        Armor expert_goldminers_head{"expert_goldminers_head", Attributes{0, 5}, Special_stats{0, 0, 0},
                                     Armor::Socket::head};
        Armor mask_of_the_unforgiven{"mask_of_the_unforgiven", Attributes{0, 0}, Special_stats{1, 2, 0},
                                     Armor::Socket::head};
        Armor crown_of_destruction{"crown_of_destruction", Attributes{0, 0}, Special_stats{2, 0, 44}, Armor::Socket::head};
        Armor helm_of_endless_rage{"helm_of_endless_rage", Attributes{26, 26}, Special_stats{0, 0, 0}, Armor::Socket::head};


    } helmet;

    struct neck_t
    {
        Armor onyxia_tooth_pendant{"onyxia_tooth_pendant", Attributes{0, 13}, Special_stats{1, 1, 0}, Armor::Socket::neck};
        Armor prestors_talisman_of_connivery{"prestors_talisman_of_connivery", Attributes{0, 30}, Special_stats{0, 1, 0},
                                             Armor::Socket::neck};
        Armor aq_barbed_choker{"aq_barbed_choker", Attributes{0, 0}, Special_stats{1, 0, 44},
                               Armor::Socket::neck};
    } neck;

    struct shoulder_t
    {
        Armor black_dragonscale_shoulders{"black_dragonscale_shoulders", Attributes{0, 0}, Special_stats{0, 0, 40},
                                          Armor::Socket::shoulder, Set::black_dragonscale};
        Armor truestrike_shoulders{"truestrike_shoulders", Attributes{0, 0}, Special_stats{0, 2, 24},
                                   Armor::Socket::shoulder};
        Armor drake_talon_pauldrons{"drake_talon_pauldrons", Attributes{20, 20}, Special_stats{0, 0, 0},
                                    Armor::Socket::shoulder};
        Armor leutenant_shoulders{"leutenant_shoulders", Attributes{17, 0}, Special_stats{1, 0, 0}, Armor::Socket::shoulder,
                                  Set::rare_pvp_set};
        Armor spaulders_of_valor{"spaulders_of_valor", Attributes{11, 9}, Special_stats{0, 0, 0}, Armor::Socket::shoulder};
        Armor aq_chitinous_shoulderguards{"aq_chitinous_shoulderguards", Attributes{11, 25}, Special_stats{0, 0, 0},
                                          Armor::Socket::shoulder};
        Armor aq_mantle_of_wicked_revenge{"aq_mantle_of_wicked_revenge", Attributes{16, 30}, Special_stats{0, 0, 0},
                                          Armor::Socket::shoulder};
        Armor aq_conquerors_spaulders{"aq_conquerors_spaulders", Attributes{20, 16}, Special_stats{0, 1, 0},
                                      Armor::Socket::shoulder};
    } shoulder;

    struct back_t
    {
        Armor cape_of_the_black_baron{"cape_of_the_black_baron", Attributes{0, 15}, Special_stats{0, 0, 20},
                                      Armor::Socket::back};
        Armor cloak_of_firemaw{"cloak_of_firemaw", Attributes{0, 0}, Special_stats{0, 0, 50}, Armor::Socket::back};
        Armor cloak_of_draconic_might{"cloak_of_draconic_might", Attributes{16, 16}, Special_stats{0, 0, 0},
                                      Armor::Socket::back};
        Armor aq_cloak_of_concentrated_hate{"aq_cloak_of_concentrated_hate", Attributes{11, 16}, Special_stats{0, 1, 0},
                                            Armor::Socket::back};
    } back;

    struct chest_t
    {
        Armor malfurions_blessed_bulwark{"malfurions_blessed_bulwark", Attributes{40, 0}, Special_stats{0, 0, 0},
                                         Armor::Socket::chest};
        Armor savage_gladiator_chain{"savage_gladiator_chain", Attributes{13, 14}, Special_stats{2, 0, 0},
                                     Armor::Socket::chest};
        Armor knight_captains_plate_hauberk{"knight_captains_plate_hauberk", Attributes{21, 0}, Special_stats{1, 0, 0},
                                            Armor::Socket::chest, Set::rare_pvp_set};
        Armor tombstone_breastplate{"tombstone_breastplate", Attributes{10, 10}, Special_stats{2, 0, 0},
                                    Armor::Socket::chest};
        Armor cadaverous_armor{"cadaverous_armor", Attributes{8, 8}, Special_stats{0, 0, 60}, Armor::Socket::chest};
        Armor aq_breatplate_of_annihilation{"aq_breatplate_of_annihilation", Attributes{37, 0}, Special_stats{1, 1, 0},
                                            Armor::Socket::chest};
        Armor aq_vest_of_swift_execution{"aq_vest_of_swift_execution", Attributes{21, 41}, Special_stats{0, 0, 0},
                                         Armor::Socket::chest};
        Armor aq_conquerors_breastplate{"aq_conquerors_breastplate", Attributes{34, 24}, Special_stats{0, 0, 0},
                                        Armor::Socket::chest};
    } chest;

    struct wrists_t
    {
        Armor wristguards_of_stability{"wristguards_of_stability", Attributes{24, 0}, Special_stats{0, 0, 0},
                                       Armor::Socket::wrists};
        Armor vambraces_of_the_sadist{"vambraces_of_the_sadist", Attributes{6, 0}, Special_stats{1, 0, 0},
                                      Armor::Socket::wrists};
        Armor wristhuards_of_true_flight{"wristhuards_of_true_flight", Attributes{0, 19}, Special_stats{0, 1, 0},
                                         Armor::Socket::wrists};
        Armor battleborn_armbraces{"battleborn_armbraces", Attributes{0, 0}, Special_stats{1, 1, 0}, Armor::Socket::wrists};
        Armor aq_bracers_of_brutality{"aq_bracers_of_brutality", Attributes{21, 12}, Special_stats{0, 0, 0},
                                      Armor::Socket::wrists};
        Armor aq_hive_defiler_wristguards{"aq_hive_defiler_wristguards", Attributes{23, 18}, Special_stats{0, 0, 0},
                                          Armor::Socket::wrists};
        Armor aq_quiraji_execution_bracers{"aq_quiraji_execution_bracers", Attributes{15, 16}, Special_stats{0, 1, 0},
                                           Armor::Socket::wrists};
    } wrist;

    struct hands_t
    {
        Armor devilsaur_gauntlets{"devilsaur_gauntlets", Attributes{0, 0}, Special_stats{1, 0, 28}, Armor::Socket::hands,
                                  Set::devilsaur};
        Armor edgemasters_handguards{"edgemasters_handguards", Attributes{0, 0}, Special_stats{0, 0, 0},
                                     Armor::Socket::hands};
        Armor flameguard_gauntlets{"flameguard_gauntlets", Attributes{0, 0}, Special_stats{1, 0, 54}, Armor::Socket::hands};
        Armor aq_gloves_of_enforcement{"aq_gloves_of_enforcement", Attributes{28, 20}, Special_stats{0, 1, 0},
                                       Armor::Socket::hands};
        Armor aged_core_leather_gloves{"aged_core_leather_gloves", Attributes{15, 0}, Special_stats{1, 0, 0},
                                       Armor::Socket::hands};
        Armor aq_gauntlets_of_annihilation{"aq_gauntlets_of_annihilation", Attributes{35, 0}, Special_stats{1, 1, 0},
                                           Armor::Socket::hands};
    } hands;

    struct belt_t
    {
        Armor onslaught_girdle{"onslaught_girdle", Attributes{31, 0}, Special_stats{1, 1, 0}, Armor::Socket::belt};
        Armor mugglers_belt{"mugglers_belt", Attributes{0, 0}, Special_stats{1, 0, 0}, Armor::Socket::belt};
        Armor omokks_girth{"omokks_girth", Attributes{15, 0}, Special_stats{1, 0, 0}, Armor::Socket::belt};
        Armor girdle_of_the_fallen_crusader{"girdle_of_the_fallen_crusader", Attributes{20, 10}, Special_stats{0, 0, 0},
                                            Armor::Socket::belt};
    } belt;

    struct legs_t
    {
        Armor devilsaur_leggings{"devilsaur_leggings", Attributes{0, 0}, Special_stats{1, 0, 46}, Armor::Socket::legs,
                                 Set::devilsaur};
        Armor black_dragonscale_leggings{"black_dragonscale_leggings", Attributes{0, 0}, Special_stats{2, 1, 54},
                                         Armor::Socket::legs, Set::black_dragonscale};
        Armor eldritch_legplates{"eldritch_legplates", Attributes{15, 9}, Special_stats{1, 0, 0}, Armor::Socket::legs};
        Armor ubrs_legs{"ubrs_legs", Attributes{29, 0}, Special_stats{0, 0, 0}, Armor::Socket::legs};
        Armor cloudkeeper_legplaters{"cloudkeeper_legplaters", Attributes{20, 20}, Special_stats{0, 0, 100},
                                     Armor::Socket::legs};
        Armor legguards_of_the_fallen_crusader{"legguards_of_the_fallen_crusader", Attributes{28, 22},
                                               Special_stats{0, 0, 0}, Armor::Socket::legs};
        Armor knight_captain_plate_leggings{"knight_captain_plate_leggings", Attributes{12, 0},
                                            Special_stats{2, 0, 0}, Armor::Socket::legs, Set::rare_pvp_set};

        Armor aq_legplates_of_the_qiraji_command{"aq_legplates_of_the_qiraji_command", Attributes{20, 0},
                                                 Special_stats{2, 0, 0}, Armor::Socket::legs};
        Armor aq_scaled_sand_reaver_leggings{"aq_scaled_sand_reaver_leggings", Attributes{0, 0},
                                             Special_stats{2, 0, 62}, Armor::Socket::legs};
        Armor aq_conquerors_legguards{"aq_conquerors_legguards", Attributes{33, 21}, Special_stats{0, 1, 0},
                                      Armor::Socket::legs};
        Armor titanic_leggings{"titanic_leggings", Attributes{30, 0}, Special_stats{1, 2, 0},
                               Armor::Socket::legs};
    } legs;

    struct boots_t
    {
        Armor windreaver_greaves{"windreaver_greaves", Attributes{0, 20}, Special_stats{0, 1, 0}, Armor::Socket::boots};
        Armor bloodmail_boots{"bloodmail_boots", Attributes{9, 9}, Special_stats{0, 1, 0}, Armor::Socket::boots};
        Armor black_dragonscale_boots{"black_dragonscale_boots", Attributes{0, 0}, Special_stats{0, 0, 28},
                                      Armor::Socket::boots, Set::black_dragonscale};
        Armor knight_leutenants_greaves{"knight_leutenants_greaves", Attributes{10, 9}, Special_stats{0, 0, 40},
                                        Armor::Socket::boots, Set::rare_pvp_set};
        Armor battlechasers{"battlechasers", Attributes{14, 13}, Special_stats{0, 0, 0}, Armor::Socket::boots};
        Armor chromatic_boots{"chromatic_boots", Attributes{20, 20}, Special_stats{0, 1, 0}, Armor::Socket::boots};
        Armor boots_of_shadow_flame{"boots_of_shadow_flame", Attributes{0, 0}, Special_stats{0, 2, 44},
                                    Armor::Socket::boots};
        Armor aq_boots_of_the_vanguard{"aq_boots_of_the_vanguard", Attributes{22, 22}, Special_stats{0, 0, 0},
                                       Armor::Socket::boots};
    } boots;

    struct rings_t
    {
        Armor don_julios_band{"don_julios_band", Attributes{0, 0}, Special_stats{1, 1, 16}, Armor::Socket::ring};
        Armor magnis_will{"magnis_will", Attributes{6, 0}, Special_stats{1, 0, 0}, Armor::Socket::ring};
        Armor quick_strike_ring{"quick_strike_ring", Attributes{8, 0}, Special_stats{1, 0, 30}, Armor::Socket::ring};
        Armor master_dragonslayers_ring{"master_dragonslayers_ring", Attributes{0, 0}, Special_stats{0, 1, 48},
                                        Armor::Socket::ring};
        Armor circle_of_applied_force{"circle_of_applied_force", Attributes{12, 22}, Special_stats{0, 0, 0},
                                      Armor::Socket::ring};
        Armor blackstone_ring{"blackstone_ring", Attributes{0, 0}, Special_stats{0, 1, 20}, Armor::Socket::ring};
        Armor tarnished_elven{"tarnished_elven", Attributes{0, 15}, Special_stats{0, 1, 0}, Armor::Socket::ring};
    } rings;

    struct trinket_t
    {
        Armor hand_of_justice{"hand_of_justice", Attributes{0, 0}, Special_stats{0, 0, 20}, Armor::Socket::trinket};
        Armor diamond_flask{"diamond_flask", Attributes{75, 0}, Special_stats{0, 0, 0}, Armor::Socket::trinket};
        Armor blackhands_breadth{"blackhands_breadth", Attributes{0, 0}, Special_stats{2, 0, 0}, Armor::Socket::trinket};
        Armor drake_fang_talisman{"drake_fang_talisman", Attributes{0, 0}, Special_stats{0, 2, 56}, Armor::Socket::trinket};
    } trinket;

    struct ranged_t
    {
        Armor satyrs_bow{"satyrs_bow", Attributes{0, 3}, Special_stats{0, 1, 0}, Armor::Socket::ranged};
        Armor precisely_calibrated_boomstick{"precisely_calibrated_boomstick", Attributes{0, 14}, Special_stats{0, 0, 0},
                                             Armor::Socket::ranged};
        Armor strikers_mark{"strikers_mark", Attributes{0, 0}, Special_stats{0, 1, 22}, Armor::Socket::ranged};
        Armor heartstriker{"heartstriker", Attributes{0, 0}, Special_stats{0, 0, 24}, Armor::Socket::ranged};
        Armor blastershot{"blastershot", Attributes{0, 0}, Special_stats{1, 0, 0}, Armor::Socket::ranged};
        Armor dragonbreath_hand_cannon{"dragonbreath_hand_cannon", Attributes{0, 14}, Special_stats{0, 0, 0},
                                       Armor::Socket::ranged};
        Armor aq_crossbow_of_imminent_doom{"aq_crossbow_of_imminent_doom", Attributes{5, 7}, Special_stats{0, 1, 0},
                                           Armor::Socket::ranged};
        Armor aq_larvae_of_the_great_worm{"aq_larvae_of_the_great_worm", Attributes{0, 0}, Special_stats{1, 0, 18},
                                          Armor::Socket::ranged};
    } ranged;

    // ---------------- Weapons ------------------
    struct swords_t
    {
        Weapon maladath{"maladath", 2.2, {86.0, 162.0}, Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0},
                        Weapon::Socket::one_hand, Skill_type::sword};
        Weapon chromatically_tempered_sword{"chromatically_tempered_sword", 2.6, {106.0, 198.0}, Attributes{14.0, 14.0},
                                            Special_stats{0.0, 0.0, 0.0}, Weapon::Socket::one_hand, Skill_type::sword};
        Weapon brutality_blade{"brutality_blade", 2.5, {90, 168}, Attributes{9, 9}, Special_stats{1, 0, 0},
                               Weapon::Socket::one_hand, Skill_type::sword};
        Weapon assassination_blade{"assassination_blade", 2.7, {71, 132}, Attributes{3, 0}, Special_stats{1, 0, 0},
                                   Weapon::Socket::one_hand, Skill_type::sword};
        Weapon mirahs_song{"mirahs_song", 1.8, {57, 87}, Attributes{9, 9}, Special_stats{0, 0, 0}, Weapon::Socket::one_hand,
                           Skill_type::sword};
        Weapon viskag{"viskag", 2.6, {100, 187}, Attributes{0, 0}, Special_stats{0, 0, 0}, Weapon::Socket::one_hand,
                      Skill_type::sword};
        Weapon quel_serrar{"quel_serrar", 2.0, {84, 126}, Attributes{0, 0}, Special_stats{0, 0, 0}, Weapon::Socket::one_hand,
                           Skill_type::sword};
        Weapon aq_ancient_quiraj_ripper{"aq_ancient_quiraj_ripper", 2.8, {114, 213}, Attributes{0, 0},
                                        Special_stats{1, 0, 20}, Weapon::Socket::one_hand, Skill_type::sword};
    } swords;

    struct axes_t
    {
        Weapon deathbringer{"deathbringer", 2.9, {114, 213}, Attributes{0, 0}, Special_stats{0, 0, 0},
                            Weapon::Socket::one_hand, Skill_type::axe};
        Weapon crul_shorukh_edge_of_chaos{"crul_shorukh_edge_of_chaos", 2.3, {101.0, 188.0}, Attributes{0.0, 0.0},
                                          Special_stats{0.0, 0.0, 36.0}, Weapon::Socket::one_hand, Skill_type::axe};
        Weapon dooms_edge{"dooms_edge", 2.3, {83.0, 154.0}, Attributes{9.0, 16.0}, Special_stats{0.0, 0.0, 0.0},
                          Weapon::Socket::one_hand, Skill_type::axe};
    } axes;

    struct daggers_t
    {
        Weapon core_hound_tooth{"core_hound_tooth", 1.6, {57, 107}, Attributes{0, 0}, Special_stats{1, 0, 20},
                                Weapon::Socket::one_hand, Skill_type::dagger};
        Weapon aq_deaths_sting{"aq_deaths_sting", 1.8, {95, 144}, Attributes{0, 0}, Special_stats{0, 0, 38},
                               Weapon::Socket::one_hand, Skill_type::dagger};
    } daggers;

    struct maces_t
    {
        Weapon spineshatter{"spineshatter", 2.5, {99.0, 184.0}, Attributes{9.0, 0.0}, Special_stats{0.0, 0.0, 0.0},
                            Weapon::Socket::main_hand, Skill_type::mace};
        Weapon stormstike_hammer{"stormstike_hammer", 2.7, {80, 150}, Attributes{15, 0}, Special_stats{0, 0, 0},
                                 Weapon::Socket::one_hand, Skill_type::mace};
        Weapon aq_sand_polished_hammer{"aq_sand_polished_hammer", 2.6, {97, 181}, Attributes{0, 0}, Special_stats{1, 0, 20},
                                       Weapon::Socket::one_hand, Skill_type::mace};
        Weapon aq_anubisath_warhammer{"aq_anubisath_warhammer", 1.8, {66, 123}, Attributes{0, 0}, Special_stats{0, 0, 32},
                                      Weapon::Socket::one_hand, Skill_type::mace};
    } maces;

    struct fists_t
    {
        Weapon claw_of_the_black_drake{"claw_of_the_black_drake", 2.6, {102, 191}, Attributes{13, 0}, Special_stats{1, 0, 0},
                                       Weapon::Socket::main_hand, Skill_type::none};
    } fists;

    struct set_bonuses_t
    {
        Set_bonus devilsaur_set_bonus{"devilsaur_set_bonus", Attributes{0, 0}, Special_stats{0, 2, 0}, 2,
                                      Set::devilsaur};
        Set_bonus black_dragonscale_bonus2{"black_dragonscale_bonus2", Attributes{0, 0}, Special_stats{0, 1, 0}, 2,
                                           Set::black_dragonscale};
        Set_bonus black_dragonscale_bonus3{"black_dragonscale_bonus3", Attributes{0, 0}, Special_stats{2, 0, 0}, 3,
                                           Set::black_dragonscale};
        Set_bonus rare_pvp_set_bonus_1{"rare_pvp_set_bonus_1", Attributes{0, 0}, Special_stats{0, 0, 40}, 2,
                                       Set::rare_pvp_set};
    } set_bonuses;

    void set_extra_item_properties()
    {
        trinket.hand_of_justice.set_chance_for_extra_hit(2);
        hands.edgemasters_handguards.set_bonus_skill(Bonus_skill{Skill_type::all, 7});
        swords.maladath.set_bonus_skill(Bonus_skill{Skill_type::sword, 4});
        helmet.expert_goldminers_head.set_bonus_skill(Bonus_skill{Skill_type::axe, 7});
        belt.mugglers_belt.set_bonus_skill(Bonus_skill{Skill_type::dagger, 5});
        daggers.aq_deaths_sting.set_bonus_skill(Bonus_skill{Skill_type::dagger, 3});
        maces.aq_anubisath_warhammer.set_bonus_skill(Bonus_skill{Skill_type::mace, 4});
        hands.aged_core_leather_gloves.set_bonus_skill(Bonus_skill{Skill_type::dagger, 5});


    }

    auto get_set_bonuses() const
    {
        return set_bonuses;
    }

};

#endif //WOW_SIMULATOR_ARMORY_HPP
