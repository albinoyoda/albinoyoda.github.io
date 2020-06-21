#include <Armory.hpp>
#include <Character.hpp>
#include <Combat_simulator.hpp>

#include "sim_interface.hpp"

#include <sstream>

#include <iostream>
#include <ctime>
#include <algorithm>

namespace {
bool find_string(const std::vector<std::string>& string_vec, const std::string& match_string)
{
    for (const auto& string : string_vec) {
        if (string==match_string) {
            return true;
        }
    }
    return false;
}

inline double normalCDF(double value)
{
    return 0.5*erfc(-value*M_SQRT1_2);
}

double find_cdf_quantile(double target_quantile, double precision)
{
    double x = 0.0;
    double quantile = 0.5;
    while (quantile<target_quantile) {
        x += precision;
        quantile = normalCDF(x);
    }
    return x;
}

constexpr int get_skill_of_type(const Special_stats& special_stats, Weapon_type weapon_type)
{
    switch (weapon_type) {
    case Weapon_type::sword:return special_stats.sword_skill;
    case Weapon_type::axe:return special_stats.axe_skill;
    case Weapon_type::dagger:return special_stats.dagger_skill;
    case Weapon_type::mace:return special_stats.mace_skill;
    case Weapon_type::unarmed:return special_stats.fist_skill;
    }
}

std::vector<double> find_cdf_quantile(const std::vector<double>& target_quantiles, double precision)
{
    std::vector<double> x_values;
    x_values.reserve(target_quantiles.size());
    for (const auto& quantile : target_quantiles) {
        x_values.emplace_back(find_cdf_quantile(quantile, precision));
    }
    return x_values;
}

Race get_race(const std::string& race)
{
    if (race=="human") {
        return Race::human;
    }
    else if (race=="gnome") {
        return Race::gnome;
    }
    else if (race=="dwarf") {
        return Race::dwarf;
    }
    else if (race=="night_elf") {
        return Race::night_elf;
    }
    else if (race=="orc") {
        return Race::orc;
    }
    else if (race=="troll") {
        return Race::troll;
    }
    else if (race=="undead") {
        return Race::undead;
    }
    else if (race=="tauren") {
        return Race::tauren;
    }
    else {
        std::cout << "Race not found!!! picking human" << "\n";
        return Race::human;
    }
}

std::string string_with_precision(double amount, size_t precision)
{
    std::ostringstream stream;
    stream << std::setprecision(precision) << amount;
    return stream.str();
}

constexpr double crit_w = 30;
constexpr double crit_w_cap = 20;
constexpr double hit_w = 40;
constexpr double hit_w_cap = 10;
constexpr double skill_w = 195.0/5;
constexpr double skill_w_soft = 60.0/5;
constexpr double skill_w_hard = 20.0/5;
constexpr double ap_per_coh = 20/6.2;

bool can_estimate_hit_effect(const Weapon& wep)
{
    for (const auto& hit_effect : wep.hit_effects) {
        if (hit_effect.name==wep.name) {
            if (hit_effect.type==Hit_effect::Type::stat_boost ||
                    hit_effect.type==Hit_effect::Type::reduce_armor) {
                return false;
            }
        }
    }
    return true;
}

bool can_estimate_use_effects(const std::vector<Use_effect>& use_effects)
{
    for (const auto& use_effect : use_effects) {
        if (use_effect.name!="badge_of_the_swarmguard") {
            if (use_effect.get_special_stat_equivalent({}).attack_power==0) {
                return false;
            }
        }
    }
    return true;
}

double get_ap_equivalent(const Special_stats& special_stats, int relevant_skill, double swing_speed,
        double weapon_damage)
{
    int target_defence_level = 315;
    int skill_diff = target_defence_level-relevant_skill;
    int base_skill_diff = 15;
    double crit_chance = special_stats.critical_strike-base_skill_diff*0.2-1.8; // 1.8 flat aura modifier

    // Miss chance
    double base_miss_chance;
    int hit_penalty = 0;
    if (skill_diff>10) {
        base_miss_chance = 5.0+skill_diff*0.2;
        hit_penalty = 1;
    }
    else if (skill_diff>0) {
        base_miss_chance = 5.0+skill_diff*0.1;
    }
    else {
        base_miss_chance = 5.0;
    }
    double miss_chance_yellow = base_miss_chance+hit_penalty;
    double dw_miss_chance = (base_miss_chance*0.8+20.0);
    double miss_chance = dw_miss_chance-std::max(special_stats.hit-hit_penalty, 0.0);

    // Dodge chance
    double dodge_chance = std::max(5+skill_diff*0.1, 5.0);
    double crit_cap = 100-(miss_chance+dodge_chance+40);

    double ap_equiv{};
    if (crit_chance>crit_cap) {
        ap_equiv = (crit_chance-crit_cap)*crit_w_cap+crit_cap*crit_w;
    }
    else {
        ap_equiv = crit_chance*crit_w;
    }

    if (special_stats.hit>miss_chance_yellow) {
        ap_equiv += (special_stats.hit-miss_chance_yellow)*hit_w_cap+miss_chance_yellow*hit_w;
    }
    else {
        ap_equiv += special_stats.hit*hit_w;
    }

    //ish good estimation
    ap_equiv += special_stats.chance_for_extra_hit/2*hit_w;

    int extra_skill = relevant_skill-300;
    if (extra_skill>0) {
        ap_equiv += std::min(5, extra_skill)*skill_w;
    }
    if (extra_skill>5) {
        ap_equiv += std::min(5, extra_skill-5)*skill_w_soft;
    }
    if (extra_skill>10) {
        ap_equiv += extra_skill-10*skill_w_hard;
    }

    ap_equiv += (weapon_damage+special_stats.bonus_damage)/swing_speed*14;

    return ap_equiv;
}

double get_total_qp_equivalent(const Special_stats& special_stats, const Weapon& wep1, const Weapon& wep2,
        const std::vector<Use_effect>& use_effects, double sim_time)
{
    double attack_power = special_stats.attack_power;
    int main_hand_skill = get_skill_of_type(special_stats, wep1.type);
    int off_hand_skill = get_skill_of_type(special_stats, wep2.type);
    double main_hand_ap = get_ap_equivalent(special_stats, main_hand_skill,
            wep1.swing_speed,
            (wep1.max_damage+wep1.min_damage)/2);
    double off_hand_ap = get_ap_equivalent(special_stats, off_hand_skill,
            wep2.swing_speed,
            (wep2.max_damage+wep2.min_damage)/2);
    double use_effects_ap = 0;
    double best_use_effects_shared_ap = 0;
    for (const auto& effect : use_effects) {
        double use_effect_ap = effect.get_special_stat_equivalent(special_stats).attack_power*
                std::min(effect.duration/sim_time, 1.0);
        if (effect.name=="badge_of_the_swarmguard") {
            use_effect_ap = 350*std::min(effect.duration/sim_time, 1.0);
        }
        if (effect.effect_socket==Use_effect::Effect_socket::unique) {
            use_effects_ap += use_effect_ap;
        }
        else {
            if (use_effect_ap>best_use_effects_shared_ap) {
                best_use_effects_shared_ap = use_effect_ap;
            }
        }
    }
    double hit_effects_ap = 0;
    for (const auto& effect : wep1.hit_effects) {
        if (effect.type==Hit_effect::Type::damage_magic_guaranteed ||
                effect.type==Hit_effect::Type::damage_magic ||
                effect.type==Hit_effect::Type::damage_physical) {
            hit_effects_ap += effect.probability*effect.damage*ap_per_coh;
        }
        else if (effect.type==Hit_effect::Type::extra_hit) {
            hit_effects_ap += effect.probability*hit_w;
        }
    }
    for (const auto& effect : wep2.hit_effects) {
        if (effect.type==Hit_effect::Type::damage_magic_guaranteed ||
                effect.type==Hit_effect::Type::damage_magic ||
                effect.type==Hit_effect::Type::damage_physical) {
            hit_effects_ap += effect.probability*effect.damage*ap_per_coh*0.5;
        }
        else if (effect.type==Hit_effect::Type::extra_hit) {
            hit_effects_ap += effect.probability*hit_w*0.5;
        }
    }
    return attack_power+(main_hand_ap+0.625*off_hand_ap)/1.625+use_effects_ap+best_use_effects_shared_ap
            +hit_effects_ap;
}

}

