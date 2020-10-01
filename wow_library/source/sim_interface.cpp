#include "sim_interface.hpp"

#include "Armory.hpp"
#include "Helper_functions.hpp"

#include <Character.hpp>
#include <Combat_simulator.hpp>
#include <Item_optimizer.hpp>
#include <sstream>

void item_upgrades(std::string& item_strengths_string, Character character_new, Item_optimizer& item_optimizer,
                   Armory& armory, std::vector<size_t> batches_per_iteration,
                   std::vector<size_t> cumulative_simulations, Combat_simulator& simulator, double dps_mean,
                   double dps_sample_std, Socket socket, const Special_stats& special_stats, bool first_item)
{
    std::string dummy;
    item_strengths_string = item_strengths_string + socket + ": " + "<b>" +
                            character_new.get_item_from_socket(socket, first_item).name + "</b>";
    auto armor_vec = armory.get_items_in_socket(socket);
    auto items = (socket != Socket::trinket) ?
                     item_optimizer.remove_weaker_items(armor_vec, character_new.total_special_stats, dummy) :
                     armor_vec;

    {
        size_t i = 0;
        while (i < items.size())
        {
            if (character_new.has_item(items[i].name))
            {
                items.erase(items.begin() + i);
            }
            else
            {
                i++;
            }
        }
    }

    Armor item_in_socket = character_new.get_item_from_socket(socket, first_item);
    Special_stats item_special_stats = item_in_socket.special_stats;
    item_special_stats += item_in_socket.attributes.convert_to_special_stats(special_stats);
    std::vector<size_t> stronger_indexies{};
    if (item_in_socket.set_name == Set::none && item_in_socket.use_effects.empty() &&
        item_in_socket.hit_effects.empty())
    {
        for (size_t i = 0; i < items.size(); i++)
        {
            if (items[i].set_name == Set::none && items[i].use_effects.empty() && items[i].hit_effects.empty())
            {
                Special_stats armor_special_stats = items[i].special_stats;
                armor_special_stats += items[i].attributes.convert_to_special_stats(special_stats);

                if (estimate_special_stats_smart_no_skill(item_special_stats, armor_special_stats))
                {
                    stronger_indexies.emplace_back(i);
                }
            }
        }
    }

    double quantile = Statistics::find_cdf_quantile(0.95, 0.01);
    std::string best_armor_name{};
    bool found_upgrade = false;
    for (size_t i = 0; i < items.size(); i++)
    {
        armory.change_armor(character_new.armor, items[i], first_item);
        armory.compute_total_stats(character_new);
        for (size_t iter = 0; iter < batches_per_iteration.size(); iter++)
        {
            simulator.simulate(character_new, batches_per_iteration[iter], simulator.get_dps_mean(),
                               simulator.get_dps_variance(), cumulative_simulations[iter]);
            double sample_std =
                Statistics::sample_deviation(std::sqrt(simulator.get_dps_variance()), cumulative_simulations[iter + 1]);
            if (simulator.get_dps_mean() - sample_std * quantile >= dps_mean && cumulative_simulations[iter + 1] > 5000)
            {
                found_upgrade = true;
                double dps_increase = simulator.get_dps_mean() - dps_mean;
                double dps_increase_std = Statistics::add_standard_deviations(sample_std, dps_sample_std);
                item_strengths_string += "<br> Proposed upgrade: <b>" + items[i].name + "</b> ( +<b>" +
                                         string_with_precision(dps_increase, 2) + " &plusmn " +
                                         string_with_precision(dps_increase_std, 2) + "</b> DPS).";
                break;
            }
            else if (simulator.get_dps_mean() + sample_std * quantile <= dps_mean)
            {
                if (does_vector_contain(stronger_indexies, i))
                {
                    found_upgrade = true;
                    item_strengths_string +=
                        "<br> Possible upgrade: <b>" + items[i].name + "</b>. (based on item stats)";
                }
                break;
            }
        }
    }
    if (!found_upgrade)
    {
        item_strengths_string += " is <b>BiS</b> in current configuration!<br><br>";
    }
    else
    {
        item_strengths_string += "<br><br>";
    }
}

