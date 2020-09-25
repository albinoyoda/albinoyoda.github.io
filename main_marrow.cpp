#include "sim_interface.hpp"

#include <Armory.hpp>
#include <Character.hpp>
#include <Combat_simulator.hpp>
#include <ctime>
#include <fstream>
#include <iostream>

int main()
{
    clock_t startTime = clock();

    // Simulator & Combat settings
    Combat_simulator_config config{};
    config.n_batches = 2000;
    config.sim_time = 60;
    config.opponent_level = 63;

    config.n_sunder_armor_stacks = 5;
    config.curse_of_recklessness_active = true;
    config.faerie_fire_feral_active = true;
    config.talents.death_wish = true;
    config.combat.use_bt_in_exec_phase = true;
    config.combat.use_hs_in_exec_phase = true;
    //    config.enable_recklessness = true;
    //    config.mode.sulfuron_harbinger = true;
    //    config.mode.golemagg = true;
    //    config.mode.vaelastrasz = true;
    //    config.mode.chromaggus = true;
    //    config.combat.cleave_if_adds = true;
    //    config.combat.use_hamstring = true;

    config.talents.improved_heroic_strike = 2;
    config.talents.unbridled_wrath = 5;
    config.talents.flurry = 5;
    config.talents.anger_management = true;
    config.talents.impale = 2;
    config.talents.improved_execute = 2;
    config.talents.dual_wield_specialization = 5;

    config.combat.heroic_strike_rage_thresh = 50;
    config.combat.cleave_rage_thresh = 50;
    config.combat.whirlwind_rage_thresh = 0;
    config.combat.hamstring_cd_thresh = 2;
    config.combat.hamstring_thresh_dd = 70;
    config.combat.initial_rage = 0;

    config.use_sim_time_ramp = true;
    config.enable_bloodrage = true;
    config.use_seed = true;
    config.seed = 110000;
    config.fuel_extra_rage = false;
    config.extra_rage_interval = 3;
    config.extra_rage_damage_amount = 150;
    config.performance_mode = true;

    Combat_simulator simulator{};
    simulator.set_config(config);

    static constexpr size_t crit_max = 50;
    static constexpr size_t hit_max = 10;
    static constexpr size_t ap_max = 2;
    std::array<std::array<std::array<std::array<double, 3>, ap_max>, hit_max>, crit_max> result{};

    Buffs buffs{};
    Armory armory{};
    Character character{Race::human, 60};
    Weapon main_hand{"mh", {}, {}, 2.6, 100, 200, Weapon_socket::main_hand, Weapon_type::sword};
    Weapon off_hand{"oh", {}, {}, 1.8, 60, 120, Weapon_socket::off_hand, Weapon_type::sword};
    character.equip_weapon(main_hand, off_hand);
    armory.compute_total_stats(character);

    for (size_t crit = 0; crit < crit_max; crit++)
    {
        for (size_t hit = 0; hit < hit_max; hit++)
        {
            for (size_t ap = 0; ap < ap_max; ap++)
            {
                character.total_special_stats.critical_strike = crit;
                character.total_special_stats.hit = hit;
                character.total_special_stats.attack_power = 1500 + ap * 500;

                simulator.simulate(character, 0);
                result[crit][hit][ap][0] = simulator.get_dps_mean();
                result[crit][hit][ap][1] = std::sqrt(simulator.get_dps_variance());
                result[crit][hit][ap][2] =
                    Statistics::sample_deviation(std::sqrt(simulator.get_dps_variance()), config.n_batches);
            }
        }
    }

    std::cout << "Done with simulations!\n";
    std::cout << "Simulations executed in: " << double(clock() - startTime) / (double)CLOCKS_PER_SEC << " seconds.\n";
    std::cout << "Writing to file.";

    std::ofstream myfile;
    myfile.open("to_marrow.txt", std::ios::out);
    if (myfile.is_open())
    {
        myfile << "#Format: crit%, hit%, AP, DPS, std(DPS), std(DPS_est)\n";
        myfile << "#Main-hand weapon: speed 2.6, damage 100-200\n";
        myfile << "#Off-hand  weapon: speed 1.8, damage 60-120\n";
        for (size_t crit = 0; crit < crit_max; crit++)
        {
            for (size_t hit = 0; hit < hit_max; hit++)
            {
                for (size_t ap = 0; ap < ap_max; ap++)
                {
                    myfile << string_with_precision(crit, 3) + ", " + string_with_precision(hit, 3) + ", " +
                                  string_with_precision(1500 + ap * 500, 4) + ", " +
                                  string_with_precision(result[crit][hit][ap][0], 4) + ", " +
                                  string_with_precision(result[crit][hit][ap][1], 3) + ", " +
                                  string_with_precision(result[crit][hit][ap][2], 3) + "\n";
                }
            }
        }
    }
    myfile.close();

    return 0;
}