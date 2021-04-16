#include "../include/Item_optimizer.hpp"

bool operator<(const Item_optimizer::Sim_result_t& left, const Item_optimizer::Sim_result_t& right)
{
    return left.mean_dps < right.mean_dps;
}

void Item_optimizer::compute_weapon_combinations()
{
    weapon_combinations.clear();
    if (main_hands.size() == 1 && off_hands.size() == 1)
    {
        weapon_combinations.emplace_back(std::vector<Weapon>{main_hands[0], off_hands[0]});
    }
    else
    {
        for (const auto& main_wep : main_hands)
        {
            for (const auto& off_wep : off_hands)
            {
                // TODO unique tag needed here!!
                if (main_wep.name != off_wep.name)
                {
                    bool new_combination = true;
                    for (const auto& combination : weapon_combinations)
                    {
                        if (combination[0].name == main_wep.name && combination[1].name == off_wep.name)
                        {
                            new_combination = false;
                        }
                    }
                    if (new_combination)
                    {
                        weapon_combinations.emplace_back(std::vector<Weapon>{main_wep, off_wep});
                    }
                }
            }
        }
    }
    for (const auto& wep : two_hands)
    {
        weapon_combinations.emplace_back(std::vector<Weapon>{wep});
    }
}

std::vector<std::vector<Armor>> Item_optimizer::get_combinations(const std::vector<Armor>& armors)
{
    std::vector<std::vector<Armor>> combinations;
    for (size_t i_1 = 0; i_1 < armors.size() - 1; i_1++)
    {
        for (size_t i_2 = i_1 + 1; i_2 < armors.size(); i_2++)
        {
            combinations.emplace_back(std::vector<Armor>{armors[i_1], armors[i_2]});
        }
    }
    return combinations;
}

bool is_armor_valid(const std::string& name)
{
    return !(name.substr(0, 14) == "item_not_found");
}

void Item_optimizer::extract_armors(const std::vector<std::string>& armor_vec)
{
    for (const auto& armor_name : armor_vec)
    {
        auto armor = armory.find_armor(Socket::head, armor_name);
        if (is_armor_valid(armor.name))
        {
            helmets.emplace_back(armor);
            continue;
        }
        armor = armory.find_armor(Socket::neck, armor_name);
        if (is_armor_valid(armor.name))
        {
            necks.emplace_back(armor);
            continue;
        }
        armor = armory.find_armor(Socket::shoulder, armor_name);
        if (is_armor_valid(armor.name))
        {
            shoulders.emplace_back(armor);
            continue;
        }
        armor = armory.find_armor(Socket::back, armor_name);
        if (is_armor_valid(armor.name))
        {
            backs.emplace_back(armor);
            continue;
        }
        armor = armory.find_armor(Socket::chest, armor_name);
        if (is_armor_valid(armor.name))
        {
            chests.emplace_back(armor);
            continue;
        }
        armor = armory.find_armor(Socket::wrist, armor_name);
        if (is_armor_valid(armor.name))
        {
            wrists.emplace_back(armor);
            continue;
        }
        armor = armory.find_armor(Socket::hands, armor_name);
        if (is_armor_valid(armor.name))
        {
            hands.emplace_back(armor);
            continue;
        }
        armor = armory.find_armor(Socket::belt, armor_name);
        if (is_armor_valid(armor.name))
        {
            belts.emplace_back(armor);
            continue;
        }
        armor = armory.find_armor(Socket::legs, armor_name);
        if (is_armor_valid(armor.name))
        {
            legs.emplace_back(armor);
            continue;
        }
        armor = armory.find_armor(Socket::boots, armor_name);
        if (is_armor_valid(armor.name))
        {
            boots.emplace_back(armor);
            continue;
        }
        armor = armory.find_armor(Socket::ring, armor_name);
        if (is_armor_valid(armor.name))
        {
            rings.emplace_back(armor);
            continue;
        }
        armor = armory.find_armor(Socket::trinket, armor_name);
        if (is_armor_valid(armor.name))
        {
            trinkets.emplace_back(armor);
            continue;
        }
        armor = armory.find_armor(Socket::ranged, armor_name);
        if (is_armor_valid(armor.name))
        {
            ranged.emplace_back(armor);
            continue;
        }
    }
}

