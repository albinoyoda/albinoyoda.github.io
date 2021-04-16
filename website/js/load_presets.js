function selectElement(id, valueToSelect) {
    if (id !== "" && id !== "undefined") {
        if (id.slice(id.length - 2, id.length) === "dd" ||
            id.slice(id.length - 4, id.length) === "mult") {
            try {
                document.getElementById(id).value = valueToSelect;
            } catch (err) {
                console.log(err);
            }
        } else if (id.slice(id.length - 6, id.length) === "talent") {
            document.getElementById(id).setAttribute("data-count", valueToSelect)
        } else if (valueToSelect === "true" || valueToSelect === "false") {
            console.log("asd");
            let all_dropdowns = armor_mult.concat(weapons_mult);
            for (let dropdown of all_dropdowns) {
                let select_menu = document.getElementById(dropdown);
                for (let i = 0; i < select_menu.length; i++) {
                    let item = select_menu[i];
                    if (item.value === id) {
                        (valueToSelect === "true") ? item.selected = true : item.selected = false;
                    }
                }
            }
        } else {
            if (id === valueToSelect) {
                try {
                    document.getElementById(id).checked = true;
                } catch (err) {
                    console.log(err);
                }
            }
        }
    }
}

function select_prebis(selected_items, selected_weapons, selected_enchants) {
    for (let i = 0; i < armor.length; i++) {
        selectElement(armor[i], selected_items[i])
    }
    for (let i = 0; i < weapons.length; i++) {
        selectElement(weapons[i], selected_weapons[i])
    }
    for (let i = 0; i < enchants.length; i++) {
        selectElement(enchants[i], selected_enchants[i])
    }
}

function select_prebis_mult(selected_items, selected_weapons, selected_enchants) {
    for (let option_name of armor_mult) {
        console.log(option_name);
        let select_option = document.getElementById(option_name);
        for (let option of select_option.options) {
            for (let item of selected_items) {
                if (option.value === item) {
                    option.selected = true;
                    break;
                } else {
                    option.selected = false;
                }
            }
        }
    }

    let select_option = document.getElementById(weapons_mult[0]);
    console.log(select_option.options.length);
    for (let option of select_option.options) {
        for (let item of selected_weapons) {
            if (option.value === item) {
                option.selected = true;
                break;
            } else {
                option.selected = false;
            }
        }
    }
    for (let i = 0; i < enchants.length; i++) {
        selectElement(enchants[i], selected_enchants[i])
    }
}


function load_p1prebisitems() {
    let selected_items = ["lionheart_helm", "eye_of_rend", "helm_of_valor", "mark_of_fordring", "imperial_jewel",
        "spaulders_of_valor", "truestrike_shoulders", "black_dragonscale_shoulders", "cape_of_the_black_baron", "blackveil_cloak",
        "black_dragonscale_chest", "savage_gladiator_chain", "vambraces_of_the_sadist", "battleborn_armbraces",
        "reiver_claws", "devilsaur_gauntlets", "brigam_girdle", "omokks_girth_restrainer", "eldritch_legplates",
        "devilsaur_leggings", "cloudkeeper_legplates", "black_dragonscale_leggings",
        "battlechasers_greaves", "black_dragonscale_boots", "painweaver_band",
        "blackstone_ring", "magnis_will", "tarnished_elven_ring",
        "blackhands_breadth", "hand_of_justice", "satyrs_bow"];
    let selected_weapons = ["dal_rends_sacred_charge", "dal_rends_tribal_guardian", "ironfoe", "thrash_blade", "mirahs_song"];
    let selected_enchants = ["+8 strength", "+30 attack_power", "+3 agility", "+4 stats", "+9 strength",
        "+7 strength", "+8 strength", "+7 agility", "crusader", "crusader"];
    select_prebis_mult(selected_items, selected_weapons, selected_enchants);
}

function load_p6pvebismult() {
    let selected_items = ["lionheart_helm", "stormrages_talisman_of_seething", "conquerors_spaulders",
        "shroud_of_dominion", "plated_abomination_ribcage", "hive_defiler_wristguards",
        "gauntlets_of_annihilation", "girdle_of_the_mentor", "leggings_of_carnage",
        "chromatic_boots", "quick_strike_ring", "band_of_unnatural_forces",
        "kiss_of_the_spider", "badge_of_the_swarmguard", "nerubian_slavemaker"];
    let selected_weapons = ["the_hungering_cold", "iblis_blade_of_the_fallen_seraph"];
    let selected_enchants = ["+8 strength", "naxxramas", "+3 agility", "+4 stats", "+9 strength",
        "+15 agility", "+8 strength", "+7 agility", "crusader", "crusader"];
    select_prebis_mult(selected_items, selected_weapons, selected_enchants);
}