void item_upgrades_wep(std::string& item_strengths_string, Character character_new, Item_optimizer& item_optimizer,
                       Armory& armory, std::vector<size_t> batches_per_iteration,
                       std::vector<size_t> cumulative_simulations, Combat_simulator& simulator, double dps_mean,
                       double dps_sample_std, Weapon_socket weapon_socket)
{
    std::string dummy;
    Socket socket = (weapon_socket == Weapon_socket::main_hand) ? Socket::main_hand : Socket::off_hand;
    item_strengths_string =
        item_strengths_string + socket + ": " + "<b>" + character_new.get_weapon_from_socket(socket).name + "</b>";
    auto items = armory.get_weapon_in_socket(weapon_socket);
    items = item_optimizer.remove_weaker_weapons(weapon_socket, items, character_new.total_special_stats, dummy);
    {
        size_t i = 0;
        while (i < items.size())
        {
            if (character_new.has_item(items[i].name))
            {
                items.erase(items.begin() + i);
            }
            else
            {
                i++;
            }
        }
    }

    double quantile = Statistics::find_cdf_quantile(0.95, 0.01);
    std::string best_armor_name{};
    bool found_upgrade = false;
    for (auto& item : items)
    {
        armory.change_weapon(character_new.weapons, item, socket);
        armory.compute_total_stats(character_new);
        for (size_t iter = 0; iter < batches_per_iteration.size(); iter++)
        {
            simulator.simulate(character_new, batches_per_iteration[iter], simulator.get_dps_mean(),
                               simulator.get_dps_variance(), cumulative_simulations[iter]);
            double sample_std =
                Statistics::sample_deviation(std::sqrt(simulator.get_dps_variance()), cumulative_simulations[iter + 1]);
            if (simulator.get_dps_mean() - sample_std * quantile >= dps_mean && cumulative_simulations[iter + 1] > 5000)
            {
                found_upgrade = true;
                double dps_increase = simulator.get_dps_mean() - dps_mean;
                double dps_increase_std = Statistics::add_standard_deviations(sample_std, dps_sample_std);
                item_strengths_string += "<br> Proposed upgrade: <b>" + item.name + "</b> ( +<b>" +
                                         string_with_precision(dps_increase, 2) + " &plusmn " +
                                         string_with_precision(dps_increase_std, 2) + "</b> DPS).";
                break;
            }
            else if (simulator.get_dps_mean() + sample_std * quantile <= dps_mean &&
                     cumulative_simulations[iter + 1] > 500)
            {
                break;
            }
        }
    }
    if (!found_upgrade)
    {
        item_strengths_string += " is <b>BiS</b> in current configuration!<br><br>";
    }
    else
    {
        item_strengths_string += "<br><br>";
    }
}

struct Stat_weight
{
    Stat_weight(double dps_plus, double std_dps_plus, double dps_minus, double std_dps_minus, double amount,
                std::string stat)
        : dps_plus{dps_plus}
        , std_dps_plus{std_dps_plus}
        , dps_minus{dps_minus}
        , std_dps_minus{std_dps_minus}
        , amount{amount}
        , stat{std::move(stat)} {};

    double dps_plus;
    double std_dps_plus;
    double dps_minus;
    double std_dps_minus;
    double amount;
    std::string stat;
};

Stat_weight compute_stat_weight(Combat_simulator& combat_simulator, Character& char_plus, Character& char_minus,
                                const std::string& permuted_stat, double permute_amount, double mean_init,
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

    return {mean_plus - mean_init,  Statistics::add_standard_deviations(sample_std_init, sample_std_plus),
            mean_minus - mean_init, Statistics::add_standard_deviations(sample_std_init, sample_std_minus),
            permute_amount,         permuted_stat};
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
            damage_sources_vector.deep_wounds_damage / damage_sources_vector.sum_damage_sources(),
            damage_sources_vector.item_hit_effects_damage / damage_sources_vector.sum_damage_sources()};
}

std::string print_stat(const std::string& stat_name, double amount)
{
    std::ostringstream stream;
    stream << stat_name << std::setprecision(4) << "<b>" << amount << "</b><br>";
    return stream.str();
}

std::string print_stat(const std::string& stat_name, double amount1, double amount2)
{
    std::ostringstream stream;
    stream << stat_name << std::setprecision(4) << "<b>" << amount1 << " &#8594 " << amount2 << "</b><br>";
    return stream.str();
}

