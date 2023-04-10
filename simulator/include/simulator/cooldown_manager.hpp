#ifndef WOW_SIMULATOR_COOLDOWN_MANAGER_HPP
#define WOW_SIMULATOR_COOLDOWN_MANAGER_HPP

#include "common/sim_time.hpp"
#include "damage_sources.hpp"
#include "spells.hpp"

class Cooldown_manager
{
public:
    void increment(Sim_time dt);

    Sim_time time_until_next_spell_is_ready();

    void add(const Spell_base& spell);

    void reset();

    [[nodiscard]] Sim_time cooldown_of(Damage_source damage_source) const;

    [[nodiscard]] bool spell_is_ready(Damage_source damage_source) const;

    [[nodiscard]] bool can_cast(Damage_source damage_source) const;

    [[nodiscard]] bool has_global_cooldown() const;

    [[nodiscard]] Sim_time& get_global_cooldown();

    void set_global_cooldown();

private:
    Sim_time global_cooldown_;
    std::map<Damage_source, Sim_time> spell_cooldowns_;
};

#endif // WOW_SIMULATOR_COOLDOWN_MANAGER_HPP