void Item_optimizer::extract_weapons(const std::vector<std::string>& weapon_vec)
{
    for (const auto& weapon_name : weapon_vec)
    {
        auto weapon = armory.find_weapon(Weapon_socket::one_hand, weapon_name);
        if (is_armor_valid(weapon.name))
        {
            if (weapon.weapon_socket == Weapon_socket::one_hand)
            {
                main_hands.emplace_back(weapon);
                off_hands.emplace_back(weapon);
            }
            else if (weapon.weapon_socket == Weapon_socket::main_hand)
            {
                main_hands.emplace_back(weapon);
            }
            else if (weapon.weapon_socket == Weapon_socket::off_hand)
            {
                off_hands.emplace_back(weapon);
            }
        }
        else
        {
            auto th_weapon = armory.find_weapon(Weapon_socket::two_hand, weapon_name);
            if (is_armor_valid(th_weapon.name))
            {
                two_hands.emplace_back(th_weapon);
            }
            else
            {
                std::cout << "Not valid weapon: " << weapon.name << std::endl;
            }
        }
    }
}

void Item_optimizer::fill(std::vector<Armor>& vec, Socket socket, std::string name)
{
    if (socket == Socket::trinket || socket == Socket::ring)
    {
        while (vec.size() < 2)
        {
            vec.emplace_back(armory.find_armor(socket, name));
        }
    }
    else
    {
        if (vec.empty())
        {
            vec.emplace_back(armory.find_armor(socket, name));
        }
    }
}

struct Weapon_struct
{
    Weapon_struct() = default;

    Weapon_struct(size_t index, Special_stats special_stats, double average_damage, double swing_speed,
                  std::string name, Weapon_type type, Weapon_socket socket)
        : index(index)
        , special_stats(special_stats)
        , average_damage(average_damage)
        , swing_speed(swing_speed)
        , name(std::move(name))
        , type(type)
        , socket(socket)
    {
    }

    size_t index{};
    Special_stats special_stats;
    Special_stats set_special_stats;
    Special_stats hit_special_stats;
    double average_damage{};
    double swing_speed{};
    std::string name{};
    Weapon_type type{};
    Weapon_socket socket{};
    bool can_be_estimated{true};
    bool remove{false};
};

bool is_strictly_weaker_wep(const Weapon_struct& wep_struct1, const Weapon_struct& wep_struct2, Weapon_socket socket)
{
    auto special_stats1 = wep_struct1.special_stats + wep_struct1.set_special_stats + wep_struct1.hit_special_stats;
    auto special_stats2 = wep_struct2.special_stats;

    bool greater_eq = (special_stats2.hit >= special_stats1.hit) &&
                      (special_stats2.critical_strike >= special_stats1.critical_strike) &&
                      (special_stats2.attack_power >= special_stats1.attack_power) &&
                      (special_stats2.axe_skill >= special_stats1.axe_skill) &&
                      (special_stats2.sword_skill >= special_stats1.sword_skill) &&
                      (special_stats2.mace_skill >= special_stats1.mace_skill) &&
                      (special_stats2.dagger_skill >= special_stats1.dagger_skill) &&
                      (special_stats2.two_hand_sword_skill >= special_stats1.two_hand_sword_skill) &&
                      (special_stats2.two_hand_mace_skill >= special_stats1.two_hand_mace_skill) &&
                      (special_stats2.two_hand_axe_skill >= special_stats1.two_hand_axe_skill);

    if (socket == Weapon_socket::main_hand)
    {
        greater_eq &= wep_struct2.swing_speed >= wep_struct1.swing_speed;
    }
    else
    {
        greater_eq &= wep_struct2.swing_speed <= wep_struct1.swing_speed;
    }
    greater_eq &=
        wep_struct2.average_damage / wep_struct2.swing_speed >= wep_struct1.average_damage / wep_struct1.swing_speed;

    bool greater = (special_stats2.hit > special_stats1.hit) ||
                   (special_stats2.critical_strike > special_stats1.critical_strike) ||
                   (special_stats2.attack_power > special_stats1.attack_power) ||
                   (special_stats2.axe_skill > special_stats1.axe_skill) ||
                   (special_stats2.sword_skill > special_stats1.sword_skill) ||
                   (special_stats2.mace_skill > special_stats1.mace_skill) ||
                   (special_stats2.dagger_skill > special_stats1.dagger_skill) ||
                   (special_stats2.two_hand_sword_skill > special_stats1.two_hand_sword_skill) ||
                   (special_stats2.two_hand_mace_skill > special_stats1.two_hand_mace_skill) ||
                   (special_stats2.two_hand_axe_skill > special_stats1.two_hand_axe_skill);

    if (socket == Weapon_socket::main_hand)
    {
        greater |= wep_struct2.swing_speed > wep_struct1.swing_speed;
    }
    else
    {
        greater |= wep_struct2.swing_speed < wep_struct1.swing_speed;
    }
    greater |=
        wep_struct2.average_damage / wep_struct2.swing_speed > wep_struct1.average_damage / wep_struct1.swing_speed;

    return greater_eq && greater;
}

