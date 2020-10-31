template <typename T>
void Combat_simulator_config::get_combat_simulator_config(const T& input)
{
    sim_time = input.fight_time;
    opponent_level = input.target_level;

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
    if (find_string(input.options, "sulfuron_harbinger"))
    {
        mode.sulfuron_harbinger = true;
    }
    if (find_string(input.options, "golemagg"))
    {
        mode.golemagg = true;
    }
    if (find_string(input.options, "vaelastrasz"))
    {
        mode.vaelastrasz = true;
    }
    if (find_string(input.options, "chromaggus"))
    {
        mode.chromaggus = true;
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
    n_sunder_armor_stacks = input.sunder_armor;
    talents.improved_heroic_strike = 2;
    talents.overpower = 1;
    talents.unbridled_wrath = 5;
    talents.flurry = 5;
    talents.anger_management = true;
    talents.impale = 2;
    talents.improved_execute = 2;
    talents.dual_wield_specialization = 5;

    combat.heroic_strike_rage_thresh = input.heroic_strike_rage_thresh;
    combat.cleave_rage_thresh = input.cleave_rage_thresh;
    combat.whirlwind_rage_thresh = input.whirlwind_rage_thresh;
    combat.hamstring_cd_thresh = input.hamstring_cd_thresh;
    combat.hamstring_thresh_dd = input.hamstring_thresh_dd;
    combat.initial_rage = input.initial_rage;

    combat.whirlwind_bt_cooldown_thresh = input.whirlwind_bt_cooldown_thresh;

    combat.overpower_rage_thresh = input.overpower_rage_thresh;
    combat.overpower_bt_cooldown_thresh = input.overpower_bt_cooldown_thresh;
    combat.overpower_ww_cooldown_thresh = input.overpower_ww_cooldown_thresh;

    enable_bloodrage = true;
    use_seed = true;
}