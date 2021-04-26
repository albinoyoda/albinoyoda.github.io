#ifndef WOW_SIMULATOR_ITEM_UPGRADES_HPP
#define WOW_SIMULATOR_ITEM_UPGRADES_HPP

#include "Armory.hpp"
#include "Item_optimizer.hpp"

#include <Combat_simulator.hpp>

using Item_upgrade_result = std::pair<std::string, std::array<double, 2>>;

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
        for (int i = 0; i < config.n_batches; i++)
        {
            batches_per_iteration.push_back(batches_per_iteration.back() * config.growth_rate);
            cumulative_simulations.push_back(cumulative_simulations.back() + batches_per_iteration[i]);
        }
        cumulative_simulations.push_back(cumulative_simulations.back() + batches_per_iteration.back());
    }

    [[nodiscard]] std::vector<Item_upgrade_result> get_armor_upgrades(const Character& character, Socket socket,
                                                                      bool first_item);

private:
    Combat_simulator simulator{};
    Item_optimizer item_optimizer{};
    Armory& armory;
    std::vector<size_t> batches_per_iteration{};
    std::vector<size_t> cumulative_simulations{};
    Item_upgrades_config config;
};
#endif // WOW_SIMULATOR_ITEM_UPGRADES_HPP