function load_p5pvebismult() {
    let selected_items = ["lionheart_helm", "barbed_choker", "mantle_of_wicked_revenge",
        "cloak_of_the_fallen_god", "breastplate_of_annihilation", "hive_defiler_wristguards",
        "gauntlets_of_annihilation", "onslaught_girdle", "conquerors_legguards",
        "chromatic_boots", "quick_strike_ring", "circle_of_applied_force",
        "jom_gabbar", "badge_of_the_swarmguard", "crossbow_of_imminent_doom"];
    let selected_weapons = ["chromatically_tempered_sword", "maladath"];
    let selected_enchants = ["+8 strength", "+30 attack_power", "+3 agility", "+4 stats", "+9 strength",
        "+7 strength", "+8 strength", "+7 agility", "crusader", "crusader"];
    select_prebis_mult(selected_items, selected_weapons, selected_enchants);
}

function load_p6pvebis() {
    let selected_items = ["lionheart_helm", "stormrages_talisman_of_seething", "conquerors_spaulders",
        "shroud_of_dominion", "plated_abomination_ribcage", "hive_defiler_wristguards",
        "gauntlets_of_annihilation", "girdle_of_the_mentor", "leggings_of_carnage",
        "chromatic_boots", "quick_strike_ring", "band_of_unnatural_forces",
        "kiss_of_the_spider", "badge_of_the_swarmguard", "nerubian_slavemaker"];
    let selected_weapons = ["the_hungering_cold", "iblis_blade_of_the_fallen_seraph", "might_of_menethil"];
    let selected_enchants = ["+8 strength", "naxxramas", "+3 agility", "+4 stats", "+9 strength",
        "+15 agility", "+8 strength", "+7 agility", "crusader", "crusader"];
    select_prebis(selected_items, selected_weapons, selected_enchants);
}

function load_p5pvebis() {
    let selected_items = ["lionheart_helm", "barbed_choker", "mantle_of_wicked_revenge",
        "cloak_of_the_fallen_god", "breastplate_of_annihilation", "hive_defiler_wristguards",
        "gauntlets_of_annihilation", "onslaught_girdle", "conquerors_legguards",
        "chromatic_boots", "quick_strike_ring", "circle_of_applied_force",
        "jom_gabbar", "badge_of_the_swarmguard", "crossbow_of_imminent_doom"];
    let selected_weapons = ["chromatically_tempered_sword", "maladath", "bonereavers_edge"];
    let selected_enchants = ["+8 strength", "+30 attack_power", "+3 agility", "+4 stats", "+9 strength",
        "+7 strength", "+8 strength", "+7 agility", "crusader", "crusader"];
    select_prebis(selected_items, selected_weapons, selected_enchants);
}

function load_p4pvebis() {
    let selected_items = ["lionheart_helm", "the_eye_of_hakkar", "drake_talon_pauldrons",
        "puissant_cape", "savage_gladiator_chain", "wristguards_of_stability",
        "flameguard_gauntlets", "onslaught_girdle", "legguards_of_the_fallen_crusader",
        "chromatic_boots", "quick_strike_ring", "circle_of_applied_force",
        "diamond_flask", "hand_of_justice", "strikers_mark"];
    let selected_weapons = ["chromatically_tempered_sword", "maladath", "bonereavers_edge"];
    let selected_enchants = ["+8 strength", "+30 attack_power", "+3 agility", "+4 stats", "+9 strength",
        "+7 strength", "+8 strength", "+7 agility", "crusader", "crusader"];
    select_prebis(selected_items, selected_weapons, selected_enchants);
}

function load_p3pvebis() {
    let selected_items = ["lionheart_helm", "onyxia_tooth_pendant", "drake_talon_pauldrons",
        "puissant_cape", "savage_gladiator_chain", "wristguards_of_stability",
        "flameguard_gauntlets", "onslaught_girdle", "legguards_of_the_fallen_crusader",
        "chromatic_boots", "quick_strike_ring", "circle_of_applied_force",
        "diamond_flask", "hand_of_justice", "strikers_mark"];
    let selected_weapons = ["chromatically_tempered_sword", "maladath", "bonereavers_edge"];
    let selected_enchants = ["+8 strength", "shoulder", "+3 agility", "+4 stats", "+9 strength",
        "+7 strength", "+8 strength", "+7 agility", "crusader", "crusader"];
    select_prebis(selected_items, selected_weapons, selected_enchants);
}

function load_p1prebis() {
    let selected_items = ["lionheart_helm", "mark_of_fordring", "truestrike_shoulders",
        "cape_of_the_black_baron", "savage_gladiator_chain", "battleborn_armbraces",
        "devilsaur_gauntlets", "omokks_girth_restrainer", "devilsaur_leggings",
        "battlechasers_greaves", "magnis_will", "blackstone_ring",
        "blackhands_breadth", "hand_of_justice", "satyrs_bow"];
    let selected_weapons = ["dal_rends_sacred_charge", "dal_rends_tribal_guardian", "treants_bane"];
    let selected_enchants = ["+8 strength", "shoulder", "+3 agility", "+4 stats", "+9 strength",
        "+7 strength", "+8 strength", "boots", "crusader", "crusader"];
    select_prebis(selected_items, selected_weapons, selected_enchants);
}