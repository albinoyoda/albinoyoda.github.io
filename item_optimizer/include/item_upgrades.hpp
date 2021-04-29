#ifndef WOW_SIMULATOR_ITEM_UPGRADES_HPP
#define WOW_SIMULATOR_ITEM_UPGRADES_HPP

#include "Armory.hpp"
#include "Combat_simulator.hpp"
#include "Item_optimizer.hpp"
#include "Statistics.hpp"

using Item_upgrade_result = std::pair<std::string, Distribution>;

struct Item_upgrades_config
{
    Item_upgrades_config() = default;

    Item_upgrades_config(const size_t first_batch_size, const size_t n_batches, const size_t minimum_iterations,
                         const size_t maximum_iterations, const size_t number_of_items_to_consider,
                         const double growth_rate, const double dps_mean, const double dps_sample_std)
        : first_batch_size{first_batch_size}
        , n_batches{n_batches}
        , minimum_iterations{minimum_iterations}
        , maximum_iterations{maximum_iterations}
        , number_of_items_to_consider{number_of_items_to_consider}
        , growth_rate{growth_rate}
        , dps_mean{dps_mean}
        , dps_sample_std{dps_sample_std} {};
    const size_t first_batch_size{};
    const size_t n_batches{};
    const size_t minimum_iterations{};
    const size_t maximum_iterations{};
    const size_t number_of_items_to_consider{};
    const double growth_rate{};
    const double dps_mean{};
    const double dps_sample_std{};
};

class Item_upgrades
{
public:
    explicit Item_upgrades(Combat_simulator_config simulator_config, Item_upgrades_config item_upgrades_config)
        : armory{item_optimizer.armory}, config{item_upgrades_config}
    {
        simulator.set_config(simulator_config);
        initialize_batch_sizes();
    };

    void initialize_batch_sizes()
    {
        batches_per_iteration.clear();
        cumulative_simulations.clear();

        batches_per_iteration = {config.first_batch_size};
        cumulative_simulations = {0};
        for (size_t i = 0; i < config.n_batches; i++)
        {
            batches_per_iteration.push_back(batches_per_iteration.back() * config.growth_rate);
            cumulative_simulations.push_back(cumulative_simulations.back() + batches_per_iteration[i]);
        }
        cumulative_simulations.push_back(cumulative_simulations.back() + batches_per_iteration.back());
    }

    [[nodiscard]] std::vector<Item_upgrade_result> get_armor_upgrades(const Character& character, Socket socket,
                                                                      bool first_item = true);

    [[nodiscard]] std::vector<Item_upgrade_result> get_weapon_upgrades(const Character& character,
                                                                       Weapon_socket socket);

    static void sort_item_upgrades_descending(std::vector<Item_upgrade_result>& item_upgrades)
    {
        std::sort(
            item_upgrades.begin(), item_upgrades.end(),
            [](const Item_upgrade_result& a, const Item_upgrade_result& b) { return a.second.mean_ > b.second.mean_; });
    }

    template <typename T_vec>
    [[nodiscard]] std::vector<Item_upgrade_result> get_item_upgrades(Character& character, const T_vec& items,
                                                                     Socket socket, bool first_item = true)
    {
        std::vector<Item_upgrade_result> item_upgrades{};
        item_upgrades.reserve(items.size());
        for (const auto& item : items)
        {
            character.equip_and_replace(item, socket, first_item);
            armory.compute_total_stats(character);
            bool stop_iterating = false;
            for (size_t iter = 0; iter < batches_per_iteration.size(); iter++)
            {
                simulator.simulate(character, batches_per_iteration[iter], simulator.get_dps_mean(),
                                   simulator.get_dps_variance(), cumulative_simulations[iter]);
                auto distribution = simulator.get_dps_distribution();
                auto confidence_interval = distribution.confidence_interval_of_the_mean(0.99);
                if ((confidence_interval.first <= config.dps_mean || confidence_interval.second >= config.dps_mean) &&
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
                    double dps_increase_std =
                        Statistics::add_standard_deviations(distribution.std_of_the_mean(), config.dps_sample_std);
                    item_upgrades.emplace_back(item.name, Distribution{dps_increase, dps_increase_std});
                    break;
                }
            }
        }
        sort_item_upgrades_descending(item_upgrades);
        return item_upgrades;
    }

    void set_race(const Race race) { item_optimizer.race = race; }

private:
    Combat_simulator simulator{};
    Item_optimizer item_optimizer{};
    Armory& armory;
    std::vector<size_t> batches_per_iteration{};
    std::vector<size_t> cumulative_simulations{};
    Item_upgrades_config config;
};
#endif // WOW_SIMULATOR_ITEM_UPGRADES_HPP
