#include <iostream>
#include <ctime>

#include "wow_library/include/Combat_simulator.hpp"
#include "wow_library/include/Armory.hpp"

void print_stat(const std::string &stat_name, double amount)
{
    std::cout << stat_name << std::setw(6) << std::left << std::setprecision(6) << amount << "\n";
}

void print_character_stat(const Character &character)
{
    std::cout << "Character stats" << "\n";
    print_stat("Strength : ", character.total_attributes.strength);
    print_stat("Agility  : ", character.total_attributes.agility);
    print_stat("Hit:     : ", character.total_special_stats.hit);
    print_stat("Crit:    : ", character.total_special_stats.critical_strike);
    print_stat("Atk Pwr  : ", character.total_special_stats.attack_power);
    print_stat("Haste    : ", character.total_special_stats.haste);
    print_stat("Swrd skil: ", character.total_special_stats.sword_skill);
    print_stat("Axe  skil: ", character.total_special_stats.axe_skill);
    print_stat("Dagr skil: ", character.total_special_stats.dagger_skill);
    print_stat("Mace skil: ", character.total_special_stats.mace_skill);
    print_stat("Unrm skil: ", character.total_special_stats.fist_skill);
    std::cout << "\n";

    std::cout << "Armor:" << "\n";
    for (auto const &armor_piece : character.armor)
    {
        std::cout << armor_piece.name << "\n";
    }

    std::cout << "\n" << "Weapons:" << "\n";
    for (auto const &wep : character.weapons)
    {
        std::cout << wep.name << "\n";
    }

    std::cout << "\n";
}

struct Stat_weight
{
    Stat_weight(double dps_plus, double std_dps_plus, double dps_minus, double std_dps_minus, double amount,
                std::string stat) :
            dps_plus{dps_plus},
            std_dps_plus{std_dps_plus},
            dps_minus{dps_minus},
            std_dps_minus{std_dps_minus},
            amount{amount},
            stat{std::move(stat)} {};

    double dps_plus;
    double std_dps_plus;
    double dps_minus;
    double std_dps_minus;
    double amount;
    std::string stat;
};

//Stat_weight
//compute_stat_weight(Combat_simulator &combat_simulator, Character &character, Combat_simulator_config &config_minus,
//                    const std::string &permuted_talent, double permute_amount, double mean_init,
//                    double sample_std_init)
//{
//    combat_simulator
//    auto dmg_minus = combat_simulator.simulate(char_minus);
//    double mean_minus = Statistics::average(dmg_minus);
//    double std_minus = Statistics::standard_deviation(dmg_minus, mean_minus);
//    double sample_std_minus = Statistics::sample_deviation(std_minus, dmg_minus.size());
//
//    return {0.0, 0.0, mean_minus - mean_init, Statistics::add_standard_deviations(sample_std_init, sample_std_minus),
//            permute_amount, permuted_stat};
//}

Stat_weight compute_stat_weight(Combat_simulator &combat_simulator, Character &char_minus,
                                const std::string &permuted_stat, double permute_amount, double mean_init,
                                double sample_std_init)
{
    auto dmg_minus = combat_simulator.simulate(char_minus);
    double mean_minus = Statistics::average(dmg_minus);
    double std_minus = Statistics::standard_deviation(dmg_minus, mean_minus);
    double sample_std_minus = Statistics::sample_deviation(std_minus, dmg_minus.size());

    return {0.0, 0.0, mean_minus - mean_init, Statistics::add_standard_deviations(sample_std_init, sample_std_minus),
            permute_amount, permuted_stat};
}

Stat_weight compute_stat_weight(Combat_simulator &combat_simulator, Character &char_plus, Character &char_minus,
                                const std::string &permuted_stat, double permute_amount, double mean_init,
                                double sample_std_init)
{
    auto dmg_plus = combat_simulator.simulate(char_plus);
    double mean_plus = Statistics::average(dmg_plus);
    double std_plus = Statistics::standard_deviation(dmg_plus, mean_plus);
    double sample_std_plus = Statistics::sample_deviation(std_plus, dmg_plus.size());

    auto dmg_minus = combat_simulator.simulate(char_minus);
    double mean_minus = Statistics::average(dmg_minus);
    double std_minus = Statistics::standard_deviation(dmg_minus, mean_minus);
    double sample_std_minus = Statistics::sample_deviation(std_minus, dmg_plus.size());

    return {mean_plus - mean_init, Statistics::add_standard_deviations(sample_std_init, sample_std_plus),
            mean_minus - mean_init, Statistics::add_standard_deviations(sample_std_init, sample_std_minus),
            permute_amount, permuted_stat};
}

