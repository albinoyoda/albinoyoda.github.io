#include "item_upgrades.hpp"

#include <Statistics.hpp>

void sort_item_upgrades_descending(std::vector<Item_upgrade_result>& item_upgrades)
{
    std::sort(item_upgrades.begin(), item_upgrades.end(),
              [](const Item_upgrade_result& a, const Item_upgrade_result& b) { return a.second[0] < b.second[0]; });
}

std::vector<Item_upgrade_result> Item_upgrades::get_armor_upgrades(const Character& character, Socket socket,
                                                                   bool first_item)
{
    Character character_copy = character;
    const auto& special_stats = character_copy.total_special_stats;
    auto items = armory.get_items_in_socket(socket);
    if (socket != Socket::trinket)
    {
        std::string debug_string;
        items =
            item_optimizer.remove_weaker_items(items, special_stats, debug_string, config.number_of_items_to_consider);
    }
    {
        size_t i = 0;
        while (i < items.size())
        {
            if (character_copy.has_item(items[i].name))
            {
                items.erase(items.begin() + i);
            }
            else
            {
                i++;
            }
        }
    }

    double quantile = Statistics::find_cdf_quantile(0.99, 0.01);

    std::vector<Item_upgrade_result> item_upgrades{};
    item_upgrades.reserve(items.size());
    for (const auto& item : items)
    {
        character_copy.replace_armor(item, first_item);
        armory.compute_total_stats(character_copy);
        bool stop_iterating = false;
        for (size_t iter = 0; iter < batches_per_iteration.size(); iter++)
        {
            simulator.simulate(character_copy, batches_per_iteration[iter], simulator.get_dps_mean(),
                               simulator.get_dps_variance(), cumulative_simulations[iter]);
            double sample_std =
                Statistics::sample_deviation(std::sqrt(simulator.get_dps_variance()), cumulative_simulations[iter + 1]);
            if ((simulator.get_dps_mean() - sample_std * quantile >= config.dps_mean ||
                 simulator.get_dps_mean() + sample_std * quantile <= config.dps_mean) &&
                cumulative_simulations[iter + 1] > config.minimum_iterations)
            {
                stop_iterating = true;
            }
            if (cumulative_simulations[iter + 1] > config.maximum_iterations)
            {
                stop_iterating = true;
            }
            if (stop_iterating)
            {
                double dps_increase = simulator.get_dps_mean() - config.dps_mean;
                double dps_increase_std = Statistics::add_standard_deviations(sample_std, config.dps_sample_std);
                item_upgrades.emplace_back(item.name, std::array<double, 2>{dps_increase, dps_increase_std});
                break;
            }
        }
    }
    sort_item_upgrades_descending(item_upgrades);
    return item_upgrades;
}
