#ifndef WOW_SIMULATOR_SWEEPING_STRIKES_MANAGER_HPP
#define WOW_SIMULATOR_SWEEPING_STRIKES_MANAGER_HPP

class Sweeping_strikes_manager
{
    if (sweeping_strikes_charges_ > 0)
    {
        if (hit_outcome.damage > 0.0 && can_sweep)
        {
            double sweeping_strike_damage{};
            if (boss_target)
            {
                sweeping_strike_damage = hit_outcome.damage / armor_reduction_factor_ * armor_reduction_factor_add;
                damage_sources.add_damage(Damage_source::sweeping_strikes, sweeping_strike_damage,
                                          time_keeper_.time);
            }
            else
            {
                double rand_var = get_uniform_random(1);
                if (number_of_extra_targets_ > 0 && rand_var > 1.0 - 1.0 / number_of_extra_targets_)
                {
                    sweeping_strike_damage =
                        hit_outcome.damage / armor_reduction_factor_add * armor_reduction_factor_;
                }
                else
                {
                    sweeping_strike_damage = hit_outcome.damage;
                }
                damage_sources.add_damage(Damage_source::sweeping_strikes, sweeping_strike_damage,
                                          time_keeper_.time);
            }
            simulator_cout("Sweeping strikes hits a nearby target.");
            cout_damage_parse(Hit_type::yellow, Socket::main_hand,
                              Hit_outcome{sweeping_strike_damage, Hit_result::hit});
            sweeping_strikes_charges_--;
            simulator_cout("Sweeping strikes charges left: ", sweeping_strikes_charges_);
        }
    }
};

#endif // WOW_SIMULATOR_SWEEPING_STRIKES_MANAGER_HPP
