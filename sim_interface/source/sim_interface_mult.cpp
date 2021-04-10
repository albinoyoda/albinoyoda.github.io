#include "sim_interface.hpp"
#include "Combat_simulator.hpp"
#include "Helper_functions.hpp"
#include "Item_optimizer.hpp"
#include "Item_popularity.hpp"
#include "include/Armory.hpp"
#include "include/Character.hpp"
#include "include/Statistics.hpp"

#include <algorithm>
#include <ctime>
#include <iostream>

Sim_output_mult Sim_interface::simulate_mult(const Sim_input_mult& input)
{
    clock_t start_time_main = clock();
    Buffs buffs{};
    Item_optimizer item_optimizer;

    // Combat settings
    auto temp_buffs = input.buffs;

    if (find_string(input.options, "mighty_rage_potion"))
    {
        temp_buffs.emplace_back("mighty_rage_potion");
    }
    if (find_string(input.options, "full_polarity"))
    {
        double full_polarity_val = find_value(input.float_options_string, input.float_options_val, "full_polarity_dd");
        item_optimizer.armory.buffs.full_polarity.special_stats.damage_mod_physical = full_polarity_val / 100.0;
        item_optimizer.armory.buffs.full_polarity.special_stats.damage_mod_spell = full_polarity_val / 100.0;
        temp_buffs.emplace_back("full_polarity");
    }
    if (find_string(input.options, "fungal_bloom"))
    {
        temp_buffs.emplace_back("fungal_bloom");
    }
    if (find_string(input.options, "battle_squawk"))
    {
        double battle_squawk_val = find_value(input.float_options_string, input.float_options_val, "battle_squawk_dd");
        item_optimizer.armory.buffs.battle_squawk.special_stats.haste = battle_squawk_val / 100.0;
        temp_buffs.emplace_back("battle_squawk");
    }

    Race race = get_race(input.race[0]);
    item_optimizer.race = race;
    item_optimizer.buffs = buffs;
    item_optimizer.buffs_vec = temp_buffs;
    item_optimizer.ench_vec = input.enchants;
    item_optimizer.talent_vec = input.talent_string;
    item_optimizer.talent_val_vec = input.talent_val;
    item_optimizer.item_setup(input.armor, input.weapons);

    // Simulator & Combat settings
    Combat_simulator_config config{input};

    std::string debug_message;
    item_optimizer.compute_combinations();

    std::cout << "init. Combinations: " << std::to_string(item_optimizer.total_combinations) << "\n";
    debug_message += "Total item combinations: " + std::to_string(item_optimizer.total_combinations) + "<br>";
    clock_t start_filter = clock();
    if (item_optimizer.total_combinations > 500)
    {
        debug_message += "Applying item filter.<br>";
        {
            auto character = item_optimizer.construct(0);
            item_optimizer.filter_weaker_items(character.total_special_stats, debug_message, 1);
            item_optimizer.compute_combinations();
            size_t set_bonuses_before = item_optimizer.possible_set_bonuses.size();
            item_optimizer.find_set_bonuses();
            if (item_optimizer.possible_set_bonuses.size() != set_bonuses_before)
            {
                debug_message += "<b>Applying item filter again since a set-bonus has been knocked out of the "
                                 "pool</b><br>";
                item_optimizer.filter_weaker_items(character.total_special_stats, debug_message, 1);
                item_optimizer.compute_combinations();
            }
        }
        debug_message +=
            "Item filter done. Combinations: " + std::to_string(item_optimizer.total_combinations) + "<br>";
        std::cout << "Item filter done. Combinations: " << std::to_string(item_optimizer.total_combinations) << "\n";
    }
    std::vector<Item_optimizer::Sim_result_t> keepers;
    if (item_optimizer.total_combinations > 5000)
    {
        debug_message += "Performing a heuristic filter on the item sets.<br>";
        std::cout << "Performing a heuristic filter on the item sets.\n";
        double highest_attack_power = 0;
        double lowest_attack_power = 1e12;
        for (size_t i = 0; i < item_optimizer.total_combinations; ++i)
        {
            Character character = item_optimizer.construct(i);
            double ap_equivalent =
                (character.weapons.size() == 2) ?
                    get_character_ap_equivalent(character.total_special_stats, character.weapons[0],
                                                character.weapons[1], config.sim_time, character.use_effects) :
                    get_character_ap_equivalent(character.total_special_stats, character.weapons[0], config.sim_time,
                                                character.use_effects);
            if (ap_equivalent > highest_attack_power)
            {
                highest_attack_power = ap_equivalent;
            }
            if (ap_equivalent < lowest_attack_power)
            {
                lowest_attack_power = ap_equivalent;
            }
        }

        debug_message += "Equivalent attack power range: [" + std::to_string(lowest_attack_power) + ", " +
                         std::to_string(highest_attack_power) + "]<br>";
        std::cout << "Equivalent attack power range: [" + std::to_string(lowest_attack_power) + ", " +
                         std::to_string(highest_attack_power) + "]\n";
        double filter_value = .4;
        double filtering_ap = (1 - filter_value) * lowest_attack_power + filter_value * highest_attack_power;
        debug_message += "Removing bottom " + string_with_precision(filter_value * 100, 2) + "% sets. AP < " +
                         string_with_precision(filtering_ap, 4) + "<br>";
        std::cout << "Removing bottom " + string_with_precision(filter_value * 100, 2) + "% sets. AP < " +
                         string_with_precision(filtering_ap, 4) + "\n";
        keepers.reserve(item_optimizer.total_combinations / 2);
        for (size_t i = 0; i < item_optimizer.total_combinations; ++i)
        {
            Character character = item_optimizer.construct(i);
            double ap_equivalent =
                (character.weapons.size() == 2) ?
                    get_character_ap_equivalent(character.total_special_stats, character.weapons[0],
                                                character.weapons[1], config.sim_time, character.use_effects) :
                    get_character_ap_equivalent(character.total_special_stats, character.weapons[0], config.sim_time,
                                                character.use_effects);
            if (ap_equivalent > filtering_ap)
            {
                keepers.emplace_back(i, 0, 0, ap_equivalent);
            }
        }
        debug_message += "Set filter done. Combinations: " + std::to_string(keepers.size()) + "<br>";
        std::cout << "Set filter done. Combinations: " << std::to_string(keepers.size()) << "\n";
        if (keepers.size() > 10000)
        {
            debug_message += "To many combinations. Combinations: " + std::to_string(keepers.size()) + "<br>";
            debug_message += "Increasing filter value until <10.000 sets remaining.<br>";
            std::cout << "To many combinations. Combinations: " << std::to_string(keepers.size()) << "\n";
            std::cout << "Increasing filter value until 10.000 sets remaining.<br>\n";
            while (keepers.size() > 10000)
            {
                filter_value += .02;
                filtering_ap = (1 - filter_value) * lowest_attack_power + filter_value * highest_attack_power;
                debug_message += "Removing bottom " + string_with_precision(filter_value * 100, 2) + "% sets. AP < " +
                                 string_with_precision(filtering_ap, 4) + "<br>";
                std::cout << "Removing bottom " << string_with_precision(filter_value * 100, 2) << "%\n";
                std::vector<Item_optimizer::Sim_result_t> temp_keepers;
                temp_keepers.reserve(keepers.size());
                for (const auto& keeper : keepers)
                {
                    if (keeper.ap_equivalent > filtering_ap)
                    {
                        temp_keepers.emplace_back(keeper);
                    }
                }
                keepers = temp_keepers;
                debug_message += "Sets remaining: " + std::to_string(keepers.size()) + "<br>";
                std::cout << "Sets remaining: " << std::to_string(keepers.size()) << "\n";
            }
        }
        double time_spent_filter = double(clock() - start_filter) / (double)CLOCKS_PER_SEC;
        debug_message += "Set filter done. Combinations: " + std::to_string(keepers.size()) + "<br>";
        if (filter_value > 0.8)
        {
            debug_message +=
                "WARNING! Pushed the heuristic filter to: " + string_with_precision(filter_value * 100, 2) +
                "%. Might have filtered away good sets. <br>";
            std::cout << "WARNING! Pushed the heuristic filter to: " << string_with_precision(filter_value * 100, 2)
                      << "\n";
        }
        debug_message += "Time spent filtering: " + std::to_string(time_spent_filter) + "s.<br><br>";
    }
    else
    {
        keepers.reserve(item_optimizer.total_combinations);
        for (size_t i = 0; i < item_optimizer.total_combinations; ++i)
        {
            Character character = item_optimizer.construct(i);
            keepers.emplace_back(i, 0, 0, 0);
        }
    }

    debug_message += "Starting optimizer! Current combinations:" + std::to_string(keepers.size()) + "<br>";
    std::vector<size_t> batches_per_iteration = {20};
    std::vector<size_t> cumulative_simulations = {0};
    for (int i = 0; i < 40; i++)
    {
        batches_per_iteration.push_back(batches_per_iteration.back() * 1.2);
        cumulative_simulations.push_back(cumulative_simulations.back() + batches_per_iteration[i]);
    }
    cumulative_simulations.push_back(cumulative_simulations.back() + batches_per_iteration.back());
    size_t n_sim{};
    size_t performed_iterations{};
    double max_optimize_time = find_value(input.float_options_string, input.float_options_val, "max_optimize_time_dd");
    Combat_simulator simulator{};
    simulator.set_config(config);
    for (size_t i = 0; i < batches_per_iteration.size(); i++)
    {
        clock_t optimizer_start_time = clock();
        debug_message +=
            "Iteration " + std::to_string(i + 1) + " of " + std::to_string(batches_per_iteration.size()) + "<br>";
        debug_message += "Total keepers: " + std::to_string(keepers.size()) + "<br>";

        std::cout << "Iter: " + std::to_string(i) + ". Total keepers: " + std::to_string(keepers.size()) << "\n";

        double best_dps = 0;
        double best_dps_variance = 0;
        size_t iter = 0;
        for (auto& keeper : keepers)
        {
            Character character = item_optimizer.construct(keeper.index);
            simulator.simulate(character, batches_per_iteration[i], keeper.mean_dps, keeper.variance,
                               cumulative_simulations[i]);
            keeper.mean_dps = simulator.get_dps_mean();
            keeper.variance = simulator.get_dps_variance();
            if (keeper.mean_dps > best_dps)
            {
                best_dps = keeper.mean_dps;
                best_dps_variance = keeper.variance;
            }

            // Time taken
            n_sim += batches_per_iteration[i];
            iter++;
            if (keepers.size() < 200)
            {
                if (iter == 2)
                {
                    double time_spent = double(clock() - optimizer_start_time) / (double)CLOCKS_PER_SEC;
                    double n_samples = keepers.size() / double(iter);
                    debug_message += "Batch done in: " + std::to_string(time_spent * n_samples) + " seconds.";
                }
            }
            else
            {
                if (iter == 20)
                {
                    double time_spent = double(clock() - optimizer_start_time) / (double)CLOCKS_PER_SEC;
                    double n_samples = keepers.size() / double(iter);
                    debug_message += "Batch done in: " + std::to_string(time_spent * n_samples) + " seconds.<br>";
                }
            }
        }

        // Check if max time is exceeded
        double time = double(clock() - start_time_main) / (double)CLOCKS_PER_SEC;
        if (time > max_optimize_time)
        {
            debug_message +=
                "<b>Maximum time limit (" + string_with_precision(max_optimize_time, 3) + ") exceeded! </b><br>";
            break;
        }

        // If there are more than 10 item sets: remove weak sets
        if (keepers.size() > 5)
        {
            double quantile = Statistics::find_cdf_quantile(1 - 1 / static_cast<double>(keepers.size()), 0.01);
            double best_dps_sample_std =
                Statistics::sample_deviation(std::sqrt(best_dps_variance), cumulative_simulations[i + 1]);
            double filter_value = best_dps - quantile * best_dps_sample_std;
            debug_message += "Best combination DPS: " + std::to_string(best_dps) +
                             ", removing sets below: " + std::to_string(best_dps - quantile * best_dps_sample_std) +
                             "<br>";
            std::vector<Item_optimizer::Sim_result_t> temp_keepers;
            temp_keepers.reserve(keepers.size());
            for (const auto& keeper : keepers)
            {
                double keeper_sample_std =
                    Statistics::sample_deviation(std::sqrt(keeper.variance), cumulative_simulations[i + 1]);
                if (keeper.mean_dps + keeper_sample_std * quantile >= filter_value)
                {
                    temp_keepers.emplace_back(keeper);
                }
            }
            // Removed to many sets
            if (temp_keepers.size() < 5)
            {
                std::cout << "removed to many sets. Adding them back again\n";
                size_t attempts = 0;
                while (temp_keepers.size() < 5)
                {
                    quantile *= 1.01;
                    temp_keepers.clear();
                    for (const auto& keeper : keepers)
                    {
                        double keeper_sample_std =
                            Statistics::sample_deviation(std::sqrt(keeper.variance), cumulative_simulations[i + 1]);
                        if (keeper.mean_dps + keeper_sample_std * quantile >= filter_value)
                        {
                            temp_keepers.emplace_back(keeper);
                        }
                    }
                    if (attempts > 200)
                    {
                        break;
                    }
                }
            }
            keepers = temp_keepers;
        }

        if (keepers.size() <= 5 && time > 20)
        {
            debug_message += +"<b>: 20 seconds passed with 5 or less combinations remaining. breaking! </b><br>";
            break;
        }
        performed_iterations = i;
    }

    std::sort(keepers.begin(), keepers.end());
    std::reverse(keepers.begin(), keepers.end());

    std::vector<Character> best_characters;
    best_characters.reserve(keepers.size());
    for (const auto& keeper : keepers)
    {
        best_characters.emplace_back(item_optimizer.construct(keeper.index));
    }
    std::vector<std::vector<Item_popularity>> item_popularity;
    item_popularity.reserve(best_characters.size());
    for (size_t j = 0; j < best_characters[0].armor.size(); j++)
    {
        Item_popularity a{"none", 0};
        item_popularity.push_back({a});
    }
    std::vector<std::vector<Item_popularity>> weapon_popularity;
    weapon_popularity.reserve(best_characters.size());
    for (size_t j = 0; j < 2; j++)
    {
        Item_popularity a{"none", 0};
        weapon_popularity.push_back({a});
    }
    for (auto& best_character : best_characters)
    {
        for (size_t j = 0; j < best_character.armor.size(); j++)
        {
            bool found = false;
            for (auto& item : item_popularity[j])
            {
                if (item.name == best_character.armor[j].name)
                {
                    item.counter++;
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                Item_popularity a{best_character.armor[j].name, 1};
                item_popularity[j].push_back(a);
            }
        }
        for (size_t j = 0; j < best_character.weapons.size(); j++)
        {
            bool found = false;
            for (auto& item : weapon_popularity[j])
            {
                if (item.name == best_character.weapons[j].name)
                {
                    item.counter++;
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                Item_popularity a{best_character.weapons[j].name, 1};
                weapon_popularity[j].push_back(a);
            }
        }
    }

    for (auto& item : item_popularity)
    {
        std::sort(item.begin(), item.end());
        std::reverse(item.begin(), item.end());
    }

    for (auto& item : weapon_popularity)
    {
        std::sort(item.begin(), item.end());
        std::reverse(item.begin(), item.end());
    }

    std::vector<std::string> socket_names = {"Helmet", "Neck",   "Shoulder", "Back",      "Chest",
                                             "Wrists", "Hands",  "Belt",     "Legs",      "Boots",
                                             "Ranged", "Ring 1", "Ring 2",   "Trinket 1", "Trinket 2"};

    std::vector<size_t> socket_combinations = {item_optimizer.helmets.size(),
                                               item_optimizer.necks.size(),
                                               item_optimizer.shoulders.size(),
                                               item_optimizer.backs.size(),
                                               item_optimizer.chests.size(),
                                               item_optimizer.wrists.size(),
                                               item_optimizer.hands.size(),
                                               item_optimizer.belts.size(),
                                               item_optimizer.legs.size(),
                                               item_optimizer.boots.size(),
                                               item_optimizer.ranged.size(),
                                               item_optimizer.ring_combinations.size(),
                                               item_optimizer.ring_combinations.size(),
                                               item_optimizer.trinket_combinations.size(),
                                               item_optimizer.trinket_combinations.size()};

    std::vector<std::string> weapon_names = {"Main-hand", "Off-hand"};
    size_t weapon_combinations = item_optimizer.weapon_combinations.size();

    std::string message = +"<b>Total number of simulations performed: </b>" + std::to_string(n_sim) + "<br>";
    if (keepers.size() > 5)
    {
        message += "Optimizer did not converge! Item presence in the remaining sets:</b><br>";
        for (size_t j = 0; j < best_characters[0].armor.size(); j++)
        {
            message += "<b>" + socket_names[j] + "</b>" + ": ";
            for (const auto& item : item_popularity[j])
            {
                if (item.name != "none")
                {
                    message +=
                        item.name + " - (" + percent_to_str(100.0 * item.counter / best_characters.size()) + "), ";
                }
            }
            message += "<br>";
        }
        for (size_t j = 0; j < 2; j++)
        {
            message += "<b>" + weapon_names[j] + "</b>" + ": ";
            for (const auto& item : weapon_popularity[j])
            {
                if (item.name != "none")
                {
                    message +=
                        item.name + " - (" + percent_to_str(100.0 * item.counter / best_characters.size()) + "), ";
                }
            }
            message += "<br>";
        }
        message += "<br>";
    }

    size_t max_number = 5;
    max_number = std::min(best_characters.size(), max_number);

    message += "<b>Showing the " + std::to_string(std::min(best_characters.size(), max_number)) +
               " best item sets <u>sorted</u> by DPS:</b><br>";
    message += "<b>Formatting</b>: <br>(Total slot combinations) - socket - item_name<br>";

    for (size_t i = 0; i < max_number; i++)
    {
        message += "<b>Set " + std::to_string(i + 1) + ":</b><br>";
        message += "DPS: " + string_with_precision(keepers[i].mean_dps, 5) + " (-<b>" +
                   string_with_precision(keepers[0].mean_dps - keepers[i].mean_dps, 3) + "</b>)<br> ";
        double error_margin = Statistics::sample_deviation(std::sqrt(keepers[i].variance),
                                                           cumulative_simulations[performed_iterations + 1]);
        message += "Error margin DPS: " + string_with_precision(error_margin, 3) + "<br>";
        message += "<b>Stats:</b><br>";
        message += "Hit: " + string_with_precision(best_characters[i].total_special_stats.hit, 3) + " %<br>";
        message +=
            "Crit: " + string_with_precision(best_characters[i].total_special_stats.critical_strike, 3) + " %<br>";
        message +=
            "Attackpower: " + string_with_precision(best_characters[i].total_special_stats.attack_power, 4) + " <br>";
        //        message += "Equivalent attackpower: " + string_with_precision(
        //                keepers[i].ap_equivalent, 4) + " <br>";
        message += "<b>Armor:</b><br>";
        for (size_t j = 0; j < best_characters[i].armor.size(); j++)
        {
            message += "(" + std::to_string(socket_combinations[j]) + ") ";
            message += "<b>" + socket_names[j] + "</b>" + " - " + best_characters[i].armor[j].name + "<br>";
        }
        message += "<b>Weapons:</b><br>";
        for (size_t j = 0; j < best_characters[i].weapons.size(); j++)
        {
            message += "(" + std::to_string(weapon_combinations) + ") ";
            message += "<b>" + weapon_names[j] + "</b>" + " - " + best_characters[i].weapons[j].name + "<br>";
        }
        message += "<br>";
    }

    return {{message, debug_message}};
}
