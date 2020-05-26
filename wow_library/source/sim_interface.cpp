#include <Armory.hpp>
#include <Character.hpp>
#include <Combat_simulator.hpp>

#include "sim_interface.hpp"

#include <sstream>


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
    stream << stat_name << std::setw(6) << std::left << std::setprecision(6) << "<b>" << amount << "</b><br />";
    return stream.str();
}

std::string get_character_stat(const Character &character)
{
    std::string out_string = "Character stats: <br />";
    out_string += print_stat("Strength: ", character.total_attributes.strength);
    out_string += print_stat("Agility: ", character.total_attributes.agility);
    out_string += print_stat("Hit: ", character.total_special_stats.hit);
    out_string += print_stat("Crit: ", character.total_special_stats.critical_strike);
    out_string += print_stat("Attack Power: ", character.total_special_stats.attack_power);
    out_string += print_stat("Haste: ", character.total_special_stats.haste);

    out_string += print_stat("Sword skill: ", character.total_special_stats.sword_skill);
    out_string += print_stat("Axe skill: ", character.total_special_stats.axe_skill);
    out_string += print_stat("Dagger skill: ", character.total_special_stats.dagger_skill);
    out_string += print_stat("Mace skill: ", character.total_special_stats.mace_skill);
    out_string += print_stat("Unarmed skill: ", character.total_special_stats.fist_skill);
    out_string += "<br />";

    out_string += "Set bonuses:<br />";
    for (const auto &bonus: character.set_bonuses)
    {
        out_string += "<b>" + bonus.name + "-" + std::to_string(bonus.pieces) + "-pieces</b>";
    }

    return out_string;
}

Character get_race(const std::string &race)
{
    if (race == "human")
    {
        return {Race::human, 60};
    }
    else if (race == "gnome")
    {
        return {Race::gnome, 60};
    }
    else if (race == "dwarf")
    {
        return {Race::dwarf, 60};
    }
    else if (race == "night_elf")
    {
        return {Race::night_elf, 60};
    }
    else if (race == "orc")
    {
        return {Race::orc, 60};
    }
    else if (race == "troll")
    {
        return {Race::troll, 60};
    }
    else if (race == "undead")
    {
        return {Race::undead, 60};
    }
    else if (race == "tauren")
    {
        return {Race::tauren, 60};
    }
    else
    {
        std::cout << "Race not found!!! picking human" << "\n";
        return {Race::human, 60};
    }
}

