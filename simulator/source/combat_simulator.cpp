#include "simulator/combat_simulator.hpp"

#include "item_heuristics.hpp"
#include "simulator/spells.hpp"
#include "simulator/use_effect_schedule.hpp"
#include "simulator/use_effects.hpp"
#include "statistics/Statistics.hpp"

namespace
{

constexpr double rage_per_damage = 15.0 / 230.6 / 2.0;

constexpr double rage_from_damage_taken(double damage)
{
    return damage * 5.0 / 2.0 / 230.6;
}

constexpr double rage_generation(double damage)
{
    return damage * rage_per_damage;
}

constexpr double armor_mitigation(int target_armor, int target_level)
{
    return static_cast<double>(target_armor) / static_cast<double>(target_armor + 400 + 85 * target_level);
}

} // namespace

void Combat_simulator::set_config(const Combat_simulator_config& new_config)
{
    config = new_config;

    heroic_strike_rage_cost = 15 - config.talents.improved_heroic_strike;
    p_unbridled_wrath_ = 0.08 * config.talents.unbridled_wrath;
    execute_rage_cost_ = 15 - static_cast<int>(2.51 * config.talents.improved_execute);

    armor_reduction_from_spells_ = 0.0;
    armor_reduction_from_spells_ += 450 * config.n_sunder_armor_stacks;
    armor_reduction_from_spells_ += 640 * config.curse_of_recklessness_active;
    armor_reduction_from_spells_ += 505 * config.faerie_fire_feral_active;
    if (config.exposed_armor)
    {
        armor_reduction_delayed_ = 2550 - 450 * config.n_sunder_armor_stacks;
    }

    flurry_haste_factor_ = (config.talents.flurry > 0) ? 0.05 + 0.05 * config.talents.flurry : 0.0;
    dual_wield_damage_factor_ = 0.5 + 0.025 * config.talents.dual_wield_specialization;
    cleave_bonus_damage_ = 50 * (1.0 + 0.4 * config.talents.improved_cleave);
    slam_manager = Slam_manager{Sim_time::from_seconds(1.5 - 0.1 * config.talents.improved_slam)};

    tactical_mastery_rage_ = 5.0 * config.talents.tactical_mastery;
    deep_wounds_ = config.talents.deep_wounds && config.combat.deep_wounds;
    use_bloodthirst_ = config.talents.bloodthirst && config.combat.use_bloodthirst;
    use_mortal_strike_ = config.talents.mortal_strike && config.combat.use_mortal_strike;
    use_sweeping_strikes_ =
        config.talents.sweeping_strikes && config.combat.use_sweeping_strikes && config.multi_target_mode_;

    over_time_effects_.clear();
    use_effects_all_.clear();

    if (config.talents.death_wish && config.combat.use_death_wish)
    {
        use_effects_all_.emplace_back(spellbook::deathwish);
    }

    if (config.enable_recklessness)
    {
        use_effects_all_.emplace_back(spellbook::recklessness);
    }

    if (config.enable_bloodrage)
    {
        use_effects_all_.emplace_back(spellbook::bloodrage);
    }

    if (config.enable_berserking)
    {
        use_effects_all_.emplace_back(Use_effect{"Berserking",
                                                 Use_effect::Effect_socket::unique,
                                                 {},
                                                 {0, 0, 0, 0, config.berserking_haste_ / 100.0},
                                                 0,
                                                 Sim_time::from_seconds(10),
                                                 Sim_time::from_seconds(180),
                                                 false});
    }

    if (config.essence_of_the_red_)
    {
        over_time_effects_.push_back(spellbook::essence_of_the_red);
    }

    if (config.take_periodic_damage_)
    {
        double rage_per_tick = rage_from_damage_taken(config.periodic_damage_amount_);
        Over_time_effect rage_from_damage = {
            "Rage gained from damage taken", {}, rage_per_tick, 0, config.periodic_damage_interval_,
            Sim_time::from_seconds(600)};
        over_time_effects_.push_back(rage_from_damage);
    }

    if (config.talents.anger_management)
    {
        over_time_effects_.push_back(spellbook::anger_management);
    }
    hit_tables = Hit_tables{config.main_target_level, config.talents.overpower, config.talents.impale};
}

Hit_outcome Combat_simulator::generate_hit(const Weapon_sim& weapon, Damage_sources& damage_sources, double damage,
                                           Hit_color hit_type, Socket weapon_hand, const Special_stats& special_stats,
                                           bool boss_target, bool is_overpower, bool is_spell, bool can_sweep)
{
    auto hit_table_result{hit_tables.generate_hit(hit_type, weapon_hand, is_overpower, is_spell,
                                                  ability_queue_manager.is_ability_queued())};
    Hit_outcome hit_outcome{hit_table_result, damage,
                            boss_target ? armor_reduction_factor_ : armor_reduction_factor_add,
                            special_stats.damage_mod_physical};

    if (sweeping_strikes_charges_ > 0)
    {
        if (hit_outcome.damage > 0.0 && can_sweep)
        {
            double sweeping_strike_damage{};
            if (boss_target)
            {
                sweeping_strike_damage = hit_outcome.damage / armor_reduction_factor_ * armor_reduction_factor_add;
                damage_sources.add_damage(Damage_source::sweeping_strikes, sweeping_strike_damage, time_keeper_.time);
            }
            else
            {
                double rand_var = get_uniform_random(1);
                if (number_of_extra_targets_ > 0 && rand_var > 1.0 - 1.0 / number_of_extra_targets_)
                {
                    sweeping_strike_damage = hit_outcome.damage / armor_reduction_factor_add * armor_reduction_factor_;
                }
                else
                {
                    sweeping_strike_damage = hit_outcome.damage;
                }
                damage_sources.add_damage(Damage_source::sweeping_strikes, sweeping_strike_damage, time_keeper_.time);
            }
            simulator_cout("Sweeping strikes hits a nearby target.");
            cout_damage_parse(Hit_color::yellow, Socket::main_hand,
                              Hit_table_result{sweeping_strike_damage, Melee_result::hit});
            sweeping_strikes_charges_--;
            simulator_cout("Sweeping strikes charges left: ", sweeping_strikes_charges_);
        }
    }

    if (deep_wounds_)
    {
        if (hit_table_result.melee_result == Melee_result::crit)
        {
            const double swing_damage{(1 + special_stats.damage_mod_physical) *
                                      weapon.swing(special_stats.attack_power) / 4};
            buff_manager_.add_over_time_effect(spellbook::deep_wounds(swing_damage), time_keeper_.time);
        }
    }

    if (hit_table_result.melee_result == Melee_result::dodge)
    {
        simulator_cout("Overpower aura gained!");
        buff_manager_.add("overpower_aura", {}, Sim_time::from_seconds(5));
    }
    return hit_outcome;
}