struct Item_popularity {
  Item_popularity() = default;

  Item_popularity(std::string name, size_t counter)
          :
          name{std::move(name)},
          counter{counter} { }

  std::string name;
  size_t counter{};
};

bool operator<(const Item_popularity& left, const std::string& right)
{
    return left.name<right;
}

bool operator==(const Item_popularity& left, const std::string& right)
{
    return left.name==right;
}

class Item_optimizer {
public:
    struct Sim_result_t {
      Sim_result_t() = default;

      Sim_result_t(size_t index, double mean_dps, double sample_std_dps)
              :
              index{index},
              mean_dps{mean_dps},
              sample_std_dps{sample_std_dps} { }

      size_t index{};
      double mean_dps{};
      double sample_std_dps{};
      double ap_equivalent{};
    };

    static std::vector<std::vector<Weapon>> get_weapon_combinations(const std::vector<Weapon>& weapons);

    std::vector<std::vector<Armor>> get_combinations(const std::vector<Armor>& armors);

    void item_setup(const std::vector<std::string>& armor_vec, const std::vector<std::string>& weapons_vec);

    void extract_armors(const std::vector<std::string>& armor_vec);

    void extract_weapons(const std::vector<std::string>& weapon_vec);

    std::vector<size_t> get_item_ids(size_t index);

