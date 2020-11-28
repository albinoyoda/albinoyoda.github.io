#include "damage_sources.hpp"

Damage_sources::Damage_sources()
{
    damage_instances.reserve(500);
};

Damage_sources& Damage_sources::operator+(const Damage_sources& rhs)
{
    whirlwind_damage = whirlwind_damage + rhs.whirlwind_damage;
    overpower_damage = overpower_damage + rhs.overpower_damage;
    slam_damage = slam_damage + rhs.slam_damage;
    bloodthirst_damage = bloodthirst_damage + rhs.bloodthirst_damage;
    mortal_strike_damage = mortal_strike_damage + rhs.mortal_strike_damage;
    sweeping_strikes_damage = sweeping_strikes_damage + rhs.sweeping_strikes_damage;
    execute_damage = execute_damage + rhs.execute_damage;
    white_mh_damage = white_mh_damage + rhs.white_mh_damage;
    white_oh_damage = white_oh_damage + rhs.white_oh_damage;
    heroic_strike_damage = heroic_strike_damage + rhs.heroic_strike_damage;
    cleave_damage = cleave_damage + rhs.cleave_damage;
    hamstring_damage = hamstring_damage + rhs.hamstring_damage;
    deep_wounds_damage = deep_wounds_damage + rhs.deep_wounds_damage;
    item_hit_effects_damage = item_hit_effects_damage + rhs.item_hit_effects_damage;

    whirlwind_count = whirlwind_count + rhs.whirlwind_count;
    overpower_count = overpower_count + rhs.overpower_count;
    slam_count = slam_count + rhs.slam_count;
    bloodthirst_count = bloodthirst_count + rhs.bloodthirst_count;
    mortal_strike_count = mortal_strike_count + rhs.mortal_strike_count;
    sweeping_strikes_count = sweeping_strikes_count + rhs.sweeping_strikes_count;
    execute_count = execute_count + rhs.execute_count;
    white_mh_count = white_mh_count + rhs.white_mh_count;
    white_oh_count = white_oh_count + rhs.white_oh_count;
    heroic_strike_count = heroic_strike_count + rhs.heroic_strike_count;
    cleave_count = cleave_count + rhs.cleave_count;
    hamstring_count = hamstring_count + rhs.hamstring_count;
    deep_wounds_count = deep_wounds_count + rhs.deep_wounds_count;
    item_hit_effects_count = item_hit_effects_count + rhs.item_hit_effects_count;
    return *(this);
}

void Damage_sources::add_damage(Damage_source source, double damage, double time_stamp, bool increment_counter)
{
    if (increment_counter)
    {
        damage_instances.emplace_back(source, damage, time_stamp);
    }
    switch (source)
    {
    case Damage_source::white_mh:
        white_mh_damage += damage;
        white_mh_count++;
        break;
    case Damage_source::white_oh:
        white_oh_damage += damage;
        white_oh_count++;
        break;
    case Damage_source::bloodthirst:
        bloodthirst_damage += damage;
        bloodthirst_count++;
        break;
    case Damage_source::sweeping_strikes:
        sweeping_strikes_damage += damage;
        sweeping_strikes_count++;
        break;
    case Damage_source::mortal_strike:
        mortal_strike_damage += damage;
        mortal_strike_count++;
        break;
    case Damage_source::slam:
        slam_damage += damage;
        slam_count++;
        break;
    case Damage_source::overpower:
        overpower_damage += damage;
        overpower_count++;
        break;
    case Damage_source::execute:
        execute_damage += damage;
        execute_count++;
        break;
    case Damage_source::heroic_strike:
        heroic_strike_damage += damage;
        heroic_strike_count++;
        break;
    case Damage_source::cleave:
        cleave_damage += damage;
        cleave_count++;
        break;
    case Damage_source::whirlwind:
        whirlwind_damage += damage;
        whirlwind_count++;
        break;
    case Damage_source::hamstring:
        hamstring_damage += damage;
        hamstring_count++;
        break;
    case Damage_source::deep_wounds:
        deep_wounds_damage += damage;
        deep_wounds_count++;
        break;
    case Damage_source::item_hit_effects:
        item_hit_effects_damage += damage;
        if (increment_counter)
        {
            item_hit_effects_count++;
        }
        break;
    }
}