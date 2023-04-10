#ifndef WOW_SIMULATOR_SPELLS_HPP
#define WOW_SIMULATOR_SPELLS_HPP

#include "damage_sources.hpp"
#include "hit_tables.hpp"
#include "weapon_sim.hpp"

#include <iostream>

namespace
{
constexpr int slam_bonus_damage = 87;
constexpr int mortal_strike_bonus_damage = 160;
constexpr int overpower_bonus_damage = 35;
constexpr int hamstring_damage = 35;
} // namespace

class Spell_base
{
public:
    constexpr Spell_base(int rage_cost, Sim_time cooldown, bool triggers_gcd, Damage_source damage_source)
        : rage_cost{rage_cost}
        , cooldown{cooldown}
        , triggers_gcd{triggers_gcd}
        , damage_source{damage_source}
        , reduced_rage_cost{rage_cost / 3} {};

    virtual double damage(Weapon_sim& main_hand_weapon, Special_stats& special_stats) const = 0;

    virtual void pay_rage(const Melee_result result, double& rage) const
    {
        if (result == Melee_result::dodge || result == Melee_result::miss)
        {
            rage -= rage_cost;
        }
        else
        {
            rage -= reduced_rage_cost;
        }
    }

    int rage_cost;
    Sim_time cooldown;
    bool triggers_gcd;
    Damage_source damage_source;
    int reduced_rage_cost;
};

class Slam : public Spell_base
{
    using Spell_base::Spell_base;

    double damage(Weapon_sim& main_hand_weapon, Special_stats& special_stats) const final
    {
        return main_hand_weapon.swing(special_stats.attack_power) + slam_bonus_damage;
    }
};

class Bloodthirst : public Spell_base
{
    using Spell_base::Spell_base;

    double damage(Weapon_sim&, Special_stats& special_stats) const final { return special_stats.attack_power * 0.45; }
};

class Mortal_strike : public Spell_base
{
    using Spell_base::Spell_base;

    double damage(Weapon_sim& main_hand_weapon, Special_stats& special_stats) const final
    {
        return main_hand_weapon.normalized_swing(special_stats.attack_power) + mortal_strike_bonus_damage;
    }
};

class Overpower : public Spell_base
{
    using Spell_base::Spell_base;

    double damage(Weapon_sim& main_hand_weapon, Special_stats& special_stats) const final
    {
        return main_hand_weapon.normalized_swing(special_stats.attack_power) + overpower_bonus_damage;
    }
};

class Whirlwind : public Spell_base
{
    using Spell_base::Spell_base;

    double damage(Weapon_sim& main_hand_weapon, Special_stats& special_stats) const final
    {
        return main_hand_weapon.normalized_swing(special_stats.attack_power);
    }
};

class Execute : public Spell_base
{
    using Spell_base::Spell_base;

    double damage(Weapon_sim&, Special_stats&) const final { return 600; } // + (rage - execute_rage_cost_) * 15; }
};

class Hamstring : public Spell_base
{
    using Spell_base::Spell_base;

    double damage(Weapon_sim&, Special_stats&) const final { return hamstring_damage; }
};

class Sweeping_strikes : public Spell_base
{
    using Spell_base::Spell_base;

    double damage(Weapon_sim&, Special_stats&) const final
    {
        std::cout << "not implemented!\n";
        return hamstring_damage;
    }
};

class Sunder_armor : public Spell_base
{
    using Spell_base::Spell_base;

    double damage(Weapon_sim&, Special_stats&) const final
    {
        std::cout << "not implemented!\n";
        return hamstring_damage;
    }
};