    Character generate_character(const std::vector<size_t>& item_ids);

    Character construct(size_t index);

    void fill_empty_armor();
    void fill_empty_weapons();
    void fill(std::vector<Armor>& vec, Socket socket, std::string name);

    std::vector<Armor> helmets;
    std::vector<Armor> necks;
    std::vector<Armor> shoulders;
    std::vector<Armor> backs;
    std::vector<Armor> chests;
    std::vector<Armor> wrists;
    std::vector<Armor> hands;
    std::vector<Armor> belts;
    std::vector<Armor> legs;
    std::vector<Armor> boots;
    std::vector<Armor> ranged;
    std::vector<Armor> rings;
    std::vector<Armor> trinkets;
    std::vector<Weapon> weapons;

    std::vector<std::vector<Weapon>> weapon_combinations;
    std::vector<std::vector<Armor>> ring_combinations;
    std::vector<std::vector<Armor>> trinket_combinations;

    size_t total_combinations{};
    std::vector<size_t> combination_vector{};
    std::vector<size_t> cum_combination_vector{};

    Race race;
    Buffs buffs;
    std::vector<std::string> buffs_vec;
    std::vector<std::string> ench_vec;
private:
    Armory armory;
};

bool operator<(const Item_optimizer::Sim_result_t& left, const Item_optimizer::Sim_result_t& right)
{
    return left.mean_dps<right.mean_dps;
}

std::vector<std::vector<Weapon>> Item_optimizer::get_weapon_combinations(const std::vector<Weapon>& weapons)
{
    std::vector<Weapon> main_hands;
    std::vector<Weapon> one_hands;
    std::vector<Weapon> off_hands;
    std::vector<std::vector<Weapon>> weapon_combinations;
    for (const auto& wep : weapons) {
        switch (wep.weapon_socket) {
        case Weapon_socket::main_hand:main_hands.emplace_back(wep);
            break;
        case Weapon_socket::one_hand:one_hands.emplace_back(wep);
            break;
        case Weapon_socket::off_hand:off_hands.emplace_back(wep);
            break;
        default:break;
        }
    }

    for (const auto& main_wep : main_hands) {
        for (const auto& one_wep : one_hands) {
            weapon_combinations.emplace_back(std::vector<Weapon>{main_wep, one_wep});
        }
        for (const auto& off_wep : off_hands) {
            weapon_combinations.emplace_back(std::vector<Weapon>{main_wep, off_wep});
        }
    }

    for (const auto& one_wep : one_hands) {
        for (const auto& off_wep : off_hands) {
            weapon_combinations.emplace_back(std::vector<Weapon>{one_wep, off_wep});
        }
    }

    // TODO unique tag needed here!!
    for (size_t i_1 = 0; i_1<one_hands.size(); i_1++) {
        for (size_t i_2 = 0; i_2<one_hands.size(); i_2++) {
            if (i_2!=i_1) {
                weapon_combinations.emplace_back(std::vector<Weapon>{one_hands[i_1], one_hands[i_2]});
            }
        }
    }

    return weapon_combinations;
}

std::vector<std::vector<Armor>> Item_optimizer::get_combinations(const std::vector<Armor>& armors)
{
    std::vector<std::vector<Armor>> combinations;
    for (size_t i_1 = 0; i_1<armors.size()-1; i_1++) {
        for (size_t i_2 = i_1+1; i_2<armors.size(); i_2++) {
            combinations.emplace_back(std::vector<Armor>{armors[i_1], armors[i_2]});
        }
    }
    return combinations;
}

bool is_armor_valid(const std::string& name)
{
    return !(name.substr(0, 14)=="item_not_found");
}

void Item_optimizer::extract_armors(const std::vector<std::string>& armor_vec)
{
    for (const auto& armor_name : armor_vec) {
        for (;;) {
            auto armor = armory.find_armor(Socket::head, armor_name);
            if (is_armor_valid(armor.name)) {
                helmets.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::neck, armor_name);
            if (is_armor_valid(armor.name)) {
                necks.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::shoulder, armor_name);
            if (is_armor_valid(armor.name)) {
                shoulders.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::back, armor_name);
            if (is_armor_valid(armor.name)) {
                backs.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::chest, armor_name);
            if (is_armor_valid(armor.name)) {
                chests.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::wrist, armor_name);
            if (is_armor_valid(armor.name)) {
                wrists.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::hands, armor_name);
            if (is_armor_valid(armor.name)) {
                hands.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::belt, armor_name);
            if (is_armor_valid(armor.name)) {
                belts.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::legs, armor_name);
            if (is_armor_valid(armor.name)) {
                legs.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::boots, armor_name);
            if (is_armor_valid(armor.name)) {
                boots.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::ring, armor_name);
            if (is_armor_valid(armor.name)) {
                rings.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::trinket, armor_name);
            if (is_armor_valid(armor.name)) {
                trinkets.emplace_back(armor);
                break;
            }
            armor = armory.find_armor(Socket::ranged, armor_name);
            if (is_armor_valid(armor.name)) {
                ranged.emplace_back(armor);
                break;
            }
        }
    }
}

