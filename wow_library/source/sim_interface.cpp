#include <Armory.hpp>
#include <Character.hpp>
#include <Combat_simulator.hpp>

#include "sim_interface.hpp"

Character character_setup(const Armory &armory, const Buffs &buffs)
{
    Character character{Race::human, 60};

    character.equip_armor(armory.helmet.lionheart_helm);
    character.equip_armor(armory.neck.onyxia_tooth_pendant);
    character.equip_armor(armory.shoulder.drake_talon_pauldrons);
    character.equip_armor(armory.back.cape_of_the_black_baron);
    character.equip_armor(armory.chest.savage_gladiator_chain);
    character.equip_armor(armory.wrist.wristguards_of_stability);
    character.equip_armor(armory.hands.flameguard_gauntlets);
    character.equip_armor(armory.belt.onslaught_girdle);
    character.equip_armor(armory.legs.legguards_of_the_fallen_crusader);
    character.equip_armor(armory.boots.chromatic_boots);
    character.equip_armor(armory.rings.don_julios_band);
    character.equip_armor(armory.rings.magnis_will);
    character.equip_armor(armory.trinket.hand_of_justice);
    character.equip_armor(armory.trinket.diamond_flask);
    character.equip_armor(armory.ranged.blastershot);

    character.equip_weapon(armory.swords.maladath, armory.swords.brutality_blade);

    character.add_enchant(Socket::head, Enchant::Type::haste);
    character.add_enchant(Socket::back, Enchant::Type::agility);
    character.add_enchant(Socket::chest, Enchant::Type::major_stats);
    character.add_enchant(Socket::wrists, Enchant::Type::strength9);
    character.add_enchant(Socket::hands, Enchant::Type::haste);
    character.add_enchant(Socket::legs, Enchant::Type::haste);
    character.add_enchant(Socket::boots, Enchant::Type::agility);
    character.add_enchant(Socket::main_hand, Enchant::Type::crusader);
    character.add_enchant(Socket::off_hand, Enchant::Type::crusader);

    character.add_buff(buffs.rallying_cry);
    character.add_buff(buffs.dire_maul);
    character.add_buff(buffs.songflower);
    character.add_buff(buffs.blessing_of_kings);
    character.add_buff(buffs.blessing_of_might);
    character.add_buff(buffs.gift_of_the_wild);
    character.add_buff(buffs.trueshot_aura);
    character.add_buff(buffs.elixir_mongoose);
    character.add_buff(buffs.blessed_sunfruit);
    character.add_buff(buffs.juju_power);
    character.add_buff(buffs.juju_might);
    character.add_buff(buffs.roids);

    character.add_weapon_buff(Socket::main_hand, buffs.dense_stone);
    character.add_weapon_buff(Socket::off_hand, buffs.dense_stone);

    armory.compute_total_stats(character);

    return character;
}

Sim_output Sim_interface::simulate(const Sim_input &input)
{
    Armory armory{};
    Buffs buffs{};

    Character character = character_setup(armory, buffs);

    // Simulator & Combat settings
    Combat_simulator_config config{};
    config.n_batches = input.n_simulations;
    config.sim_time = input.fight_time;
    config.opponent_level = input.target_level;

    config.curse_of_recklessness_active = true;
    config.faerie_fire_feral_active = true;
    config.sunder_armor_active = true;
    config.n_sunder_armor_stacks = 5;

    config.talents.improved_heroic_strike = 2;
    config.talents.unbridled_wrath = 5;
    config.talents.flurry = 5;
    config.talents.anger_management = true;
    config.talents.death_wish = true;
    config.talents.impale = 2;
    config.talents.improved_execute = 2;
    config.talents.dual_wield_specialization = 5;

    config.use_sim_time_ramp = true;
    config.enable_spell_rotation = true;
    config.use_mighty_rage_potion = true;
    config.enable_bloodrage = true;
    config.enable_recklessness = false;
//    config.display_combat_debug = true;
    config.use_seed = true;
    config.seed = 110000;
    config.fuel_extra_rage = false;
    config.extra_rage_interval = 3;
    config.extra_rage_damage_amount = 150;

    Combat_simulator simulator(config);

    std::vector<double> dps_vec = simulator.simulate(character);
    double mean_init = Statistics::average(dps_vec);
    double std_init = Statistics::standard_deviation(dps_vec, mean_init);
    double sample_std_init = Statistics::sample_deviation(std_init, config.n_batches);




    return {dps_vec, mean_init, sample_std_init};
}

