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
    World_buff rallying_cry{Stats{0.0, 0.0}, Special_stats{5.0, 0.0, 140}, World_buff::Id::rallying_cry};
    World_buff dire_maul{Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 200}, World_buff::Id::dire_maul};
    World_buff songflower{Stats{15, 15}, Special_stats{5, 0, 0}, World_buff::Id::songflower};

    // Player_buffs
    Player_buff blessing_of_kings{Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Player_buff::Id::blessing_of_kings};
    Player_buff blessing_of_might{Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 155}, Player_buff::Id::blessing_of_might};
    Player_buff gift_of_the_wild{Stats{12.0, 12.0}, Special_stats{0.0, 0.0, 0.0}, Player_buff::Id::mark_of_the_wild};
    Player_buff trueshot_aura{Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 100}, Player_buff::Id::trueshot_aura};

    // Consumables
    Consumable elixir_mongoose{Stats{0.0, 25.0}, Special_stats{2.0, 0.0, 0.0}, Consumable::Id::elixir_mongoose};
    Consumable dense_stone_mh{Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Consumable::Id::dense_stone_mh};
    Consumable dense_stone_oh{Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Consumable::Id::dense_stone_oh};
    Consumable elemental_stone_mh{Stats{0.0, 0.0}, Special_stats{2.0, 0.0, 0.0}, Consumable::Id::elemental_stone_mh};
    Consumable elemental_stone_oh{Stats{0.0, 0.0}, Special_stats{2.0, 0.0, 0.0}, Consumable::Id::elemental_stone_oh};
    Consumable blessed_sunfruit{Stats{10.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Consumable::Id::blessed_sunfruit};
    Consumable juju_power{Stats{30.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Consumable::Id::juju_power};
    Consumable juju_might{Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 40}, Consumable::Id::juju_might};
    Consumable roids{Stats{25.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Consumable::Id::roids};

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

    // Current items
    Armor lionheart_helm{Stats{18, 0}, Special_stats{2, 2, 0}, Armor::Socket::head};
    Armor onyxia_tooth_pendant{Stats{0, 13}, Special_stats{1, 1, 0}, Armor::Socket::neck};
    Armor truestrike_shoulders{Stats{0, 0}, Special_stats{0, 2, 24}, Armor::Socket::shoulder};
    Armor cape_of_the_black_baron{Stats{0, 15}, Special_stats{0, 0, 20}, Armor::Socket::back};
    Armor savage_gladiator_chain{Stats{13, 14}, Special_stats{2, 0, 0}, Armor::Socket::chest};
    Armor cadaverous_armor{Stats{8, 8}, Special_stats{0, 0, 60}, Armor::Socket::chest};
    Armor wristguards_of_stability{Stats{24, 0}, Special_stats{0, 0, 0}, Armor::Socket::wrists};
    Armor devilsaur_gauntlets{Stats{0, 0}, Special_stats{1, 1, 28}, Armor::Socket::hands};
    Armor edgemasters_handguards{Stats{0, 0}, Special_stats{0, 0, 0}, Armor::Socket::hands};
    Armor onslaught_girdle{Stats{31, 0}, Special_stats{1, 1, 0}, Armor::Socket::belt};
    Armor devilsaur_leggings{Stats{0, 0}, Special_stats{1, 1, 46}, Armor::Socket::legs};
    Armor windreaver_greaves{Stats{0, 20}, Special_stats{0, 1, 0}, Armor::Socket::boots};
    Armor bloodmail_boots{Stats{9, 9}, Special_stats{0, 1, 0}, Armor::Socket::boots};
    Armor don_julios_band{Stats{0, 0}, Special_stats{1, 1, 16}, Armor::Socket::ring};
    Armor magnis_will{Stats{6, 0}, Special_stats{1, 0, 0}, Armor::Socket::ring};
    Armor hand_of_justice{Stats{0, 0}, Special_stats{0, 0, 20}, Armor::Socket::trinket};
    Armor blackhands_breadth{Stats{0, 0}, Special_stats{2, 0, 0}, Armor::Socket::trinket};
    Armor satyrs_bow{Stats{0, 3}, Special_stats{0, 1, 0}, Armor::Socket::ranged};

    Weapon brutality_blade{2.5, {90, 168}, Stats{9, 9}, Special_stats{1, 0, 0}, Weapon::Socket::one_hand,
                           Skill_type::sword};
    Weapon mirahs_song{1.8, {57, 87}, Stats{9, 9}, Special_stats{0, 0, 0}, Weapon::Socket::one_hand, Skill_type::sword};
    Weapon viskag{2.6, {100, 187}, Stats{0, 0}, Special_stats{0, 0, 0}, Weapon::Socket::one_hand, Skill_type::sword};
    Weapon deathbringer{2.9, {114, 213}, Stats{0, 0}, Special_stats{0, 0, 0}, Weapon::Socket::one_hand,
                        Skill_type::axe};
    Weapon core_hound_tooth{1.6, {57, 107}, Stats{0, 0}, Special_stats{1, 0, 20}, Weapon::Socket::one_hand,
                            Skill_type::dagger};

    // Others
    Armor expert_goldminers_head{Stats{0, 5}, Special_stats{0, 0, 0}, Armor::Socket::head};
    Armor mask_of_the_unforgiven{Stats{0, 0}, Special_stats{1, 2, 0}, Armor::Socket::head};
    Armor crown_of_destruction{Stats{0, 0}, Special_stats{2, 0, 44}, Armor::Socket::head};
    Armor quick_strike_ring{Stats{8, 0}, Special_stats{1, 0, 30}, Armor::Socket::ring};
    Armor flameguard_gauntlets{Stats{0, 0}, Special_stats{1, 0, 54}, Armor::Socket::hands};
    Armor mugglers_belt{Stats{0, 0}, Special_stats{1, 0, 0}, Armor::Socket::belt};

    Armor leutenant_shoulders{Stats{17, 0}, Special_stats{1, 0, 0}, Armor::Socket::shoulder};
    Armor omokks_girth{Stats{15, 0}, Special_stats{1, 0, 0}, Armor::Socket::belt};
    Armor cloudkeeper_legplaters{Stats{20, 20}, Special_stats{0, 0, 100}, Armor::Socket::legs};
    Armor knight_leutenants_greaves{Stats{10, 9}, Special_stats{0, 0, 40}, Armor::Socket::boots};
    Armor battleborn_armbraces{Stats{0, 0}, Special_stats{1, 1, 0}, Armor::Socket::wrists};
    Armor strikers_mark{Stats{0, 0}, Special_stats{0, 1, 22}, Armor::Socket::ranged};

    /** BWL */
    // Armor
    Armor helm_of_endless_rage{Stats{26, 26}, Special_stats{0, 0, 0}, Armor::Socket::head};
    Armor heartstriker{Stats{0, 0}, Special_stats{0, 0, 24}, Armor::Socket::ranged};
    Armor drake_talon_pauldrons{Stats{20, 20}, Special_stats{0, 0, 0}, Armor::Socket::shoulder};
    Armor cloak_of_firemaw{Stats{0, 0}, Special_stats{0, 0, 50}, Armor::Socket::back};
    Armor legguards_of_the_fallen_crusader{Stats{28, 22}, Special_stats{0, 0, 0}, Armor::Socket::legs};
    Armor malfurions_blessed_bulwark{Stats{40, 0}, Special_stats{0, 0, 0}, Armor::Socket::chest};
    Armor dragonbreath_hand_cannon{Stats{0, 14}, Special_stats{0, 0, 0}, Armor::Socket::ranged};
    Armor circle_of_applied_force{Stats{12, 22}, Special_stats{0, 0, 0}, Armor::Socket::ring};
    Armor girdle_of_the_fallen_crusader{Stats{20, 10}, Special_stats{0, 0, 0}, Armor::Socket::belt};
    Armor chromatic_boots{Stats{20, 2}, Special_stats{0, 1, 0}, Armor::Socket::boots};
    Armor prestors_talisman_of_connivery{Stats{0, 30}, Special_stats{0, 1, 0}, Armor::Socket::neck};
    Armor boots_of_shadow_flame{Stats{0, 0}, Special_stats{0, 2, 44}, Armor::Socket::boots};
    Armor drake_fang_talisman{Stats{0, 0}, Special_stats{0, 2, 56}, Armor::Socket::trinket};
    Armor master_dragonslayers_ring{Stats{0, 0}, Special_stats{0, 1, 48}, Armor::Socket::ring};
    Armor cloak_of_draconic_might{Stats{16, 16}, Special_stats{0, 0, 0}, Armor::Socket::back};

    // Weapons
    Weapon spineshatter = Weapon{2.5, {99.0, 184.0}, Stats{9.0, 0.0}, Special_stats{0.0, 0.0, 0.0},
                                 Weapon::Socket::main_hand, Skill_type::mace};
    Weapon maladath = Weapon{2.2, {86.0, 162.0}, Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0},
                             Weapon::Socket::one_hand, Skill_type::sword};
    Weapon chromatically_tempered_sword = Weapon{2.6, {106.0, 198.0}, Stats{14.0, 14.0}, Special_stats{0.0, 0.0, 0.0},
                                                 Weapon::Socket::one_hand, Skill_type::sword};
    Weapon crul_shorukh_edge_of_chaos = Weapon{2.3, {101.0, 188.0}, Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 36.0},
                                               Weapon::Socket::one_hand, Skill_type::axe};
    Weapon dooms_edge = Weapon{2.3, {83.0, 154.0}, Stats{9.0, 16.0}, Special_stats{0.0, 0.0, 0.0},
                               Weapon::Socket::one_hand, Skill_type::axe};


    void set_extra_item_properties()
    {
        hand_of_justice.set_chance_for_extra_hit(2);
        edgemasters_handguards.set_bonus_skill(Extra_skill{Skill_type::all, 7});
        maladath.set_bonus_skill(Extra_skill{Skill_type::sword, 7});
        expert_goldminers_head.set_bonus_skill(Extra_skill{Skill_type::axe, 7});
        mugglers_belt.set_bonus_skill(Extra_skill{Skill_type::dagger, 5});
    }
};

#endif //WOW_SIMULATOR_ARMORY_HPP
