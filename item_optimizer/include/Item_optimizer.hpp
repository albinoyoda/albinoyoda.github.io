#ifndef WOW_SIMULATOR_ITEM_OPTIMIZER_HPP
#define WOW_SIMULATOR_ITEM_OPTIMIZER_HPP

#include "Armory.hpp"
#include "Character.hpp"
#include "item_heuristics.hpp"
#include "string_helpers.hpp"

#include <algorithm>
#include <ctime>
#include <iostream>
#include <sstream>

class Item_optimizer
{
public:
    struct Sim_result_t
    {
        Sim_result_t() = default;

        Sim_result_t(size_t index, double mean_dps, double variance, double ap_equivalent)
            : index{index}, mean_dps{mean_dps}, variance{variance}, ap_equivalent{ap_equivalent}
        {
        }

        size_t index{};
        double mean_dps{};
        double variance{};
        double ap_equivalent{};
    };

    void compute_weapon_combinations();

    std::vector<std::vector<Armor>> get_combinations(const std::vector<Armor>& armors);

    void item_setup(const std::vector<std::string>& armor_vec, const std::vector<std::string>& weapons_vec);

    void compute_combinations();

    void extract_armors(const std::vector<std::string>& armor_vec);

    void extract_weapons(const std::vector<std::string>& weapon_vec);

    std::vector<size_t> get_item_ids(size_t index);

    Character generate_character(const std::vector<size_t>& item_ids);

    Character construct(size_t index);

    std::vector<Weapon> remove_weaker_weapons(Weapon_socket weapon_socket, const std::vector<Weapon>& weapon_vec,
                                              const Special_stats& special_stats, std::string& debug_message,
                                              int keep_n_stronger_items);

    std::vector<Armor> remove_weaker_items(const std::vector<Armor>& armors, const Special_stats& special_stats,
                                           std::string& debug_message, int keep_n_stronger_items);

    void filter_weaker_items(const Special_stats& special_stats, std::string& debug_message, int min_removal);

    void fill_empty_armor();

    void fill_empty_weapons();

    void find_set_bonuses();

    void fill(std::vector<Armor>& vec, Socket socket, std::string name);

    std::vector<Armor> helmets;
    std::vector<Armor> necks;
    std::vector<Armor> shoulders;
    std::vector<Armor> backs;
    std::vector<Armor> chests;
    std::vector<Armor> wrists;
    std::vector<Armor> hands;
    std::vector<Armor> belts;
    std::vector<Armor> legs;
    std::vector<Armor> boots;
    std::vector<Armor> ranged;
    std::vector<Armor> rings;
    std::vector<Armor> trinkets;
    std::vector<Weapon> main_hands;
    std::vector<Weapon> off_hands;
    std::vector<Weapon> two_hands;

    std::vector<Set> set_names;
    std::vector<Set_bonus> possible_set_bonuses;

    std::string best_use_effect_name{};
    double sim_time{};

    std::vector<std::vector<Weapon>> weapon_combinations;
    std::vector<std::vector<Armor>> ring_combinations;
    std::vector<std::vector<Armor>> trinket_combinations;

    size_t total_combinations{};
    std::vector<size_t> combination_vector{};
    std::vector<size_t> cum_combination_vector{};

    Race race{};
    Buffs buffs;
    std::vector<std::string> buffs_vec;
    std::vector<std::string> ench_vec;
    std::vector<std::string> talent_vec;
    std::vector<int> talent_val_vec;

    Armory armory;
};

bool operator<(const Item_optimizer::Sim_result_t& left, const Item_optimizer::Sim_result_t& right);

#endif // WOW_SIMULATOR_ITEM_OPTIMIZER_HPP
