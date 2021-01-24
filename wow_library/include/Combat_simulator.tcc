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
        combat.use_death_wish = true;
    }
    if (find_string(input.options, "recklessness"))
    {
        enable_recklessness = true;
    }
    if (find_string(input.options, "enable_blood_fury"))
    {
        enable_blood_fury = true;
    }
    if (find_string(input.options, "enable_berserking"))
    {
        enable_berserking = true;
    }
    if (find_string(input.options, "use_bt_in_exec_phase"))
    {
        combat.use_bt_in_exec_phase = true;
    }
    if (find_string(input.options, "use_ms_in_exec_phase"))
    {
        combat.use_ms_in_exec_phase = true;
    }
    if (find_string(input.options, "use_hs_in_exec_phase"))
    {
        combat.use_hs_in_exec_phase = true;
    }
    if (find_string(input.options, "cleave_if_adds"))
    {
        combat.cleave_if_adds = true;
    }
    if (find_string(input.options, "use_sweeping_strikes"))
    {
        combat.use_sweeping_strikes = true;
    }
    if (find_string(input.options, "dont_use_hm_when_ss"))
    {
        combat.dont_use_hm_when_ss = true;
    }
    if (find_string(input.options, "use_hamstring"))
    {
        combat.use_hamstring = true;
    }
    if (find_string(input.options, "use_bloodthirst"))
    {
        combat.use_bloodthirst = true;
    }
    if (find_string(input.options, "use_mortal_strike"))
    {
        combat.use_mortal_strike = true;
    }
    if (find_string(input.options, "use_slam"))
    {
        combat.use_slam = true;
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

    enable_bloodrage = true;
    {
        Find_values<double> fv(input.float_options_string, input.float_options_val);

        combat.heroic_strike_rage_thresh = fv.find("heroic_strike_rage_thresh_dd");
        combat.cleave_rage_thresh = fv.find("cleave_rage_thresh_dd");
        combat.whirlwind_rage_thresh = fv.find("whirlwind_rage_thresh_dd");
        combat.hamstring_cd_thresh = fv.find("hamstring_cd_thresh_dd");
        combat.hamstring_thresh_dd = fv.find("hamstring_thresh_dd");
        combat.initial_rage = fv.find("initial_rage_dd");
        combat.whirlwind_bt_cooldown_thresh = fv.find("whirlwind_bt_cooldown_thresh_dd");
        combat.overpower_rage_thresh = fv.find("overpower_rage_thresh_dd");
        combat.overpower_bt_cooldown_thresh = fv.find("overpower_bt_cooldown_thresh_dd");
        combat.overpower_ww_cooldown_thresh = fv.find("overpower_ww_cooldown_thresh_dd");
        combat.slam_cd_thresh = fv.find("slam_cd_thresh_dd");
        combat.slam_spam_max_time = fv.find("slam_spam_max_time_dd");
        combat.slam_spam_rage = fv.find("slam_spam_rage_dd");
        combat.slam_rage_dd = fv.find("slam_rage_dd");

        sim_time = fv.find("fight_time_dd");

        main_target_level = fv.find("opponent_level_dd");
        main_target_initial_armor_ = fv.find("boss_armor_dd");

        n_sunder_armor_stacks = fv.find("sunder_armor_dd");

        number_of_extra_targets = fv.find("number_of_extra_targets_dd");
        extra_target_duration = fv.find("extra_target_duration_dd") / 100;
        extra_target_initial_armor_ = fv.find("extra_target_armor_dd");
        extra_target_level = fv.find("extra_target_level_dd");
        periodic_damage_interval_ = fv.find("periodic_damage_interval_dd");
        periodic_damage_amount_ = fv.find("periodic_damage_amount_dd");
        execute_phase_percentage_ = fv.find("execute_phase_percentage_dd");
        ability_queue_rage_thresh_ = fv.find("re_queue_abilities_dd");
        berserking_haste_ = fv.find("berserking_haste_dd");
    }
    {
        Find_values<int> fv(input.talent_string, input.talent_val);
        talents.improved_heroic_strike = fv.find("improved_heroic_strike_talent");
        talents.overpower = fv.find("improved_overpower_talent");
        talents.unbridled_wrath = fv.find("unbridled_wrath_talent");
        talents.flurry = fv.find("flurry_talent");
        talents.anger_management = fv.find("anger_management_talent");
        talents.impale = fv.find("impale_talent");
        talents.improved_execute = fv.find("improved_execute_talent");
        talents.dual_wield_specialization = fv.find("dual_wield_specialization_talent");
        talents.improved_cleave = fv.find("improved_cleave_talent");
        talents.improved_slam = fv.find("improved_slam_talent");
        talents.death_wish = fv.find("death_wish_talent");
        talents.tactical_mastery = fv.find("tactical_mastery_talent");
        talents.deep_wounds = fv.find("deep_wounds_talent");
        talents.bloodthirst = fv.find("bloodthirst_talent");
        talents.mortal_strike = fv.find("mortal_strike_talent");
        talents.sweeping_strikes = fv.find("sweeping_strikes_talent");
    }
}