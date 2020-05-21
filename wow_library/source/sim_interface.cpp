#include <Armory.hpp>
#include <Character.hpp>
#include <Combat_simulator.hpp>

#include "sim_interface.hpp"

#include <sstream>

std::vector<double> get_damage_sources(const std::vector<Damage_sources> &damage_sources_vector)
{
    Damage_sources total_damage_source{};

    for (const auto &damage_source: damage_sources_vector)
    {
        total_damage_source = total_damage_source + damage_source;
    }
    return {total_damage_source.white_mh_damage / total_damage_source.sum_damage_sources(),
            total_damage_source.white_oh_damage / total_damage_source.sum_damage_sources(),
            total_damage_source.heroic_strike_damage / total_damage_source.sum_damage_sources(),
            total_damage_source.bloodthirst_damage / total_damage_source.sum_damage_sources(),
            total_damage_source.whirlwind_damage / total_damage_source.sum_damage_sources(),
            total_damage_source.execute_damage / total_damage_source.sum_damage_sources(),
            total_damage_source.item_hit_effects_damage / total_damage_source.sum_damage_sources()};
}

bool find_string(const std::vector<std::string> &string_vec, const std::string &match_string)
{
    for (const auto &string : string_vec)
    {
        if (string == match_string)
        {
            return true;
        }
    }
    return false;
}

std::string print_stat(const std::string &stat_name, double amount)
{
    std::ostringstream stream;
    stream << stat_name << std::setw(6) << std::left << std::setprecision(6) << amount << "<br />";
    return stream.str();
}

std::string get_character_stat(const Character &character)
{
    std::string out_string = "Character stats <br />";
    out_string += print_stat("Strength : ", character.total_attributes.strength);
    out_string += print_stat("Agility  : ", character.total_attributes.agility);
    out_string += print_stat("Hit:     : ", character.total_special_stats.hit);
    out_string += print_stat("Crit:    : ", character.total_special_stats.critical_strike);
    out_string += print_stat("Atk Pwr  : ", character.total_special_stats.attack_power);
    out_string += print_stat("Haste    : ", character.total_special_stats.haste);
    out_string += print_stat("Swrd skil: ", character.total_special_stats.sword_skill);
    out_string += print_stat("Axe  skil: ", character.total_special_stats.axe_skill);
    out_string += print_stat("Dagr skil: ", character.total_special_stats.dagger_skill);
    out_string += print_stat("Mace skil: ", character.total_special_stats.mace_skill);
    out_string += print_stat("Unrm skil: ", character.total_special_stats.fist_skill);
    out_string += "<br />";

    out_string += "Armor:<br />";
    for (auto const &armor_piece : character.armor)
    {
        out_string += armor_piece.name + "<br />";
    }

    out_string += "<br />Weapons:<br />";
    for (auto const &wep : character.weapons)
    {
        out_string += wep.name + "<br />";
    }

    out_string += "<br />";
    return out_string;
}

Character character_setup(const Armory &armory, const Buffs &buffs, const std::vector<std::string> &armor_vec,
                          const std::vector<std::string> &weapons_vec, const std::vector<std::string> &buffs_vec)
{
    Character character{Race::human, 60};

    character.equip_armor(armory.find_armor(Socket::head, armor_vec[0]));
    character.equip_armor(armory.find_armor(Socket::neck, armor_vec[1]));
    character.equip_armor(armory.find_armor(Socket::shoulder, armor_vec[2]));
    character.equip_armor(armory.find_armor(Socket::back, armor_vec[3]));
    character.equip_armor(armory.find_armor(Socket::chest, armor_vec[4]));
    character.equip_armor(armory.find_armor(Socket::wrist, armor_vec[5]));
    character.equip_armor(armory.find_armor(Socket::hands, armor_vec[6]));
    character.equip_armor(armory.find_armor(Socket::belt, armor_vec[7]));
    character.equip_armor(armory.find_armor(Socket::legs, armor_vec[8]));
    character.equip_armor(armory.find_armor(Socket::boots, armor_vec[9]));
    character.equip_armor(armory.find_armor(Socket::ring, armor_vec[10]));
    character.equip_armor(armory.find_armor(Socket::ring, armor_vec[11]));
    character.equip_armor(armory.find_armor(Socket::trinket, armor_vec[12]));
    character.equip_armor(armory.find_armor(Socket::trinket, armor_vec[13]));
    character.equip_armor(armory.find_armor(Socket::ranged, armor_vec[14]));

    character.equip_weapon(armory.find_weapon(weapons_vec[0]), armory.find_weapon(weapons_vec[1]));

    character.add_enchant(Socket::head, Enchant::Type::haste);
    character.add_enchant(Socket::back, Enchant::Type::agility);
    character.add_enchant(Socket::chest, Enchant::Type::major_stats);
    character.add_enchant(Socket::wrist, Enchant::Type::strength9);
    character.add_enchant(Socket::hands, Enchant::Type::haste);
    character.add_enchant(Socket::legs, Enchant::Type::haste);
    character.add_enchant(Socket::boots, Enchant::Type::agility);
    character.add_enchant(Socket::main_hand, Enchant::Type::crusader);
    character.add_enchant(Socket::off_hand, Enchant::Type::crusader);

    // rayying cry
    if (find_string(buffs_vec, "rallying_cry"))
    {
        character.add_buff(buffs.rallying_cry);
    }
    if (find_string(buffs_vec, "dire_maul"))
    {
        character.add_buff(buffs.dire_maul);
    }
    if (find_string(buffs_vec, "songflower"))
    {
        character.add_buff(buffs.songflower);
    }
    if (find_string(buffs_vec, "warchiefs_blessing"))
    {
        character.add_buff(buffs.warchiefs_blessing);
    }
    if (find_string(buffs_vec, "spirit_of_zandalar"))
    {
        character.add_buff(buffs.spirit_of_zandalar);
    }
    if (find_string(buffs_vec, "sayges_fortune"))
    {
        character.add_buff(buffs.sayges_fortune);
    }

    // Player buffs
    if (find_string(buffs_vec, "blessing_of_kings"))
    {
        character.add_buff(buffs.blessing_of_kings);
    }
    if (find_string(buffs_vec, "blessing_of_might"))
    {
        character.add_buff(buffs.blessing_of_might);
    }
    if (find_string(buffs_vec, "gift_of_the_wild"))
    {
        character.add_buff(buffs.gift_of_the_wild);
    }
    if (find_string(buffs_vec, "trueshot_aura"))
    {
        character.add_buff(buffs.trueshot_aura);
    }

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

    Character character = character_setup(armory, buffs, input.armor, input.weapons, input.buffs);

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
    std::vector<double> dps_dist = get_damage_sources(simulator.get_damage_distribution());
    double mean_init = Statistics::average(dps_vec);
    double std_init = Statistics::standard_deviation(dps_vec, mean_init);
    double sample_std_init = Statistics::sample_deviation(std_init, config.n_batches);

    return {dps_vec, dps_dist, mean_init, sample_std_init, {get_character_stat(character)}};
}