double estimate_wep_stat_diff(const Weapon_struct& wep1, const Weapon_struct& wep2, bool main_hand)
{
    // Penalize fast MH and slow OH. Set 1 second to equal 100 AP.
    double wep_1_ap = wep1.average_damage / wep1.swing_speed * 14;
    wep_1_ap += main_hand ? 100 * (wep1.swing_speed - 2.3) : -100 * (wep1.swing_speed - 2.3);

    double wep_2_ap = wep2.average_damage / wep2.swing_speed * 14;
    wep_2_ap += main_hand ? 100 * (wep2.swing_speed - 2.3) : -100 * (wep2.swing_speed - 2.3);

    return wep_2_ap - wep_1_ap;
}

std::vector<Weapon> Item_optimizer::remove_weaker_weapons(const Weapon_socket weapon_socket,
                                                          const std::vector<Weapon>& weapon_vec,
                                                          const Special_stats& special_stats,
                                                          std::string& debug_message, int keep_n_stronger_items)
{
    std::vector<Weapon_struct> weapon_struct_vec;
    Character character{race, 70};
    //Special_stats racial_stats = character.base_special_stats;
    for (size_t i = 0; i < weapon_vec.size(); ++i)
    {
        Special_stats wep_special_stats = weapon_vec[i].special_stats;
        wep_special_stats += weapon_vec[i].attributes.convert_to_special_stats(special_stats);

        Weapon_struct wep_struct{i,
                                 wep_special_stats,
                                 (weapon_vec[i].min_damage + weapon_vec[i].max_damage) / 2,
                                 weapon_vec[i].swing_speed,
                                 weapon_vec[i].name,
                                 weapon_vec[i].type,
                                 weapon_vec[i].weapon_socket};

        if (weapon_vec[i].set_name != Set::none)
        {
            double ap_equiv_max = 0;
            Special_stats best_special_stats;
            for (const auto& set_bonus : armory.set_bonuses)
            {
                if (weapon_vec[i].set_name == set_bonus.set)
                {
                    Special_stats set_special_stats = set_bonus.special_stats;
                    set_special_stats += set_bonus.attributes.convert_to_special_stats(special_stats);
                    double ap_equiv = estimate_special_stats_high(set_special_stats);
                    if (ap_equiv > ap_equiv_max)
                    {
                        ap_equiv_max = ap_equiv;
                        best_special_stats = set_special_stats;
                    }
                }
            }
            wep_struct.set_special_stats = best_special_stats;
        }
        if (!weapon_vec[i].hit_effects.empty())
        {
            for (const auto& effect : weapon_vec[i].hit_effects)
            {
                switch (effect.type)
                {
                case Hit_effect::Type::damage_magic:
                case Hit_effect::Type::damage_physical:
                case Hit_effect::Type::extra_hit: {
                    double factor = weapon_socket == Weapon_socket::main_hand ? 1.0 : 0.5;
                    if (effect.affects_both_weapons)
                    {
                        factor *= 2;
                    }
                    wep_struct.hit_special_stats.attack_power +=
                        get_hit_effect_ap_equivalent(effect, 0.0, weapon_vec[i].swing_speed, factor);
                    break;
                }
                default:
                    wep_struct.can_be_estimated = false;
                }
            }
        }
        if (!weapon_vec[i].use_effects.empty())
        {
            wep_struct.can_be_estimated = false;
        }
        weapon_struct_vec.push_back(wep_struct);
    }

    std::string wep_socket{};
    wep_socket = wep_socket + weapon_socket;
    for (auto& wep1 : weapon_struct_vec)
    {
        if (wep1.can_be_estimated)
        {
            int stronger_found = 0;
            for (const auto& wep2 : weapon_struct_vec)
            {
                if (wep1.index != wep2.index)
                {
                    if ((wep1.type == wep2.type) && is_strictly_weaker_wep(wep1, wep2, weapon_socket))
                    {
                        stronger_found++;
                        debug_message += string_with_precision(stronger_found) + "/" +
                                         string_with_precision(keep_n_stronger_items) + " since <b>" + wep2.name +
                                         "</b> is better than <b>" + wep1.name + "</b> in all aspects.<br>";
                    }
                    else
                    {
                        double stat_diff =
                            estimate_stat_diff(wep1.special_stats + wep1.set_special_stats + wep1.hit_special_stats,
                                               wep2.special_stats + wep2.hit_special_stats);
                        double wep_stat_diff =
                            estimate_wep_stat_diff(wep1, wep2, weapon_socket == Weapon_socket::main_hand);
                        if (stat_diff + wep_stat_diff > 0)
                        {
                            stronger_found++;
                            debug_message += string_with_precision(stronger_found) + "/" +
                                             string_with_precision(keep_n_stronger_items) + " since <b>" + wep2.name +
                                             "</b> was estimated to be <b>" +
                                             string_with_precision(stat_diff + wep_stat_diff, 3) +
                                             " AP </b>better than <b>" + wep1.name + "</b>.<br>";
                        }
                    }
                    if (stronger_found >= keep_n_stronger_items)
                    {
                        wep1.remove = true;
                        debug_message += "REMOVED:<b> " + wep1.name + "</b>.<br>";
                        break;
                    }
                }
            }
        }
    }

    debug_message += "Weapons left:<br>";
    std::vector<Weapon> filtered_weapons;
    for (size_t i = 0; i < weapon_vec.size(); ++i)
    {
        if (!weapon_struct_vec[i].remove)
        {
            filtered_weapons.push_back(weapon_vec[i]);
            debug_message += "<b>" + weapon_vec[i].name + "</b><br>";
        }
    }

    return filtered_weapons;
}