void Item_optimizer::extract_weapons(const std::vector<std::string>& weapon_vec)
{
    for (const auto& weapon_name : weapon_vec) {
        auto weapon = armory.find_weapon(weapon_name);
        if (is_armor_valid(weapon.name)) {
            weapons.emplace_back(weapon);
        }
    }
}

void Item_optimizer::fill(std::vector<Armor>& vec, Socket socket, std::string name)
{
    if (socket==Socket::trinket || socket==Socket::ring) {
        while (vec.size()<2) {
            vec.emplace_back(armory.find_armor(socket, name));
        }
    }
    else {
        if (vec.empty()) {
            vec.emplace_back(armory.find_armor(socket, name));
        }
    }
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
    fill(trinkets, Socket::trinket, "trinket");
}

void Item_optimizer::fill_empty_weapons()
{
    while (weapons.size()<2) {
        weapons.emplace_back(armory.find_weapon("none"));
    }
    bool is_all_mh = true;
    bool is_all_oh = true;
    for (const auto& wep : weapons) {
        if (wep.weapon_socket==Weapon_socket::main_hand) {
            is_all_oh = false;
        }
        else if (wep.weapon_socket==Weapon_socket::off_hand) {
            is_all_mh = false;
        }
        else {
            is_all_mh = false;
            is_all_oh = false;
        }
    }
    if (is_all_mh || is_all_oh) {
        weapons.emplace_back(armory.find_weapon("none"));
    }
}

void Item_optimizer::item_setup(const std::vector<std::string>& armor_vec, const std::vector<std::string>& weapons_vec)
{
    extract_armors(armor_vec);
    fill_empty_armor();
    extract_weapons(weapons_vec);
    fill_empty_weapons();
    weapon_combinations = get_weapon_combinations(weapons);
    ring_combinations = get_combinations(rings);
    trinket_combinations = get_combinations(trinkets);

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
    for (int i = 1; i<combination_vector.size(); i++) {
        cum_combination_vector.push_back(cum_combination_vector.back()*combination_vector[i]);
        total_combinations *= combination_vector[i];
    }
}