std::ostream &operator<<(std::ostream &os, Stat_weight const &stat_weight)
{
    os << std::setw(15) << std::left << stat_weight.stat << " stat weight: (" << stat_weight.dps_plus << " +- "
       << 1.96 * stat_weight.std_dps_minus << ", " << stat_weight.dps_minus << " +- "
       << 1.96 * stat_weight.std_dps_minus << "). Incremented/decremented by " << stat_weight.amount << "\n";
    return os;
}

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
    character.equip_armor(armory.legs.eldritch_legplates);
    character.equip_armor(armory.boots.chromatic_boots);
    character.equip_armor(armory.rings.don_julios_band);
    character.equip_armor(armory.rings.magnis_will);
    character.equip_armor(armory.trinket.hand_of_justice);
    character.equip_armor(armory.trinket.blackhands_breadth);
    character.equip_armor(armory.ranged.blastershot);

//    character.equip_armor(armory.helmet.lionheart_helm);
//    character.equip_armor(armory.neck.onyxia_tooth_pendant);
//    character.equip_armor(armory.shoulder.black_dragonscale_shoulders);
//    character.equip_armor(armory.back.blackveil_cloak);
//    character.equip_armor(armory.chest.tombstone_breastplate);
//    character.equip_armor(armory.wrist.battleborn_armbraces);
//    character.equip_armor(armory.hands.bloodmail_gauntlets);
//    character.equip_armor(armory.belt.brigam_girdle);
//    character.equip_armor(armory.legs.black_dragonscale_leggings);
//    character.equip_armor(armory.boots.black_dragonscale_boots);
//    character.equip_armor(armory.rings.magnis_will);
//    character.equip_armor(armory.rings.magnis_will);
//    character.equip_armor(armory.trinket.diamond_flask);
//    character.equip_armor(armory.trinket.hand_of_justice);
//    character.equip_armor(armory.ranged.precisely_calibrated_boomstick);

    character.equip_weapon(armory.swords.maladath, armory.swords.brutality_blade);

    character.add_enchant(Socket::head, Enchant::Type::haste);
    character.add_enchant(Socket::back, Enchant::Type::agility);
    character.add_enchant(Socket::chest, Enchant::Type::major_stats);
    character.add_enchant(Socket::wrists, Enchant::Type::strength9);
    character.add_enchant(Socket::hands, Enchant::Type::strength);
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

