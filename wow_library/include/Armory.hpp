#ifndef WOW_SIMULATOR_ARMORY_HPP
#define WOW_SIMULATOR_ARMORY_HPP

#include "Character.hpp"

struct Buffs
{
    // World buffs
    Buff rallying_cry{"rallying_cry", Attributes{0.0, 0.0}, Special_stats{5.0, 0.0, 140}};
    Buff dire_maul{"dire_maul", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 200}};
    Buff songflower{"songflower", Attributes{15, 15}, Special_stats{5, 0, 0}};

    // Player_buffs
    Buff blessing_of_kings{"blessing_of_kings", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, .10};
    Buff blessing_of_might{"blessing_of_might", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 155}};
    Buff gift_of_the_wild{"gift_of_the_wild", Attributes{12.0, 12.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff trueshot_aura{"trueshot_aura", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 100}};

    // Consumables
    Buff elixir_mongoose{"elixir_mongoose", Attributes{0.0, 25.0}, Special_stats{2.0, 0.0, 0.0}};
    Buff blessed_sunfruit{"blessed_sunfruit", Attributes{10.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff juju_power{"juju_power", Attributes{30.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff juju_might{"juju_might", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 40}};
    Buff roids{"roids", Attributes{25.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};

    Weapon_buff dense_stone{"dense_stone", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, 0, 8};
    Buff elemental_stone{"elemental_stone", Attributes{0.0, 0.0}, Special_stats{2.0, 0.0, 0.0}};
};

struct Armory
{
    struct helmet_t
    {
        Armor lionheart_helm{"lionheart_helm", Attributes{18, 0}, Special_stats{2, 2, 0}, Socket::head};
        Armor expert_goldminers_head{"expert_goldminers_head", Attributes{0, 5}, Special_stats{0, 0, 0, 0, 0, 0, 7},
                                     Socket::head};
        Armor mask_of_the_unforgiven{"mask_of_the_unforgiven", Attributes{0, 0}, Special_stats{1, 2, 0},
                                     Socket::head};
        Armor crown_of_destruction{"crown_of_destruction", Attributes{0, 0}, Special_stats{2, 0, 44},
                                   Socket::head};
        Armor helm_of_endless_rage{"helm_of_endless_rage", Attributes{26, 26}, Special_stats{0, 0, 0},
                                   Socket::head};


    } helmet;

    struct neck_t
    {
        Armor onyxia_tooth_pendant{"onyxia_tooth_pendant", Attributes{0, 13}, Special_stats{1, 1, 0},
                                   Socket::neck};
        Armor prestors_talisman_of_connivery{"prestors_talisman_of_connivery", Attributes{0, 30},
                                             Special_stats{0, 1, 0},
                                             Socket::neck};
        Armor aq_barbed_choker{"aq_barbed_choker", Attributes{0, 0}, Special_stats{1, 0, 44},
                               Socket::neck};
        Armor mark_of_fordring{"mark_of_fordring", Attributes{0, 0}, Special_stats{1, 0, 26},
                               Socket::neck};
    } neck;

    struct shoulder_t
    {
        Armor black_dragonscale_shoulders{"black_dragonscale_shoulders", Attributes{0, 0}, Special_stats{0, 0, 40},
                                          Socket::shoulder, Set::black_dragonscale};
        Armor truestrike_shoulders{"truestrike_shoulders", Attributes{0, 0}, Special_stats{0, 2, 24},
                                   Socket::shoulder};
        Armor drake_talon_pauldrons{"drake_talon_pauldrons", Attributes{20, 20}, Special_stats{0, 0, 0},
                                    Socket::shoulder};
        Armor leutenant_shoulders{"leutenant_shoulders", Attributes{17, 0}, Special_stats{1, 0, 0},
                                  Socket::shoulder,
                                  Set::rare_pvp_set};
        Armor spaulders_of_valor{"spaulders_of_valor", Attributes{11, 9}, Special_stats{0, 0, 0},
                                 Socket::shoulder};
        Armor aq_chitinous_shoulderguards{"aq_chitinous_shoulderguards", Attributes{11, 25}, Special_stats{0, 0, 0},
                                          Socket::shoulder};
        Armor aq_mantle_of_wicked_revenge{"aq_mantle_of_wicked_revenge", Attributes{16, 30}, Special_stats{0, 0, 0},
                                          Socket::shoulder};
        Armor aq_conquerors_spaulders{"aq_conquerors_spaulders", Attributes{20, 16}, Special_stats{0, 1, 0},
                                      Socket::shoulder};
    } shoulder;

    struct back_t
    {
        Armor cape_of_the_black_baron{"cape_of_the_black_baron", Attributes{0, 15}, Special_stats{0, 0, 20},
                                      Socket::back};
        Armor cloak_of_firemaw{"cloak_of_firemaw", Attributes{0, 0}, Special_stats{0, 0, 50}, Socket::back};
        Armor cloak_of_draconic_might{"cloak_of_draconic_might", Attributes{16, 16}, Special_stats{0, 0, 0},
                                      Socket::back};
        Armor aq_cloak_of_concentrated_hate{"aq_cloak_of_concentrated_hate", Attributes{11, 16}, Special_stats{0, 1, 0},
                                            Socket::back};
    } back;

    struct chest_t
    {
        Armor malfurions_blessed_bulwark{"malfurions_blessed_bulwark", Attributes{40, 0}, Special_stats{0, 0, 0},
                                         Socket::chest};
        Armor savage_gladiator_chain{"savage_gladiator_chain", Attributes{13, 14}, Special_stats{2, 0, 0},
                                     Socket::chest};
        Armor knight_captains_plate_hauberk{"knight_captains_plate_hauberk", Attributes{21, 0}, Special_stats{1, 0, 0},
                                            Socket::chest, Set::rare_pvp_set};
        Armor tombstone_breastplate{"tombstone_breastplate", Attributes{10, 10}, Special_stats{2, 0, 0},
                                    Socket::chest};
        Armor cadaverous_armor{"cadaverous_armor", Attributes{8, 8}, Special_stats{0, 0, 60}, Socket::chest};
        Armor aq_breatplate_of_annihilation{"aq_breatplate_of_annihilation", Attributes{37, 0}, Special_stats{1, 1, 0},
                                            Socket::chest};
        Armor aq_vest_of_swift_execution{"aq_vest_of_swift_execution", Attributes{21, 41}, Special_stats{0, 0, 0},
                                         Socket::chest};
        Armor aq_conquerors_breastplate{"aq_conquerors_breastplate", Attributes{34, 24}, Special_stats{0, 0, 0},
                                        Socket::chest};
        Armor black_dragonscale_chest{"black_dragonscale_chest", Attributes{0, 0}, Special_stats{0, 0, 50},
                                      Socket::chest, Set::black_dragonscale};
        Armor breastplate_of_bloodthirst{"breastplate_of_bloodthirst", Attributes{10, 0}, Special_stats{2, 0, 0},
                                         Socket::chest};
    } chest;

    struct wrists_t
    {
        Armor wristguards_of_stability{"wristguards_of_stability", Attributes{24, 0}, Special_stats{0, 0, 0},
                                       Socket::wrists};
        Armor vambraces_of_the_sadist{"vambraces_of_the_sadist", Attributes{6, 0}, Special_stats{1, 0, 0},
                                      Socket::wrists};
        Armor wristhuards_of_true_flight{"wristhuards_of_true_flight", Attributes{0, 19}, Special_stats{0, 1, 0},
                                         Socket::wrists};
        Armor battleborn_armbraces{"battleborn_armbraces", Attributes{0, 0}, Special_stats{1, 1, 0},
                                   Socket::wrists};
        Armor aq_bracers_of_brutality{"aq_bracers_of_brutality", Attributes{21, 12}, Special_stats{0, 0, 0},
                                      Socket::wrists};
        Armor aq_hive_defiler_wristguards{"aq_hive_defiler_wristguards", Attributes{23, 18}, Special_stats{0, 0, 0},
                                          Socket::wrists};
        Armor aq_quiraji_execution_bracers{"aq_quiraji_execution_bracers", Attributes{15, 16}, Special_stats{0, 1, 0},
                                           Socket::wrists};
    } wrist;

    struct hands_t
    {
        Armor devilsaur_gauntlets{"devilsaur_gauntlets", Attributes{0, 0}, Special_stats{1, 0, 28},
                                  Socket::hands,
                                  Set::devilsaur};

        Armor edgemasters_handguards{"edgemasters_handguards", Attributes{0, 0},
                                     Special_stats{0, 0, 0, 0, 0, 7, 7, 7}, Socket::hands};
        Armor flameguard_gauntlets{"flameguard_gauntlets", Attributes{0, 0}, Special_stats{1, 0, 54},
                                   Socket::hands};
        Armor aq_gloves_of_enforcement{"aq_gloves_of_enforcement", Attributes{28, 20}, Special_stats{0, 1, 0},
                                       Socket::hands};
        Armor aged_core_leather_gloves{"aged_core_leather_gloves", Attributes{15, 0},
                                       Special_stats{1, 0, 0, 0, 0, 0, 0, 5},
                                       Socket::hands};
        Armor aq_gauntlets_of_annihilation{"aq_gauntlets_of_annihilation", Attributes{35, 0}, Special_stats{1, 1, 0},
                                           Socket::hands};
    } hands;

    struct belt_t
    {
        Armor onslaught_girdle{"onslaught_girdle", Attributes{31, 0}, Special_stats{1, 1, 0}, Socket::belt};
        Armor mugglers_belt{"mugglers_belt", Attributes{0, 0}, Special_stats{1, 0, 0, 0, 0, 0, 0, 5}, Socket::belt};
        Armor omokks_girth{"omokks_girth", Attributes{15, 0}, Special_stats{1, 0, 0}, Socket::belt};
        Armor girdle_of_the_fallen_crusader{"girdle_of_the_fallen_crusader", Attributes{20, 10}, Special_stats{0, 0, 0},
                                            Socket::belt};
    } belt;

    struct legs_t
    {
        Armor devilsaur_leggings{"devilsaur_leggings", Attributes{0, 0}, Special_stats{1, 0, 46}, Socket::legs,
                                 Set::devilsaur};
        Armor black_dragonscale_leggings{"black_dragonscale_leggings", Attributes{0, 0}, Special_stats{0, 0, 54},
                                         Socket::legs, Set::black_dragonscale};
        Armor eldritch_legplates{"eldritch_legplates", Attributes{15, 9}, Special_stats{1, 0, 0}, Socket::legs};
        Armor ubrs_legs{"ubrs_legs", Attributes{29, 0}, Special_stats{0, 0, 0}, Socket::legs};
        Armor cloudkeeper_legplaters{"cloudkeeper_legplaters", Attributes{20, 20}, Special_stats{0, 0, 100},
                                     Socket::legs};
        Armor legguards_of_the_fallen_crusader{"legguards_of_the_fallen_crusader", Attributes{28, 22},
                                               Special_stats{0, 0, 0}, Socket::legs};
        Armor knight_captain_plate_leggings{"knight_captain_plate_leggings", Attributes{12, 0},
                                            Special_stats{2, 0, 0}, Socket::legs, Set::rare_pvp_set};
        Armor aq_legplates_of_the_qiraji_command{"aq_legplates_of_the_qiraji_command", Attributes{20, 0},
                                                 Special_stats{2, 0, 0}, Socket::legs};
        Armor aq_scaled_sand_reaver_leggings{"aq_scaled_sand_reaver_leggings", Attributes{0, 0},
                                             Special_stats{2, 0, 62}, Socket::legs};
        Armor aq_conquerors_legguards{"aq_conquerors_legguards", Attributes{33, 21}, Special_stats{0, 1, 0},
                                      Socket::legs};
        Armor titanic_leggings{"titanic_leggings", Attributes{30, 0}, Special_stats{1, 2, 0},
                               Socket::legs};
    } legs;

    struct boots_t
    {
        Armor windreaver_greaves{"windreaver_greaves", Attributes{0, 20}, Special_stats{0, 1, 0}, Socket::boots};
        Armor bloodmail_boots{"bloodmail_boots", Attributes{9, 9}, Special_stats{0, 1, 0}, Socket::boots};
        Armor black_dragonscale_boots{"black_dragonscale_boots", Attributes{0, 0}, Special_stats{0, 0, 28},
                                      Socket::boots, Set::black_dragonscale};
        Armor knight_leutenants_greaves{"knight_leutenants_greaves", Attributes{10, 9}, Special_stats{0, 0, 40},
                                        Socket::boots, Set::rare_pvp_set};
        Armor battlechasers{"battlechasers", Attributes{14, 13}, Special_stats{0, 0, 0}, Socket::boots};
        Armor chromatic_boots{"chromatic_boots", Attributes{20, 20}, Special_stats{0, 1, 0}, Socket::boots};
        Armor boots_of_shadow_flame{"boots_of_shadow_flame", Attributes{0, 0}, Special_stats{0, 2, 44},
                                    Socket::boots};
        Armor aq_boots_of_the_vanguard{"aq_boots_of_the_vanguard", Attributes{22, 22}, Special_stats{0, 0, 0},
                                       Socket::boots};
    } boots;

    struct rings_t
    {
        Armor don_julios_band{"don_julios_band", Attributes{0, 0}, Special_stats{1, 1, 16}, Socket::ring};
        Armor magnis_will{"magnis_will", Attributes{6, 0}, Special_stats{1, 0, 0}, Socket::ring};
        Armor quick_strike_ring{"quick_strike_ring", Attributes{8, 0}, Special_stats{1, 0, 30}, Socket::ring};
        Armor master_dragonslayers_ring{"master_dragonslayers_ring", Attributes{0, 0}, Special_stats{0, 1, 48},
                                        Socket::ring};
        Armor circle_of_applied_force{"circle_of_applied_force", Attributes{12, 22}, Special_stats{0, 0, 0},
                                      Socket::ring};
        Armor blackstone_ring{"blackstone_ring", Attributes{0, 0}, Special_stats{0, 1, 20}, Socket::ring};
        Armor tarnished_elven{"tarnished_elven", Attributes{0, 15}, Special_stats{0, 1, 0}, Socket::ring};
    } rings;

    struct trinket_t
    {
        Armor hand_of_justice{"hand_of_justice", Attributes{0, 0}, Special_stats{0, 0, 20, .02}, Socket::trinket};
        Armor diamond_flask{"diamond_flask", Attributes{75, 0}, Special_stats{0, 0, 0}, Socket::trinket};
        Armor blackhands_breadth{"blackhands_breadth", Attributes{0, 0}, Special_stats{2, 0, 0},
                                 Socket::trinket};
        Armor drake_fang_talisman{"drake_fang_talisman", Attributes{0, 0}, Special_stats{0, 2, 56},
                                  Socket::trinket};
    } trinket;

    struct ranged_t
    {
        Armor satyrs_bow{"satyrs_bow", Attributes{0, 3}, Special_stats{0, 1, 0}, Socket::ranged};
        Armor precisely_calibrated_boomstick{"precisely_calibrated_boomstick", Attributes{0, 14},
                                             Special_stats{0, 0, 0},
                                             Socket::ranged};
        Armor strikers_mark{"strikers_mark", Attributes{0, 0}, Special_stats{0, 1, 22}, Socket::ranged};
        Armor heartstriker{"heartstriker", Attributes{0, 0}, Special_stats{0, 0, 24}, Socket::ranged};
        Armor blastershot{"blastershot", Attributes{0, 0}, Special_stats{1, 0, 0}, Socket::ranged};
        Armor dragonbreath_hand_cannon{"dragonbreath_hand_cannon", Attributes{0, 14}, Special_stats{0, 0, 0},
                                       Socket::ranged};
        Armor aq_crossbow_of_imminent_doom{"aq_crossbow_of_imminent_doom", Attributes{5, 7}, Special_stats{0, 1, 0},
                                           Socket::ranged};
        Armor aq_larvae_of_the_great_worm{"aq_larvae_of_the_great_worm", Attributes{0, 0}, Special_stats{1, 0, 18},
                                          Socket::ranged};
        Armor bloodseeker{"bloodseeker", Attributes{8, 7}, Special_stats{0, 0, 0}, Socket::ranged};
        Armor riphook{"riphook", Attributes{0, 0}, Special_stats{0, 0, 22}, Socket::ranged};
    } ranged;

    // ---------------- Weapons ------------------
    struct swords_t
    {
        Weapon maladath{"maladath", Attributes{0.0, 0.0},
                        Special_stats{0.0, 0.0, 0.0, 0, 0, 4}, 2.2, 86.0, 162.0,
                        Weapon_socket::one_hand, Weapon_type::sword};
        Weapon chromatically_tempered_sword{"chromatically_tempered_sword", Attributes{14.0, 14.0},
                                            Special_stats{0.0, 0.0, 0.0}, 2.6, 106.0, 198.0, Weapon_socket::one_hand,
                                            Weapon_type::sword};
        Weapon brutality_blade{"brutality_blade", Attributes{9, 9}, Special_stats{1, 0, 0}, 2.5, 90, 168,
                               Weapon_socket::one_hand, Weapon_type::sword};
        Weapon thrash_blade{"thrash_blade", Attributes{0, 0}, Special_stats{0, 0, 0}, 2.7, 66, 124,
                            Weapon_socket::one_hand, Weapon_type::sword,
                            {{"thrash_blade", Hit_effect::Type::extra_hit, {}, {}, 0, 0, 0.038}}};
        Weapon assassination_blade{"assassination_blade", Attributes{3, 0}, Special_stats{1, 0, 0}, 2.7, 71, 132,
                                   Weapon_socket::one_hand, Weapon_type::sword};
        Weapon mirahs_song{"mirahs_song", Attributes{9, 9}, Special_stats{0, 0, 0}, 1.8, 57, 87,
                           Weapon_socket::one_hand, Weapon_type::sword};
        Weapon viskag{"viskag", Attributes{0, 0}, Special_stats{0, 0, 0}, 2.6, 100, 187, Weapon_socket::one_hand,
                      Weapon_type::sword, {{"viskag", Hit_effect::Type::damage_physical, {}, {}, 240, 0, 0.06}}};
        Weapon quel_serrar{"quel_serrar", Attributes{0, 0}, Special_stats{0, 0, 0}, 2.0, 84, 126,
                           Weapon_socket::one_hand, Weapon_type::sword};
        Weapon aq_ancient_quiraj_ripper{"aq_ancient_quiraj_ripper", Attributes{0, 0}, Special_stats{1, 0, 20}, 2.8, 114,
                                        213, Weapon_socket::one_hand, Weapon_type::sword};
        Weapon dal_rends_sacred_charge{"dal_rends_sacred_charge", Attributes{4, 0}, Special_stats{1, 0, 0}, 2.8, 81,
                                       151, Weapon_socket::main_hand, Weapon_type::sword, std::vector<Hit_effect>(),
                                       Set::dal_rends};
        Weapon dal_rends_tribal_guardian{"dal_rends_tribal_guardian", Attributes{0, 0}, Special_stats{0, 0, 0}, 1.8, 52,
                                         97, Weapon_socket::off_hand, Weapon_type::sword, std::vector<Hit_effect>(),
                                         Set::dal_rends};
    } swords;

    struct axes_t
    {
        Weapon deathbringer{"deathbringer", Attributes{0, 0}, Special_stats{0, 0, 0}, 2.9, 114, 213,
                            Weapon_socket::one_hand, Weapon_type::axe};
        Weapon crul_shorukh_edge_of_chaos{"crul_shorukh_edge_of_chaos", Attributes{0.0, 0.0},
                                          Special_stats{0.0, 0.0, 36.0}, 2.3, 101.0, 188.0, Weapon_socket::one_hand,
                                          Weapon_type::axe};
        Weapon dooms_edge{"dooms_edge", Attributes{9.0, 16.0}, Special_stats{0.0, 0.0, 0.0}, 2.3, 83.0, 154.0,
                          Weapon_socket::one_hand, Weapon_type::axe};
    } axes;

    struct daggers_t
    {
        Weapon core_hound_tooth{"core_hound_tooth", Attributes{0, 0}, Special_stats{1, 0, 20}, 1.6, 57, 107,
                                Weapon_socket::one_hand, Weapon_type::dagger};
        Weapon aq_deaths_sting{"aq_deaths_sting", Attributes{0, 0}, Special_stats{0, 0, 38, 0, 0, 0, 0, 3}, 1.8, 95,
                               144, Weapon_socket::one_hand, Weapon_type::dagger};
    } daggers;

    struct maces_t
    {
        Weapon ebon_hand{"ebon_hand", Attributes{0.0, 0.0},
                         Special_stats{0.0, 0.0, 0.0}, 2.3, 83.0, 154.0, Weapon_socket::one_hand,
                         Weapon_type::mace, {{"ebon_hand", Hit_effect::Type::damage_magic, {}, {}, 200, 0, 0.08}}};
        Weapon spineshatter{"spineshatter", Attributes{9.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, 2.5, 99.0, 184.0,
                            Weapon_socket::main_hand, Weapon_type::mace};
        Weapon stormstike_hammer{"stormstike_hammer", Attributes{15, 0}, Special_stats{0, 0, 0}, 2.7, 80, 150,
                                 Weapon_socket::one_hand, Weapon_type::mace};
        Weapon aq_sand_polished_hammer{"aq_sand_polished_hammer", Attributes{0, 0}, Special_stats{1, 0, 20}, 2.6, 97,
                                       181, Weapon_socket::one_hand, Weapon_type::mace};
        Weapon aq_anubisath_warhammer{"aq_anubisath_warhammer", Attributes{0, 0},
                                      Special_stats{0, 0, 32, 0, 0, 0, 0, 0, 4}, 1.8, 66, 123,
                                      Weapon_socket::one_hand, Weapon_type::mace};
    } maces;

    struct fists_t
    {
        Weapon claw_of_the_black_drake{"claw_of_the_black_drake", Attributes{13, 0}, Special_stats{1, 0, 0}, 2.6, 102,
                                       191, Weapon_socket::main_hand, Weapon_type::unarmed};
    } fists;

    struct set_bonuses_t
    {
        Set_bonus devilsaur_set_bonus{Attributes{0, 0}, Special_stats{0, 2, 0}, 2, Set::devilsaur};
        Set_bonus black_dragonscale_bonus2{Attributes{0, 0}, Special_stats{0, 1, 0}, 2, Set::black_dragonscale};
        Set_bonus black_dragonscale_bonus3{Attributes{0, 0}, Special_stats{2, 0, 0}, 3, Set::black_dragonscale};
        Set_bonus rare_pvp_set_bonus_1{Attributes{0, 0}, Special_stats{0, 0, 40}, 2, Set::rare_pvp_set};
        Set_bonus dal_rends{Attributes{0, 0}, Special_stats{0, 0, 50}, 2, Set::dal_rends};
    } set_bonuses;

    auto get_set_bonuses() const
    {
        return set_bonuses;
    }

    Attributes get_enchant_attributes(Socket socket, Enchant::Type type) const
    {
        switch (socket)
        {
            case Socket::head:
            {
                switch (type)
                {
                    case Enchant::Type::agility:
                        return {0, 8};
                    case Enchant::Type::strength:
                        return {8, 0};
                    default:
                        return {0, 0};
                }
            }
            case Socket::back:
            {
                switch (type)
                {
                    case Enchant::Type::agility:
                        return {0, 3};
                    default:
                        return {0, 0};
                }
            }
            case Socket::chest:
            {
                switch (type)
                {
                    case Enchant::Type::minor_stats:
                        return {3, 3};
                    case Enchant::Type::major_stats:
                        return {4, 4};
                    default:
                        return {0, 0};
                }
            }
            case Socket::wrists :
            {
                switch (type)
                {
                    case Enchant::Type::strength7:
                        return {7, 0};
                    case Enchant::Type::strength9:
                        return {9, 0};
                    default:
                        return {0, 0};
                }
            }
            case Socket::hands :
            {
                switch (type)
                {
                    case Enchant::Type::agility:
                        return {0, 7};
                    case Enchant::Type::strength:
                        return {7, 0};
                    default:
                        return {0, 0};
                }
            }
            case Socket::legs :
            {
                switch (type)
                {
                    case Enchant::Type::agility:
                        return {0, 8};
                    case Enchant::Type::strength:
                        return {8, 0};
                    default:
                        return {0, 0};
                }
            }
            case Socket::boots :
            {
                switch (type)
                {
                    case Enchant::Type::agility:
                        return {0, 7};
                    default:
                        return {0, 0};
                }
            }
            default:
                return {0, 0};
        }
    }

    Special_stats get_enchant_special_stats(Socket socket, Enchant::Type type) const
    {
        switch (socket)
        {
            case Socket::head:
            {
                switch (type)
                {
                    case Enchant::Type::haste:
                        return {0, 0, 0, 0, .01};
                    default:
                        return {0, 0, 0};
                }
            }
            case Socket::hands :
            {
                switch (type)
                {
                    case Enchant::Type::haste:
                        return {0, 0, 0, 0, .01};
                    default:
                        return {0, 0, 0};
                }
            }
            case Socket::legs:
            {
                switch (type)
                {
                    case Enchant::Type::haste:
                        return {0, 0, 0, 0, .01};
                    default:
                        return {0, 0, 0};
                }
            }
            default:
                return {0, 0, 0};
        }
    }

    Hit_effect enchant_hit_effect(double weapon_speed, Enchant::Type type) const
    {
        if (type == Enchant::Type::crusader)
        {
            return {"crusader", Hit_effect::Type::stat_boost, {100, 0}, {0, 0, 0}, 0, 15, weapon_speed / 40};
        }
        return {"none", Hit_effect::Type::none, {}, {}, 0, 0, 0};
    }

    void clean_weapon(Weapon &weapon) const
    {
        if (!weapon.hit_effects.empty())
        {
            if (weapon.hit_effects[0].name == weapon.name)
            {
                weapon.hit_effects = {weapon.hit_effects[0]};
            }
            else
            {
                weapon.hit_effects = {};
            }
        }
    }

    void compute_total_stats(Character &character) const
    {
        check_if_weapons_valid(character.weapons);
        check_if_armor_valid(character.armor);
        character.total_attributes = {};
        character.total_special_stats = {};
        clean_weapon(character.weapons[0]);
        clean_weapon(character.weapons[1]);
        Attributes total_attributes{};
        Special_stats total_special_stats{};
        double stat_multiplier = 0;

        total_attributes += character.base_attributes;
        total_special_stats += character.base_special_stats;
        std::vector<Set> set_names{};
        for (const Armor &armor : character.armor)
        {
            total_attributes += armor.attributes;
            total_special_stats += armor.special_stats;

            total_attributes += get_enchant_attributes(armor.socket, armor.enchant.type);
            total_special_stats += get_enchant_special_stats(armor.socket, armor.enchant.type);

            set_names.emplace_back(armor.set_name);
        }

        if (total_special_stats.chance_for_extra_hit > 0)
        {
            for (Weapon &weapon : character.weapons)
            {
                weapon.hit_effects.emplace_back(Hit_effect{"HoJ", Hit_effect::Type::extra_hit, {}, {}, 0, 0,
                                                           total_special_stats.chance_for_extra_hit});
            }
        }

        for (Weapon &weapon : character.weapons)
        {
            total_attributes += weapon.attributes;
            total_special_stats += weapon.special_stats;

            total_attributes += get_enchant_attributes(weapon.socket, weapon.enchant.type);
            total_special_stats += get_enchant_special_stats(weapon.socket, weapon.enchant.type);

            auto hit_effect = enchant_hit_effect(weapon.swing_speed, weapon.enchant.type);
            if (hit_effect.type != Hit_effect::Type::none)
            {
                weapon.hit_effects.emplace_back(enchant_hit_effect(weapon.swing_speed, weapon.enchant.type));
            }

            set_names.emplace_back(weapon.set_name);
        }

        // TODO fix this mess
        int set_pieces_devil = 0;
        int set_pieces_bds = 0;
        int set_pieces_pvp = 0;
        int set_pieces_dal_rend = 0;
        for (Set &set_name : set_names)
        {
            switch (set_name)
            {
                case Set::devilsaur:
                    set_pieces_devil++;
                    break;
                case Set::black_dragonscale:
                    set_pieces_bds++;
                    break;
                case Set::rare_pvp_set:
                    set_pieces_pvp++;
                    break;
                case Set::dal_rends:
                    set_pieces_dal_rend++;
                    break;
                default:
                    break;
            }
        }
        if (set_pieces_devil >= set_bonuses.devilsaur_set_bonus.pieces)
        {
            total_attributes += set_bonuses.devilsaur_set_bonus.attributes;
            total_special_stats += set_bonuses.devilsaur_set_bonus.special_stats;
        }
        if (set_pieces_bds >= set_bonuses.black_dragonscale_bonus2.pieces)
        {
            total_attributes += set_bonuses.black_dragonscale_bonus2.attributes;
            total_special_stats += set_bonuses.black_dragonscale_bonus2.special_stats;
        }
        if (set_pieces_bds >= set_bonuses.black_dragonscale_bonus3.pieces)
        {
            total_attributes += set_bonuses.black_dragonscale_bonus3.attributes;
            total_special_stats += set_bonuses.black_dragonscale_bonus3.special_stats;
        }
        if (set_pieces_pvp >= set_bonuses.rare_pvp_set_bonus_1.pieces)
        {
            total_attributes += set_bonuses.rare_pvp_set_bonus_1.attributes;
            total_special_stats += set_bonuses.rare_pvp_set_bonus_1.special_stats;
        }
        if (set_pieces_dal_rend >= set_bonuses.dal_rends.pieces)
        {
            total_attributes += set_bonuses.dal_rends.attributes;
            total_special_stats += set_bonuses.dal_rends.special_stats;
        }

        for (const auto &buff : character.buffs)
        {
            total_attributes += buff.attributes;
            total_special_stats += buff.special_stats;
            stat_multiplier += buff.stat_multiplier;
        }

        // TODO implement shout in simulator instead
        total_special_stats.attack_power += 241;  // battle shout
        total_special_stats.critical_strike += 5; // crit from talent
        total_special_stats.critical_strike += 3; // crit from berserker stance

        total_attributes *= (1.0 + stat_multiplier);
        total_special_stats += total_attributes.convert_to_special_stats();

        character.total_attributes = total_attributes;
        character.total_special_stats = total_special_stats;
    }

    bool check_if_armor_valid(const std::vector<Armor> &armor) const
    {
        std::vector<Socket> sockets;
        bool one_ring{false};
        bool one_trinket{false};
        for (auto const &armor_piece : armor)
        {
            for (auto const &socket : sockets)
            {
                if (armor_piece.socket == socket)
                {
                    if (armor_piece.socket == Socket::ring)
                    {
                        if (armor_piece.socket == Socket::ring && one_ring)
                        {
                            std::cout << "extra copy of " << armor_piece.socket << "\n";
                            return false;
                        }
                        one_ring = true;
                    }
                    else if (armor_piece.socket == Socket::trinket)
                    {
                        if (armor_piece.socket == Socket::trinket && one_trinket)
                        {
                            std::cout << "extra copy of " << armor_piece.socket << "\n";
                            return false;
                        }
                        one_trinket = true;
                    }
                    else
                    {
                        std::cout << "extra copy of " << armor_piece.socket << "\n";
                        return false;
                    }
                }
            }
            sockets.emplace_back(armor_piece.socket);
        }
        return true;
    }

    bool check_if_weapons_valid(std::vector<Weapon> &weapons) const
    {
        bool is_valid{true};
        is_valid &= weapons.size() <= 2;
        is_valid &= (weapons[0].socket != Socket::off_hand);
        if (weapons.size() == 2)
        {
            is_valid &= (weapons[1].socket != Socket::main_hand);
        }
        return is_valid;
    }

    void change_weapon(std::vector<Weapon> &weapons, const Weapon &weapon, const Socket &socket) const
    {
        assert(socket == Socket::main_hand || socket == Socket::off_hand);
        Weapon &current_wep = (socket == Socket::main_hand) ? weapons[0] : weapons[1];
        Weapon weapon_copy = weapon;
        weapon_copy.buff = current_wep.buff;
        weapon_copy.enchant = current_wep.enchant;
        current_wep = weapon_copy;
    }

    void change_armor(std::vector<Armor> &armor_vec, const Armor &armor, bool first_misc_slot = true) const
    {
        auto socket = armor.socket;
        for (auto &armor_piece : armor_vec)
        {
            if (armor_piece.socket == socket)
            {
                if (socket == Socket::ring || socket == Socket::trinket)
                {
                    if (first_misc_slot)
                    {
                        armor_piece = armor;
                        return;
                    }
                    first_misc_slot = true; // Will trigger on the second hit instead
                }
                else
                {
                    // Reuse the same enchant
                    auto enchant = armor_piece.enchant;
                    armor_piece = armor;
                    armor_piece.enchant = enchant;
                    return;
                }
            }
        }
    }
};

#endif //WOW_SIMULATOR_ARMORY_HPP