std::string get_character_stat(const Character& char1, const Character& char2)
{
    std::string out_string = "<b>Setup 1 &#8594 Setup 2</b> <br>";
    out_string += print_stat("Strength: ", char1.total_attributes.strength, char2.total_attributes.strength);
    out_string += print_stat("Agility: ", char1.total_attributes.agility, char2.total_attributes.agility);
    out_string += print_stat("Hit: ", char1.total_special_stats.hit, char2.total_special_stats.hit);
    out_string += print_stat("Crit (spellbook):", char1.total_special_stats.critical_strike,
                             char2.total_special_stats.critical_strike);
    out_string +=
        print_stat("Attack Power: ", char1.total_special_stats.attack_power, char2.total_special_stats.attack_power);
    out_string +=
        print_stat("Haste factor: ", 1 + char1.total_special_stats.haste, 1 + char2.total_special_stats.haste);

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
    for (const auto& bonus : char1.set_bonuses)
    {
        out_string += "<b>" + bonus.name + "-" + std::to_string(bonus.pieces) + "-pieces</b><br>";
    }

    out_string += "<br>Set bonuses setup 2:<br>";
    for (const auto& bonus : char1.set_bonuses)
    {
        out_string += "<b>" + bonus.name + "-" + std::to_string(bonus.pieces) + "-pieces</b><br>";
    }

    return out_string;
}

std::string get_character_stat(const Character& character)
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
    for (const auto& bonus : character.set_bonuses)
    {
        out_string += "<b>" + bonus.name + "-" + std::to_string(bonus.pieces) + "-pieces</b><br>";
    }

    return out_string;
}

Character character_setup(const Armory& armory, const std::string& race, const std::vector<std::string>& armor_vec,
                          const std::vector<std::string>& weapons_vec, const std::vector<std::string>& buffs_vec,
                          const std::vector<std::string>& ench_vec)
{
    auto character = get_character_of_race(race);

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

    armory.add_enchants_to_character(character, ench_vec);
    armory.add_buffs_to_character(character, buffs_vec);

    armory.compute_total_stats(character);

    return character;
}

