template <typename T>
void Combat_simulator_config::get_combat_simulator_config(const T& input)
{
    if (find_string(input.options, "exposed_armor"))
    {
        exposed_armor = true;
    }
    if (find_string(input.options, "curse_of_recklessness"))
    {
        curse_of_recklessness_active = true;
    }
    if (find_string(input.options, "faerie_fire"))
    {
        faerie_fire_feral_active = true;
    }
    if (find_string(input.options, "death_wish"))
    {
        talents.death_wish = true;
    }
    if (find_string(input.options, "recklessness"))
    {
        enable_recklessness = true;
    }
    if (find_string(input.options, "enable_blood_fury"))
    {
        enable_blood_fury = true;
    }
    if (find_string(input.options, "use_bt_in_exec_phase"))
    {
        combat.use_bt_in_exec_phase = true;
    }
    if (find_string(input.options, "use_hs_in_exec_phase"))
    {
        combat.use_hs_in_exec_phase = true;
    }
    if (find_string(input.options, "cleave_if_adds"))
    {
        combat.cleave_if_adds = true;
    }
    if (find_string(input.options, "use_hamstring"))
    {
        combat.use_hamstring = true;
    }
    if (find_string(input.options, "use_bloodthirst"))
    {
        combat.use_bloodthirst = true;
    }
    if (find_string(input.options, "use_whirlwind"))
    {
        combat.use_whirlwind = true;
    }
    if (find_string(input.options, "use_overpower"))
    {
        combat.use_overpower = true;
    }
    if (find_string(input.options, "use_heroic_strike"))
    {
        combat.use_heroic_strike = true;
    }
    if (find_string(input.options, "deep_wounds"))
    {
        combat.deep_wounds = true;
    }
    if (find_string(input.options, "heroic_strike_aq"))
    {
        combat.heroic_strike_damage = 157;
    }
    else
    {
        combat.heroic_strike_damage = 138;
    }
    if (find_string(input.options, "first_hit_heroic_strike"))
    {
        combat.first_hit_heroic_strike = true;
    }

    if (find_string(input.options, "multi_target_mode"))
    {
        multi_target_mode_ = true;
    }
    if (find_string(input.options, "essence_of_the_red"))
    {
        essence_of_the_red_ = true;
    }
    if (find_string(input.options, "can_trigger_enrage"))
    {
        can_trigger_enrage_ = true;
    }
    if (find_string(input.options, "first_global_sunder"))
    {
        first_global_sunder_ = true;
    }
    if (find_string(input.options, "periodic_damage"))
    {
        take_periodic_damage_ = true;
    }
    if (find_string(input.options, "ability_queue"))
    {
        ability_queue_ = true;
    }

    talents.improved_heroic_strike = 2;
    talents.overpower = 1;
    talents.unbridled_wrath = 5;
    talents.flurry = 5;
    talents.anger_management = true;
    talents.impale = 2;
    talents.improved_execute = 2;
    talents.dual_wield_specialization = 5;

    combat.heroic_strike_rage_thresh =
        find_value(input.float_options_string, input.float_options_val, "heroic_strike_rage_thresh_dd");
    combat.cleave_rage_thresh =
        find_value(input.float_options_string, input.float_options_val, "cleave_rage_thresh_dd");
    combat.whirlwind_rage_thresh =
        find_value(input.float_options_string, input.float_options_val, "whirlwind_rage_thresh_dd");
    combat.hamstring_cd_thresh =
        find_value(input.float_options_string, input.float_options_val, "hamstring_cd_thresh_dd");
    combat.hamstring_thresh_dd = find_value(input.float_options_string, input.float_options_val, "hamstring_thresh_dd");
    combat.initial_rage = find_value(input.float_options_string, input.float_options_val, "initial_rage_dd");
    combat.whirlwind_bt_cooldown_thresh =
        find_value(input.float_options_string, input.float_options_val, "whirlwind_bt_cooldown_thresh_dd");
    combat.overpower_rage_thresh =
        find_value(input.float_options_string, input.float_options_val, "overpower_rage_thresh_dd");
    combat.overpower_bt_cooldown_thresh =
        find_value(input.float_options_string, input.float_options_val, "overpower_bt_cooldown_thresh_dd");
    combat.overpower_ww_cooldown_thresh =
        find_value(input.float_options_string, input.float_options_val, "overpower_ww_cooldown_thresh_dd");

    sim_time = find_value(input.float_options_string, input.float_options_val, "fight_time_dd");

    main_target_level = find_value(input.float_options_string, input.float_options_val, "opponent_level_dd");
    main_target_initial_armor_ = find_value(input.float_options_string, input.float_options_val, "boss_armor_dd");

    n_sunder_armor_stacks = find_value(input.float_options_string, input.float_options_val, "sunder_armor_dd");

    number_of_extra_targets =
        find_value(input.float_options_string, input.float_options_val, "number_of_extra_targets_dd");
    extra_target_initial_armor_ =
        find_value(input.float_options_string, input.float_options_val, "extra_target_armor_dd");
    extra_target_level = find_value(input.float_options_string, input.float_options_val, "extra_target_level_dd");
    periodic_damage_interval_ =
        find_value(input.float_options_string, input.float_options_val, "periodic_damage_interval_dd");
    periodic_damage_amount_ =
        find_value(input.float_options_string, input.float_options_val, "periodic_damage_amount_dd");
    execute_phase_percentage_ =
        find_value(input.float_options_string, input.float_options_val, "execute_phase_percentage_dd");
    ability_queue_rage_thresh_ =
        find_value(input.float_options_string, input.float_options_val, "re_queue_abilities_dd");

    enable_bloodrage = true;
}