bool Combat_simulator::start_cast_slam(bool mh_swing, double rage, Sim_time& swing_time_left)
{
    bool use_sl = true;
    if (use_bloodthirst_)
    {
        use_sl &= cooldown_manager_.cooldown_of(Damage_source::bloodthirst) > config.combat.slam_cd_thresh;
    }
    if (use_mortal_strike_)
    {
        use_sl &= cooldown_manager_.cooldown_of(Damage_source::mortal_strike) > config.combat.slam_cd_thresh;
    }
    if (config.combat.use_whirlwind)
    {
        use_sl &= cooldown_manager_.cooldown_of(Damage_source::whirlwind) > config.combat.slam_cd_thresh;
    }
    if (use_sl)
    {
        if (swing_time_left > config.combat.slam_spam_max_time)
        {
            if ((mh_swing && rage > config.combat.slam_rage_dd) || rage > config.combat.slam_spam_rage)
            {
                simulator_cout("Starting to channel slam.");
                slam_manager.queue_slam(time_keeper_.time);
                cooldown_manager_.set_global_cooldown();
                // We have started 'channeling' so set a value for the swing time for now which is larger than GCD
                swing_time_left = Sim_time::from_seconds(1.6);
                return true;
            }
        }
    }
    return false;
}

void Combat_simulator::cast_spell(const Spell_base& spell, Weapon_sim& weapon, Special_stats& special_stats)
{
    double damage{spell.damage(weapon, special_stats)};

    auto hit_outcome =
        hit_tables.generate_hit(Hit_color::yellow, Socket::main_hand, spell.damage_source == Damage_source::overpower,
                                true, ability_queue_manager.is_ability_queued());

    cooldown_manager_.add(spell);

    //    rage_manager_.rage_

    //    rage_manager_
    //    rage = spell.

    // todo: pay rage
    // todo: dpr
    // todo: hit effects
    // todo: cd
    //    time_keeper_.update

    flurry_manager_.update(hit_outcome.melee_result);
    damage_distribution_.add_damage(spell.damage_source, damage, time_keeper_.time);
    // todo: damage_sources.add_damage
}

