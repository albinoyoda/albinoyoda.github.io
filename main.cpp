#include <iostream>
#include <ctime>

#include "Stats.hpp"
#include "Item.hpp"
#include "Combat_simulator.hpp"
#include "Character.hpp"
#include "Enchant.hpp"

// TODO Graphics?

// TODO damage sources

// TODO 2h weapons

// TODO rage when missing?
// TODO heroic strike bug OH implementation

// TODO double check whirlwind damage

// TODO anger management
// TODO deep wounds

// TODO move battle shout in the simulation
// TODO cooldowns
// TODO stances
// TODO execute
// TODO overpower

// TODO setbonuses

struct Buffs
{
    Buffs()
    {
        set_extra_properties();
    }

    // World buffs
    World_buff rallying_cry{Stats{0.0, 0.0}, Special_stats{5.0, 0.0, 140}, World_buff::Id::rallying_cry};
    World_buff dire_maul{Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 200}, World_buff::Id::dire_maul};
    World_buff songflower{Stats{15.0, 15.0}, Special_stats{5.0, 0.0, 0.0}, World_buff::Id::songflower};

    // Player_buffs
    Player_buff blessing_of_kings{Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Player_buff::Id::blessing_of_kings};
    Player_buff blessing_of_might{Stats{0.0, 0.0}, Special_stats{0.0, 0.0, 155}, Player_buff::Id::blessing_of_might};
    Player_buff mark_of_the_wild{Stats{12.0, 12.0}, Special_stats{0.0, 0.0, 0.0}, Player_buff::Id::mark_of_the_wild};
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

    // Others
    Armor expert_goldminers_head{Stats{0, 5}, Special_stats{0, 0, 0}, Armor::Socket::head};
    Armor mask_of_the_unforgiven{Stats{0, 0}, Special_stats{1, 2, 0}, Armor::Socket::head};
    Armor crown_of_destruction{Stats{0, 0}, Special_stats{2, 0, 44}, Armor::Socket::head};
    Armor quick_strike_ring{Stats{8, 0}, Special_stats{1, 0, 30}, Armor::Socket::ring};
    Armor flameguard_gauntlets{Stats{0, 0}, Special_stats{1, 0, 54}, Armor::Socket::hands};

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
    }
};

int main()
{
    clock_t startTime = clock();

    Character character{Race::human};
    Armory armory;
    Buffs buffs;

    character.equip_armor(
//            armory.mask_of_the_unforgiven,
            armory.lionheart_helm,
//            armory.expert_goldminers_head,

            armory.onyxia_tooth_pendant,
//            armory.prestors_talisman_of_connivery,

            armory.truestrike_shoulders,
//            armory.drake_talon_pauldrons,

//            armory.cloak_of_firemaw,
//            armory.cloak_of_draconic_might,
            armory.cape_of_the_black_baron,

            armory.savage_gladiator_chain,
//            armory.malfurions_blessed_bulwark,
//            armory.cadaverous_armor,

            armory.wristguards_of_stability,

//            armory.flameguard_gauntlets,
            armory.devilsaur_gauntlets,
//            armory.edgemasters_handguards,

            armory.onslaught_girdle,

            armory.devilsaur_leggings,
//            armory.legguards_of_the_fallen_crusader,

            armory.bloodmail_boots,
//            armory.chromatic_boots,
//            armory.boots_of_shadow_flame,

            armory.don_julios_band,
            armory.magnis_will,
//            armory.master_dragonslayers_ring,
//            armory.circle_of_applied_force,
//            armory.quick_strike_ring,

            armory.hand_of_justice,
//            armory.drake_fang_talisman,
            armory.blackhands_breadth,

            armory.satyrs_bow
//            armory.dragonbreath_hand_cannon
                         );

    character.equip_weapon(armory.brutality_blade,
                           armory.mirahs_song);

//    character.equip_weapon(armory.chromatically_tempered_sword,
//                           armory.maladath);

    if (!character.check_if_armor_valid())
    {
        std::cout << "Incorrect armor setup!\n";
        return -1;
    }
    if (!character.check_if_weapons_valid())
    {
        std::cout << "Incorrect weapon setup!\n";
        return -1;
    }

    character.add_enchants(Enchant{Enchant::Socket::head, Enchant::Type::haste},
                           Enchant{Enchant::Socket::back, Enchant::Type::agility},
                           Enchant{Enchant::Socket::chest, Enchant::Type::minor_stats},
                           Enchant{Enchant::Socket::wrists, Enchant::Type::strength7},
                           Enchant{Enchant::Socket::hands, Enchant::Type::haste},
                           Enchant{Enchant::Socket::legs, Enchant::Type::haste},
                           Enchant{Enchant::Socket::weapon_mh, Enchant::Type::crusader},
                           Enchant{Enchant::Socket::weapon_oh, Enchant::Type::crusader}
                          );

    character.add_buffs(buffs.rallying_cry,
                        buffs.dire_maul,
//                        buffs.songflower,
                        buffs.blessing_of_kings,
                        buffs.blessing_of_might,
                        buffs.mark_of_the_wild,
                        buffs.trueshot_aura,
                        buffs.elixir_mongoose,
                        buffs.dense_stone_mh,
                        buffs.dense_stone_oh,
//                        buffs.elemental_stone_mh,
//                        buffs.elemental_stone_oh,
                        buffs.blessed_sunfruit,
//                        buffs.juju_power,
                        buffs.juju_might,
                        buffs.roids);

    character.compute_all_stats(Character::Talent::fury);
    std::cout << character.get_stats() << "\n";
    std::cout << character.get_total_special_stats();
    std::cout << "haste: " << character.get_haste() << "\n";
    std::cout << "chance for extra hit: " << character.get_chance_for_extra_hit() << "%" << "\n\n";

    Combat_simulator combat_simulator;

    // Combat settings
    combat_simulator.enable_spell_rotation();
    combat_simulator.enable_talents();
    combat_simulator.enable_item_chance_on_hit_effects();
    combat_simulator.enable_crusader();
        srand(static_cast <unsigned> (time(nullptr)));

    int n_batches = 20000;
    auto dps_snapshots = combat_simulator.simulate(character, 60, 63, n_batches);

    auto hit_table = combat_simulator.get_hit_probabilities_white_mh();
    std::cout << "Crit % left to crit cap: " << 100 - hit_table.back() << ". (Negative number means capped)\n";

    double mean_dps = Combat_simulator::average(dps_snapshots);
    double std_dps = Combat_simulator::standard_deviation(dps_snapshots, mean_dps);
    double sample_std_dps = Combat_simulator::sample_deviation(std_dps, n_batches);
    std::cout << "DPS from simulation: \n" << mean_dps << " +- " << 1.96 * sample_std_dps
              << " (95% confidence interval)\n\n";

    auto stat_weight_vector = combat_simulator.compute_stat_weights(character, 60, 63, n_batches);
    std::cout << "Stat weights: \n";
    for (const auto &stat_weight : stat_weight_vector)
    {
        std::cout << stat_weight;
    }

    std::cout << "Code executed in: " << double(clock() - startTime) / (double) CLOCKS_PER_SEC << " seconds."
              << std::endl;

    return 0;
}