int main()
{
    clock_t startTime = clock();
    Armory armory{};
    Buffs buffs{};

    Character character = character_setup(armory, buffs);

    print_character_stat(character);

    // Simulator & Combat settings
    Combat_simulator_config config{};
    config.n_batches = 200000;
    config.sim_time = 60;
    config.opponent_level = 63;

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
    config.enable_rng_melee = false;
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

    Character char_plus = character;
    Character char_minus = character;
    std::vector<Stat_weight> stat_weight_vector{};

    char_plus.total_special_stats.attack_power += 20;
    char_minus.total_special_stats.attack_power -= 20;
    stat_weight_vector.emplace_back(compute_stat_weight(simulator, char_plus, char_minus, "Attack power", 20,
                                                        mean_init, sample_std_init));
    char_plus.total_special_stats = character.total_special_stats;
    char_minus.total_special_stats = character.total_special_stats;

    char_plus.total_special_stats.critical_strike += 1;
    char_minus.total_special_stats.critical_strike -= 1;
    stat_weight_vector.emplace_back(compute_stat_weight(simulator, char_plus, char_minus, "Crit", 1,
                                                        mean_init, sample_std_init));
    char_plus.total_special_stats = character.total_special_stats;
    char_minus.total_special_stats = character.total_special_stats;

    char_plus.total_special_stats.hit += 1;
    char_minus.total_special_stats.hit -= 1;
    stat_weight_vector.emplace_back(compute_stat_weight(simulator, char_plus, char_minus, "Hit", 1,
                                                        mean_init, sample_std_init));
    char_plus.total_special_stats = character.total_special_stats;
    char_minus.total_special_stats = character.total_special_stats;

    char_plus.total_special_stats.haste += .01;
    char_minus.total_special_stats.haste -= .01;
    stat_weight_vector.emplace_back(compute_stat_weight(simulator, char_plus, char_minus, "Haste", 1,
                                                        mean_init, sample_std_init));
    char_plus.total_special_stats = character.total_special_stats;
    char_minus.total_special_stats = character.total_special_stats;

    if (character.weapons[0].type == Weapon_type::sword || character.weapons[1].type == Weapon_type::sword)
    {
        char_plus.total_special_stats.sword_skill += 5;
        char_minus.total_special_stats.sword_skill -= 5;
        stat_weight_vector.emplace_back(compute_stat_weight(simulator, char_plus, char_minus, "Sword skill", 5,
                                                            mean_init, sample_std_init));
        char_plus.total_special_stats = character.total_special_stats;
        char_minus.total_special_stats = character.total_special_stats;
    }

    if (character.weapons[0].type == Weapon_type::axe || character.weapons[1].type == Weapon_type::axe)
    {
        char_plus.total_special_stats.axe_skill += 5;
        char_minus.total_special_stats.axe_skill -= 5;
        stat_weight_vector.emplace_back(compute_stat_weight(simulator, char_plus, char_minus, "Axe skill", 5,
                                                            mean_init, sample_std_init));
        char_plus.total_special_stats = character.total_special_stats;
        char_minus.total_special_stats = character.total_special_stats;
    }

    if (character.weapons[0].type == Weapon_type::mace || character.weapons[1].type == Weapon_type::mace)
    {
        char_plus.total_special_stats.mace_skill += 5;
        char_minus.total_special_stats.mace_skill -= 5;
        stat_weight_vector.emplace_back(compute_stat_weight(simulator, char_plus, char_minus, "Mace skill", 5,
                                                            mean_init, sample_std_init));
        char_plus.total_special_stats = character.total_special_stats;
        char_minus.total_special_stats = character.total_special_stats;
    }

    if (character.weapons[0].type == Weapon_type::dagger || character.weapons[1].type == Weapon_type::dagger)
    {
        char_plus.total_special_stats.dagger_skill += 5;
        char_minus.total_special_stats.dagger_skill -= 5;
        stat_weight_vector.emplace_back(compute_stat_weight(simulator, char_plus, char_minus, "Dagger skill", 5,
                                                            mean_init, sample_std_init));
        char_plus.total_special_stats = character.total_special_stats;
        char_minus.total_special_stats = character.total_special_stats;
    }

    Hit_effect extra_hit_1_percent{"stat_weight_extra_hit", Hit_effect::Type::extra_hit, {0, 0}, {0, 0, 0}, 0, 0, 0.01};
    char_plus.weapons[0].hit_effects.push_back(extra_hit_1_percent);
    char_plus.weapons[1].hit_effects.push_back(extra_hit_1_percent);
    stat_weight_vector.emplace_back(compute_stat_weight(simulator, char_plus, char_minus, "Extra hit", 1,
                                                        mean_init, sample_std_init));
    char_plus = character;

    char_plus.weapons[0].buff.bonus_damage += 8;
    char_plus.weapons[1].buff.bonus_damage += 8;
    stat_weight_vector.emplace_back(compute_stat_weight(simulator, char_plus, char_minus, "Weapon damage", 8,
                                                        mean_init, sample_std_init));
    char_plus = character;

    std::cout << "Stat weights:" << "\n";
    for (const auto &stat_weight : stat_weight_vector)
    {
        std::cout << stat_weight;
    }

    std::cout << "Code executed in: " << double(clock() - startTime) / (double) CLOCKS_PER_SEC << " seconds."
              << std::endl;

    return 0;
}