namespace spellbook
{
constexpr Slam slam{15, Sim_time::from_seconds(0), false, Damage_source::slam};
constexpr Bloodthirst bloodthirst{30, Sim_time::from_seconds(6), true, Damage_source::bloodthirst};
constexpr Mortal_strike mortal_strike{30, Sim_time::from_seconds(6), true, Damage_source::mortal_strike};
constexpr Overpower overpower{5, Sim_time::from_seconds(5), true, Damage_source::overpower};
constexpr Hamstring hamstring{10, Sim_time::from_seconds(0), true, Damage_source::hamstring};
constexpr Execute execute{10, Sim_time::from_seconds(0), true, Damage_source::execute};
constexpr Whirlwind whirlwind{25, Sim_time::from_seconds(10), true, Damage_source::whirlwind};
constexpr Sweeping_strikes sweeping_strikes{30, Sim_time::from_seconds(30), false, Damage_source::sweeping_strikes};
constexpr Sunder_armor sunder_armor{15, Sim_time::from_seconds(0), true, Damage_source::sweeping_strikes};

const Over_time_effect essence_of_the_red = {"Essence of the Red",       {}, 20, 0, Sim_time::from_seconds(1),
                                             Sim_time::from_seconds(600)};

const Over_time_effect anger_management = {"Anger Management",         {}, 1, 0, Sim_time::from_seconds(3),
                                           Sim_time::from_seconds(600)};

Over_time_effect deep_wounds(double hit_damage);

} // namespace spellbook
// void Combat_simulator::slam(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
//                             Damage_sources& damage_sources, int& flurry_charges)
//{
//     if (config.dpr_settings.compute_dpr_sl_)
//     {
//         get_uniform_random(100) < hit_table_yellow_[1] ? rage -= 3 : rage -= 15;
//         return;
//     }
//     simulator_cout("Slam!");
//     double damage = main_hand_weapon.swing(special_stats.attack_power) + slam_bonus_damage;
//     auto hit_outcome =
//         generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats, damage_sources);
//     if (hit_outcome.hit_result == Hit_result::dodge || hit_outcome.hit_result == Hit_result::miss)
//     {
//         rage -= 3;
//     }
//     else
//     {
//         rage -= 15;
//         hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges);
//     }
//     manage_flurry(hit_outcome.hit_result, special_stats, flurry_charges, true);
//     damage_sources.add_damage(Damage_source::slam, hit_outcome.damage, time_keeper_.time);
//     simulator_cout("Current rage: ", int(rage));
// }
//
// void Combat_simulator::mortal_strike(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
//                                      Damage_sources& damage_sources, int& flurry_charges)
//{
//     if (config.dpr_settings.compute_dpr_ms_)
//     {
//         get_uniform_random(100) < hit_table_yellow_[1] ? rage -= 6 : rage -= 30;
//         return;
//     }
//     simulator_cout("Mortal Strike!");
//     double damage = main_hand_weapon.normalized_swing(special_stats.attack_power) + 160;
//     auto hit_outcome =
//         generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats, damage_sources);
//     if (hit_outcome.hit_result == Hit_result::dodge || hit_outcome.hit_result == Hit_result::miss)
//     {
//         rage -= 6;
//     }
//     else
//     {
//         rage -= 30;
//         hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges);
//     }
//     time_keeper_.mortal_strike_cd = Sim_time::from_seconds(6.0);
//     time_keeper_.global_cd = Sim_time::from_seconds(1.5);
//     manage_flurry(hit_outcome.hit_result, special_stats, flurry_charges, true);
//     damage_sources.add_damage(Damage_source::mortal_strike, hit_outcome.damage, time_keeper_.time);
//     simulator_cout("Current rage: ", int(rage));
// }
//
// void Combat_simulator::bloodthirst(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
//                                    Damage_sources& damage_sources, int& flurry_charges)
//{
//     if (config.dpr_settings.compute_dpr_bt_)
//     {
//         get_uniform_random(100) < hit_table_yellow_[1] ? rage -= 6 : rage -= 30;
//         time_keeper_.blood_thirst_cd = Sim_time::from_seconds(6.0);
//         time_keeper_.global_cd = Sim_time::from_seconds(1.5);
//         return;
//     }
//     simulator_cout("Bloodthirst!");
//     double damage = special_stats.attack_power * 0.45;
//     auto hit_outcome = generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats,
//                                     damage_sources, true, false, true, true);
//     if (hit_outcome.hit_result == Hit_result::dodge || hit_outcome.hit_result == Hit_result::miss)
//     {
//         rage -= 6;
//     }
//     else
//     {
//         rage -= 30;
//         hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges);
//     }
//     time_keeper_.blood_thirst_cd = Sim_time::from_seconds(6.0);
//     time_keeper_.global_cd = Sim_time::from_seconds(1.5);
//     manage_flurry(hit_outcome.hit_result, special_stats, flurry_charges, true);
//     damage_sources.add_damage(Damage_source::bloodthirst, hit_outcome.damage, time_keeper_.time);
//     simulator_cout("Current rage: ", int(rage));
// }
//
// void Combat_simulator::overpower(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
//                                  Damage_sources& damage_sources, int& flurry_charges)
//{
//     if (config.dpr_settings.compute_dpr_op_)
//     {
//         if (rage > tactical_mastery_rage_)
//         {
//             rage = tactical_mastery_rage_;
//         }
//         rage -= 5;
//         buff_manager_.add("battle_stance", {-3.0, 0, 0}, Sim_time::from_seconds(1.5));
//         time_keeper_.overpower_cd = Sim_time::from_seconds(5.0);
//         time_keeper_.global_cd = Sim_time::from_seconds(1.5);
//         return;
//     }
//     simulator_cout("Changed stance: Battle Stance.");
//     simulator_cout("Overpower!");
//     buff_manager_.add("battle_stance", {-3.0, 0, 0}, Sim_time::from_seconds(1.5));
//     double damage = main_hand_weapon.normalized_swing(special_stats.attack_power) + 35;
//     auto hit_outcome = generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats,
//                                     damage_sources, true, true);
//     if (rage > tactical_mastery_rage_)
//     {
//         rage_lost_stance_swap_ += rage - tactical_mastery_rage_;
//         rage = tactical_mastery_rage_;
//     }
//     rage -= 5;
//     if (hit_outcome.hit_result != Hit_result::miss)
//     {
//         hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges);
//     }
//     time_keeper_.overpower_cd = Sim_time::from_seconds(5.0);
//     time_keeper_.global_cd = Sim_time::from_seconds(1.5);
//     manage_flurry(hit_outcome.hit_result, special_stats, flurry_charges, true);
//     damage_sources.add_damage(Damage_source::overpower, hit_outcome.damage, time_keeper_.time);
//     simulator_cout("Current rage: ", int(rage));
// }
//
// void Combat_simulator::whirlwind(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
//                                  Damage_sources& damage_sources, int& flurry_charges)
//{
//     if (config.dpr_settings.compute_dpr_ww_)
//     {
//         rage -= 25;
//         time_keeper_.whirlwind_cd = Sim_time::from_seconds(10);
//         time_keeper_.global_cd = Sim_time::from_seconds(1.5);
//         return;
//     }
//     simulator_cout("Whirlwind! #targets = boss + ", number_of_extra_targets_, " adds");
//     simulator_cout("Whirlwind hits: ", std::min(number_of_extra_targets_ + 1, 4), " targets");
//     double damage = main_hand_weapon.normalized_swing(special_stats.attack_power);
//     std::vector<Hit_outcome> hit_outcomes{};
//     hit_outcomes.reserve(4);
//     for (int i = 0; i < std::min(number_of_extra_targets_ + 1, 4); i++)
//     {
//         hit_outcomes.emplace_back(generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand,
//                                                special_stats, damage_sources, i == 0, false, i == 0));
//         if (hit_outcomes.back().hit_result != Hit_result::dodge && hit_outcomes.back().hit_result !=
//         Hit_result::miss)
//         {
//             hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges);
//         }
//     }
//     rage -= 25;
//     time_keeper_.whirlwind_cd = Sim_time::from_seconds(10);
//     time_keeper_.global_cd = Sim_time::from_seconds(1.5);
//     Hit_result result_used_for_flurry = Hit_result::TBD;
//     double total_damage = 0;
//     for (const auto& hit_outcome : hit_outcomes)
//     {
//         total_damage += hit_outcome.damage;
//         if (hit_outcome.hit_result == Hit_result::crit)
//         {
//             result_used_for_flurry = Hit_result::crit;
//         }
//     }
//     manage_flurry(result_used_for_flurry, special_stats, flurry_charges, true);
//     damage_sources.add_damage(Damage_source::whirlwind, total_damage, time_keeper_.time);
//     simulator_cout("Current rage: ", int(rage));
// }
//
// void Combat_simulator::execute(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
//                                Damage_sources& damage_sources, int& flurry_charges)
//{
//     if (config.dpr_settings.compute_dpr_ex_)
//     {
//         get_uniform_random(100) < hit_table_yellow_[1] ? rage *= 0.8 : rage -= execute_rage_cost_;
//         time_keeper_.global_cd = Sim_time::from_seconds(1.5);
//         rage_spent_on_execute_ += rage;
//         rage = 0;
//         return;
//     }
//     simulator_cout("Execute!");
//     double damage = 600 + (rage - execute_rage_cost_) * 15;
//     auto hit_outcome = generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats,
//                                     damage_sources, true, false, true, true);
//     if (hit_outcome.hit_result == Hit_result::dodge || hit_outcome.hit_result == Hit_result::miss)
//     {
//         rage *= 0.8;
//     }
//     else
//     {
//         rage -= execute_rage_cost_;
//         hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges);
//     }
//     rage_spent_on_execute_ += rage;
//     rage = 0;
//     time_keeper_.global_cd = Sim_time::from_seconds(1.5);
//     manage_flurry(hit_outcome.hit_result, special_stats, flurry_charges, true);
//     damage_sources.add_damage(Damage_source::execute, hit_outcome.damage, time_keeper_.time);
//     simulator_cout("Current rage: ", int(rage));
// }
//
// void Combat_simulator::hamstring(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
//                                  Damage_sources& damage_sources, int& flurry_charges)
//{
//     if (config.dpr_settings.compute_dpr_ha_)
//     {
//         get_uniform_random(100) < hit_table_yellow_[1] ? rage -= 2 : rage -= 10;
//         time_keeper_.global_cd = Sim_time::from_seconds(1.5);
//         return;
//     }
//     simulator_cout("Hamstring!");
//     double damage = 45;
//     auto hit_outcome =
//         generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats, damage_sources);
//     time_keeper_.global_cd = Sim_time::from_seconds(1.5);
//     if (hit_outcome.hit_result == Hit_result::dodge || hit_outcome.hit_result == Hit_result::miss)
//     {
//         rage -= 2;
//     }
//     else
//     {
//         rage -= 10;
//         hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges);
//     }
//     manage_flurry(hit_outcome.hit_result, special_stats, flurry_charges, true);
//     damage_sources.add_damage(Damage_source::hamstring, hit_outcome.damage, time_keeper_.time);
//     simulator_cout("Current rage: ", int(rage));
//}

#endif // WOW_SIMULATOR_SPELLS_HPP
