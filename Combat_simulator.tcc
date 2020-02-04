template<typename Struct_t, typename Field_t>
Combat_simulator::Stat_weight
Combat_simulator::permute_stat(const Character &character, Struct_t struct_t, Field_t field_t,
                               Combat_simulator::Stat stat, double amount,
                               double sim_time,
                               double dt,
                               int opponent_level, int n_batches)
{
    auto char_copy = character;
    char_copy.clear_permutations();
    auto &stat_struct = char_copy.get_field(struct_t);
    auto dmg_init = simulate(char_copy, sim_time, dt, opponent_level, n_batches);
    double mean_init = average(dmg_init);
    double std_init = standard_deviation(dmg_init, mean_init);
    double sample_std_init = sample_deviation(std_init, n_batches);

    stat_struct.*field_t += amount;
    char_copy.compute_all_stats(Character::Talent::fury);
    auto dmg_plus = simulate(char_copy, sim_time, dt, opponent_level, n_batches);
    double mean_plus = average(dmg_plus);
    double std_plus = standard_deviation(dmg_plus, mean_plus);
    double sample_std_plus = sample_deviation(std_plus, n_batches);

    stat_struct.*field_t -= 2 * amount;
    char_copy.compute_all_stats(Character::Talent::fury);
    auto dmg_minus = simulate(char_copy, sim_time, dt, opponent_level, n_batches);
    double mean_minus = average(dmg_minus);
    double std_minus = standard_deviation(dmg_minus, mean_minus);
    double sample_std_minus = sample_deviation(std_minus, n_batches);

    return {mean_plus - mean_init, add_standard_deviations(sample_std_init, sample_std_plus),
            mean_minus - mean_init, add_standard_deviations(sample_std_init, sample_std_minus),
            amount,
            stat};
}
