#include "simulator/cooldown_manager.hpp"

void Cooldown_manager::increment(Sim_time dt)
{
    for (auto it = spell_cooldowns_.begin(), end = spell_cooldowns_.end(); it != end;)
    {
        it->second -= dt;
        if (it->second < Sim_time::zero())
        {
            it = spell_cooldowns_.erase(it);
        }
        else
        {
            ++it;
        }
    }
    global_cooldown_ -= dt;
}

Sim_time Cooldown_manager::time_until_next_spell_is_ready()
{
    auto min_time{Sim_time::from_seconds(100)};
    for (auto& [_, time] : spell_cooldowns_)
    {
        min_time = std::min(min_time, time);
    }
    if (global_cooldown_ > Sim_time::zero())
    {
        min_time = std::min(min_time, global_cooldown_);
    }
    return min_time;
}

void Cooldown_manager::add(const Spell_base& spell)
{
    spell_cooldowns_[spell.damage_source] = spell.cooldown;
    if (spell.triggers_gcd)
    {
        global_cooldown_ = Sim_time::from_seconds(1.5);
    }
}

void Cooldown_manager::reset()
{
    spell_cooldowns_.clear();
    global_cooldown_ = Sim_time::zero();
}

Sim_time Cooldown_manager::cooldown_of(Damage_source damage_source) const
{
    if (spell_cooldowns_.count(damage_source))
    {
        return spell_cooldowns_.at(damage_source);
    }
    return Sim_time::from_seconds(0);
}

bool Cooldown_manager::spell_is_ready(Damage_source damage_source) const
{
    return cooldown_of(damage_source) == Sim_time::from_seconds(0);
}

bool Cooldown_manager::can_cast(Damage_source damage_source) const
{
    return cooldown_of(damage_source) <= Sim_time::zero() && !has_global_cooldown();
}

bool Cooldown_manager::has_global_cooldown() const
{
    return global_cooldown_.milliseconds() > 0;
}

Sim_time& Cooldown_manager::get_global_cooldown()
{
    return global_cooldown_;
}

void Cooldown_manager::set_global_cooldown()
{
    global_cooldown_ = Sim_time::from_seconds(1.5);
}