void Combat_simulator::slam(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                            Damage_sources& damage_sources)
{
    if (config.dpr_settings.compute_dpr_sl_)
    {
        get_uniform_random(100) < hit_tables.yellow_[1] ? rage -= 3 : rage -= 15;
        return;
    }
    simulator_cout("Slam!");
    double damage = main_hand_weapon.swing(special_stats.attack_power) + 87.0;
    auto hit_outcome =
        generate_hit(main_hand_weapon, damage_sources, damage, Hit_color::yellow, Socket::main_hand, special_stats);
    if (hit_outcome.hit_table_result.melee_result == Melee_result::dodge ||
        hit_outcome.hit_table_result.melee_result == Melee_result::miss)
    {
        rage -= 3;
    }
    else
    {
        rage -= 15;
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources);
    }
    flurry_manager_.update(hit_outcome.hit_table_result.melee_result);
    damage_sources.add_damage(Damage_source::slam, hit_outcome.damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::mortal_strike(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                                     Damage_sources& damage_sources)
{
    if (config.dpr_settings.compute_dpr_ms_)
    {
        get_uniform_random(100) < hit_tables.yellow_[1] ? rage -= 6 : rage -= 30;
        return;
    }
    simulator_cout("Mortal Strike!");
    double damage = main_hand_weapon.normalized_swing(special_stats.attack_power) + 160;
    auto hit_outcome =
        generate_hit(main_hand_weapon, damage_sources, damage, Hit_color::yellow, Socket::main_hand, special_stats);
    if (hit_outcome.hit_table_result.melee_result == Melee_result::dodge ||
        hit_outcome.hit_table_result.melee_result == Melee_result::miss)
    {
        rage -= 6;
    }
    else
    {
        rage -= 30;
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources);
    }
    cooldown_manager_.add(spellbook::mortal_strike);
    flurry_manager_.update(hit_outcome.hit_table_result.melee_result);
    damage_sources.add_damage(Damage_source::mortal_strike, hit_outcome.damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::bloodthirst(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                                   Damage_sources& damage_sources)
{
    if (config.dpr_settings.compute_dpr_bt_)
    {
        get_uniform_random(100) < hit_tables.yellow_[1] ? rage -= 6 : rage -= 30;
        cooldown_manager_.add(spellbook::bloodthirst);
        return;
    }
    simulator_cout("Bloodthirst!");
    double damage = special_stats.attack_power * 0.45;
    auto hit_outcome = generate_hit(main_hand_weapon, damage_sources, damage, Hit_color::yellow, Socket::main_hand,
                                    special_stats, true, false, true, true);
    if (hit_outcome.hit_table_result.melee_result == Melee_result::dodge ||
        hit_outcome.hit_table_result.melee_result == Melee_result::miss)
    {
        rage -= 6;
    }
    else
    {
        rage -= 30;
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources);
    }
    cooldown_manager_.add(spellbook::bloodthirst);
    flurry_manager_.update(hit_outcome.hit_table_result.melee_result);
    damage_sources.add_damage(Damage_source::bloodthirst, hit_outcome.damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::overpower(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                                 Damage_sources& damage_sources)
{
    if (config.dpr_settings.compute_dpr_op_)
    {
        if (rage > tactical_mastery_rage_)
        {
            rage = tactical_mastery_rage_;
        }
        rage -= 5;
        buff_manager_.add("battle_stance", {-3.0, 0, 0}, Sim_time::from_seconds(1.5));
        cooldown_manager_.add(spellbook::overpower);
        return;
    }
    simulator_cout("Changed stance: Battle Stance.");
    simulator_cout("Overpower!");
    buff_manager_.add("battle_stance", {-3.0, 0, 0}, Sim_time::from_seconds(1.5));
    double damage = main_hand_weapon.normalized_swing(special_stats.attack_power) + 35;
    auto hit_outcome = generate_hit(main_hand_weapon, damage_sources, damage, Hit_color::yellow, Socket::main_hand,
                                    special_stats, true, true);
    if (rage > tactical_mastery_rage_)
    {
        rage_lost_stance_swap_ += rage - tactical_mastery_rage_;
        rage = tactical_mastery_rage_;
    }
    rage -= 5;
    if (hit_outcome.hit_table_result.melee_result != Melee_result::miss)
    {
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources);
    }
    cooldown_manager_.add(spellbook::overpower);
    flurry_manager_.update(hit_outcome.hit_table_result.melee_result);
    damage_sources.add_damage(Damage_source::overpower, hit_outcome.damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::whirlwind(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                                 Damage_sources& damage_sources)
{
    if (config.dpr_settings.compute_dpr_ww_)
    {
        rage -= 25;
        cooldown_manager_.add(spellbook::whirlwind);
        return;
    }
    simulator_cout("Whirlwind! #targets = boss + ", number_of_extra_targets_, " adds");
    simulator_cout("Whirlwind hits: ", std::min(number_of_extra_targets_ + 1, 4), " targets");
    double damage = main_hand_weapon.normalized_swing(special_stats.attack_power);
    std::vector<Hit_outcome> hit_outcomes{};
    hit_outcomes.reserve(4);
    for (int i = 0; i < std::min(number_of_extra_targets_ + 1, 4); i++)
    {
        Hit_outcome hit_outcome{generate_hit(main_hand_weapon, damage_sources, damage, Hit_color::yellow,
                                             Socket::main_hand, special_stats, i == 0, false, i == 0)};
        hit_outcomes.emplace_back(hit_outcome);
        if (hit_outcome.hit_table_result.melee_result != Melee_result::dodge &&
            hit_outcome.hit_table_result.melee_result != Melee_result::miss)
        {
            hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources);
        }
    }
    rage -= 25;
    cooldown_manager_.add(spellbook::whirlwind);
    Melee_result result_used_for_flurry = Melee_result::hit;
    double total_damage = 0;
    for (const auto& hit_outcome : hit_outcomes)
    {
        total_damage += hit_outcome.damage;
        if (hit_outcome.hit_table_result.melee_result == Melee_result::crit)
        {
            result_used_for_flurry = Melee_result::crit;
        }
    }
    flurry_manager_.update(result_used_for_flurry);
    damage_sources.add_damage(Damage_source::whirlwind, total_damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::execute(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                               Damage_sources& damage_sources)
{
    if (config.dpr_settings.compute_dpr_ex_)
    {
        get_uniform_random(100) < hit_tables.yellow_[1] ? rage *= 0.8 : rage -= execute_rage_cost_;
        cooldown_manager_.add(spellbook::execute);
        rage_spent_on_execute_ += rage;
        rage = 0;
        return;
    }
    simulator_cout("Execute!");
    double damage = 600 + (rage - execute_rage_cost_) * 15;
    auto hit_outcome = generate_hit(main_hand_weapon, damage_sources, damage, Hit_color::yellow, Socket::main_hand,
                                    special_stats, true, false, true, true);
    if (hit_outcome.hit_table_result.melee_result == Melee_result::dodge ||
        hit_outcome.hit_table_result.melee_result == Melee_result::miss)
    {
        rage *= 0.8;
    }
    else
    {
        rage -= execute_rage_cost_;
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources);
    }
    rage_spent_on_execute_ += rage;
    rage = 0;
    cooldown_manager_.add(spellbook::execute);
    flurry_manager_.update(hit_outcome.hit_table_result.melee_result);
    damage_sources.add_damage(Damage_source::execute, hit_outcome.damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::hamstring(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                                 Damage_sources& damage_sources)
{
    if (config.dpr_settings.compute_dpr_ha_)
    {
        get_uniform_random(100) < hit_tables.yellow_[1] ? rage -= 2 : rage -= 10;
        cooldown_manager_.add(spellbook::hamstring);
        return;
    }
    simulator_cout("Hamstring!");
    double damage = 45;
    auto hit_outcome =
        generate_hit(main_hand_weapon, damage_sources, damage, Hit_color::yellow, Socket::main_hand, special_stats);
    cooldown_manager_.add(spellbook::hamstring);
    if (hit_outcome.hit_table_result.melee_result == Melee_result::dodge ||
        hit_outcome.hit_table_result.melee_result == Melee_result::miss)
    {
        rage -= 2;
    }
    else
    {
        rage -= 10;
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources);
    }
    flurry_manager_.update(hit_outcome.hit_table_result.melee_result);
    damage_sources.add_damage(Damage_source::hamstring, hit_outcome.damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::hit_effects(Weapon_sim& weapon, Weapon_sim& main_hand_weapon, Special_stats& special_stats,
                                   double& rage, Damage_sources& damage_sources, bool is_extra_attack)
{
    for (const auto& hit_effect : weapon.hit_effects)
    {
        double r = get_uniform_random(1);
        if (r < hit_effect.probability)
        {
            proc_data_[hit_effect.name]++;
            switch (hit_effect.type)
            {
            case Hit_effect::Type::extra_hit:
                if (!is_extra_attack)
                {
                    simulator_cout("PROC: extra hit from: ", hit_effect.name);
                    swing_weapon(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources,
                                 hit_effect.attack_power_boost, true);
                }
                else
                {
                    // Decrement the proc statistics for extra hit if it got triggered by an extra hit
                    proc_data_[hit_effect.name]--;
                }
                break;
            case Hit_effect::Type::damage_magic: {
                // * 0.83 Assumes a static 17% chance to resist.
                // (100 + special_stats.spell_crit / 2) / 100 is the average damage gained from a x1.5 spell crit
                double effect_damage = hit_effect.damage * 0.83 * (100 + special_stats.spell_crit / 2) / 100 *
                                       (1 + special_stats.damage_mod_spell);
                damage_sources.add_damage(Damage_source::item_hit_effects, effect_damage, time_keeper_.time);
                simulator_cout("PROC: ", hit_effect.name, " does ", effect_damage, " magic damage.");
                break;
            }
            case Hit_effect::Type::damage_physical: {
                auto hit = generate_hit(main_hand_weapon, damage_sources, hit_effect.damage, Hit_color::yellow,
                                        Socket::main_hand, special_stats);
                damage_sources.add_damage(Damage_source::item_hit_effects, hit.damage, time_keeper_.time);
                if (config.display_combat_debug)
                {
                    simulator_cout("PROC: ", hit_effect.name, hit_result_to_string(hit.hit_table_result.melee_result),
                                   " does ", int(hit.damage), " physical damage");
                }
                if (hit.hit_table_result.melee_result != Melee_result::miss &&
                    hit.hit_table_result.melee_result != Melee_result::dodge)
                {
                    hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources,
                                is_extra_attack);
                }
            }
            break;
            case Hit_effect::Type::stat_boost:
                simulator_cout("PROC: ", hit_effect.name, " stats increased for ", hit_effect.duration.seconds(), "s");
                buff_manager_.add(weapon.socket_name + "_" + hit_effect.name,
                                  hit_effect.get_special_stat_equivalent(special_stats), hit_effect.duration);
                break;
            case Hit_effect::Type::reduce_armor: {
                if (current_armor_red_stacks_ < hit_effect.max_stacks)
                {
                    recompute_mitigation_ = true;
                    current_armor_red_stacks_++;
                    armor_penetration_ = current_armor_red_stacks_ * hit_effect.armor_reduction;
                    simulator_cout("PROC: ", hit_effect.name, ", current stacks: ", current_armor_red_stacks_);
                }
                else
                {
                    simulator_cout("PROC: ", hit_effect.name,
                                   ". At max stacks. Current stacks: ", current_armor_red_stacks_);
                }
            }
            break;
            }
        }
    }
}

void Combat_simulator::swing_weapon(Weapon_sim& weapon, Weapon_sim& main_hand_weapon, Special_stats& special_stats,
                                    double& rage, Damage_sources& damage_sources, double attack_power_bonus,
                                    bool is_extra_attack)
{
    std::vector<Hit_outcome> hit_outcomes{};
    hit_outcomes.reserve(2);
    double swing_damage = weapon.swing(special_stats.attack_power + attack_power_bonus);
    if (weapon.socket == Socket::off_hand)
    {
        swing_damage *= dual_wield_damage_factor_;
    }
    weapon.internal_swing_timer = weapon.swing_speed / (1 + special_stats.haste);

    // Check if heroic strike should be performed
    if (ability_queue_manager.is_heroic_strike_queued() && weapon.socket == Socket::main_hand &&
        rage >= heroic_strike_rage_cost)
    {
        simulator_cout("Performing Heroic Strike");
        swing_damage += config.combat.heroic_strike_damage;
        hit_outcomes.emplace_back(generate_hit(main_hand_weapon, damage_sources, swing_damage, Hit_color::yellow,
                                               weapon.socket, special_stats));
        ability_queue_manager.reset();
        if (config.ability_queue_ && rage > config.ability_queue_rage_thresh_)
        {
            simulator_cout("Re-queue Heroic Strike!");
            ability_queue_manager.queue_heroic_strike();
        }
        if (hit_outcomes[0].hit_table_result.melee_result == Melee_result::dodge ||
            hit_outcomes[0].hit_table_result.melee_result == Melee_result::miss)
        {
            rage -= 0.2 * heroic_strike_rage_cost; // Refund rage for missed/dodged heroic strikes.
        }
        else
        {
            rage -= heroic_strike_rage_cost;
        }
        damage_sources.add_damage(Damage_source::heroic_strike, hit_outcomes[0].damage, time_keeper_.time);
        simulator_cout("Current rage: ", int(rage));
    }
    else if (ability_queue_manager.is_cleave_queued() && weapon.socket == Socket::main_hand && rage >= 20)
    {
        simulator_cout("Performing cleave! #targets = boss + ", number_of_extra_targets_, " adds");
        simulator_cout("Cleave hits: ", std::min(number_of_extra_targets_ + 1, 2), " targets");
        swing_damage += cleave_bonus_damage_;

        for (int i = 0; i < std::min(number_of_extra_targets_ + 1, 2); i++)
        {
            hit_outcomes.emplace_back(generate_hit(main_hand_weapon, damage_sources, swing_damage, Hit_color::yellow,
                                                   weapon.socket, special_stats, i == 0));
        }
        ability_queue_manager.reset();
        if (config.ability_queue_ && rage > config.ability_queue_rage_thresh_)
        {
            simulator_cout("Re-queue Cleave!");
            ability_queue_manager.queue_cleave();
        }
        rage -= 20;
        double total_damage = 0;
        for (const auto& hit_outcome : hit_outcomes)
        {
            total_damage += hit_outcome.damage;
        }
        damage_sources.add_damage(Damage_source::cleave, total_damage, time_keeper_.time);
        simulator_cout("Current rage: ", int(rage));
    }
    else
    {
        if (weapon.socket == Socket::main_hand && ability_queue_manager.is_ability_queued())
        {
            if (ability_queue_manager.is_heroic_strike_queued())
            {
                // Failed to pay rage for heroic strike
                simulator_cout("Failed to pay rage for heroic strike");
                ability_queue_manager.reset();
            }
            else
            {
                // Failed to pay rage for cleave
                simulator_cout("Failed to pay rage for cleave");
                ability_queue_manager.reset();
            }
        }

        // Otherwise do white hit
        hit_outcomes.emplace_back(generate_hit(main_hand_weapon, damage_sources, swing_damage, Hit_color::white,
                                               weapon.socket, special_stats));
        if (dpr_heroic_strike_queued_)
        {
            rage -= heroic_strike_rage_cost;
            dpr_heroic_strike_queued_ = false;
        }
        else if (dpr_cleave_queued_)
        {
            rage -= 20;
            dpr_cleave_queued_ = false;
        }
        else
        {
            rage += rage_generation(hit_outcomes[0].damage);
        }
        if (hit_outcomes[0].hit_table_result.melee_result == Melee_result::dodge)
        {
            simulator_cout("Rage gained since the enemy dodged.");
            rage += 0.75 *
                    rage_generation(swing_damage * armor_reduction_factor_ * (1 + special_stats.damage_mod_physical));
        }
        if (rage > 100.0)
        {
            rage_lost_capped_ += rage - 100.0;
            rage = 100.0;
        }
        simulator_cout("Current rage: ", int(rage));
        if (weapon.socket == Socket::main_hand)
        {
            damage_sources.add_damage(Damage_source::white_mh, hit_outcomes[0].damage, time_keeper_.time);
        }
        else
        {
            damage_sources.add_damage(Damage_source::white_oh, hit_outcomes[0].damage, time_keeper_.time);
        }
    }

    Melee_result result_used_for_flurry = Melee_result::hit;
    for (const auto& hit_outcome : hit_outcomes)
    {
        if (hit_outcome.hit_table_result.melee_result == Melee_result::crit)
        {
            result_used_for_flurry = Melee_result::crit;
            break;
        }
    }
    flurry_manager_.update(result_used_for_flurry);

    for (const auto& hit_outcome : hit_outcomes)
    {
        if (hit_outcome.hit_table_result.melee_result != Melee_result::miss &&
            hit_outcome.hit_table_result.melee_result != Melee_result::dodge)
        {
            hit_effects(weapon, main_hand_weapon, special_stats, rage, damage_sources, is_extra_attack);
        }
    }

    // Unbridled wrath
    if (get_uniform_random(1) < p_unbridled_wrath_)
    {
        rage += 1;
        if (rage > 100.0)
        {
            rage_lost_capped_ += rage - 100.0;
            rage = 100.0;
        }
        simulator_cout("Unbridled wrath. Current rage: ", int(rage));
    }
}

void Combat_simulator::simulate(const Character& character, size_t n_simulations, double init_mean,
                                double init_variance, size_t init_simulations)
{
    dps_distribution_.mean_ = init_mean;
    dps_distribution_.variance_ = init_variance;
    dps_distribution_.n_samples_ = init_simulations;
    config.n_batches = n_simulations;
    simulate(character, init_simulations, false, false);
}

void Combat_simulator::simulate(const Character& character, int init_iteration, bool log_data, bool reset_dps)
{
    int n_damage_batches = config.n_batches;
    if (config.display_combat_debug)
    {
        debug_topic_ = "";
        n_damage_batches = 1;
    }
    if (log_data)
    {
        reset_time_lapse();
        init_histogram();
    }
    buff_manager_.aura_uptime.clear();
    damage_distribution_ = Damage_sources{};
    if (reset_dps)
    {
        dps_distribution_.reset();
    }
    flurry_uptime_mh_ = 0;
    flurry_uptime_oh_ = 0;
    rage_lost_stance_swap_ = 0;
    rage_lost_capped_ = 0;
    heroic_strike_uptime_ = 0;
    const auto starting_special_stats = character.total_special_stats;
    std::vector<Weapon_sim> weapons;
    for (const auto& wep : character.weapons)
    {
        weapons.emplace_back(wep.swing_speed, wep.min_damage, wep.max_damage, wep.socket, wep.type, wep.weapon_socket,
                             wep.hit_effects);
        weapons.back().compute_weapon_damage(wep.buff.bonus_damage + starting_special_stats.bonus_damage);
        hit_tables.compute_hit_table(get_weapon_skill(starting_special_stats, wep.type, wep.weapon_socket),
                                     starting_special_stats, wep.socket, wep.weapon_socket);
    }

    // TODO can move this to armory::compute_total_stats method
    if (character.is_dual_wield())
    {
        for (size_t i = 0; i < 2; i++)
        {
            size_t j = (i == 0) ? 1 : 0;
            for (const auto& hit_effect : character.weapons[i].hit_effects)
            {
                if (hit_effect.affects_both_weapons)
                {
                    auto new_hit_effect = hit_effect;
                    if (hit_effect.ppm != 0)
                    {
                        new_hit_effect.probability =
                            hit_effect.ppm / (Sim_time::from_seconds(60) / weapons[j].swing_speed);
                    }
                    else
                    {
                        std::cout << "missing PPM for hit effect that affects both weapons!\n";
                    }
                    weapons[j].hit_effects.emplace_back(new_hit_effect);
                }
            }
        }
    }

    const bool is_dual_wield = character.is_dual_wield();
    const auto hit_effects_mh_copy = weapons[0].hit_effects;
    auto hit_effects_oh_copy = is_dual_wield ? weapons[1].hit_effects : std::vector<Hit_effect>{};

    Sim_time sim_time = config.sim_time;
    const Sim_time averaging_interval = Sim_time::from_seconds(2); // Duration of the uniform interval that is evaluated
    sim_time -= averaging_interval;

    // Configure use effects
    std::vector<Use_effect> use_effects_all = use_effects_all_;
    for (const auto& use_effect : character.use_effects)
    {
        use_effects_all.push_back(use_effect);
    }

    if (config.enable_blood_fury)
    {
        // TODO need strength multiplier to make this more accurate
        double ap_boost =
            character.total_attributes.convert_to_special_stats(character.total_special_stats).attack_power * 0.25;
        use_effects_all.emplace_back(Use_effect{"Blood_fury",
                                                Use_effect::Effect_socket::unique,
                                                {},
                                                {0, 0, ap_boost},
                                                0,
                                                Sim_time::from_seconds(15),
                                                Sim_time::from_seconds(120),
                                                true});
    }

    double ap_equiv{};
    if (is_dual_wield)
    {
        ap_equiv = get_character_ap_equivalent(starting_special_stats, character.weapons[0], character.weapons[1],
                                               sim_time, {});
    }
    else
    {
        ap_equiv = get_character_ap_equivalent(starting_special_stats, character.weapons[0], sim_time, {});
    }

    auto use_effect_order = Use_effects::compute_use_effect_order(use_effects_all, starting_special_stats,
                                                                  sim_time + averaging_interval / 2, ap_equiv, 0,
                                                                  config.combat.initial_rage);

    for (int iter = init_iteration; iter < n_damage_batches + init_iteration; iter++)
    {
        cooldown_manager_.reset();
        time_keeper_.reset(); // Class variable that keeps track of the time spent, cooldowns, iteration number
        ability_queue_manager.reset();
        slam_manager.reset();
        auto special_stats = starting_special_stats;
        Damage_sources damage_sources{};
        double rage = config.combat.initial_rage;
        flurry_manager_ = Flurry_manager{special_stats, flurry_haste_factor_, 0, config.talents.flurry > 0};

        // Reset hit effects
        weapons[0].hit_effects = hit_effects_mh_copy;
        if (is_dual_wield)
        {
            weapons[1].hit_effects = hit_effects_oh_copy;
            buff_manager_.initialize(special_stats, use_effect_order, weapons[0].hit_effects, weapons[1].hit_effects,
                                     tactical_mastery_rage_, config.performance_mode);
        }
        else
        {
            buff_manager_.initialize(special_stats, use_effect_order, weapons[0].hit_effects, hit_effects_oh_copy,
                                     tactical_mastery_rage_, config.performance_mode);
        }

        recompute_mitigation_ = true;
        current_armor_red_stacks_ = 0;
        armor_penetration_ = 0;
        rage_spent_on_execute_ = 0;

        bool apply_delayed_armor_reduction = false;
        bool execute_phase = false;

        int mh_hits = 0;
        int mh_hits_w_flurry = 0;
        int oh_hits = 0;
        int oh_hits_w_flurry = 0;
        int oh_hits_w_heroic = 0;

        for (auto& wep : weapons)
        {
            wep.internal_swing_timer = Sim_time::zero();
        }

        // To avoid local max/min results from running a specific run time
        sim_time += averaging_interval / n_damage_batches;
        Sim_time time_execute_phase{sim_time * (1.0 - config.execute_phase_percentage_ / 100.0)};

        // Combat configuration
        if (!config.multi_target_mode_)
        {
            number_of_extra_targets_ = 0;
        }
        else
        {
            number_of_extra_targets_ = config.number_of_extra_targets;
        }

        // Check if the simulator should use any use effects before the fight
        if (!use_effect_order.empty() && use_effect_order.back().first.milliseconds() < 0)
        {
            time_keeper_.time = use_effect_order.back().first;
            while (time_keeper_.time.milliseconds() < 0)
            {
                Sim_time buff_dt{buff_manager_.get_dt(time_keeper_.time)};
                Sim_time use_effect_dt{buff_manager_.use_effect_order.back().first - time_keeper_.time};
                if (use_effect_dt.milliseconds() <= 0)
                {
                    use_effect_dt = Sim_time::from_milliseconds(1);
                    if (buff_manager_.use_effect_order.back().second.triggers_gcd)
                    {
                        use_effect_dt = (cooldown_manager_.has_global_cooldown()) ?
                                            cooldown_manager_.get_global_cooldown() :
                                            use_effect_dt;
                    }
                }
                Sim_time dt{Time_keeper::get_dynamic_time_step(Sim_time::from_seconds(100), Sim_time::from_seconds(100),
                                                               buff_dt, Sim_time::zero() - time_keeper_.time,
                                                               use_effect_dt, Sim_time::from_seconds(100))};
                time_keeper_.increment(dt);
                std::vector<std::string> debug_msg{};
                buff_manager_.increment(dt, time_keeper_.time, rage, rage_lost_stance_swap_,
                                        cooldown_manager_.get_global_cooldown(), config.display_combat_debug,
                                        debug_msg);
                for (const auto& msg : debug_msg)
                {
                    simulator_cout(msg);
                }
            }
        }

        for (const auto& over_time_effect : over_time_effects_)
        {
            buff_manager_.add_over_time_effect(over_time_effect, Sim_time::zero());
        }

        // First global sunder
        bool first_global_sunder = config.first_global_sunder_;

        if (config.combat.first_hit_heroic_strike && rage > heroic_strike_rage_cost)
        {
            ability_queue_manager.queue_heroic_strike();
        }

        while (time_keeper_.time < sim_time)
        {
            Sim_time mh_dt{weapons[0].internal_swing_timer};
            Sim_time oh_dt{is_dual_wield ? weapons[1].internal_swing_timer : Sim_time::from_seconds(1000)};
            Sim_time buff_dt{buff_manager_.get_dt(time_keeper_.time)};
            Sim_time slam_dt{slam_manager.time_left(time_keeper_.time)};
            Sim_time spell_dt{cooldown_manager_.time_until_next_spell_is_ready()};
            Sim_time dt{Time_keeper::get_dynamic_time_step(mh_dt, oh_dt, buff_dt, sim_time - time_keeper_.time,
                                                           spell_dt, slam_dt)};
            time_keeper_.increment(dt);
            cooldown_manager_.increment(dt);
            std::vector<std::string> debug_msg{};
            buff_manager_.increment(dt, time_keeper_.time, rage, rage_lost_stance_swap_,
                                    cooldown_manager_.get_global_cooldown(), config.display_combat_debug, debug_msg);
            for (const auto& msg : debug_msg)
            {
                simulator_cout(msg);
            }

            if ((sim_time - time_keeper_.time).milliseconds() < 0)
            {
                break;
            }

            if (buff_manager_.need_to_recompute_hittables)
            {
                for (const auto& weapon : weapons)
                {
                    hit_tables.compute_hit_table(
                        get_weapon_skill(special_stats, weapon.weapon_type, weapon.weapon_socket), special_stats,
                        weapon.socket, weapon.weapon_socket);
                }
                buff_manager_.need_to_recompute_hittables = false;
            }

            if (buff_manager_.reset_armor_reduction)
            {
                recompute_mitigation_ = true;
                current_armor_red_stacks_ = 0;
                armor_penetration_ = 0;
                buff_manager_.reset_armor_reduction = false;
            }

            if (!apply_delayed_armor_reduction && time_keeper_.time > Sim_time::from_milliseconds(6000) &&
                config.exposed_armor)
            {
                apply_delayed_armor_reduction = true;
                recompute_mitigation_ = true;
                simulator_cout("Applying improved exposed armor!");
            }

            if (recompute_mitigation_)
            {
                int target_armor =
                    config.main_target_initial_armor_ - armor_reduction_from_spells_ - armor_penetration_;
                if (apply_delayed_armor_reduction)
                {
                    target_armor -= armor_reduction_delayed_;
                }
                target_armor = std::max(target_armor, 0);
                armor_reduction_factor_ = 1 - armor_mitigation(target_armor, config.main_target_level);
                simulator_cout("Target armor: ", target_armor, ". Mitigation factor: ", 1 - armor_reduction_factor_,
                               "%.");
                if (config.multi_target_mode_)
                {
                    int extra_target_armor = config.extra_target_initial_armor_ - armor_penetration_;
                    extra_target_armor = std::max(extra_target_armor, 0);
                    armor_reduction_factor_add = 1 - armor_mitigation(extra_target_armor, config.extra_target_level);

                    simulator_cout("Extra targets armor: ", extra_target_armor,
                                   ". Mitigation factor: ", 1 - armor_reduction_factor_add, "%.");
                }
                recompute_mitigation_ = false;
            }

            if (config.multi_target_mode_ && number_of_extra_targets_ > 0 &&
                time_keeper_.time / sim_time > config.extra_target_percent_uptime)
            {
                simulator_cout("Extra targets die.");
                number_of_extra_targets_ = 0;
            }

            bool mh_swing = weapons[0].time_for_swing(dt);
            bool oh_swing = is_dual_wield ? weapons[1].time_for_swing(dt) : false;

            if (mh_swing)
            {
                mh_hits++;
                if (flurry_manager_.has_flurry_charges())
                {
                    mh_hits_w_flurry++;
                }
                swing_weapon(weapons[0], weapons[0], special_stats, rage, damage_sources);
            }

            if (oh_swing)
            {
                oh_hits++;
                if (flurry_manager_.has_flurry_charges())
                {
                    oh_hits_w_flurry++;
                }
                if (ability_queue_manager.is_heroic_strike_queued() || ability_queue_manager.is_cleave_queued())
                {
                    oh_hits_w_heroic++;
                }
                swing_weapon(weapons[1], weapons[0], special_stats, rage, damage_sources);
            }

            if (!execute_phase)
            {
                if (time_keeper_.time > time_execute_phase)
                {
                    simulator_cout("------------ Execute phase! ------------");
                    execute_phase = true;
                }
            }

            if (use_sweeping_strikes_)
            {
                if (rage > 30.0 && cooldown_manager_.can_cast(Damage_source::sweeping_strikes))
                {
                    simulator_cout("Changed stance: Battle Stance.");
                    simulator_cout("Sweeping strkes!");
                    buff_manager_.add("battle_stance", {-3.0, 0, 0}, Sim_time::from_seconds(1.5));
                    sweeping_strikes_charges_ = 5;
                    cooldown_manager_.add(spellbook::sweeping_strikes);
                }
            }

            if (first_global_sunder)
            {
                if (!cooldown_manager_.has_global_cooldown() && rage > 15)
                {
                    simulator_cout("Sunder Armor!");
                    cooldown_manager_.set_global_cooldown();
                    rage -= 15;
                    first_global_sunder = false;
                }
            }
            else if (execute_phase)
            {
                if (rage > heroic_strike_rage_cost && !ability_queue_manager.is_heroic_strike_queued() &&
                    config.combat.use_hs_in_exec_phase)
                {
                    if (config.combat.use_heroic_strike)
                    {
                        if (config.dpr_settings.compute_dpr_hs_)
                        {
                            if (mh_swing)
                            {
                                rage -= config.combat.heroic_strike_rage_thresh;
                            }
                        }
                        else
                        {
                            ability_queue_manager.queue_heroic_strike();
                            simulator_cout("Heroic strike activated");
                        }
                    }
                }
                if (use_bloodthirst_ && config.combat.use_bt_in_exec_phase)
                {
                    if (cooldown_manager_.can_cast(Damage_source::bloodthirst) && rage > 30)
                    {
                        bloodthirst(weapons[0], special_stats, rage, damage_sources);
                        //                        cast_spell(spellbook::bloodthirst, weapons[0], special_stats);
                    }
                }
                if (use_mortal_strike_ && config.combat.use_ms_in_exec_phase)
                {
                    if (cooldown_manager_.can_cast(Damage_source::mortal_strike) && rage > 30)
                    {
                        mortal_strike(weapons[0], special_stats, rage, damage_sources);
                        //                        cast_spell(spellbook::mortal_strike, weapons[0], special_stats);
                    }
                }
                if (!cooldown_manager_.has_global_cooldown() && rage > execute_rage_cost_)
                {
                    execute(weapons[0], special_stats, rage, damage_sources);
                    //                    cast_spell(spellbook::execute, weapons[0], special_stats);
                }
            }
            else
            {
                if (weapons[0].weapon_socket == Weapon_socket::two_hand && config.combat.use_slam)
                {
                    if (not slam_manager.is_slam_queued() && not cooldown_manager_.has_global_cooldown())
                    {
                        if (start_cast_slam(mh_swing, rage, weapons[0].internal_swing_timer))
                        {
                            continue;
                        }
                    }
                    else if (slam_manager.time_left(time_keeper_.time).milliseconds() <= 0.0)
                    {
                        slam(weapons[0], special_stats, rage, damage_sources);
                        //                        cast_spell(spellbook::slam, weapons[0], special_stats);
                        slam_manager.un_queue_slam();
                        weapons[0].internal_swing_timer = weapons[0].swing_speed / (1 + special_stats.haste);
                        if (not cooldown_manager_.has_global_cooldown())
                        {
                            if (start_cast_slam(mh_swing, rage, weapons[0].internal_swing_timer))
                            {
                                continue;
                            }
                        }
                    }
                    else
                    {
                        continue;
                    }
                }

                if (use_bloodthirst_)
                {
                    if (cooldown_manager_.can_cast(Damage_source::bloodthirst) && rage > 30)
                    {
                        bloodthirst(weapons[0], special_stats, rage, damage_sources);
                        //                        cast_spell(spellbook::bloodthirst, weapons[0], special_stats);
                    }
                }

                if (use_mortal_strike_)
                {
                    if (cooldown_manager_.can_cast(Damage_source::mortal_strike) && rage > 30)
                    {
                        mortal_strike(weapons[0], special_stats, rage, damage_sources);
                        //                        cast_spell(spellbook::mortal_strike, weapons[0], special_stats);
                    }
                }

                if (config.combat.use_whirlwind)
                {
                    bool use_ww = true;
                    if (use_bloodthirst_)
                    {
                        use_ww = cooldown_manager_.cooldown_of(Damage_source::bloodthirst) >
                                 config.combat.whirlwind_bt_cooldown_thresh;
                    }
                    if (use_mortal_strike_)
                    {
                        use_ww &= cooldown_manager_.cooldown_of(Damage_source::mortal_strike) >
                                  config.combat.whirlwind_bt_cooldown_thresh;
                    }
                    if (use_ww && cooldown_manager_.can_cast(Damage_source::whirlwind) &&
                        rage > config.combat.whirlwind_rage_thresh && rage > 25)
                    {
                        whirlwind(weapons[0], special_stats, rage, damage_sources);
                        //                        cast_spell(spellbook::whirlwind, weapons[0], special_stats);
                    }
                }

                if (config.combat.use_overpower)
                {
                    bool use_op = true;
                    if (use_bloodthirst_)
                    {
                        use_op &= cooldown_manager_.cooldown_of(Damage_source::bloodthirst) >
                                  config.combat.overpower_bt_cooldown_thresh;
                    }
                    if (use_mortal_strike_)
                    {
                        use_op &= cooldown_manager_.cooldown_of(Damage_source::mortal_strike) >
                                  config.combat.overpower_bt_cooldown_thresh;
                    }
                    if (config.combat.use_whirlwind)
                    {
                        use_op &= cooldown_manager_.cooldown_of(Damage_source::whirlwind) >
                                  config.combat.overpower_ww_cooldown_thresh;
                    }
                    if (use_op && cooldown_manager_.can_cast(Damage_source::overpower) &&
                        rage < config.combat.overpower_rage_thresh && rage > 5 && buff_manager_.can_do_overpower())
                    {
                        overpower(weapons[0], special_stats, rage, damage_sources);
                        //                        cast_spell(spellbook::overpower, weapons[0], special_stats);
                    }
                }

                if (config.combat.use_hamstring)
                {
                    bool use_ham = true;
                    if (config.combat.dont_use_hm_when_ss && sweeping_strikes_charges_ > 0)
                    {
                        use_ham = false;
                    }
                    if (use_bloodthirst_)
                    {
                        use_ham &= cooldown_manager_.cooldown_of(Damage_source::bloodthirst) >
                                   config.combat.hamstring_cd_thresh;
                    }
                    if (use_mortal_strike_)
                    {
                        use_ham &= cooldown_manager_.cooldown_of(Damage_source::mortal_strike) >
                                   config.combat.hamstring_cd_thresh;
                    }
                    if (config.combat.use_whirlwind)
                    {
                        use_ham &=
                            cooldown_manager_.cooldown_of(Damage_source::whirlwind) > config.combat.hamstring_cd_thresh;
                    }
                    if (rage > config.combat.hamstring_thresh_dd && not cooldown_manager_.has_global_cooldown() &&
                        use_ham)
                    {
                        hamstring(weapons[0], special_stats, rage, damage_sources);
                        //                        cast_spell(spellbook::hamstring, weapons[0], special_stats);
                    }
                }

                // Heroic strike or Cleave
                // TODO move to the top of sim loop, enables 'continue' after casting spells for performance
                if (number_of_extra_targets_ > 0 && config.combat.cleave_if_adds)
                {
                    if (rage > config.combat.cleave_rage_thresh && !ability_queue_manager.is_cleave_queued())
                    {
                        if (config.dpr_settings.compute_dpr_cl_)
                        {
                            dpr_cleave_queued_ = true;
                        }
                        else
                        {
                            ability_queue_manager.queue_cleave();
                            simulator_cout("Cleave activated");
                        }
                    }
                }
                else
                {
                    if (rage > config.combat.heroic_strike_rage_thresh &&
                        !ability_queue_manager.is_heroic_strike_queued() && rage > heroic_strike_rage_cost)
                    {
                        if (config.combat.use_heroic_strike)
                        {
                            if (config.dpr_settings.compute_dpr_hs_)
                            {
                                dpr_heroic_strike_queued_ = true;
                            }
                            else
                            {
                                ability_queue_manager.queue_heroic_strike();
                                simulator_cout("Heroic strike activated");
                            }
                        }
                    }
                }
            }
        }
        if (deep_wounds_)
        {
            double dw_average_damage =
                buff_manager_.deep_wounds_damage / double(buff_manager_.deep_wounds_timestamps.size());
            for (Sim_time deep_wounds_timestamp : buff_manager_.deep_wounds_timestamps)
            {
                damage_sources.add_damage(Damage_source::deep_wounds, dw_average_damage, deep_wounds_timestamp);
            }
        }
        double new_sample = damage_sources.sum_damage_sources() / sim_time.seconds();
        dps_distribution_.add_sample(new_sample);
        damage_distribution_ = damage_distribution_ + damage_sources;
        flurry_uptime_mh_ = Statistics::update_mean(flurry_uptime_mh_, iter + 1, double(mh_hits_w_flurry) / mh_hits);
        if (weapons.size() > 1)
        {
            flurry_uptime_oh_ =
                Statistics::update_mean(flurry_uptime_oh_, iter + 1, double(oh_hits_w_flurry) / oh_hits);
            heroic_strike_uptime_ =
                Statistics::update_mean(heroic_strike_uptime_, iter + 1, double(oh_hits_w_heroic) / oh_hits);
        }
        avg_rage_spent_executing_ =
            Statistics::update_mean(avg_rage_spent_executing_, iter + 1, rage_spent_on_execute_);
        if (log_data)
        {
            add_damage_source_to_time_lapse(damage_sources.damage_instances);
            hist_y[new_sample / 20]++;
        }
    }
    if (log_data)
    {
        normalize_timelapse();
        prune_histogram();
    }
}

std::vector<std::pair<Sim_time, Use_effect>> Combat_simulator::get_use_effect_order(const Character& character)
{
    // Copy paste from the simulate function
    std::vector<Use_effect> use_effects_all = use_effects_all_;
    for (const auto& use_effect : character.use_effects)
    {
        use_effects_all.push_back(use_effect);
    }
    if (config.enable_blood_fury)
    {
        double ap_boost =
            character.total_attributes.convert_to_special_stats(character.total_special_stats).attack_power * 0.25;
        use_effects_all.emplace_back(Use_effect{"Blood_fury",
                                                Use_effect::Effect_socket::unique,
                                                {},
                                                {0, 0, ap_boost},
                                                0,
                                                Sim_time::from_seconds(15),
                                                Sim_time::from_seconds(120),
                                                true});
    }
    double ap_equiv{};
    if (character.is_dual_wield())
    {
        ap_equiv = get_character_ap_equivalent(character.total_special_stats, character.weapons[0],
                                               character.weapons[1], config.sim_time, {});
    }
    else
    {
        ap_equiv =
            get_character_ap_equivalent(character.total_special_stats, character.weapons[0], config.sim_time, {});
    }
    return Use_effects::compute_use_effect_order(use_effects_all, character.total_special_stats, config.sim_time,
                                                 ap_equiv, 0, config.combat.initial_rage);
}

void Combat_simulator::init_histogram()
{
    double res = 20;
    for (int i = 0; i < 1000; i++)
    {
        hist_x.push_back(i * res);
        hist_y.push_back(0);
    }
}

void Combat_simulator::normalize_timelapse()
{
    for (auto& damage_time_lapse_i : damage_time_lapse)
    {
        for (auto& singe_damage_instance : damage_time_lapse_i)
        {
            singe_damage_instance /= config.n_batches;
        }
    }
}

void Combat_simulator::prune_histogram()
{
    int start_idx{};
    int end_idx{};
    for (size_t i = 0; i < hist_x.size(); i++)
    {
        if (hist_y[i] != 0 && !start_idx)
        {
            start_idx = i;
            break;
        }
    }
    for (size_t i = hist_x.size() - 1; i > 0; i--)
    {
        if (hist_y[i] != 0 && !end_idx)
        {
            end_idx = i;
            break;
        }
    }
    {
        auto first = hist_x.begin() + start_idx;
        auto last = hist_x.begin() + end_idx + 1;
        hist_x = std::vector<int>(first, last);
    }
    {
        auto first = hist_y.begin() + start_idx;
        auto last = hist_y.begin() + end_idx + 1;
        hist_y = std::vector<int>(first, last);
    }
}

std::vector<std::string> Combat_simulator::get_aura_uptimes() const
{
    std::vector<std::string> aura_uptimes;
    Sim_time total_sim_time = config.n_batches * (config.sim_time - Sim_time::from_seconds(1));
    for (const auto& aura : buff_manager_.aura_uptime)
    {
        double uptime = aura.second / total_sim_time;
        aura_uptimes.emplace_back(aura.first + " " + std::to_string(100 * uptime));
    }
    if (flurry_uptime_mh_ != 0.0)
    {
        aura_uptimes.emplace_back("Flurry_main_hand " + std::to_string(100 * flurry_uptime_mh_));
    }
    if (flurry_uptime_oh_ != 0.0)
    {
        aura_uptimes.emplace_back("Flurry_off_hand " + std::to_string(100 * flurry_uptime_oh_));
    }
    if (heroic_strike_uptime_ != 0.0)
    {
        aura_uptimes.emplace_back("'Heroic_strike_bug' " + std::to_string(100 * heroic_strike_uptime_));
    }
    return aura_uptimes;
}

std::vector<std::string> Combat_simulator::get_proc_statistics() const
{
    std::vector<std::string> proc_counter;
    for (const auto& proc : proc_data_)
    {
        double counter = static_cast<double>(proc.second) / config.n_batches;
        proc_counter.emplace_back(proc.first + " " + std::to_string(counter));
    }
    return proc_counter;
}

void Combat_simulator::reset_time_lapse()
{
    Sim_time resolution{Sim_time::from_seconds(0.5)};
    std::vector<double> history;
    history.reserve(config.sim_time / resolution);
    for (Sim_time t{Sim_time::zero()}; t < config.sim_time; t += resolution)
    {
        history.push_back(0);
    }
    history.push_back(0); // This extra bin might be used for last second dot effects
    if (!(damage_time_lapse.empty()))
    {
        damage_time_lapse.clear();
    }
    for (size_t i = 0; i < source_map.size(); i++)
    {
        damage_time_lapse.push_back(history);
    }
}

void Combat_simulator::add_damage_source_to_time_lapse(std::vector<Damage_instance>& damage_instances)
{
    Sim_time resolution{Sim_time::from_seconds(0.5)};
    for (const auto& damage_instance : damage_instances)
    {
        size_t first_idx = source_map.at(damage_instance.damage_source);
        size_t second_idx = damage_instance.time_stamp / resolution; // automatically floored
        damage_time_lapse[first_idx][second_idx] += damage_instance.damage;
    }
}

std::vector<std::vector<double>> Combat_simulator::get_damage_time_lapse() const
{
    return damage_time_lapse;
}

std::string Combat_simulator::get_debug_topic() const
{
    return debug_topic_;
}