struct Armor_struct
{
    Armor_struct() = default;

    Armor_struct(size_t index, Special_stats special_stats, std::string name)
        : index(index), special_stats(special_stats), name(std::move(name))
    {
    }

    size_t index{};
    Special_stats special_stats{};
    Special_stats set_special_stats{};
    Special_stats use_special_stats{};
    Special_stats hit_special_stats{};
    std::string name{};
    bool can_be_estimated{true};
    bool remove{false};
};

std::vector<Armor> Item_optimizer::remove_weaker_items(const std::vector<Armor>& armors,
                                                       const Special_stats& special_stats, std::string& debug_message,
                                                       int keep_n_stronger_items)
{
    std::vector<Armor_struct> armors_special_stats;
    for (size_t i = 0; i < armors.size(); ++i)
    {
        Special_stats armor_special_stats = armors[i].special_stats;
        armor_special_stats += armors[i].attributes.convert_to_special_stats(special_stats);
        Armor_struct armor_equiv{i, armor_special_stats, armors[i].name};
        if (armors[i].set_name != Set::none)
        {
            double ap_equiv_max = 0;
            Special_stats best_special_stats;
            for (const auto& set_bonus : possible_set_bonuses)
            {
                if (armors[i].set_name == set_bonus.set)
                {
                    Special_stats set_special_stats = set_bonus.special_stats;
                    set_special_stats += set_bonus.attributes.convert_to_special_stats(special_stats);
                    double ap_equiv = estimate_special_stats_high(set_special_stats);
                    if (ap_equiv > ap_equiv_max)
                    {
                        ap_equiv_max = ap_equiv;
                        best_special_stats = set_special_stats;
                    }
                }
            }
            armor_equiv.set_special_stats = best_special_stats;
        }
        if (!armors[i].use_effects.empty())
        {
            armor_equiv.can_be_estimated = false;
        }
        if (!armors[i].hit_effects.empty())
        {
            for (const auto& effect : armors[i].hit_effects)
            {
                switch (effect.type)
                {
                case Hit_effect::Type::damage_magic:
                case Hit_effect::Type::damage_physical:
                    armor_equiv.hit_special_stats.attack_power += effect.probability * effect.damage * ap_per_coh;
                    break;
                case Hit_effect::Type::extra_hit:
                    //  the factor 0.5 since extra hit resets main hand attack
                    armor_equiv.hit_special_stats.critical_strike += effect.probability * 0.5;
                    break;
                default:
                    armor_equiv.can_be_estimated = false;
                }
            }
        }
        armors_special_stats.push_back(armor_equiv);
    }

    for (auto& armor1 : armors_special_stats)
    {
        int stronger_found = 0;
        if (armor1.can_be_estimated)
        {
            for (const auto& armor2 : armors_special_stats)
            {
                if (armor1.index != armor2.index)
                {
                    if (is_strictly_weaker(armor1.special_stats + armor1.set_special_stats + armor1.use_special_stats +
                                               armor1.hit_special_stats,
                                           armor2.special_stats + armor2.hit_special_stats))
                    {
                        stronger_found++;
                        debug_message += string_with_precision(stronger_found) + "/" +
                                         string_with_precision(keep_n_stronger_items) + " since <b>" + armor2.name +
                                         "</b> is better than <b>" + armor1.name + "</b> in all aspects.<br>";
                    }
                    else
                    {
                        double stat_diff = estimate_stat_diff(armor1.special_stats + armor1.set_special_stats +
                                                                  armor1.use_special_stats + armor1.hit_special_stats,
                                                              armor2.special_stats + armor2.use_special_stats +
                                                                  armor2.hit_special_stats);
                        if (stat_diff > 0)
                        {
                            stronger_found++;
                            debug_message += string_with_precision(stronger_found) + "/" +
                                             string_with_precision(keep_n_stronger_items) + " since <b>" + armor2.name +
                                             "</b> was estimated to be <b>" + string_with_precision(stat_diff, 3) +
                                             " AP </b>better than <b>" + armor1.name + "</b>.<br>";
                        }
                    }
                    if (stronger_found >= keep_n_stronger_items)
                    {
                        armor1.remove = true;
                        debug_message += "REMOVED:<b> " + armor1.name + "</b>.<br>";
                        break;
                    }
                }
            }
        }
    }

    debug_message += "Armors left:<br>";
    std::vector<Armor> filtered_armors;
    for (size_t i = 0; i < armors.size(); ++i)
    {
        if (!armors_special_stats[i].remove)
        {
            filtered_armors.push_back(armors[i]);
            debug_message += "<b>" + armors[i].name + "</b><br>";
        }
    }

    return filtered_armors;
}