Character
character_setup(const Armory &armory, const Buffs &buffs, const std::string &race,
                const std::vector<std::string> &armor_vec,
                const std::vector<std::string> &weapons_vec, const std::vector<std::string> &buffs_vec,
                const std::vector<std::string> &ench_vec)
{
    auto character = get_race(race);

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

    if (find_string(ench_vec, "e+8 strength"))
    {
        character.add_enchant(Socket::head, Enchant::Type::strength);
    }
    else if (find_string(ench_vec, "e+1 haste"))
    {
        character.add_enchant(Socket::head, Enchant::Type::haste);
    }

    if (find_string(ench_vec, "s+30 attack_power"))
    {
        character.add_enchant(Socket::shoulder, Enchant::Type::attack_power);
    }

    if (find_string(ench_vec, "b+3 agility"))
    {
        character.add_enchant(Socket::back, Enchant::Type::agility);
    }

    if (find_string(ench_vec, "c+3 stats"))
    {
        character.add_enchant(Socket::chest, Enchant::Type::minor_stats);
    }
    else if (find_string(ench_vec, "c+4 stats"))
    {
        character.add_enchant(Socket::chest, Enchant::Type::major_stats);
    }

    if (find_string(ench_vec, "w+7 strength"))
    {
        character.add_enchant(Socket::wrist, Enchant::Type::strength7);
    }
    else if (find_string(ench_vec, "w+9 strength"))
    {
        character.add_enchant(Socket::wrist, Enchant::Type::strength9);
    }

    if (find_string(ench_vec, "h+7 strength"))
    {
        character.add_enchant(Socket::hands, Enchant::Type::strength);
    }
    else if (find_string(ench_vec, "h+7 agility"))
    {
        character.add_enchant(Socket::hands, Enchant::Type::agility);
    }
    else if (find_string(ench_vec, "h+1 haste"))
    {
        character.add_enchant(Socket::hands, Enchant::Type::haste);
    }

    if (find_string(ench_vec, "l+8 strength"))
    {
        character.add_enchant(Socket::legs, Enchant::Type::strength);
    }
    else if (find_string(ench_vec, "l+1 haste"))
    {
        character.add_enchant(Socket::legs, Enchant::Type::haste);
    }

    if (find_string(ench_vec, "b+7 agility"))
    {
        character.add_enchant(Socket::boots, Enchant::Type::agility);
    }

    if (find_string(ench_vec, "mcrusader"))
    {
        character.add_enchant(Socket::main_hand, Enchant::Type::crusader);
    }

    if (find_string(ench_vec, "ocrusader"))
    {
        character.add_enchant(Socket::off_hand, Enchant::Type::crusader);
    }

    // rallying cry
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
    if (find_string(buffs_vec, "battle_shout"))
    {
        character.add_buff(buffs.battle_shout);
    }
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
    if (find_string(buffs_vec, "leader_of_the_pack"))
    {
        character.add_buff(buffs.leader_of_the_pack);
    }
    if (find_string(buffs_vec, "trueshot_aura"))
    {
        character.add_buff(buffs.trueshot_aura);
    }
    if (find_string(buffs_vec, "elixir_mongoose"))
    {
        character.add_buff(buffs.elixir_mongoose);
    }
    if (find_string(buffs_vec, "blessed_sunfruit"))
    {
        character.add_buff(buffs.blessed_sunfruit);
    }
    if (find_string(buffs_vec, "smoked_dessert_dumplings"))
    {
        character.add_buff(buffs.smoked_dessert_dumplings);
    }
    if (find_string(buffs_vec, "juju_power"))
    {
        character.add_buff(buffs.juju_power);
    }
    if (find_string(buffs_vec, "elixir_of_giants"))
    {
        character.add_buff(buffs.elixir_of_giants);
    }
    if (find_string(buffs_vec, "juju_might"))
    {
        character.add_buff(buffs.juju_might);
    }
    if (find_string(buffs_vec, "winterfall_firewater"))
    {
        character.add_buff(buffs.winterfall_firewater);
    }
    if (find_string(buffs_vec, "roids"))
    {
        character.add_buff(buffs.roids);
    }
    if (find_string(buffs_vec, "dense_stone_main_hand"))
    {
        character.add_weapon_buff(Socket::main_hand, buffs.dense_stone);
    }
    else if (find_string(buffs_vec, "elemental_stone_main_hand"))
    {
        character.add_buff(buffs.elemental_stone);
    }
    if (find_string(buffs_vec, "dense_stone_off_hand"))
    {
        character.add_weapon_buff(Socket::off_hand, buffs.dense_stone);
    }
    else if (find_string(buffs_vec, "elemental_stone_off_hand"))
    {
        character.add_buff(buffs.elemental_stone);
    }

    armory.compute_total_stats(character);

    return character;
}

