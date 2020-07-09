#include "sim_interface.hpp"

#include <Armory.hpp>
#include <Character.hpp>
#include <Combat_simulator.hpp>
#include <ctime>
#include <fstream>
#include <iostream>

int main()
{
    //    clock_t startTime = clock();
    //    Buffs buffs{};
    //    Armory armory{};
    //    Character character{Race::human, 60};
    //
    //    character.equip_weapon(armory.find_weapon(""), armory.find_weapon(""));
    //
    //    armory.compute_total_stats(character);
    //
    //    // Simulator & Combat settings
    //    Combat_simulator_config config{};
    //    config.n_batches = 1000;
    //    config.sim_time = 60;
    //    config.opponent_level = 63;
    //
    //    config.n_sunder_armor_stacks = 5;
    //    config.curse_of_recklessness_active = true;
    //    config.faerie_fire_feral_active = true;
    //    config.talents.death_wish = true;
    //    config.combat.use_bt_in_exec_phase = true;
    //    config.combat.use_hs_in_exec_phase = true;
    //    //    config.enable_recklessness = true;
    //    //    config.mode.sulfuron_harbinger = true;
    //    //    config.mode.golemagg = true;
    //    //    config.mode.vaelastrasz = true;
    //    //    config.mode.chromaggus = true;
    //    //    config.combat.cleave_if_adds = true;
    //    //    config.combat.use_hamstring = true;
    //
    //    config.talents.improved_heroic_strike = 2;
    //    config.talents.unbridled_wrath = 5;
    //    config.talents.flurry = 5;
    //    config.talents.anger_management = true;
    //    config.talents.impale = 2;
    //    config.talents.improved_execute = 2;
    //    config.talents.dual_wield_specialization = 5;
    //
    //    config.combat.heroic_strike_rage_thresh = 50;
    //    config.combat.cleave_rage_thresh = 50;
    //    config.combat.whirlwind_rage_thresh = 0;
    //    config.combat.hamstring_cd_thresh = 2;
    //    config.combat.hamstring_thresh_dd = 70;
    //    config.combat.initial_rage = 0;
    //
    //    config.use_sim_time_ramp = true;
    //    config.enable_bloodrage = true;
    //    config.use_seed = true;
    //    config.seed = 110000;
    //    config.fuel_extra_rage = false;
    //    config.extra_rage_interval = 3;
    //    config.extra_rage_damage_amount = 150;
    //    config.performance_mode = true;
    //
    //    Combat_simulator simulator(config);
    //
    //    std::array<std::array<std::array<std::array<double, 2>, 4>, 5>, 10> result{};

    std::ofstream myfile;
    myfile.open("example.txt", std::ios::out);
    if (myfile.is_open())
    {
        myfile << 31231414;
    }
    myfile.close();

    //    for (size_t crit = 0; crit < 10; crit++)
    //    {
    //        for (size_t hit = 0; hit < 5; hit++)
    //        {
    //            for (size_t ap = 0; ap < 4; ap++)
    //            {
    //                character.total_special_stats.critical_strike = crit;
    //                character.total_special_stats.hit = hit;
    //                character.total_special_stats.attack_power = 1500 + ap * 25;
    //
    //                simulator.simulate(character, 0);
    //                result[crit][hit][ap][0] = simulator.get_dps_mean();
    //                result[crit][hit][ap][1] = simulator.get_dps_variance();
    //            }
    //        }
    //    }

    return 0;
}