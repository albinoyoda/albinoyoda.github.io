#include "sim_interface.hpp"

#include "Armory.hpp"
#include "Helper_functions.hpp"

#include <Character.hpp>
#include <Combat_simulator.hpp>
#include <Item_optimizer.hpp>
#include <sstream>

namespace
{
void mod_hit_effects(std::vector<Hit_effect>& hit_effects, double factor)
{
    for (auto& hit_effect : hit_effects)
    {
        if (hit_effect.name != "windfury_totem")
        {
            hit_effect.probability *= factor;
        }
    }
}
} // namespace

void item_upgrades(std::string& item_strengths_string, Character character_new, Item_optimizer& item_optimizer,
                   Armory& armory, std::vector<size_t> batches_per_iteration,
                   std::vector<size_t> cumulative_simulations, Combat_simulator& simulator, double dps_mean,
                   double dps_sample_std, Socket socket, const Special_stats& special_stats, bool first_item)
{
    std::string dummy;
    std::string current{"Current "};
    item_strengths_string += current;
    item_strengths_string =
        item_strengths_string + socket + ": <b>" + character_new.get_item_from_socket(socket, first_item).name + "</b>";
    auto armor_vec = armory.get_items_in_socket(socket);
    auto items = (socket != Socket::trinket) ?
                     item_optimizer.remove_weaker_items(armor_vec, character_new.total_special_stats, dummy, 3) :
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

    double quantile = Statistics::find_cdf_quantile(0.99, 0.01);
    std::string best_armor_name{};
    bool found_upgrade = false;
    std::string item_downgrades_string{};
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
                item_strengths_string += "<br><b>Up</b>grade: <b>" + items[i].name + "</b> ( +<b>" +
                                         string_with_precision(dps_increase, 2) + " &plusmn " +
                                         string_with_precision(dps_increase_std, 2) + "</b> DPS).";
                break;
            }
            else if (simulator.get_dps_mean() + sample_std * quantile <= dps_mean &&
                     cumulative_simulations[iter + 1] > 500)
            {
                double dps_decrease = simulator.get_dps_mean() - dps_mean;
                double dps_decrease_std = Statistics::add_standard_deviations(sample_std, dps_sample_std);
                item_downgrades_string += "<br><b>Down</b>grade: <b>" + items[i].name + "</b> ( <b>" +
                                          string_with_precision(dps_decrease, 3) + " &plusmn " +
                                          string_with_precision(dps_decrease_std, 2) + "</b> DPS).";
                if (does_vector_contain(stronger_indexies, i))
                {
                    found_upgrade = true;
                    item_downgrades_string += " Note: Similar item stats, difficult to draw conclusions.";
                }
                break;
            }
        }
    }
    if (!found_upgrade)
    {
        item_strengths_string += " is <b>BiS</b> in current configuration!";
        item_strengths_string += item_downgrades_string;
        item_strengths_string += "<br><br>";
    }
    else
    {
        item_strengths_string += item_downgrades_string;
        item_strengths_string += "<br><br>";
    }
}

void item_upgrades_wep(std::string& item_strengths_string, Character character_new, Item_optimizer& item_optimizer,
                       Armory& armory, std::vector<size_t> batches_per_iteration,
                       std::vector<size_t> cumulative_simulations, Combat_simulator& simulator, double dps_mean,
                       double dps_sample_std, Weapon_socket weapon_socket)
{
    std::string dummy;
    Socket socket = ((weapon_socket == Weapon_socket::main_hand) || (weapon_socket == Weapon_socket::two_hand)) ?
                        Socket::main_hand :
                        Socket::off_hand;
    std::string current{"Current "};
    item_strengths_string += current;
    item_strengths_string =
        item_strengths_string + socket + ": " + "<b>" + character_new.get_weapon_from_socket(socket).name + "</b>";
    auto items = armory.get_weapon_in_socket(weapon_socket);
    items = item_optimizer.remove_weaker_weapons(weapon_socket, items, character_new.total_special_stats, dummy, 10);
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
    std::string item_downgrades_string{};
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
                item_strengths_string += "<br><b>Up</b>grade: <b>" + item.name + "</b> ( +<b>" +
                                         string_with_precision(dps_increase, 3) + " &plusmn " +
                                         string_with_precision(dps_increase_std, 2) + "</b> DPS).";
                break;
            }
            else if (simulator.get_dps_mean() + sample_std * quantile <= dps_mean &&
                     cumulative_simulations[iter + 1] > 500)
            {
                double dps_decrease = simulator.get_dps_mean() - dps_mean;
                double dps_decrease_std = Statistics::add_standard_deviations(sample_std, dps_sample_std);
                item_downgrades_string += "<br><b>Down</b>grade: <b>" + item.name + "</b> ( <b>" +
                                          string_with_precision(dps_decrease, 3) + " &plusmn " +
                                          string_with_precision(dps_decrease_std, 2) + "</b> DPS).";
                break;
            }
        }
    }
    if (!found_upgrade)
    {
        item_strengths_string += " is <b>BiS</b> in current configuration!";
        item_strengths_string += item_downgrades_string;
        item_strengths_string += "<br><br>";
    }
    else
    {
        item_strengths_string += item_downgrades_string;
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
                                const std::string& permuted_stat, double permute_amount, double permute_factor,
                                double mean_init, double sample_std_init)
{
    combat_simulator.simulate(char_plus);
    double mean_plus = combat_simulator.get_dps_mean();
    double std_plus = std::sqrt(combat_simulator.get_dps_variance());
    double sample_std_plus = Statistics::sample_deviation(std_plus, combat_simulator.get_n_simulations());

    combat_simulator.simulate(char_minus);
    double mean_minus = combat_simulator.get_dps_mean();
    double std_minus = std::sqrt(combat_simulator.get_dps_variance());
    double sample_std_minus = Statistics::sample_deviation(std_minus, combat_simulator.get_n_simulations());

    return {(mean_plus - mean_init) / permute_factor,
            (Statistics::add_standard_deviations(sample_std_init, sample_std_plus)) / permute_factor,
            (mean_minus - mean_init) / permute_factor,
            (Statistics::add_standard_deviations(sample_std_init, sample_std_minus)) / permute_factor,
            permute_amount,
            permuted_stat};
}

