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
    combat_simulator.simulate(char_plus);
    double mean_plus = combat_simulator.get_dps_mean();
    double std_plus = std::sqrt(combat_simulator.get_dps_variance());
    double sample_std_plus = Statistics::sample_deviation(std_plus, combat_simulator.get_n_simulations());

    combat_simulator.simulate(char_minus);
    double mean_minus = combat_simulator.get_dps_mean();
    double std_minus = std::sqrt(combat_simulator.get_dps_variance());
    double sample_std_minus = Statistics::sample_deviation(std_minus, combat_simulator.get_n_simulations());

    return {mean_plus - mean_init, Statistics::add_standard_deviations(sample_std_init, sample_std_plus),
            mean_minus - mean_init, Statistics::add_standard_deviations(sample_std_init, sample_std_minus),
            permute_amount, permuted_stat};
}

std::vector<double> get_damage_sources(Damage_sources damage_sources_vector)
{
    return {damage_sources_vector.white_mh_damage / damage_sources_vector.sum_damage_sources(),
            damage_sources_vector.white_oh_damage / damage_sources_vector.sum_damage_sources(),
            damage_sources_vector.bloodthirst_damage / damage_sources_vector.sum_damage_sources(),
            damage_sources_vector.execute_damage / damage_sources_vector.sum_damage_sources(),
            damage_sources_vector.heroic_strike_damage / damage_sources_vector.sum_damage_sources(),
            damage_sources_vector.cleave_damage / damage_sources_vector.sum_damage_sources(),
            damage_sources_vector.whirlwind_damage / damage_sources_vector.sum_damage_sources(),
            damage_sources_vector.hamstring_damage / damage_sources_vector.sum_damage_sources(),
            damage_sources_vector.item_hit_effects_damage / damage_sources_vector.sum_damage_sources()};
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


std::string percent_to_str(const std::string &stat_name, double value, const std::string &description)
{
    std::ostringstream stream;
    stream << stat_name << ": " << std::setprecision(3) << "<b>" << value << "%</b> " << description << "<br>";
    return stream.str();
}

std::string percent_to_str(const std::string &stat_name, double value1, const std::string &description1,
                           double value2, const std::string &description2)
{
    std::ostringstream stream;
    stream << stat_name << ": " << std::setprecision(3) << "<b>" << value1 << "%</b> " << description1 << ". (<b>"
           << value2 << "%</b> " << description2 << ")<br>";
    return stream.str();
}

std::string print_stat(const std::string &stat_name, double amount)
{
    std::ostringstream stream;
    stream << stat_name << std::setprecision(4) << "<b>" << amount << "</b><br>";
    return stream.str();
}

std::string print_stat(const std::string &stat_name, double amount1, double amount2)
{
    std::ostringstream stream;
    stream << stat_name << std::setprecision(4) << "<b>" << amount1 << " &#8594 " << amount2 << "</b><br>";
    return stream.str();
}

std::string get_character_stat(const Character &char1, const Character &char2)
{
    std::string out_string = "<b>Setup 1 &#8594 Setup 2</b> <br>";
    out_string += print_stat("Strength: ", char1.total_attributes.strength, char2.total_attributes.strength);
    out_string += print_stat("Agility: ", char1.total_attributes.agility, char2.total_attributes.agility);
    out_string += print_stat("Hit: ", char1.total_special_stats.hit, char2.total_special_stats.hit);
    out_string += print_stat("Crit (spellbook):", char1.total_special_stats.critical_strike,
                             char2.total_special_stats.critical_strike);
    out_string += print_stat("Attack Power: ", char1.total_special_stats.attack_power,
                             char2.total_special_stats.attack_power);
    out_string += print_stat("Haste factor: ", 1 + char1.total_special_stats.haste,
                             1 + char2.total_special_stats.haste);

    out_string += "<br><b>Armor:</b><br>";
    for (size_t i = 0; i < char1.armor.size(); i++)
    {
        if (char1.armor[i].name != char2.armor[i].name)
        {
            out_string += char1.armor[i].name + " &#8594 " + char2.armor[i].name + "<br>";
        }
    }

    out_string += "<br><b>Weapons:</b><br>";
    for (size_t i = 0; i < char1.weapons.size(); i++)
    {
        if (char1.weapons[i].name != char2.weapons[i].name)
        {
            if (i == 0)
            {
                out_string += "Mainhand: ";
            }
            else
            {
                out_string += "Offhand: ";
            }
            out_string += char1.weapons[i].name + " &#8594 " + char2.weapons[i].name + "<br><br>";
        }
    }

    out_string += "Set bonuses setup 1:<br>";
    for (const auto &bonus: char1.set_bonuses)
    {
        out_string += "<b>" + bonus.name + "-" + std::to_string(bonus.pieces) + "-pieces</b><br>";
    }

    out_string += "<br>Set bonuses setup 2:<br>";
    for (const auto &bonus: char1.set_bonuses)
    {
        out_string += "<b>" + bonus.name + "-" + std::to_string(bonus.pieces) + "-pieces</b><br>";
    }

    return out_string;
}

std::string get_character_stat(const Character &character)
{
    std::string out_string = "<b>Character stats:</b> <br />";
    out_string += print_stat("Strength: ", character.total_attributes.strength);
    out_string += print_stat("Agility: ", character.total_attributes.agility);
    out_string += print_stat("Hit: ", character.total_special_stats.hit);
    out_string += print_stat("Crit (spellbook): ", character.total_special_stats.critical_strike);
    out_string += print_stat("Attack Power: ", character.total_special_stats.attack_power);
    out_string += print_stat("Haste factor: ", 1 + character.total_special_stats.haste);
    if (character.weapons[0].type == Weapon_type::sword || character.weapons[1].type == Weapon_type::sword)
    {
        out_string += print_stat("Sword skill: ", character.total_special_stats.sword_skill);
    }
    if (character.weapons[0].type == Weapon_type::axe || character.weapons[1].type == Weapon_type::axe)
    {
        out_string += print_stat("Axe skill: ", character.total_special_stats.axe_skill);
    }
    if (character.weapons[0].type == Weapon_type::dagger || character.weapons[1].type == Weapon_type::dagger)
    {
        out_string += print_stat("Dagger skill: ", character.total_special_stats.dagger_skill);
    }
    if (character.weapons[0].type == Weapon_type::mace || character.weapons[1].type == Weapon_type::mace)
    {
        out_string += print_stat("Mace skill: ", character.total_special_stats.mace_skill);
    }
    if (character.weapons[0].type == Weapon_type::unarmed || character.weapons[1].type == Weapon_type::unarmed)
    {
        out_string += print_stat("Unarmed skill: ", character.total_special_stats.fist_skill);
    }
    out_string += "<br />";

    out_string += "Set bonuses:<br />";
    for (const auto &bonus: character.set_bonuses)
    {
        out_string += "<b>" + bonus.name + "-" + std::to_string(bonus.pieces) + "-pieces</b><br>";
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
    else if (find_string(ench_vec, "h+15 agility"))
    {
        character.add_enchant(Socket::hands, Enchant::Type::greater_agility);
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
    if (find_string(buffs_vec, "windfury_totem"))
    {
        character.add_buff(buffs.windfury_totem);
    }
    if (find_string(buffs_vec, "strength_of_earth_totem"))
    {
        character.add_buff(buffs.strength_of_earth_totem);
    }
    if (find_string(buffs_vec, "grace_of_air_totem"))
    {
        character.add_buff(buffs.grace_of_air_totem);
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
    if (find_string(buffs_vec, "mighty_rage_potion"))
    {
        character.add_buff(buffs.mighty_rage_potion);
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

    auto temp_buffs = input.buffs;

    if (find_string(input.options, "mighty_rage_potion"))
    {
        //temporary solution
        temp_buffs.emplace_back("mighty_rage_potion");
    }

    Character character = character_setup(armory, buffs, input.race[0], input.armor, input.weapons, temp_buffs,
                                          input.enchants);

    // Simulator & Combat settings
    Combat_simulator_config config{};
    config.n_batches = input.n_simulations;
    config.n_batches_statweights = input.n_simulations_stat_weights;
    config.sim_time = input.fight_time;
    config.opponent_level = input.target_level;

    if (find_string(input.options, "curse_of_recklessness"))
    {
        config.curse_of_recklessness_active = true;
    }
    if (find_string(input.options, "faerie_fire"))
    {
        config.faerie_fire_feral_active = true;
    }
    if (find_string(input.options, "death_wish"))
    {
        config.talents.death_wish = true;
    }
    if (find_string(input.options, "recklessness"))
    {
        config.enable_recklessness = true;
    }
    if (find_string(input.options, "mighty_rage_potion"))
    {
        config.enable_recklessness = true;
    }
    if (find_string(input.options, "sulfuron_harbinger"))
    {
        config.mode.sulfuron_harbinger = true;
    }
    if (find_string(input.options, "golemagg"))
    {
        config.mode.golemagg = true;
    }
    if (find_string(input.options, "vaelastrasz"))
    {
        config.mode.vaelastrasz = true;
    }
    if (find_string(input.options, "chromaggus"))
    {
        config.mode.chromaggus = true;
    }
    if (find_string(input.options, "use_bt_in_exec_phase"))
    {
        config.combat.use_bt_in_exec_phase = true;
    }
    if (find_string(input.options, "use_hs_in_exec_phase"))
    {
        config.combat.use_hs_in_exec_phase = true;
    }
    if (find_string(input.options, "cleave_if_adds"))
    {
        config.combat.cleave_if_adds = true;
    }
    if (find_string(input.options, "use_hamstring"))
    {
        config.combat.use_hamstring = true;
    }
    config.n_sunder_armor_stacks = input.sunder_armor;

    config.talents.improved_heroic_strike = 2;
    config.talents.unbridled_wrath = 5;
    config.talents.flurry = 5;
    config.talents.anger_management = true;
    config.talents.impale = 2;
    config.talents.improved_execute = 2;
    config.talents.dual_wield_specialization = 5;

    config.combat.heroic_strike_rage_thresh = input.heroic_strike_rage_thresh;
    config.combat.cleave_rage_thresh = input.cleave_rage_thresh;
    config.combat.whirlwind_rage_thresh = input.whirlwind_rage_thresh;
    config.combat.hamstring_cd_thresh = input.hamstring_cd_thresh;
    config.combat.hamstring_thresh_dd = input.hamstring_thresh_dd;
    config.combat.initial_rage = input.initial_rage;

    config.use_sim_time_ramp = true;
    config.enable_bloodrage = true;
    config.use_seed = true;
    config.seed = 110000;
    config.fuel_extra_rage = false;
    config.extra_rage_interval = 3;
    config.extra_rage_damage_amount = 150;

    Combat_simulator simulator(config);

    simulator.simulate(character, true, true);

    std::string character_stats;
    std::vector<double> mean_dps_vec;
    std::vector<double> sample_std_dps_vec;

    auto hist_x = simulator.get_hist_x();
    auto hist_y = simulator.get_hist_y();

    std::vector<double> dps_dist = get_damage_sources(simulator.get_damage_distribution());
    double mean_init = simulator.get_dps_mean();
    double std_init = std::sqrt(simulator.get_dps_variance());
    double sample_std_init = Statistics::sample_deviation(std_init, config.n_batches);

    mean_dps_vec.push_back(mean_init);
    sample_std_dps_vec.push_back(sample_std_init);
    character_stats = get_character_stat(character);
    if (input.compare_armor.size() == 15 && input.compare_weapons.size() == 2)
    {
        Combat_simulator simulator_compare(config);
        Character character2 = character_setup(armory, buffs, input.race[0], input.compare_armor,
                                               input.compare_weapons, temp_buffs, input.enchants);

        simulator_compare.simulate(character2);

        double mean_init_2 = simulator_compare.get_dps_mean();
        double std_init_2 = std::sqrt(simulator_compare.get_dps_variance());
        double sample_std_init_2 = Statistics::sample_deviation(std_init_2, config.n_batches);

        character_stats = get_character_stat(character, character2);

        mean_dps_vec.push_back(mean_init_2);
        sample_std_dps_vec.push_back(sample_std_init_2);
    }

    std::vector<std::string> aura_uptimes = simulator.get_aura_uptimes();
    std::vector<std::string> proc_statistics = simulator.get_proc_statistics();
    auto a = simulator.get_damage_time_lapse();
    auto yellow_ht = simulator.get_hit_probabilities_yellow();
    auto white_mh_ht = simulator.get_hit_probabilities_white_mh();
    auto white_oh_ht = simulator.get_hit_probabilities_white_oh();

    std::string extra_info_string = "<b>Fight stats vs. target:</b> <br/>";
    extra_info_string += "<b>Hit:</b> <br/>";
    double yellow_miss_chance = yellow_ht[0];
    double white_mh__miss_chance = white_mh_ht[0];
    double white_oh__miss_chance = white_oh_ht[0];
    extra_info_string += percent_to_str("Yellow hits", yellow_miss_chance, "chance to miss");
    extra_info_string += percent_to_str("Main-hand, white hits", white_mh__miss_chance, "chance to miss");
    extra_info_string += percent_to_str("off-hand, white hits", white_oh__miss_chance, "chance to miss");

    extra_info_string += "<b>Crit:</b> <br/>";
    double yellow_crit = std::min(yellow_ht[3], 100.0) - yellow_ht[2];
    double left_to_crit_cap_yellow = std::max(100.0 - yellow_ht.back(), 0.0);
    double white_mh_crit = std::min(white_mh_ht[3], 100.0) - white_mh_ht[2];
    double left_to_crit_cap_white_mh = std::max(100.0 - white_mh_ht.back(), 0.0);
    double white_oh_crit = std::min(white_oh_ht[3], 100.0) - white_oh_ht[2];
    double left_to_crit_cap_white_oh = std::max(100.0 - white_oh_ht.back(), 0.0);
    extra_info_string += percent_to_str("Yellow hits", yellow_crit, "chance to crit", left_to_crit_cap_yellow,
                                        "left to crit-cap");
    extra_info_string += percent_to_str("White hits main hand", white_mh_crit, "chance to crit",
                                        left_to_crit_cap_white_mh, "left to crit-cap");
    extra_info_string += percent_to_str("White hits off hand", white_oh_crit, "chance to crit",
                                        left_to_crit_cap_white_oh, "left to crit-cap");

    extra_info_string += "<b>Glancing blows:</b><br/>";
    double glancing_probability = white_oh_ht[2] - white_oh_ht[1];
    double glancing_penalty_mh = 100 * simulator.get_glancing_penalty_mh();
    double glancing_penalty_oh = 100 * simulator.get_glancing_penalty_oh();
    extra_info_string += percent_to_str("Chance to occur", glancing_probability, "(based on level difference)");
    extra_info_string += percent_to_str("Glancing damage main-hand", glancing_penalty_mh,
                                        "(based on skill difference)");
    extra_info_string += percent_to_str("Glancing damage off-hand", glancing_penalty_oh, "(based on skill difference)");

    extra_info_string += "<b>Other:</b><br/>";
    double dodge_chance = yellow_ht[1] - yellow_ht[0];
    extra_info_string += percent_to_str("Target dodge chance", dodge_chance, "(based on skill difference)");

    config.n_batches = input.n_simulations_stat_weights;
    simulator.set_config(config);
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
            if (stat_weight == "skill")
            {
                std::string name{};
                switch (char_plus.weapons[0].type)
                {
                    case Weapon_type::axe:
                        char_plus.total_special_stats.axe_skill += 5;
                        char_minus.total_special_stats.axe_skill -= 5;
                        name = "5-axe-skill ";
                        break;
                    case Weapon_type::sword:
                        char_plus.total_special_stats.sword_skill += 5;
                        char_minus.total_special_stats.sword_skill -= 5;
                        name = "5-sword-skill ";
                        break;
                    case Weapon_type::mace:
                        char_plus.total_special_stats.mace_skill += 5;
                        char_minus.total_special_stats.mace_skill -= 5;
                        name = "5-mace-skill ";
                        break;
                    case Weapon_type::dagger:
                        char_plus.total_special_stats.dagger_skill += 5;
                        char_minus.total_special_stats.dagger_skill -= 5;
                        name = "5-dagger-skill ";
                        break;
                    case Weapon_type::unarmed:
                        char_plus.total_special_stats.fist_skill += 5;
                        char_minus.total_special_stats.fist_skill -= 5;
                        name = "5-unarmed-skill ";
                        break;
                }

                Stat_weight hit = compute_stat_weight(simulator, char_plus, char_minus, "skill", 5, mean_init,
                                                      sample_std_init);
                char_plus.total_special_stats = character.total_special_stats;
                char_minus.total_special_stats = character.total_special_stats;

                stat_weights.emplace_back(name + std::to_string(hit.dps_plus) + " " + std::to_string(
                        hit.std_dps_plus) + " " + std::to_string(hit.dps_minus) + " " + std::to_string(
                        hit.std_dps_minus));
            }
        }
    }

    std::string debug_topic{};
    if (find_string(input.options, "debug_on"))
    {
        config.display_combat_debug = true;
        simulator = Combat_simulator(config);
        double dps;
        for (int i = 0; i < 1000; i++)
        {
            simulator.simulate(character);
            dps = simulator.get_dps_mean();
            if (std::abs(dps - mean_init) < 5)
            {
                break;
            }
        }
        debug_topic = simulator.get_debug_topic();

        debug_topic += "<br><br>";
        debug_topic += "Fight statistics:<br>";
        debug_topic += "DPS: " + std::to_string(dps) + "<br><br>";

        auto dist = simulator.get_damage_distribution();
        debug_topic += "DPS from sources:<br>";
        debug_topic += "DPS white MH: " + std::to_string(dist.white_mh_damage / config.sim_time) + "<br>";
        debug_topic += "DPS white OH: " + std::to_string(dist.white_oh_damage / config.sim_time) + "<br>";
        debug_topic += "DPS bloodthirst: " + std::to_string(dist.bloodthirst_damage / config.sim_time) + "<br>";
        debug_topic += "DPS execute: " + std::to_string(dist.execute_damage / config.sim_time) + "<br>";
        debug_topic += "DPS heroic strike: " + std::to_string(dist.heroic_strike_damage / config.sim_time) + "<br>";
        debug_topic += "DPS heroic strike: " + std::to_string(dist.cleave_damage / config.sim_time) + "<br>";
        debug_topic += "DPS whirlwind: " + std::to_string(dist.whirlwind_damage / config.sim_time) + "<br>";
        debug_topic += "DPS hamstring: " + std::to_string(dist.hamstring_damage / config.sim_time) + "<br>";
        debug_topic += "DPS item effects: " + std::to_string(
                dist.item_hit_effects_damage / config.sim_time) + "<br><br>";

        debug_topic += "Casts:<br>";
        debug_topic += "#Hits white MH: " + std::to_string(dist.white_mh_count) + "<br>";
        debug_topic += "#Hits white OH: " + std::to_string(dist.white_oh_count) + "<br>";
        debug_topic += "#Hits bloodthirst: " + std::to_string(dist.bloodthirst_count) + "<br>";
        debug_topic += "#Hits execute: " + std::to_string(dist.execute_count) + "<br>";
        debug_topic += "#Hits heroic strike: " + std::to_string(dist.heroic_strike_count) + "<br>";
        debug_topic += "#Hits heroic strike: " + std::to_string(dist.cleave_count) + "<br>";
        debug_topic += "#Hits whirlwind: " + std::to_string(dist.whirlwind_count) + "<br>";
        debug_topic += "#Hits hamstring: " + std::to_string(dist.hamstring_count) + "<br>";
        debug_topic += "#Hits item effects: " + std::to_string(dist.item_hit_effects_count) + "<br>";
    }

    return {hist_x, hist_y, dps_dist, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8],
            aura_uptimes, proc_statistics, stat_weights, {extra_info_string, debug_topic}, mean_dps_vec,
            sample_std_dps_vec, {character_stats}};
}