Sim_output Sim_interface::simulate(const Sim_input &input)
{
    Armory armory{};
    Buffs buffs{};

    Character character = character_setup(armory, buffs, input.race[0], input.armor, input.weapons, input.buffs,
                                          input.enchants);

    // Simulator & Combat settings
    Combat_simulator_config config{};
    config.n_batches = input.n_simulations;
    config.sim_time = input.fight_time;
    config.opponent_level = input.target_level;

    config.curse_of_recklessness_active = input.curse_of_recklessness;
    config.faerie_fire_feral_active = input.faerie_fire;
    config.sunder_armor_active = true;
    config.n_sunder_armor_stacks = input.sunder_armor;

    config.talents.improved_heroic_strike = 2;
    config.talents.unbridled_wrath = 5;
    config.talents.flurry = 5;
    config.talents.anger_management = true;
    config.talents.death_wish = input.death_wish;
    config.talents.impale = 2;
    config.talents.improved_execute = 2;
    config.talents.dual_wield_specialization = 5;

    config.use_sim_time_ramp = true;
    config.enable_spell_rotation = true;
    config.use_mighty_rage_potion = input.mighty_rage_potion;
    config.enable_bloodrage = true;
    config.enable_recklessness = input.recklessness;
//    config.display_combat_debug = true;
    config.use_seed = true;
    config.seed = 110000;
    config.fuel_extra_rage = false;
    config.extra_rage_interval = 3;
    config.extra_rage_damage_amount = 150;
    config.mode.vaelastrasz = input.vaelastrasz;

    Combat_simulator simulator(config);

    std::vector<double> dps_vec = simulator.simulate(character);
    std::vector<double> dps_dist = get_damage_sources(simulator.get_damage_distribution());
    double mean_init = Statistics::average(dps_vec);
    double std_init = Statistics::standard_deviation(dps_vec, mean_init);
    double sample_std_init = Statistics::sample_deviation(std_init, config.n_batches);

    std::vector<std::string> aura_uptimes = simulator.get_aura_uptimes();

    auto a = simulator.get_damage_time_lapse();

    std::string extra_info_string = "Hit/crit details: <br/>";
    extra_info_string = "<b>Hit:</b> <br/>";
    int hit_left_to_yellow_cap = simulator.get_hit_probabilities_yellow()[0];
    if (hit_left_to_yellow_cap > 0)
    {
        extra_info_string += "Yellow hits: " + std::to_string(
                hit_left_to_yellow_cap) + "% hit to yellow hard-cap! <br/>";
    }
    else
    {
        extra_info_string += "Yellow hit hard-cap reached! <br/>";
    }
    extra_info_string += "Main-hand, white hits: ";
    int left_to_crit_cap_mh = simulator.get_hit_probabilities_white_mh()[0];
    if (left_to_crit_cap_mh > 0)
    {
        extra_info_string += std::to_string(left_to_crit_cap_mh) + "% left to hard-cap. <br/>";
    }
    else
    {
        extra_info_string += std::to_string(left_to_crit_cap_mh) + "% over hard-cap! <br/>";
    }

    extra_info_string += "Off-hand, white hits: ";
    int left_to_crit_cap_oh = simulator.get_hit_probabilities_white_oh()[0];
    if (left_to_crit_cap_oh > 0)
    {
        extra_info_string += std::to_string(left_to_crit_cap_oh) + "% left to hard-cap. <br/>";
    }
    else
    {
        extra_info_string += std::to_string(left_to_crit_cap_oh) + "% over hard-cap! <br/>";
    }

    extra_info_string += "<b>Crit:</b> <br/>";
    extra_info_string += "Yellow hits: ";
    int left_to_crit_cap_yellow = 100 - simulator.get_hit_probabilities_yellow().back();
    if (left_to_crit_cap_yellow > 0)
    {
        extra_info_string += std::to_string(left_to_crit_cap_yellow) + "% left to hard-cap. <br/>";
    }
    else
    {
        extra_info_string += std::to_string(left_to_crit_cap_yellow) + "% over hard-cap! <br/>";
    }
    extra_info_string += "Main-hand white hits: ";
    int left_to_white_crit_cap_mh = 100 - simulator.get_hit_probabilities_white_mh().back();
    if (left_to_white_crit_cap_mh > 0)
    {
        extra_info_string += std::to_string(left_to_white_crit_cap_mh) + "% left to crit hard-cap. <br/>";
    }
    else
    {
        extra_info_string += std::to_string(left_to_white_crit_cap_mh) + "% over crit hard-cap! <br/>";
    }
    extra_info_string += "Off-hand white hits: ";
    int left_to_white_crit_cap_oh = 100 - simulator.get_hit_probabilities_white_oh().back();
    if (left_to_white_crit_cap_oh > 0)
    {
        extra_info_string += std::to_string(left_to_white_crit_cap_oh) + "% left to crit hard-cap. <br/>";
    }
    else
    {
        extra_info_string += std::to_string(left_to_white_crit_cap_oh) + "% over crit hard-cap! <br/>";
    }
    extra_info_string += "<b>Glancing blows:</b><br/>";
    extra_info_string += "Chance to occur: " + std::to_string(
            int(simulator.get_hit_probabilities_white_oh()[2] - simulator
                    .get_hit_probabilities_white_oh()[1])) + "% (based on level difference)<br/>";
    extra_info_string += "Glancing damage main-hand: " + std::to_string(
            int(100 * simulator.get_glancing_penalty_mh())) + "% (based on weapon skill)<br/>";
    extra_info_string += "Glancing damage off-hand: " + std::to_string(
            int(100 * simulator.get_glancing_penalty_oh())) + "% (based on weapon skill)<br/>";

    std::vector<std::string> stat_weights;
    if (!input.stat_weights.empty())
    {
        Character char_plus = character;
        Character char_minus = character;
        char_plus.total_special_stats.attack_power += 100;
        char_minus.total_special_stats.attack_power -= 100;
        Stat_weight base_line = compute_stat_weight(simulator, char_plus, char_minus, "attack power", 100, mean_init,
                                                    sample_std_init);
        char_plus.total_special_stats = character.total_special_stats;
        char_minus.total_special_stats = character.total_special_stats;

        stat_weights.emplace_back("attack_power: " + std::to_string(base_line.dps_plus) + " " + std::to_string(
                base_line.std_dps_plus) + " " + std::to_string(base_line.dps_minus) + " " + std::to_string(
                base_line.std_dps_minus));

        for (const auto &stat_weight : input.stat_weights)
        {
            if (stat_weight == "crit")
            {
                char_plus.total_special_stats.critical_strike += 1;
                char_minus.total_special_stats.critical_strike -= 1;
                Stat_weight crit = compute_stat_weight(simulator, char_plus, char_minus, "crit", 1, mean_init,
                                                       sample_std_init);
                char_plus.total_special_stats = character.total_special_stats;
                char_minus.total_special_stats = character.total_special_stats;

                stat_weights.emplace_back("1%Crit " + std::to_string(crit.dps_plus) + " " + std::to_string(
                        crit.std_dps_plus) + " " + std::to_string(crit.dps_minus) + " " + std::to_string(
                        crit.std_dps_minus));
            }

            if (stat_weight == "hit")
            {
                char_plus.total_special_stats.hit += 1;
                char_minus.total_special_stats.hit -= 1;
                Stat_weight hit = compute_stat_weight(simulator, char_plus, char_minus, "hit", 1, mean_init,
                                                      sample_std_init);
                char_plus.total_special_stats = character.total_special_stats;
                char_minus.total_special_stats = character.total_special_stats;

                stat_weights.emplace_back("1%Hit " + std::to_string(hit.dps_plus) + " " + std::to_string(
                        hit.std_dps_plus) + " " + std::to_string(hit.dps_minus) + " " + std::to_string(
                        hit.std_dps_minus));
            }
            if (stat_weight == "haste")
            {
                char_plus.total_special_stats.haste = (char_plus.total_special_stats.haste + 1) * 1.01 - 1;
                char_minus.total_special_stats.haste = (char_minus.total_special_stats.haste + 1) / 1.01 - 1;
                Stat_weight hit = compute_stat_weight(simulator, char_plus, char_minus, "haste", 1, mean_init,
                                                      sample_std_init);
                char_plus.total_special_stats = character.total_special_stats;
                char_minus.total_special_stats = character.total_special_stats;

                stat_weights.emplace_back("1%Haste " + std::to_string(hit.dps_plus) + " " + std::to_string(
                        hit.std_dps_plus) + " " + std::to_string(hit.dps_minus) + " " + std::to_string(
                        hit.std_dps_minus));
            }
//            if (stat_weight == "skill")
//            {
//                char_plus.total_special_stats.hit += 1;
//                char_minus.total_special_stats.hit -= 1;
//                Stat_weight hit = compute_stat_weight(simulator, char_plus, char_minus, "hit", 1, mean_init,
//                                                      sample_std_init);
//                char_plus.total_special_stats = character.total_special_stats;
//                char_minus.total_special_stats = character.total_special_stats;
//
//                stat_weights.emplace_back("hit: " + std::to_string(hit.dps_plus) + " " + std::to_string(
//                        hit.std_dps_plus) + " " + std::to_string(hit.dps_minus) + " " + std::to_string(
//                        hit.std_dps_minus));
//            }
        }
    }

    sort(dps_vec.begin(), dps_vec.end());
    int n_bins = std::min(input.n_simulations, 20.0);
    std::vector<double> hist_x{};
    std::vector<int> hist_y{};
    double dps_per_bin = (dps_vec.back() - dps_vec[0]) / n_bins;
    double start_dps = dps_vec[0];
    for (double dps_val = start_dps; dps_val <= dps_vec.back() + .01; dps_val += dps_per_bin)
    {
        hist_x.push_back(dps_val);
        hist_y.push_back(0);
    }

    int bin_idx = 0;
    for (double i : dps_vec)
    {
        while (i > hist_x[bin_idx] + dps_per_bin / 2)
        {
            bin_idx++;
        }
        hist_y[bin_idx]++;
    }

    return {hist_x, hist_y, dps_dist, a[0], a[1], a[2], a[3], a[4], a[5], a[6],
            aura_uptimes, stat_weights, {extra_info_string}, mean_init, sample_std_init,
            {get_character_stat(character)}};
}