std::vector<double> get_damage_sources(const Damage_sources& damage_sources_vector)
{
    return {
        damage_sources_vector.white_mh_damage / damage_sources_vector.sum_damage_sources(),
        damage_sources_vector.white_oh_damage / damage_sources_vector.sum_damage_sources(),
        damage_sources_vector.bloodthirst_damage / damage_sources_vector.sum_damage_sources(),
        damage_sources_vector.execute_damage / damage_sources_vector.sum_damage_sources(),
        damage_sources_vector.heroic_strike_damage / damage_sources_vector.sum_damage_sources(),
        damage_sources_vector.cleave_damage / damage_sources_vector.sum_damage_sources(),
        damage_sources_vector.whirlwind_damage / damage_sources_vector.sum_damage_sources(),
        damage_sources_vector.hamstring_damage / damage_sources_vector.sum_damage_sources(),
        damage_sources_vector.deep_wounds_damage / damage_sources_vector.sum_damage_sources(),
        damage_sources_vector.item_hit_effects_damage / damage_sources_vector.sum_damage_sources(),
        damage_sources_vector.overpower_damage / damage_sources_vector.sum_damage_sources(),
        damage_sources_vector.slam_damage / damage_sources_vector.sum_damage_sources(),
        damage_sources_vector.mortal_strike_damage / damage_sources_vector.sum_damage_sources(),
        damage_sources_vector.sweeping_strikes_damage / damage_sources_vector.sum_damage_sources(),
    };
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
    out_string += print_stat("Expertise: ", character.total_special_stats.expertise);
    out_string += print_stat("Crit (spellbook): ", character.total_special_stats.critical_strike);
    out_string += print_stat("Attack Power: ", character.total_special_stats.attack_power);
    out_string += print_stat("Haste factor: ", 1 + character.total_special_stats.haste);
    if (character.is_dual_wield())
    {
        if (character.has_weapon_of_type(Weapon_type::sword))
        {
            out_string += print_stat("Sword bonus expertise: ", character.total_special_stats.sword_expertise);
        }
        if (character.has_weapon_of_type(Weapon_type::axe))
        {
            out_string += print_stat("Axe bonus expertise: ", character.total_special_stats.axe_expertise);
        }
        if (character.has_weapon_of_type(Weapon_type::dagger))
        {
            out_string += print_stat("Dagger bonus expertise: ", 0);
        }
        if (character.has_weapon_of_type(Weapon_type::mace))
        {
            out_string += print_stat("Mace bonus expertise: ", character.total_special_stats.mace_expertise);
        }
        if (character.has_weapon_of_type(Weapon_type::unarmed))
        {
            out_string += print_stat("Unarmed bonus expertise: ", 0);
        }
    }
    else
    {
        if (character.has_weapon_of_type(Weapon_type::sword))
        {
            out_string += print_stat("Two Hand Sword expertise: ", character.total_special_stats.sword_expertise);
        }
        if (character.has_weapon_of_type(Weapon_type::axe))
        {
            out_string += print_stat("Two Hand Axe expertise: ", character.total_special_stats.axe_expertise);
        }
        if (character.has_weapon_of_type(Weapon_type::mace))
        {
            out_string += print_stat("Two Hand Mace expertise: ", character.total_special_stats.mace_expertise);
        }
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
                          const std::vector<std::string>& talent_string, const std::vector<int>& talent_val,
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

    if (weapons_vec.size() > 1)
    {
        character.equip_weapon(armory.find_weapon(Weapon_socket::one_hand, weapons_vec[0]),
                               armory.find_weapon(Weapon_socket::one_hand, weapons_vec[1]));
    }
    else
    {
        character.equip_weapon(armory.find_weapon(Weapon_socket::two_hand, weapons_vec[0]));
    }

    armory.add_enchants_to_character(character, ench_vec);
    armory.add_buffs_to_character(character, buffs_vec);
    armory.add_talents_to_character(character, talent_string, talent_val);

    armory.compute_total_stats(character);

    return character;
}

void compute_talent_weight(Combat_simulator& combat_simulator, const Character& character, std::string& talents_info,
                           const std::string& talent_name, Combat_simulator_config config,
                           int Combat_simulator_config::talents_t::*talent, int n_points)
{
    config.talents.*talent = 0;
    combat_simulator.set_config(config);
    combat_simulator.simulate(character, 0);
    double dps_without = combat_simulator.get_dps_mean();
    double var_without = combat_simulator.get_dps_variance();

    config.talents.*talent = n_points;
    combat_simulator.set_config(config);
    combat_simulator.simulate(character, 0);
    double dps_with = combat_simulator.get_dps_mean();
    double var_with = combat_simulator.get_dps_variance();

    double delta_dps = (dps_with - dps_without) / n_points;
    double std_dps = std::sqrt(var_without + var_with) / n_points / std::sqrt(config.n_batches);
    talents_info += "<br>Talent: <b>" + talent_name + "</b><br>Value: <b>" + string_with_precision(delta_dps, 4) +
                    " &plusmn " + string_with_precision(std_dps, 3) + " DPS</b><br>";
}

void compute_talent_weight(Combat_simulator& combat_simulator, const Character& character, std::string& talents_info,
                           const std::string& talent_name, Combat_simulator_config config,
                           bool Combat_simulator_config::talents_t::*talent)
{
    config.talents.*talent = false;
    combat_simulator.set_config(config);
    combat_simulator.simulate(character, 0);
    double dps_without = combat_simulator.get_dps_mean();
    double var_without = combat_simulator.get_dps_variance();

    config.talents.*talent = true;
    combat_simulator.set_config(config);
    combat_simulator.simulate(character, 0);
    double dps_with = combat_simulator.get_dps_mean();
    double var_with = combat_simulator.get_dps_variance();

    double delta_dps = dps_with - dps_without;
    double std_dps = std::sqrt(var_without + var_with) / std::sqrt(config.n_batches);
    talents_info += "<br>Talent: <b>" + talent_name + "</b><br>Value: <b>" + string_with_precision(delta_dps, 4) +
                    " &plusmn " + string_with_precision(std_dps, 3) + " DPS</b><br>";
}

Sim_output Sim_interface::simulate(const Sim_input& input)
{
    Armory armory{};

    auto temp_buffs = input.buffs;

    // Separate case for options which in reality are buffs. Add them to the buff list
    if (find_string(input.options, "mighty_rage_potion"))
    {
        temp_buffs.emplace_back("mighty_rage_potion");
    }
    if (find_string(input.options, "full_polarity"))
    {
        double full_polarity_val = find_value(input.float_options_string, input.float_options_val, "full_polarity_dd");
        armory.buffs.full_polarity.special_stats.damage_mod_physical = full_polarity_val / 100.0;
        armory.buffs.full_polarity.special_stats.damage_mod_spell = full_polarity_val / 100.0;
        temp_buffs.emplace_back("full_polarity");
    }
    if (find_string(input.options, "fungal_bloom"))
    {
        temp_buffs.emplace_back("fungal_bloom");
    }
    if (find_string(input.options, "battle_squawk"))
    {
        double battle_squawk_val = find_value(input.float_options_string, input.float_options_val, "battle_squawk_dd");
        armory.buffs.battle_squawk.special_stats.haste = battle_squawk_val / 100.0;
        temp_buffs.emplace_back("battle_squawk");
    }

    const Character character = character_setup(armory, input.race[0], input.armor, input.weapons, temp_buffs,
                                                input.talent_string, input.talent_val, input.enchants);

    // Simulator & Combat settings
    Combat_simulator_config config{input};
    Combat_simulator simulator{};
    simulator.set_config(config);

    std::vector<std::string> use_effect_order_string{};
    {
        auto use_effect_order = simulator.get_use_effect_order(character);
        for (const auto& use_effect_timing : use_effect_order)
        {
            use_effect_order_string.emplace_back(use_effect_timing.second.name + " " +
                                                 string_with_precision(use_effect_timing.first, 3) + " " +
                                                 string_with_precision(use_effect_timing.second.duration, 3));
        }
    }
    for (const auto& wep : character.weapons)
    {
        simulator.compute_hit_table(character.total_special_stats, wep.socket, wep.weapon_socket, wep.type);
    }
    const bool is_two_handed = !character.is_dual_wield();
    const auto yellow_ht = simulator.get_hit_probabilities_yellow();
    const auto white_mh_ht = simulator.get_hit_probabilities_white_mh();
    const auto white_oh_ht = simulator.get_hit_probabilities_white_oh();
    const auto white_oh_ht_2h = simulator.get_hit_probabilities_white_2h();
    double n_simulations_base = config.n_batches;

    simulator.simulate(character, 0, true);

    const double dps_mean = simulator.get_dps_mean();
    const double dps_sample_std =
        Statistics::sample_deviation(std::sqrt(simulator.get_dps_variance()), config.n_batches);
    std::vector<double> mean_dps_vec{dps_mean};
    std::vector<double> sample_std_dps_vec{dps_sample_std};

    const auto hist_x = simulator.get_hist_x();
    const auto hist_y = simulator.get_hist_y();

    const auto dmg_dist = simulator.get_damage_distribution();
    const std::vector<double> dps_dist_raw = get_damage_sources(dmg_dist);

    std::vector<std::string> aura_uptimes = simulator.get_aura_uptimes();
    std::vector<std::string> proc_statistics = simulator.get_proc_statistics();
    auto damage_time_lapse_raw = simulator.get_damage_time_lapse();
    std::vector<std::string> time_lapse_names;
    std::vector<std::vector<double>> damage_time_lapse;
    std::vector<double> dps_dist;
    std::vector<std::string> damage_names = {"White MH",      "White OH",         "Bloodthirst", "Execute",
                                             "Heroic Strike", "Cleave",           "Whirlwind",   "Hamstring",
                                             "Deep Wounds",   "Item Hit Effects", "Overpower",   "Slam",
                                             "Mortal Strike", "Sweeping Strikes"};
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

    std::string character_stats = get_character_stat(character);

    std::string rage_info = "<b>Rage Statistics:</b><br>";
    rage_info += "(Average per simulation)<br>";
    rage_info += "Rage lost to rage cap (gaining rage when at 100): <b>" +
                 string_with_precision(simulator.get_rage_lost_capped() / double(simulator.get_n_simulations()), 3) +
                 "</b><br>";
    rage_info += "</b>Rage lost when changing stace (cutting rage at 25): <b>" +
                 string_with_precision(simulator.get_rage_lost_stance() / double(simulator.get_n_simulations()), 3) +
                 "</b><br>";

    std::string extra_info_string = "<b>Fight stats vs. target:</b> <br/>";
    extra_info_string += "<b>Hit:</b> <br/>";
    extra_info_string += percent_to_str("Yellow hits", yellow_ht[0], "chance to miss");
    extra_info_string += percent_to_str("Main-hand, white hits", white_mh_ht[0], "chance to miss");
    if (!is_two_handed)
    {
        extra_info_string += percent_to_str("off-hand, white hits", white_oh_ht[0], "chance to miss");
        extra_info_string += percent_to_str("off-hand, while ability queued", white_oh_ht_2h[0], "chance to miss");
    }

    extra_info_string += "<b>Crit chance:</b> <br/>";
    double white_mh_crit = std::min(white_mh_ht[3], 100.0) - white_mh_ht[2];
    double left_to_crit_cap_white_mh = std::max(100.0 - white_mh_ht.back(), 0.0);
    double yellow_crit = std::min(yellow_ht[3], 100.0) - yellow_ht[2];
    extra_info_string += percent_to_str("Yellow", yellow_crit, "chance to crit per cast (double roll suppression)",
                                        100 - yellow_crit * (1 + yellow_ht[1] / 100.0), "left to crit-cap");
    extra_info_string += percent_to_str("White main hand", white_mh_crit, "chance to crit", left_to_crit_cap_white_mh,
                                        "left to crit-cap");

    if (!is_two_handed)
    {
        double white_oh_crit = std::min(white_oh_ht[3], 100.0) - white_oh_ht[2];
        double left_to_crit_cap_white_oh = std::max(100.0 - white_oh_ht.back(), 0.0);

        extra_info_string += percent_to_str("White off hand", white_oh_crit, "chance to crit",
                                            left_to_crit_cap_white_oh, "left to crit-cap");
    }
    extra_info_string += "<b>Glancing blows:</b><br/>";
    double glancing_probability = white_mh_ht[2] - white_mh_ht[1];
    double glancing_penalty_mh = 100 * simulator.get_glancing_penalty_mh();
    extra_info_string += percent_to_str("Chance to occur", glancing_probability, "(based on level difference)");
    extra_info_string +=
        percent_to_str("Glancing damage main-hand", glancing_penalty_mh, "(based on skill difference)");
    if (!is_two_handed)
    {
        double glancing_penalty_oh = 100 * simulator.get_glancing_penalty_oh();
        extra_info_string +=
            percent_to_str("Glancing damage off-hand", glancing_penalty_oh, "(based on skill difference)");
    }
    extra_info_string += "<b>Other:</b><br/>";
    double dodge_chance = yellow_ht[1] - yellow_ht[0];
    extra_info_string += percent_to_str("Target dodge chance", dodge_chance, "(based on skill difference and expertise)") + "<br>"
                                                                                                              "<"
                                                                                                              "br>";

    std::string dpr_info = "<br>(Hint: Ability damage per rage computations can be turned on under 'Simulation "
                           "settings')";
    config.performance_mode = true;
    if (find_string(input.options, "compute_dpr"))
    {
        config.n_batches = 10000;
        dpr_info = "<br><b>Ability damage per rage:</b><br>";
        dpr_info += "DPR for ability X is computed as following:<br> "
                    "((Normal DPS) - (DPS where ability X costs rage but has no effect)) / (rage cost of ability "
                    "X)<br>";
        if (config.combat.use_bloodthirst)
        {
            double avg_bt_casts = static_cast<double>(dmg_dist.bloodthirst_count) / n_simulations_base;
            if (avg_bt_casts > 0.0)
            {
                config.dpr_settings.compute_dpr_bt_ = true;
                Combat_simulator simulator_dpr{};
                simulator_dpr.set_config(config);
                simulator_dpr.simulate(character, 0);
                double delta_dps = dps_mean - simulator_dpr.get_dps_mean();
                double dmg_tot = delta_dps * (config.sim_time - 1);
                double dmg_per_hit = dmg_tot / avg_bt_casts;
                double dmg_per_rage = dmg_per_hit / 30.0;
                dpr_info += "<b>Bloodthirst</b>: <br>Damage per cast: <b>" + string_with_precision(dmg_per_hit, 4) +
                            "</b><br>Average rage cost: <b>" + string_with_precision(30.0, 3) + "</b><br>DPR: <b>" +
                            string_with_precision(dmg_per_rage, 4) + "</b><br>";
                config.dpr_settings.compute_dpr_bt_ = false;
            }
        }
        if (config.combat.use_whirlwind)
        {
            double avg_ww_casts = static_cast<double>(dmg_dist.whirlwind_count) / n_simulations_base;
            if (avg_ww_casts > 0.0)
            {
                config.dpr_settings.compute_dpr_ww_ = true;
                Combat_simulator simulator_dpr{};
                simulator_dpr.set_config(config);
                simulator_dpr.simulate(character, 0);
                double delta_dps = dps_mean - simulator_dpr.get_dps_mean();
                double dmg_tot = delta_dps * (config.sim_time - 1);
                double dmg_per_hit = dmg_tot / avg_ww_casts;
                double dmg_per_rage = dmg_per_hit / 25.0;
                dpr_info += "<b>Whirlwind</b>: <br>Damage per cast: <b>" + string_with_precision(dmg_per_hit, 4) +
                            "</b><br>Average rage cost: <b>" + string_with_precision(25.0, 3) + "</b><br>DPR: <b>" +
                            string_with_precision(dmg_per_rage, 4) + "</b><br>";
                config.dpr_settings.compute_dpr_ww_ = false;
            }
        }
        if (config.combat.use_slam)
        {
            double avg_sl_casts = static_cast<double>(dmg_dist.slam_count) / n_simulations_base;
            if (avg_sl_casts > 0.0)
            {
                config.dpr_settings.compute_dpr_sl_ = true;
                Combat_simulator simulator_dpr{};
                simulator_dpr.set_config(config);
                simulator_dpr.simulate(character, 0);
                double delta_dps = dps_mean - simulator_dpr.get_dps_mean();
                double dmg_tot = delta_dps * (config.sim_time - 1);
                double dmg_per_hit = dmg_tot / avg_sl_casts;
                double dmg_per_rage = dmg_per_hit / 15.0;
                dpr_info += "<b>Slam</b>: <br>Damage per cast: <b>" + string_with_precision(dmg_per_hit, 4) +
                            "</b><br>Average rage cost: <b>" + string_with_precision(15.0, 3) + "</b><br>DPR: <b>" +
                            string_with_precision(dmg_per_rage, 4) + "</b><br>";
                config.dpr_settings.compute_dpr_sl_ = false;
            }
        }
        if (config.combat.use_heroic_strike)
        {
            double avg_hs_casts = static_cast<double>(dmg_dist.heroic_strike_count) / n_simulations_base;
            if (avg_hs_casts > 0.0)
            {
                config.dpr_settings.compute_dpr_hs_ = true;
                Combat_simulator simulator_dpr{};
                simulator_dpr.set_config(config);
                simulator_dpr.simulate(character, 0);
                double delta_dps = dps_mean - simulator_dpr.get_dps_mean();
                double dmg_tot = delta_dps * (config.sim_time - 1);
                double dmg_per_hs = dmg_tot / avg_hs_casts;
                double avg_mh_dmg =
                    static_cast<double>(dmg_dist.white_mh_damage) / static_cast<double>(dmg_dist.white_mh_count);
                double avg_mh_rage_lost = avg_mh_dmg * 3.75 / 274.7 + (3.5 * character.weapons[0].swing_speed / 2);
                double dmg_per_rage = dmg_per_hs / (13 + avg_mh_rage_lost);
                dpr_info += "<b>Heroic Strike</b>: <br>Damage per cast: <b>" + string_with_precision(dmg_per_hs, 4) +
                            "</b><br>Average rage cost: <b>" + string_with_precision((13 + avg_mh_rage_lost), 3) +
                            "</b><br>DPR: <b>" + string_with_precision(dmg_per_rage, 4) + "</b><br>";
                config.dpr_settings.compute_dpr_hs_ = false;
            }
        }
        if (config.combat.cleave_if_adds)
        {
            double avg_cl_casts = static_cast<double>(dmg_dist.cleave_count) / n_simulations_base;
            if (avg_cl_casts > 0.0)
            {
                config.dpr_settings.compute_dpr_cl_ = true;
                Combat_simulator simulator_dpr{};
                simulator_dpr.set_config(config);
                simulator_dpr.simulate(character, 0);
                double delta_dps = dps_mean - simulator_dpr.get_dps_mean();
                double dmg_tot = delta_dps * (config.sim_time - 1);
                double dmg_per_hs = dmg_tot / avg_cl_casts;
                double avg_mh_dmg =
                    static_cast<double>(dmg_dist.white_mh_damage) / static_cast<double>(dmg_dist.white_mh_count);
                double avg_mh_rage_lost = avg_mh_dmg * 3.75 / 274.7 + (3.5 * character.weapons[0].swing_speed / 2);
                double dmg_per_rage = dmg_per_hs / (20 + avg_mh_rage_lost);
                dpr_info += "<b>Cleave</b>: <br>Damage per cast: <b>" + string_with_precision(dmg_per_hs, 4) +
                            "</b><br>Average rage cost: <b>" + string_with_precision((20 + avg_mh_rage_lost), 3) +
                            "</b><br>DPR: <b>" + string_with_precision(dmg_per_rage, 4) + "</b><br>";
                config.dpr_settings.compute_dpr_cl_ = false;
            }
        }
        if (config.combat.use_hamstring)
        {
            double avg_ha_casts = static_cast<double>(dmg_dist.hamstring_count) / n_simulations_base;
            if (avg_ha_casts > 0.0)
            {
                config.dpr_settings.compute_dpr_ha_ = true;
                Combat_simulator simulator_dpr{};
                simulator_dpr.set_config(config);
                simulator_dpr.simulate(character, 0);
                double delta_dps = dps_mean - simulator_dpr.get_dps_mean();
                double dmg_tot = delta_dps * (config.sim_time - 1);
                double dmg_per_ha = dmg_tot / avg_ha_casts;
                double dmg_per_rage = dmg_per_ha / 10;
                dpr_info += "<b>Hamstring</b>: <br>Damage per cast: <b>" + string_with_precision(dmg_per_ha, 4) +
                            "</b><br>Average rage cost: <b>" + string_with_precision(10, 3) + "</b><br>DPR: <b>" +
                            string_with_precision(dmg_per_rage, 4) + "</b><br>";
                config.dpr_settings.compute_dpr_ha_ = false;
            }
        }
        if (config.combat.use_overpower)
        {
            double avg_op_casts = static_cast<double>(dmg_dist.overpower_count) / n_simulations_base;
            if (avg_op_casts > 0.0)
            {
                config.dpr_settings.compute_dpr_op_ = true;
                Combat_simulator simulator_dpr{};
                simulator_dpr.set_config(config);
                simulator_dpr.simulate(character, 0);
                double delta_dps = dps_mean - simulator_dpr.get_dps_mean();
                double dmg_tot = delta_dps * (config.sim_time - 1);
                double dmg_per_hit = dmg_tot / avg_op_casts;
                double overpower_cost =
                    simulator.get_rage_lost_stance() / double(simulator.get_n_simulations()) / avg_op_casts + 5.0;
                double dmg_per_rage = dmg_per_hit / overpower_cost;
                dpr_info += "<b>Overpower</b>: <br>Damage per cast: <b>" + string_with_precision(dmg_per_hit, 4) +
                            "</b><br>Average rage cost: <b>" + string_with_precision(overpower_cost, 3) +
                            "</b><br>DPR: <b>" + string_with_precision(dmg_per_rage, 4) + "</b><br>";
                config.dpr_settings.compute_dpr_op_ = false;
            }
        }

        double avg_ex_casts = static_cast<double>(dmg_dist.execute_count) / n_simulations_base;
        if (avg_ex_casts > 0.0)
        {
            config.dpr_settings.compute_dpr_ex_ = true;
            Combat_simulator simulator_dpr{};
            simulator_dpr.set_config(config);
            simulator_dpr.simulate(character, 0);
            double delta_dps = dps_mean - simulator_dpr.get_dps_mean();
            double dmg_tot = delta_dps * (config.sim_time - 1);
            double dmg_per_hit = dmg_tot / avg_ex_casts;
            double execute_rage_cost = 15 - static_cast<int>(2.51 * config.talents.improved_execute);
            double execute_cost = simulator.get_avg_rage_spent_executing() / avg_ex_casts + execute_rage_cost;
            double dmg_per_rage = dmg_per_hit / execute_cost;
            dpr_info += "<b>Execute</b>: <br>Damage per cast: <b>" + string_with_precision(dmg_per_hit, 4) +
                        "</b><br>Average rage cost: <b>" + string_with_precision(execute_cost, 3) + "</b><br>DPR: <b>" +
                        string_with_precision(dmg_per_rage, 4) + "</b><br>";
            config.dpr_settings.compute_dpr_ex_ = false;
        }
        config.n_batches = static_cast<int>(n_simulations_base);
    }

    std::string talents_info = "<br>(Hint: Talent stat-weights can be activated under 'Simulation settings')";
    if (find_string(input.options, "talents_stat_weights"))
    {
        config.n_batches = static_cast<int>(
            find_value(input.float_options_string, input.float_options_val, "n_simulations_talent_dd"));
        Combat_simulator simulator_talent{};

        talents_info = "<br><b>Value per 1 talent point:</b>";
        if (!config.combat.cleave_if_adds)
        {
            compute_talent_weight(simulator_talent, character, talents_info, "Improved Heroic Strike", config,
                                  &Combat_simulator_config::talents_t::improved_heroic_strike, 3);
        }
        else
        {
            compute_talent_weight(simulator_talent, character, talents_info, "Improved Cleave", config,
                                  &Combat_simulator_config::talents_t::improved_cleave, 3);
        }

        compute_talent_weight(simulator_talent, character, talents_info, "Improved Overpower", config,
                              &Combat_simulator_config::talents_t::overpower, 2);

        if (config.combat.use_slam && is_two_handed)
        {
            compute_talent_weight(simulator_talent, character, talents_info, "Improved Slam", config,
                                  &Combat_simulator_config::talents_t::improved_slam, 5);
        }

        compute_talent_weight(simulator_talent, character, talents_info, "Improved Execute", config,
                              &Combat_simulator_config::talents_t::improved_execute, 2);

        compute_talent_weight(simulator_talent, character, talents_info, "Unbridled Wrath", config,
                              &Combat_simulator_config::talents_t::unbridled_wrath, 5);

        compute_talent_weight(simulator_talent, character, talents_info, "Flurry", config,
                              &Combat_simulator_config::talents_t::flurry, 5);

        compute_talent_weight(simulator_talent, character, talents_info, "Impale", config,
                              &Combat_simulator_config::talents_t::impale, 2);

        compute_talent_weight(simulator_talent, character, talents_info, "Anger Management", config,
                              &Combat_simulator_config::talents_t::anger_management);

        compute_talent_weight(simulator_talent, character, talents_info, "Death wish", config,
                              &Combat_simulator_config::talents_t::death_wish);

        if (!is_two_handed)
        {
            compute_talent_weight(simulator_talent, character, talents_info, "Dual Wield Specialization", config,
                                  &Combat_simulator_config::talents_t::dual_wield_specialization, 5);
        }
    }

    if (input.compare_armor.size() == 15 && input.compare_weapons.size() == 2)
    {
        Combat_simulator simulator_compare{};
        simulator_compare.set_config(config);
        Character character2 = character_setup(armory, input.race[0], input.compare_armor, input.compare_weapons,
                                               temp_buffs, input.talent_string, input.talent_val, input.enchants);

        simulator_compare.simulate(character2);

        double mean_init_2 = simulator_compare.get_dps_mean();
        double std_init_2 = std::sqrt(simulator_compare.get_dps_variance());
        double sample_std_init_2 = Statistics::sample_deviation(std_init_2, config.n_batches);

        character_stats = get_character_stat(character, character2);

        mean_dps_vec.push_back(mean_init_2);
        sample_std_dps_vec.push_back(sample_std_init_2);
    }

    std::string item_strengths_string;
    if (find_string(input.options, "item_strengths") || find_string(input.options, "wep_strengths"))
    {
        item_strengths_string = "<b>Character items and proposed upgrades:</b><br>";

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
        item_optimizer.race = get_race(input.race[0]);
        Character character_new = character_setup(armory, input.race[0], input.armor, input.weapons, temp_buffs,
                                                  input.talent_string, input.talent_val, input.enchants);
        std::string dummy{};
        std::vector<Socket> all_sockets = {
            Socket::head, Socket::neck, Socket::shoulder, Socket::back, Socket::chest,   Socket::wrist,  Socket::hands,
            Socket::belt, Socket::legs, Socket::boots,    Socket::ring, Socket::trinket, Socket::ranged,
        };

        if (find_string(input.options, "item_strengths"))
        {
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
        }
        if (find_string(input.options, "wep_strengths"))
        {
            if (is_two_handed)
            {
                item_upgrades_wep(item_strengths_string, character_new, item_optimizer, armory, batches_per_iteration,
                                  cumulative_simulations, simulator, dps_mean, dps_sample_std, Weapon_socket::two_hand);
            }
            else
            {
                item_upgrades_wep(item_strengths_string, character_new, item_optimizer, armory, batches_per_iteration,
                                  cumulative_simulations, simulator, dps_mean, dps_sample_std,
                                  Weapon_socket::main_hand);
                item_upgrades_wep(item_strengths_string, character_new, item_optimizer, armory, batches_per_iteration,
                                  cumulative_simulations, simulator, dps_mean, dps_sample_std, Weapon_socket::off_hand);
            }
        }
        item_strengths_string += "<br><br>";
    }

    config.n_batches = find_value(input.float_options_string, input.float_options_val, "n_simulations_stat_dd");
    simulator.set_config(config);
    std::vector<std::string> stat_weights;
    if (!input.stat_weights.empty())
    {
        {
            Character char_plus = character;
            Character char_minus = character;
            char_plus.total_special_stats.attack_power += 300;
            char_minus.total_special_stats.attack_power -= 300;
            Stat_weight base_line =
                compute_stat_weight(simulator, char_plus, char_minus, "attack power", 100, 3, dps_mean, dps_sample_std);
            stat_weights.emplace_back(
                "attack_power: " + std::to_string(base_line.dps_plus) + " " + std::to_string(base_line.std_dps_plus) +
                " " + std::to_string(base_line.dps_minus) + " " + std::to_string(base_line.std_dps_minus));
        }

        for (const auto& stat_weight : input.stat_weights)
        {
            if (stat_weight == "crit")
            {
                Character char_plus = character;
                Character char_minus = character;
                char_plus.total_special_stats.critical_strike += 2;
                char_minus.total_special_stats.critical_strike -= 2;
                Stat_weight crit =
                    compute_stat_weight(simulator, char_plus, char_minus, "crit", 1, 2, dps_mean, dps_sample_std);
                stat_weights.emplace_back("1%Crit " + std::to_string(crit.dps_plus) + " " +
                                          std::to_string(crit.std_dps_plus) + " " + std::to_string(crit.dps_minus) +
                                          " " + std::to_string(crit.std_dps_minus));
            }

            if (stat_weight == "expertise")
            {
                Character char_plus = character;
                Character char_minus = character;
                char_plus.total_special_stats.hit += 1;
                char_minus.total_special_stats.hit -= 1;
                Stat_weight expertise =
                    compute_stat_weight(simulator, char_plus, char_minus, "expertise", 1, 1, dps_mean, dps_sample_std);
                stat_weights.emplace_back("1%Expertise " + std::to_string(expertise.dps_plus) + " " +
                                          std::to_string(expertise.std_dps_plus) + " " + std::to_string(expertise.dps_minus) + " " +
                                          std::to_string(expertise.std_dps_minus));
            }

            if (stat_weight == "hit")
            {
                Character char_plus = character;
                Character char_minus = character;
                char_plus.total_special_stats.hit += 1;
                char_minus.total_special_stats.hit -= 1;
                Stat_weight hit =
                    compute_stat_weight(simulator, char_plus, char_minus, "hit", 1, 1, dps_mean, dps_sample_std);
                stat_weights.emplace_back("1%Hit " + std::to_string(hit.dps_plus) + " " +
                                          std::to_string(hit.std_dps_plus) + " " + std::to_string(hit.dps_minus) + " " +
                                          std::to_string(hit.std_dps_minus));
            }
            if (stat_weight == "haste")
            {
                Character char_plus = character;
                Character char_minus = character;
                char_plus.total_special_stats.haste = (char_plus.total_special_stats.haste + 1) * 1.1 - 1;
                char_minus.total_special_stats.haste = (char_minus.total_special_stats.haste + 1) / 1.1 - 1;
                Stat_weight hit =
                    compute_stat_weight(simulator, char_plus, char_minus, "haste", 1, 10, dps_mean, dps_sample_std);
                stat_weights.emplace_back("1%Haste " + std::to_string(hit.dps_plus) + " " +
                                          std::to_string(hit.std_dps_plus) + " " + std::to_string(hit.dps_minus) + " " +
                                          std::to_string(hit.std_dps_minus));
            }
            if (stat_weight == "extra_hit")
            {
                Character char_plus = character;
                Character char_minus = character;
                Hit_effect extra_hit{"stat_weight_extra_hit", Hit_effect::Type::extra_hit, {}, {}, 0, 0, 0.05};
                char_plus.weapons[0].hit_effects.emplace_back(extra_hit);
                if (char_plus.is_dual_wield())
                {
                    char_plus.weapons[1].hit_effects.emplace_back(extra_hit);
                }
                Stat_weight hit =
                    compute_stat_weight(simulator, char_plus, char_minus, "extra_hit", 1, 5, dps_mean, dps_sample_std);
                stat_weights.emplace_back("1%ExtraAttack " + std::to_string(hit.dps_plus) + " " +
                                          std::to_string(hit.std_dps_plus) + " " + std::to_string(hit.dps_minus) + " " +
                                          std::to_string(hit.std_dps_minus));
            }
            if (stat_weight == "mh_speed")
            {
                Character char_plus = character;
                Character char_minus = character;
                double swing_speed_diff =
                    find_value(input.float_options_string, input.float_options_val, "stat_weight_mh_speed_dd");
                double factor_p =
                    (char_plus.weapons[0].swing_speed + swing_speed_diff) / char_plus.weapons[0].swing_speed;
                double factor_n =
                    (char_plus.weapons[0].swing_speed - swing_speed_diff) / char_plus.weapons[0].swing_speed;
                char_plus.weapons[0].min_damage *= factor_p;
                char_plus.weapons[0].max_damage *= factor_p;
                char_plus.weapons[0].swing_speed += swing_speed_diff;
                mod_hit_effects(char_plus.weapons[0].hit_effects, factor_p);
                char_minus.weapons[0].min_damage *= factor_n;
                char_minus.weapons[0].max_damage *= factor_n;
                char_minus.weapons[0].swing_speed -= swing_speed_diff;
                mod_hit_effects(char_minus.weapons[0].hit_effects, factor_n);
                Stat_weight hit = compute_stat_weight(simulator, char_plus, char_minus, "mh_speed", swing_speed_diff, 1,
                                                      dps_mean, dps_sample_std);
                stat_weights.emplace_back(string_with_precision(swing_speed_diff, 2) + "-MH-speed " +
                                          std::to_string(hit.dps_plus) + " " + std::to_string(hit.std_dps_plus) + " " +
                                          std::to_string(hit.dps_minus) + " " + std::to_string(hit.std_dps_minus));
            }
            if (stat_weight == "oh_speed" && !is_two_handed)
            {
                Character char_plus = character;
                Character char_minus = character;
                double swing_speed_diff =
                    find_value(input.float_options_string, input.float_options_val, "stat_weight_oh_speed_dd");
                double factor_p =
                    (char_plus.weapons[1].swing_speed + swing_speed_diff) / char_plus.weapons[1].swing_speed;
                double factor_n =
                    (char_plus.weapons[1].swing_speed - swing_speed_diff) / char_plus.weapons[1].swing_speed;
                char_plus.weapons[1].min_damage *= factor_p;
                char_plus.weapons[1].max_damage *= factor_p;
                char_plus.weapons[1].swing_speed += swing_speed_diff;
                mod_hit_effects(char_plus.weapons[1].hit_effects, factor_p);
                char_minus.weapons[1].min_damage *= factor_n;
                char_minus.weapons[1].max_damage *= factor_n;
                char_minus.weapons[1].swing_speed -= swing_speed_diff;
                mod_hit_effects(char_minus.weapons[1].hit_effects, factor_n);
                Stat_weight hit = compute_stat_weight(simulator, char_plus, char_minus, "oh_speed", swing_speed_diff, 1,
                                                      dps_mean, dps_sample_std);
                stat_weights.emplace_back(string_with_precision(swing_speed_diff, 2) + "-OH-speed " +
                                          std::to_string(hit.dps_plus) + " " + std::to_string(hit.std_dps_plus) + " " +
                                          std::to_string(hit.dps_minus) + " " + std::to_string(hit.std_dps_minus));
            }
        }
    }

    std::string debug_topic{};
    if (find_string(input.options, "debug_on"))
    {
        config.display_combat_debug = true;
        config.performance_mode = false;

        simulator.set_config(config);
        double dps{};
        for (int i = 0; i < 100000; i++)
        {
            simulator.simulate(character);
            dps = simulator.get_dps_mean();
            if (std::abs(dps - dps_mean) < 5)
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
        debug_topic += "DPS mortal strike: " + std::to_string(dist.mortal_strike_damage / config.sim_time) + "<br>";
        debug_topic +=
            "DPS sweeping strikes: " + std::to_string(dist.sweeping_strikes_damage / config.sim_time) + "<br>";
        debug_topic += "DPS overpower: " + std::to_string(dist.overpower_damage / config.sim_time) + "<br>";
        debug_topic += "DPS slam: " + std::to_string(dist.slam_damage / config.sim_time) + "<br>";
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
        debug_topic += "#Hits mortal strike: " + std::to_string(dist.mortal_strike_count) + "<br>";
        debug_topic += "#Hits sweeping strikes: " + std::to_string(dist.sweeping_strikes_count) + "<br>";
        debug_topic += "#Hits overpower: " + std::to_string(dist.overpower_count) + "<br>";
        debug_topic += "#Hits slam: " + std::to_string(dist.slam_count) + "<br>";
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
            use_effect_order_string,
            proc_statistics,
            stat_weights,
            {item_strengths_string + extra_info_string + rage_info + dpr_info + talents_info, debug_topic},
            mean_dps_vec,
            sample_std_dps_vec,
            {character_stats}};
}