void Item_optimizer::filter_weaker_items(const Special_stats& special_stats, std::string& debug_message,
                                         int min_removal)
{
    //    debug_message += "<br>Filtering <b>Shared cooldown use-effects: </b><br>";
    //    find_best_use_effect(special_stats, debug_message);

    debug_message += "<br>Filtering <b> Helmets: </b><br>";
    helmets = remove_weaker_items(helmets, special_stats, debug_message, min_removal);
    debug_message += "<br>Filtering <b> necks: </b><br>";
    necks = remove_weaker_items(necks, special_stats, debug_message, min_removal);
    debug_message += "<br>Filtering <b> shoulders: </b><br>";
    shoulders = remove_weaker_items(shoulders, special_stats, debug_message, min_removal);
    debug_message += "<br>Filtering <b> backs: </b><br>";
    backs = remove_weaker_items(backs, special_stats, debug_message, min_removal);
    debug_message += "<br>Filtering <b> chests: </b><br>";
    chests = remove_weaker_items(chests, special_stats, debug_message, min_removal);
    debug_message += "<br>Filtering <b> wrists: </b><br>";
    wrists = remove_weaker_items(wrists, special_stats, debug_message, min_removal);
    debug_message += "<br>Filtering <b> hands: </b><br>";
    hands = remove_weaker_items(hands, special_stats, debug_message, min_removal);
    debug_message += "<br>Filtering <b> belts: </b><br>";
    belts = remove_weaker_items(belts, special_stats, debug_message, min_removal);
    debug_message += "<br>Filtering <b> legs: </b><br>";
    legs = remove_weaker_items(legs, special_stats, debug_message, min_removal);
    debug_message += "<br>Filtering <b> boots: </b><br>";
    boots = remove_weaker_items(boots, special_stats, debug_message, min_removal);
    debug_message += "<br>Filtering <b> ranged: </b><br>";
    ranged = remove_weaker_items(ranged, special_stats, debug_message, min_removal);
    debug_message += "<br>Filtering <b> rings: </b><br>";
    rings = remove_weaker_items(rings, special_stats, debug_message, min_removal + 1);
    debug_message += "<br>Filtering <b> trinkets: </b><br>";
    trinkets = remove_weaker_items(trinkets, special_stats, debug_message, min_removal + 1);

    debug_message += "<br>Filtering <b> Main-hand weapons: </b><br>";
    main_hands =
        remove_weaker_weapons(Weapon_socket::main_hand, main_hands, special_stats, debug_message, min_removal + 1);
    debug_message += "<br>Filtering <b> Off-hand weapons: </b><br>";
    off_hands =
        remove_weaker_weapons(Weapon_socket::off_hand, off_hands, special_stats, debug_message, min_removal + 1);
    debug_message += "<br>Filtering <b> Two-hand weapons: </b><br>";
    two_hands = remove_weaker_weapons(Weapon_socket::main_hand, two_hands, special_stats, debug_message, min_removal);
}

