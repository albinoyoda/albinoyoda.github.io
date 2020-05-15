#ifndef WOW_SIMULATOR_DAMAGE_SOURCES_TCC
#define WOW_SIMULATOR_DAMAGE_SOURCES_TCC

template<typename T>
std::vector<double> extract_percentage(const std::vector<Damage_sources> &damage_sources_vector, T field_ptr)
{
    std::vector<double> vector_of_t;
    vector_of_t.reserve(damage_sources_vector.size());
    for (const auto &damage_source : damage_sources_vector)
    {
        vector_of_t.emplace_back(damage_source.*field_ptr / damage_source.sum_damage_sources());
    }
    return vector_of_t;
}

template<typename T>
std::vector<double> extract_field(const std::vector<Damage_sources> &damage_sources_vector, T field_ptr)
{
    std::vector<double> vector_of_t;
    vector_of_t.reserve(damage_sources_vector.size());
    for (const auto &damage_source : damage_sources_vector)
    {
        vector_of_t.emplace_back(damage_source.*field_ptr);
    }
    return vector_of_t;
}

template<typename T1, typename T2>
std::vector<double>
extract_field_average(const std::vector<Damage_sources> &damage_sources_vector, T1 field_damage_ptr, T2 field_count_ptr)
{
    std::vector<double> vector_of_t;
    vector_of_t.reserve(damage_sources_vector.size());
    for (const auto &damage_source : damage_sources_vector)
    {
        if (damage_source.*field_count_ptr != 0)
        {
            vector_of_t.emplace_back(damage_source.*field_damage_ptr / damage_source.*field_count_ptr);
        }
        else
        {
            vector_of_t.emplace_back(0);
        }
    }
    return vector_of_t;
}

template<typename T1, typename T2>
void extract_damage_source_data(const std::vector<Damage_sources> &damage_sources_vector, const std::string &name,
                                T1 source_damage_field, T2 source_count_field)
{
    auto source_percentage_vec = extract_percentage(damage_sources_vector, source_damage_field);
    auto source_percentage_avg = Statistics::average(source_percentage_vec);
    auto source_percentage_std = Statistics::standard_deviation(source_percentage_vec, source_percentage_avg);

    auto source_count_vec = extract_field(damage_sources_vector, source_count_field);
    auto source_count_avg = Statistics::average(source_count_vec);

    auto source_damage_vec = extract_field_average(damage_sources_vector, source_damage_field, source_count_field);
    auto source_damage_avg = Statistics::average(source_damage_vec);

    print_damage_sources(name + ":", source_percentage_avg, source_percentage_std, source_count_avg, source_damage_avg);
}

#endif //WOW_SIMULATOR_DAMAGE_SOURCES_TCC