std::vector<size_t> Item_optimizer::get_item_ids(size_t index)
{
    std::vector<size_t> item_ids;
    item_ids.reserve(14);
    item_ids.push_back(index%combination_vector[0]);
    for (size_t i = 1; i<14; i++) {
        item_ids.push_back(index/cum_combination_vector[i-1]%combination_vector[i]);
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
    character.equip_weapon(weapon_combinations[item_ids[13]][0],
            weapon_combinations[item_ids[13]][1]
    );
    return character;
}

Character Item_optimizer::construct(size_t index)
{
    Character character = generate_character(get_item_ids(index));
    if (find_string(ench_vec, "e+8 strength")) {
        character.add_enchant(Socket::head, Enchant::Type::strength);
    }
    else if (find_string(ench_vec, "e+1 haste")) {
        character.add_enchant(Socket::head, Enchant::Type::haste);
    }

    if (find_string(ench_vec, "s+30 attack_power")) {
        character.add_enchant(Socket::shoulder, Enchant::Type::attack_power);
    }

    if (find_string(ench_vec, "b+3 agility")) {
        character.add_enchant(Socket::back, Enchant::Type::agility);
    }

    if (find_string(ench_vec, "c+3 stats")) {
        character.add_enchant(Socket::chest, Enchant::Type::minor_stats);
    }
    else if (find_string(ench_vec, "c+4 stats")) {
        character.add_enchant(Socket::chest, Enchant::Type::major_stats);
    }

    if (find_string(ench_vec, "w+7 strength")) {
        character.add_enchant(Socket::wrist, Enchant::Type::strength7);
    }
    else if (find_string(ench_vec, "w+9 strength")) {
        character.add_enchant(Socket::wrist, Enchant::Type::strength9);
    }

    if (find_string(ench_vec, "h+7 strength")) {
        character.add_enchant(Socket::hands, Enchant::Type::strength);
    }
    else if (find_string(ench_vec, "h+7 agility")) {
        character.add_enchant(Socket::hands, Enchant::Type::agility);
    }
    else if (find_string(ench_vec, "h+15 agility")) {
        character.add_enchant(Socket::hands, Enchant::Type::greater_agility);
    }
    else if (find_string(ench_vec, "h+1 haste")) {
        character.add_enchant(Socket::hands, Enchant::Type::haste);
    }

    if (find_string(ench_vec, "l+8 strength")) {
        character.add_enchant(Socket::legs, Enchant::Type::strength);
    }
    else if (find_string(ench_vec, "l+1 haste")) {
        character.add_enchant(Socket::legs, Enchant::Type::haste);
    }

    if (find_string(ench_vec, "b+7 agility")) {
        character.add_enchant(Socket::boots, Enchant::Type::agility);
    }

    if (find_string(ench_vec, "mcrusader")) {
        character.add_enchant(Socket::main_hand, Enchant::Type::crusader);
    }

    if (find_string(ench_vec, "ocrusader")) {
        character.add_enchant(Socket::off_hand, Enchant::Type::crusader);
    }

    // rallying cry
    if (find_string(buffs_vec, "rallying_cry")) {
        character.add_buff(buffs.rallying_cry);
    }
    if (find_string(buffs_vec, "dire_maul")) {
        character.add_buff(buffs.dire_maul);
    }
    if (find_string(buffs_vec, "songflower")) {
        character.add_buff(buffs.songflower);
    }
    if (find_string(buffs_vec, "warchiefs_blessing")) {
        character.add_buff(buffs.warchiefs_blessing);
    }
    if (find_string(buffs_vec, "spirit_of_zandalar")) {
        character.add_buff(buffs.spirit_of_zandalar);
    }
    if (find_string(buffs_vec, "sayges_fortune")) {
        character.add_buff(buffs.sayges_fortune);
    }

    // Player buffs
    if (find_string(buffs_vec, "battle_shout")) {
        character.add_buff(buffs.battle_shout);
    }
    if (find_string(buffs_vec, "blessing_of_kings")) {
        character.add_buff(buffs.blessing_of_kings);
    }
    if (find_string(buffs_vec, "blessing_of_might")) {
        character.add_buff(buffs.blessing_of_might);
    }
    if (find_string(buffs_vec, "windfury_totem")) {
        character.add_buff(buffs.windfury_totem);
    }
    if (find_string(buffs_vec, "strength_of_earth_totem")) {
        character.add_buff(buffs.strength_of_earth_totem);
    }
    if (find_string(buffs_vec, "grace_of_air_totem")) {
        character.add_buff(buffs.grace_of_air_totem);
    }
    if (find_string(buffs_vec, "gift_of_the_wild")) {
        character.add_buff(buffs.gift_of_the_wild);
    }
    if (find_string(buffs_vec, "leader_of_the_pack")) {
        character.add_buff(buffs.leader_of_the_pack);
    }
    if (find_string(buffs_vec, "trueshot_aura")) {
        character.add_buff(buffs.trueshot_aura);
    }
    if (find_string(buffs_vec, "elixir_mongoose")) {
        character.add_buff(buffs.elixir_mongoose);
    }
    if (find_string(buffs_vec, "blessed_sunfruit")) {
        character.add_buff(buffs.blessed_sunfruit);
    }
    if (find_string(buffs_vec, "smoked_dessert_dumplings")) {
        character.add_buff(buffs.smoked_dessert_dumplings);
    }
    if (find_string(buffs_vec, "juju_power")) {
        character.add_buff(buffs.juju_power);
    }
    if (find_string(buffs_vec, "elixir_of_giants")) {
        character.add_buff(buffs.elixir_of_giants);
    }
    if (find_string(buffs_vec, "juju_might")) {
        character.add_buff(buffs.juju_might);
    }
    if (find_string(buffs_vec, "winterfall_firewater")) {
        character.add_buff(buffs.winterfall_firewater);
    }
    if (find_string(buffs_vec, "roids")) {
        character.add_buff(buffs.roids);
    }
    if (find_string(buffs_vec, "mighty_rage_potion")) {
        character.add_buff(buffs.mighty_rage_potion);
    }
    if (find_string(buffs_vec, "dense_stone_main_hand")) {
        character.add_weapon_buff(Socket::main_hand, buffs.dense_stone);
    }
    else if (find_string(buffs_vec, "elemental_stone_main_hand")) {
        character.add_buff(buffs.elemental_stone);
    }
    if (find_string(buffs_vec, "dense_stone_off_hand")) {

        character.add_weapon_buff(Socket::off_hand, buffs.dense_stone);
    }
    else if (find_string(buffs_vec, "elemental_stone_off_hand")) {
        character.add_buff(buffs.elemental_stone);
    }

    armory.compute_total_stats(character);
    return character;
}

Sim_output_mult Sim_interface::simulate_mult(const Sim_input& input)
{
    Buffs buffs{};
    Item_optimizer item_optimizer;

    Race race = get_race(input.race[0]);
    item_optimizer.race = race;
    item_optimizer.buffs = buffs;
    item_optimizer.buffs_vec = input.buffs;
    item_optimizer.ench_vec = input.enchants;
    item_optimizer.item_setup(input.mult_armor, input.mult_weapons);
    // Combat settings
    auto temp_buffs = input.buffs;

    if (find_string(input.options, "mighty_rage_potion")) {
        //temporary solution
        temp_buffs.emplace_back("mighty_rage_potion");
    }

    // Simulator & Combat settings
    Combat_simulator_config config{};
    config.n_batches = input.n_simulations;
    config.n_batches_statweights = input.n_simulations_stat_weights;
    config.sim_time = input.fight_time;
    config.opponent_level = input.target_level;

    if (find_string(input.options, "curse_of_recklessness")) {
        config.curse_of_recklessness_active = true;
    }
    if (find_string(input.options, "faerie_fire")) {
        config.faerie_fire_feral_active = true;
    }
    if (find_string(input.options, "death_wish")) {
        config.talents.death_wish = true;
    }
    if (find_string(input.options, "recklessness")) {
        config.enable_recklessness = true;
    }
    if (find_string(input.options, "sulfuron_harbinger")) {
        config.mode.sulfuron_harbinger = true;
    }
    if (find_string(input.options, "golemagg")) {
        config.mode.golemagg = true;
    }
    if (find_string(input.options, "vaelastrasz")) {
        config.mode.vaelastrasz = true;
    }
    if (find_string(input.options, "chromaggus")) {
        config.mode.chromaggus = true;
    }
    if (find_string(input.options, "use_bt_in_exec_phase")) {
        config.combat.use_bt_in_exec_phase = true;
    }
    if (find_string(input.options, "use_hs_in_exec_phase")) {
        config.combat.use_hs_in_exec_phase = true;
    }
    if (find_string(input.options, "cleave_if_adds")) {
        config.combat.cleave_if_adds = true;
    }
    if (find_string(input.options, "use_hamstring")) {
        config.combat.use_hamstring = true;
    }
    config.n_sunder_armor_stacks = input.sunder_armor;

    config.talents.improved_heroic_strike = 2;
    config.talents.unbridled_wrath = 5;
    config.talents.flurry = 5;
    config.talents.anger_management = true;
    config.talents.impale = 2;
    config.talents.improved_execute = 2;
    config.talents.dual_wield_specialization = 5;

    config.combat.heroic_strike_rage_thresh = input.heroic_strike_rage_thresh;
    config.combat.cleave_rage_thresh = input.cleave_rage_thresh;
    config.combat.whirlwind_rage_thresh = input.whirlwind_rage_thresh;
    config.combat.hamstring_cd_thresh = input.hamstring_cd_thresh;
    config.combat.hamstring_thresh_dd = input.hamstring_thresh_dd;
    config.combat.initial_rage = input.initial_rage;

    config.use_sim_time_ramp = true;
    config.enable_bloodrage = true;
    config.use_seed = true;
    config.seed = clock();
    config.fuel_extra_rage = false;
    config.extra_rage_interval = 3;
    config.extra_rage_damage_amount = 150;

    Combat_simulator simulator(config);

    std::vector<Item_optimizer::Sim_result_t> keepers;
    keepers.reserve(item_optimizer.total_combinations);
    for (size_t j = 0; j<item_optimizer.total_combinations; ++j) {
        keepers.emplace_back(j, 0, 0);
    }

    std::cout << "Total combinations: " << keepers.size() << "\n";
    std::cout << "Heuristic filter computing... " << keepers.size() << "\n";
    clock_t start_filter = clock();
    double best_attack_power = 0;
    for (auto& keeper : keepers) {
        Character character = item_optimizer.construct(keeper.index);
        if (can_estimate_hit_effect(character.weapons[0]) &&
                can_estimate_hit_effect(character.weapons[1]) &&
                can_estimate_use_effects(character.use_effects)) {
            keeper.ap_equivalent = get_total_qp_equivalent(character.total_special_stats, character.weapons[0],
                    character.weapons[1], character.use_effects, config.sim_time);
            if (keeper.ap_equivalent>best_attack_power) {
                best_attack_power = keeper.ap_equivalent;
            }
        }
    }

    std::cout << "Best equivalent attack power: " << best_attack_power << "\n";
    std::cout << "Filtering sets below attack power: " << 0.95*best_attack_power << "\n";

    std::vector<Item_optimizer::Sim_result_t> temp_keepers;
    temp_keepers.reserve(keepers.size());
    for (const auto& keeper : keepers) {
        if (keeper.ap_equivalent>0.0) {
            if (keeper.ap_equivalent>0.95*best_attack_power) {
                temp_keepers.push_back(keeper);
            }
        }
        else {
            temp_keepers.push_back(keeper);
        }
    }
    keepers = temp_keepers;

    double time_spent_filter = double(clock()-start_filter)/(double) CLOCKS_PER_SEC;
    std::cout << "Combinations after filtering: " << keepers.size() << "\n";
    std::cout << "Time spent filtering: " << time_spent_filter << "\n";
    std::cout << "Starting optimizer: " << keepers.size() << "\n";

    std::vector<size_t> batches_per_iteration = {20, 50, 100, 1000, 10000};
    size_t n_sim{};
    for (size_t i = 0; i<batches_per_iteration.size(); i++) {
        clock_t startTime = clock();
        std::cout << "Iteration " << i+1 << " of " << batches_per_iteration.size() << "\n";
        std::cout << "total_keepers: " << keepers.size() << "\n";

        double best_dps = 0;
        double best_dps_std = 0;
        size_t iter = 0;
        for (auto& keeper : keepers) {
            Character character = item_optimizer.construct(keeper.index);
            simulator.simulate(character, batches_per_iteration[i]);
            keeper.mean_dps = simulator.get_dps_mean();
            auto std_dps = std::sqrt(simulator.get_dps_variance());
            keeper.sample_std_dps = Statistics::sample_deviation(std_dps, batches_per_iteration[i]);
            if (keeper.mean_dps>best_dps) {
                best_dps = keeper.mean_dps;
                best_dps_std = keeper.sample_std_dps;
            }

            // Time taken
            n_sim += batches_per_iteration[i];
            iter++;
            if (keepers.size()<200) {
                if (iter==2) {
                    double time_spent = double(clock()-startTime)/(double) CLOCKS_PER_SEC;
                    double n_samples = keepers.size()/double(iter);
                    std::cout << "Batch done in: " << time_spent*n_samples << " seconds.";
                }
            }
            else {
                if (iter==20) {
                    double time_spent = double(clock()-startTime)/(double) CLOCKS_PER_SEC;
                    double n_samples = keepers.size()/double(iter);
                    std::cout << "Batch done in: " << time_spent*n_samples << " seconds.\n";
                }
            }
        }

        double quantile = find_cdf_quantile(1-1/static_cast<double>(keepers.size()*sqrt(keepers.size())), 0.01);
        double filter_value = best_dps-quantile*best_dps_std;
        std::cout << "Best combination DPS: " << best_dps << ", removing sets below: "
                  << best_dps-quantile*best_dps_std << "\n";

        std::vector<Item_optimizer::Sim_result_t> temp_keepers;
        temp_keepers.reserve(keepers.size());
        for (const auto& keeper : keepers) {
            if (keeper.mean_dps>=filter_value) {
                temp_keepers.emplace_back(keeper);
            }
        }
        keepers = temp_keepers;
        if (keepers.size()==1) {
            break;
        }
    }

    std::string message = +"<b>Total number of simulations performed: </b>"+std::to_string(n_sim)+"<br>";
    message += "<b>Best item set/sets <u>sorted</u> by DPS:</b><br>";
    message += "<b>Item formatting</b>: <br>[Item uccurances / total sets] - socket - item_name<br>";
    message += "(higher uccurance = 'Better' item)<br><br>";

    std::sort(keepers.begin(), keepers.end());
    std::reverse(keepers.begin(), keepers.end());

    std::vector<Character> best_characters;
    best_characters.reserve(keepers.size());
    for (const auto& keeper: keepers) {
        best_characters.emplace_back(item_optimizer.construct(keeper.index));
    }
    std::vector<std::vector<Item_popularity>> item_popularity;
    item_popularity.reserve(best_characters.size());
    for (size_t j = 0; j<best_characters[0].armor.size(); j++) {
        Item_popularity a{"none", 0};
        item_popularity.push_back({a});
    }
    std::vector<std::vector<Item_popularity>> weapon_popularity;
    weapon_popularity.reserve(best_characters.size());
    for (size_t j = 0; j<2; j++) {
        Item_popularity a{"none", 0};
        weapon_popularity.push_back({a});
    }
    for (auto& best_character : best_characters) {
        for (size_t j = 0; j<best_character.armor.size(); j++) {
            bool found = false;
            for (auto& item : item_popularity[j]) {
                if (item.name==best_character.armor[j].name) {
                    item.counter++;
                    found = true;
                    break;
                }
            }
            if (!found) {
                Item_popularity a{best_character.armor[j].name, 1};
                item_popularity[j].push_back(a);
            }
        }
        for (size_t j = 0; j<2; j++) {
            bool found = false;
            for (auto& item : weapon_popularity[j]) {
                if (item.name==best_character.weapons[j].name) {
                    item.counter++;
                    found = true;
                    break;
                }
            }
            if (!found) {
                Item_popularity a{best_character.weapons[j].name, 1};
                weapon_popularity[j].push_back(a);
            }
        }
    }

    std::vector<std::string> socket_names = {"Helmet", "Neck", "Shoulder", "Back", "Chest", "Bracers", "Hands", "Belt",
                                             "Legs", "Boots", "Ranged", "Ring 1", "Ring 2", "Trinket 1", "Trinket 2"};

    std::vector<std::string> weapon_names = {"Main-hand", "Off-hand"};

    for (size_t i = 0; i<best_characters.size(); i++) {
        message += "<b>Set "+std::to_string(i+1)+":</b>\n";
        message += "DPS: "+string_with_precision(keepers[i].mean_dps, 5)+"\n";
        message += "Error margin: "+string_with_precision(keepers[i].sample_std_dps, 3)+"\n";
        message += "<b>Stats:</b>\n";
        message += "Hit: "+string_with_precision(best_characters[i].total_special_stats.hit, 3)+" %\n";
        message += "Crit: "+string_with_precision(best_characters[i].total_special_stats.critical_strike,
                3)+" %\n";
        message += "Attackpower: "+string_with_precision(
                best_characters[i].total_special_stats.attack_power, 4)+" \n";
        message += "Equivalent attackpower: "+string_with_precision(
                keepers[i].ap_equivalent, 4)+" \n";
        message += "<b>Armor:</b>\n";
        for (size_t j = 0; j<best_characters[i].armor.size(); j++) {
            auto item_pop = (*std::find(item_popularity[j].begin(), item_popularity[j].end(),
                    best_characters[i].armor[j].name));
            message += "["+std::to_string(item_pop.counter)+"/"+std::to_string(best_characters.size())+"] ";
            message += "<b>"+socket_names[j]+"</b>"+" - "+best_characters[i].armor[j].name+"\n";
        }
        message += "<b>Weapons:</b>\n";
        for (size_t j = 0; j<2; j++) {
            auto item_pop = (*std::find(weapon_popularity[j].begin(), weapon_popularity[j].end(),
                    best_characters[i].weapons[j].name));
            message += "["+std::to_string(item_pop.counter)+"/"+std::to_string(best_characters.size())+"] ";
            message += "<b>"+weapon_names[j]+"</b>"+" - "+best_characters[i].weapons[j].name+"\n";
        }
        message += "\n";
    }

    return {{message}};
}

////    std::vector<Item_optimizer::Sim_result_t> keepers;
////    keepers.reserve(item_optimizer.total_combinations);
////    for (size_t j = 0; j<item_optimizer.total_combinations; ++j) {
////        keepers.emplace_back(j, 0, 0);
////    }
//
//std::cout << "Total combinations: " << item_optimizer.total_combinations << "\n";
//std::cout << "Heuristic filter computing... " << "\n";
//clock_t start_filter = clock();
//double best_attack_power = 0;
//for (size_t i = 0; i<item_optimizer.total_combinations; i++) {
//Character character = item_optimizer.construct(i);
//if (can_estimate_hiteffect(character.weapons[0]) &&
//can_estimate_hiteffect(character.weapons[1]) &&
//can_estimate_use_effects(character.use_effects)) {
//double ap_equivalent = get_total_qp_equivalent(character.total_special_stats, character.weapons[0],
//        character.weapons[1], character.use_effects, config.sim_time);
//if (ap_equivalent>best_attack_power) {
//best_attack_power = ap_equivalent;
//}
//}
//}
//
//std::cout << "Best equivalent attack power: " << best_attack_power << "\n";
//std::cout << "Filtering sets below attack power: " << 0.95*best_attack_power << "\n";
//
//std::vector<Item_optimizer::Sim_result_t> keepers;
//keepers.reserve(item_optimizer.total_combinations/10);
//for (size_t i = 0; i<item_optimizer.total_combinations; i++) {
//Character character = item_optimizer.construct(i);
//if (can_estimate_hiteffect(character.weapons[0]) &&
//can_estimate_hiteffect(character.weapons[1]) &&
//can_estimate_use_effects(character.use_effects)) {
//double ap_equivalent = get_total_qp_equivalent(character.total_special_stats, character.weapons[0],
//        character.weapons[1], character.use_effects, config.sim_time);
//if (ap_equivalent>0.95*best_attack_power) {
//keepers.emplace_back(i, 0, 0);
//}
//}
//else {
//keepers.emplace_back(i, 0, 0);
//}
//}
////    keepers = temp_keepers;