void Item_optimizer::fill_empty_armor()
{
    fill(helmets, Socket::head, "helmet");
    fill(necks, Socket::neck, "neck");
    fill(shoulders, Socket::shoulder, "shoulder");
    fill(backs, Socket::back, "back");
    fill(chests, Socket::chest, "chest");
    fill(wrists, Socket::wrist, "wrists");
    fill(hands, Socket::hands, "hands");
    fill(belts, Socket::belt, "belt");
    fill(legs, Socket::legs, "legs");
    fill(boots, Socket::boots, "boots");
    fill(ranged, Socket::ranged, "ranged");
    fill(rings, Socket::ring, "ring");
    fill(trinkets, Socket::trinket, "trinket");
}

void Item_optimizer::fill_empty_weapons()
{
    if (main_hands.empty())
    {
        main_hands.emplace_back(armory.find_weapon(Weapon_socket::one_hand, "none"));
    }
    if (off_hands.empty())
    {
        off_hands.emplace_back(armory.find_weapon(Weapon_socket::one_hand, "none"));
    }
    if (two_hands.empty())
    {
        two_hands.emplace_back(armory.find_weapon(Weapon_socket::two_hand, "none"));
    }
}

void Item_optimizer::find_set_bonuses()
{
    std::vector<std::vector<Armor>> armors;
    armors.push_back(helmets);
    armors.push_back(necks);
    armors.push_back(shoulders);
    armors.push_back(backs);
    armors.push_back(chests);
    armors.push_back(wrists);
    armors.push_back(hands);
    armors.push_back(belts);
    armors.push_back(legs);
    armors.push_back(boots);
    armors.push_back(ranged);
    armors.push_back(rings);
    armors.push_back(trinkets);

    std::vector<std::vector<Weapon>> weapons;
    weapons.push_back(main_hands);
    weapons.push_back(off_hands);

    set_names.clear();
    for (const auto& armor_vec : armors)
    {
        for (const auto& armor : armor_vec)
        {
            set_names.emplace_back(armor.set_name);
        }
    }
    for (const auto& weapon_vec : weapons)
    {
        for (const auto& armor : weapon_vec)
        {
            set_names.emplace_back(armor.set_name);
        }
    }

    std::vector<Set> unique_set_names{};
    for (const Set& set_name : set_names)
    {
        if (set_name != Set::none)
        {
            bool unique = true;
            for (const Set& set_name_unique : unique_set_names)
            {
                if (set_name == set_name_unique)
                {
                    unique = false;
                }
            }
            if (unique)
            {
                unique_set_names.emplace_back(set_name);
            }
        }
    }

    std::cout << "Possible set-bonuses: " << std::endl;
    possible_set_bonuses.clear();
    for (const Set& unique_set_name : unique_set_names)
    {
        int count = 0;
        for (const Set& set_name : set_names)
        {
            if (set_name == unique_set_name)
            {
                count++;
            }
        }
        for (const Set_bonus& set_bonus : armory.set_bonuses)
        {
            if (set_bonus.set == unique_set_name && count >= set_bonus.pieces)
            {
                std::cout << "Possible set-bonus: " << set_bonus.name << std::endl;
                possible_set_bonuses.push_back(set_bonus);
            }
        }
    }
}