Sim_output Sim_interface::simulate(const Sim_input& input)
{
    Armory armory{};
    Buffs buffs{};

    auto temp_buffs = input.buffs;

    if (find_string(input.options, "mighty_rage_potion"))
    {
        // temporary solution
        temp_buffs.emplace_back("mighty_rage_potion");
    }

    Character character =
        character_setup(armory, input.race[0], input.armor, input.weapons, temp_buffs, input.enchants);

    // Simulator & Combat settings
    Combat_simulator_config config{};
    config.n_batches = input.n_simulations;
    config.n_batches_statweights = input.n_simulations_stat_weights;
    config.sim_time = input.fight_time;
    config.opponent_level = input.target_level;

    if (find_string(input.options, "exposed_armor"))
    {
        config.exposed_armor = true;
    }
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
    if (find_string(input.options, "deep_wounds"))
    {
        config.combat.deep_wounds = true;
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
    //    config.performance_mode = true;

    Combat_simulator simulator{};
    simulator.set_config(config);

    simulator.simulate(character, 0, true, true);
    double dps_mean = simulator.get_dps_mean();
    double dps_sample_std = Statistics::sample_deviation(std::sqrt(simulator.get_dps_variance()), input.n_simulations);

    std::vector<double> mean_dps_vec;
    std::vector<double> sample_std_dps_vec;

    auto hist_x = simulator.get_hist_x();
    auto hist_y = simulator.get_hist_y();

    std::vector<double> dps_dist_raw = get_damage_sources(simulator.get_damage_distribution());
    double mean_init = simulator.get_dps_mean();
    double std_init = std::sqrt(simulator.get_dps_variance());
    double sample_std_init = Statistics::sample_deviation(std_init, config.n_batches);

    std::vector<std::string> aura_uptimes = simulator.get_aura_uptimes();
    std::vector<std::string> proc_statistics = simulator.get_proc_statistics();
    auto damage_time_lapse_raw = simulator.get_damage_time_lapse();
    std::vector<std::string> time_lapse_names;
    std::vector<std::vector<double>> damage_time_lapse;
    std::vector<double> dps_dist;
    std::vector<std::string> damage_names = {"White MH", "White OH",  "Bloodthirst", "Execute",     "Heroic Strike",
                                             "Cleave",   "Whirlwind", "Hamstring",   "Deep Wounds", "Item Hit Effects"};
    for (size_t i = 0; i < damage_time_lapse_raw.size(); i++)
    {
        double total_damage = 0;
        for (const auto& damage : damage_time_lapse_raw[i])
        {
            total_damage += damage;
        }
        if (total_damage > 0)
        {
            time_lapse_names.push_back(damage_names[i]);
            damage_time_lapse.push_back(damage_time_lapse_raw[i]);
            dps_dist.push_back(dps_dist_raw[i]);
        }
    }

    auto yellow_ht = simulator.get_hit_probabilities_yellow();
    auto white_mh_ht = simulator.get_hit_probabilities_white_mh();
    auto white_oh_ht = simulator.get_hit_probabilities_white_oh();
    auto white_oh_ht_2h = simulator.get_hit_probabilities_white_2h();

    mean_dps_vec.push_back(mean_init);
    sample_std_dps_vec.push_back(sample_std_init);
    std::string character_stats = get_character_stat(character);

    config.performance_mode = true;
    if (input.compare_armor.size() == 15 && input.compare_weapons.size() == 2)
    {
        Combat_simulator simulator_compare{};
        simulator_compare.set_config(config);
        Character character2 = character_setup(armory, input.race[0], input.compare_armor, input.compare_weapons,
                                               temp_buffs, input.enchants);

        simulator_compare.simulate(character2);

        double mean_init_2 = simulator_compare.get_dps_mean();
        double std_init_2 = std::sqrt(simulator_compare.get_dps_variance());
        double sample_std_init_2 = Statistics::sample_deviation(std_init_2, config.n_batches);

        character_stats = get_character_stat(character, character2);

        mean_dps_vec.push_back(mean_init_2);
        sample_std_dps_vec.push_back(sample_std_init_2);
    }

    std::string extra_info_string = "<b>Fight stats vs. target:</b> <br/>";
    extra_info_string += "<b>Hit:</b> <br/>";
    double yellow_miss_chance = yellow_ht[0];
    double white_mh_miss_chance = white_mh_ht[0];
    double white_oh_miss_chance = white_oh_ht[0];
    double white_oh_2h_miss_chance = white_oh_ht_2h[0];
    extra_info_string += percent_to_str("Yellow hits", yellow_miss_chance, "chance to miss");
    extra_info_string += percent_to_str("Main-hand, white hits", white_mh_miss_chance, "chance to miss");
    extra_info_string += percent_to_str("off-hand, white hits", white_oh_miss_chance, "chance to miss");
    extra_info_string += percent_to_str("off-hand, while ability queued", white_oh_2h_miss_chance, "chance to miss");

    extra_info_string += "<b>Crit chance:</b> <br/>";
    double white_mh_crit = std::min(white_mh_ht[3], 100.0) - white_mh_ht[2];
    double left_to_crit_cap_white_mh = std::max(100.0 - white_mh_ht.back(), 0.0);
    double yellow_crit = std::min(yellow_ht[3], 100.0) - yellow_ht[2];
    double white_oh_crit = std::min(white_oh_ht[3], 100.0) - white_oh_ht[2];
    double left_to_crit_cap_white_oh = std::max(100.0 - white_oh_ht.back(), 0.0);
    extra_info_string += percent_to_str("Yellow", yellow_crit, "chance to crit per cast (double roll suppression)",
                                        100 - white_mh_crit, "left to crit-cap");
    extra_info_string += percent_to_str("White main hand", white_mh_crit, "chance to crit", left_to_crit_cap_white_mh,
                                        "left to crit-cap");
    extra_info_string += percent_to_str("White off hand", white_oh_crit, "chance to crit", left_to_crit_cap_white_oh,
                                        "left to crit-cap");

    extra_info_string += "<b>Glancing blows:</b><br/>";
    double glancing_probability = white_oh_ht[2] - white_oh_ht[1];
    double glancing_penalty_mh = 100 * simulator.get_glancing_penalty_mh();
    double glancing_penalty_oh = 100 * simulator.get_glancing_penalty_oh();
    extra_info_string += percent_to_str("Chance to occur", glancing_probability, "(based on level difference)");
    extra_info_string +=
        percent_to_str("Glancing damage main-hand", glancing_penalty_mh, "(based on skill difference)");
    extra_info_string += percent_to_str("Glancing damage off-hand", glancing_penalty_oh, "(based on skill difference)");

    extra_info_string += "<b>Other:</b><br/>";
    double dodge_chance = yellow_ht[1] - yellow_ht[0];
    extra_info_string += percent_to_str("Target dodge chance", dodge_chance, "(based on skill difference)");

    std::string item_strengths_string;
    if (find_string(input.options, "item_strengths"))
    {
        item_strengths_string = "<b>Character items and proposed upgrades:</b><br>";
        //                std::vector<size_t> batches_per_iteration = {100, 200, 500, 1000};
        //                std::vector<size_t> cumulative_simulations = {0, 100, 300, 800, 1800};

        std::vector<size_t> batches_per_iteration = {100};
        std::vector<size_t> cumulative_simulations = {0};
        for (int i = 0; i < 25; i++)
        {
            batches_per_iteration.push_back(batches_per_iteration.back() * 1.2);
            cumulative_simulations.push_back(cumulative_simulations.back() + batches_per_iteration[i]);
        }
        cumulative_simulations.push_back(cumulative_simulations.back() + batches_per_iteration.back());

        Combat_simulator simulator_strength{};
        simulator_strength.set_config(config);
        Item_optimizer item_optimizer{};
        Character character_new =
            character_setup(armory, input.race[0], input.armor, input.weapons, temp_buffs, input.enchants);
        std::string dummy{};
        std::vector<Socket> all_sockets = {
            Socket::head,
            Socket::neck,
            Socket::shoulder,
            Socket::back,
            Socket::chest,
            Socket::wrist,
            Socket::hands,
            Socket::belt,
            Socket::legs,
            Socket::boots,
            Socket::ring,
            Socket::trinket,
            //                        Socket::main_hand, Socket::off_hand,
            Socket::ranged,
        };
        for (auto socket : all_sockets)
        {
            if (socket == Socket::ring || socket == Socket::trinket)
            {
                item_upgrades(item_strengths_string, character_new, item_optimizer, armory, batches_per_iteration,
                              cumulative_simulations, simulator, dps_mean, dps_sample_std, socket,
                              character_new.total_special_stats, true);
                item_upgrades(item_strengths_string, character_new, item_optimizer, armory, batches_per_iteration,
                              cumulative_simulations, simulator, dps_mean, dps_sample_std, socket,
                              character_new.total_special_stats, false);
            }
            else
            {
                item_upgrades(item_strengths_string, character_new, item_optimizer, armory, batches_per_iteration,
                              cumulative_simulations, simulator, dps_mean, dps_sample_std, socket,
                              character_new.total_special_stats, true);
            }
        }

        item_upgrades_wep(item_strengths_string, character_new, item_optimizer, armory, batches_per_iteration,
                          cumulative_simulations, simulator, dps_mean, dps_sample_std, Weapon_socket::main_hand);
        item_upgrades_wep(item_strengths_string, character_new, item_optimizer, armory, batches_per_iteration,
                          cumulative_simulations, simulator, dps_mean, dps_sample_std, Weapon_socket::off_hand);
        //        item_strengths_string += "<b>Weapon</b> proposals coming soon!<br>";
        item_strengths_string += "<br><br>";
    }

    config.n_batches = input.n_simulations_stat_weights;
    simulator.set_config(config);
    std::vector<std::string> stat_weights;
    if (!input.stat_weights.empty())
    {
        Character char_plus = character;
        Character char_minus = character;
        char_plus.total_special_stats.attack_power += 100;
        char_minus.total_special_stats.attack_power -= 100;
        Stat_weight base_line =
            compute_stat_weight(simulator, char_plus, char_minus, "attack power", 100, mean_init, sample_std_init);
        char_plus.total_special_stats = character.total_special_stats;
        char_minus.total_special_stats = character.total_special_stats;

        stat_weights.emplace_back("attack_power: " + std::to_string(base_line.dps_plus) + " " +
                                  std::to_string(base_line.std_dps_plus) + " " + std::to_string(base_line.dps_minus) +
                                  " " + std::to_string(base_line.std_dps_minus));

        for (const auto& stat_weight : input.stat_weights)
        {
            if (stat_weight == "crit")
            {
                char_plus.total_special_stats.critical_strike += 1;
                char_minus.total_special_stats.critical_strike -= 1;
                Stat_weight crit =
                    compute_stat_weight(simulator, char_plus, char_minus, "crit", 1, mean_init, sample_std_init);
                char_plus.total_special_stats = character.total_special_stats;
                char_minus.total_special_stats = character.total_special_stats;

                stat_weights.emplace_back("1%Crit " + std::to_string(crit.dps_plus) + " " +
                                          std::to_string(crit.std_dps_plus) + " " + std::to_string(crit.dps_minus) +
                                          " " + std::to_string(crit.std_dps_minus));
            }

            if (stat_weight == "hit")
            {
                char_plus.total_special_stats.hit += 1;
                char_minus.total_special_stats.hit -= 1;
                Stat_weight hit =
                    compute_stat_weight(simulator, char_plus, char_minus, "hit", 1, mean_init, sample_std_init);
                char_plus.total_special_stats = character.total_special_stats;
                char_minus.total_special_stats = character.total_special_stats;

                stat_weights.emplace_back("1%Hit " + std::to_string(hit.dps_plus) + " " +
                                          std::to_string(hit.std_dps_plus) + " " + std::to_string(hit.dps_minus) + " " +
                                          std::to_string(hit.std_dps_minus));
            }
            if (stat_weight == "haste")
            {
                char_plus.total_special_stats.haste = (char_plus.total_special_stats.haste + 1) * 1.01 - 1;
                char_minus.total_special_stats.haste = (char_minus.total_special_stats.haste + 1) / 1.01 - 1;
                Stat_weight hit =
                    compute_stat_weight(simulator, char_plus, char_minus, "haste", 1, mean_init, sample_std_init);
                char_plus.total_special_stats = character.total_special_stats;
                char_minus.total_special_stats = character.total_special_stats;

                stat_weights.emplace_back("1%Haste " + std::to_string(hit.dps_plus) + " " +
                                          std::to_string(hit.std_dps_plus) + " " + std::to_string(hit.dps_minus) + " " +
                                          std::to_string(hit.std_dps_minus));
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

                Stat_weight hit =
                    compute_stat_weight(simulator, char_plus, char_minus, "skill", 5, mean_init, sample_std_init);
                char_plus.total_special_stats = character.total_special_stats;
                char_minus.total_special_stats = character.total_special_stats;

                stat_weights.emplace_back(name + std::to_string(hit.dps_plus) + " " + std::to_string(hit.std_dps_plus) +
                                          " " + std::to_string(hit.dps_minus) + " " +
                                          std::to_string(hit.std_dps_minus));
            }
        }
    }

    std::string debug_topic{};
    if (find_string(input.options, "debug_on"))
    {
        config.display_combat_debug = true;
        config.performance_mode = false;

        simulator.set_config(config);
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
        debug_topic += "DPS cleave: " + std::to_string(dist.cleave_damage / config.sim_time) + "<br>";
        debug_topic += "DPS whirlwind: " + std::to_string(dist.whirlwind_damage / config.sim_time) + "<br>";
        debug_topic += "DPS hamstring: " + std::to_string(dist.hamstring_damage / config.sim_time) + "<br>";
        debug_topic += "DPS deep wounds: " + std::to_string(dist.deep_wounds_damage / config.sim_time) + "<br>";
        debug_topic +=
            "DPS item effects: " + std::to_string(dist.item_hit_effects_damage / config.sim_time) + "<br><br>";

        debug_topic += "Casts:<br>";
        debug_topic += "#Hits white MH: " + std::to_string(dist.white_mh_count) + "<br>";
        debug_topic += "#Hits white OH: " + std::to_string(dist.white_oh_count) + "<br>";
        debug_topic += "#Hits bloodthirst: " + std::to_string(dist.bloodthirst_count) + "<br>";
        debug_topic += "#Hits execute: " + std::to_string(dist.execute_count) + "<br>";
        debug_topic += "#Hits heroic strike: " + std::to_string(dist.heroic_strike_count) + "<br>";
        debug_topic += "#Hits cleave: " + std::to_string(dist.cleave_count) + "<br>";
        debug_topic += "#Hits whirlwind: " + std::to_string(dist.whirlwind_count) + "<br>";
        debug_topic += "#Hits hamstring: " + std::to_string(dist.hamstring_count) + "<br>";
        debug_topic += "#Hits deep_wounds: " + std::to_string(dist.deep_wounds_count) + "<br>";
        debug_topic += "#Hits item effects: " + std::to_string(dist.item_hit_effects_count) + "<br>";
    }

    return {hist_x,
            hist_y,
            dps_dist,
            time_lapse_names,
            damage_time_lapse,
            aura_uptimes,
            proc_statistics,
            stat_weights,
            {item_strengths_string + extra_info_string, debug_topic},
            mean_dps_vec,
            sample_std_dps_vec,
            {character_stats}};
}