void Item_optimizer::item_setup(const std::vector<std::string>& armor_vec, const std::vector<std::string>& weapons_vec)
{
    extract_armors(armor_vec);
    fill_empty_armor();
    extract_weapons(weapons_vec);
    fill_empty_weapons();
    find_set_bonuses();
}

void Item_optimizer::compute_combinations()
{
    compute_weapon_combinations();
    ring_combinations = get_combinations(rings);
    trinket_combinations = get_combinations(trinkets);

    combination_vector.clear();
    combination_vector.push_back(helmets.size());
    combination_vector.push_back(necks.size());
    combination_vector.push_back(shoulders.size());
    combination_vector.push_back(backs.size());
    combination_vector.push_back(chests.size());
    combination_vector.push_back(wrists.size());
    combination_vector.push_back(hands.size());
    combination_vector.push_back(belts.size());
    combination_vector.push_back(legs.size());
    combination_vector.push_back(boots.size());
    combination_vector.push_back(ranged.size());
    combination_vector.push_back(ring_combinations.size());
    combination_vector.push_back(trinket_combinations.size());
    combination_vector.push_back(weapon_combinations.size());

    cum_combination_vector.clear();
    total_combinations = combination_vector[0];
    cum_combination_vector.push_back(combination_vector[0]);
    for (size_t i = 1; i < combination_vector.size(); i++)
    {
        cum_combination_vector.push_back(cum_combination_vector.back() * combination_vector[i]);
        total_combinations *= combination_vector[i];
    }
}

std::vector<size_t> Item_optimizer::get_item_ids(size_t index)
{
    std::vector<size_t> item_ids;
    item_ids.reserve(14);
    item_ids.push_back(index % combination_vector[0]);
    for (size_t i = 1; i < 14; i++)
    {
        item_ids.push_back(index / cum_combination_vector[i - 1] % combination_vector[i]);
    }
    return item_ids;
}

Character Item_optimizer::generate_character(const std::vector<size_t>& item_ids)
{
    Character character{race, 60};
    character.equip_armor(helmets[item_ids[0]]);
    character.equip_armor(necks[item_ids[1]]);
    character.equip_armor(shoulders[item_ids[2]]);
    character.equip_armor(backs[item_ids[3]]);
    character.equip_armor(chests[item_ids[4]]);
    character.equip_armor(wrists[item_ids[5]]);
    character.equip_armor(hands[item_ids[6]]);
    character.equip_armor(belts[item_ids[7]]);
    character.equip_armor(legs[item_ids[8]]);
    character.equip_armor(boots[item_ids[9]]);
    character.equip_armor(ranged[item_ids[10]]);
    character.equip_armor(ring_combinations[item_ids[11]][0]);
    character.equip_armor(ring_combinations[item_ids[11]][1]);
    character.equip_armor(trinket_combinations[item_ids[12]][0]);
    character.equip_armor(trinket_combinations[item_ids[12]][1]);
    character.equip_weapon(weapon_combinations[item_ids[13]]);
    return character;
}

Character Item_optimizer::construct(size_t index)
{
    Character character = generate_character(get_item_ids(index));

    armory.add_enchants_to_character(character, ench_vec);

    armory.add_buffs_to_character(character, buffs_vec);

    armory.add_talents_to_character(character, talent_vec, talent_val_vec);

    armory.compute_total_stats(character);

    return character;
}
