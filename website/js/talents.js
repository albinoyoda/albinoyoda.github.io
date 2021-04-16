let talents_vec = [
    "improved_heroic_strike_talent",
    "deflection_talent",
    "improved_rend_talent",
    "improved_charge_talent",
    "tactical_mastery_talent",
    "improved_thunder_clap_talent",
    "improved_overpower_talent",
    "anger_management_talent",
    "deep_wounds_talent",
    "two_handed_weapon_specialization_talent",
    "impale_talent",
    "axe_specialization_talent",
    "sweeping_strikes_talent",
    "mace_specialization_talent",
    "sword_specialization_talent",
    "polearm_specialization_talent",
    "improved_hamstring_talent",
    "mortal_strike_talent",
    "booming_voice_talent",
    "cruelty_talent",
    "improved_demoralizing_shout_talent",
    "unbridled_wrath_talent",
    "improved_cleave_talent",
    "peircing_howl_talent",
    "blood_craze_talent",
    "improved_battle_shout_talent",
    "dual_wield_specialization_talent",
    "improved_execute_talent",
    "enrage_talent",
    "improved_slam_talent",
    "death_wish_talent",
    "improved_intercept_talent",
    "improved_berserker_rage_talent",
    "flurry_talent",
    "bloodthirst_talent",
    "shield_specialization_talent",
    "anticipation_talent",
    "improved_bloodrage_talent",
    "toughness_talent",
    "iron_will_talent",
    "last_stand_talent",
    "improved_shield_block_talent",
    "improved_revenge_talent",
    "defiance_talent",
    "improved_sunder_armor_talent",
    "improved_disarm_talent",
    "improved_taunt_talent",
    "improved_shield_wall_talent",
    "concussion_blow_talent",
    "improved_shield_bash_talent",
    "one_handed_weapon_specialization_talent",
    "shield_slam_talent"];

let not_implemented_talents = [
    "improved_berserker_rage_talent",
    "enrage_talent",
    "deflection_talent",
    "improved_rend_talent",
    "improved_charge_talent",
    "improved_thunder_clap_talent",
    "improved_hamstring_talent",
    "improved_demoralizing_shout_talent",
    "peircing_howl_talent",
    "blood_craze_talent",
    "improved_intercept_talent",
    "shield_specialization_talent",
    "anticipation_talent",
    "improved_bloodrage_talent",
    "toughness_talent",
    "iron_will_talent",
    "last_stand_talent",
    "improved_shield_block_talent",
    "improved_revenge_talent",
    "defiance_talent",
    "improved_sunder_armor_talent",
    "improved_disarm_talent",
    "improved_taunt_talent",
    "improved_shield_wall_talent",
    "concussion_blow_talent",
    "improved_shield_bash_talent",
    "shield_slam_talent"];

function load_talent_clear() {
    for (let i = 0; i < talents_vec.length; i++) {
        document.getElementById(talents_vec[i]).setAttribute("data-count", "0");
    }
    displayTalentBootup();
    calcTotalTalents();
}

function load_talent_standard_fury() {
    load_talent_clear();
    document.getElementById("improved_heroic_strike_talent").setAttribute("data-count", "3");
    document.getElementById("improved_rend_talent").setAttribute("data-count", "3");
    document.getElementById("tactical_mastery_talent").setAttribute("data-count", "5");
    document.getElementById("anger_management_talent").setAttribute("data-count", "1");
    document.getElementById("deep_wounds_talent").setAttribute("data-count", "3");
    document.getElementById("impale_talent").setAttribute("data-count", "2");
    document.getElementById("cruelty_talent").setAttribute("data-count", "5");
    document.getElementById("unbridled_wrath_talent").setAttribute("data-count", "5");
    document.getElementById("improved_battle_shout_talent").setAttribute("data-count", "5");
    document.getElementById("dual_wield_specialization_talent").setAttribute("data-count", "5");
    document.getElementById("improved_execute_talent").setAttribute("data-count", "2");
    document.getElementById("enrage_talent").setAttribute("data-count", "5");
    document.getElementById("death_wish_talent").setAttribute("data-count", "1");
    document.getElementById("flurry_talent").setAttribute("data-count", "5");
    document.getElementById("bloodthirst_talent").setAttribute("data-count", "1");

    displayTalentBootup();
    calcTotalTalents();
}

function load_talent_cleave_fury() {
    load_talent_standard_fury();
    document.getElementById("improved_battle_shout_talent").setAttribute("data-count", "3");
    document.getElementById("improved_heroic_strike_talent").setAttribute("data-count", "2");
    document.getElementById("improved_cleave_talent").setAttribute("data-count", "3");

    displayTalentBootup();
    calcTotalTalents();
}

function load_talent_two_hand_fury() {
    load_talent_standard_fury();
    document.getElementById("improved_heroic_strike_talent").setAttribute("data-count", "2");
    document.getElementById("two_handed_weapon_specialization_talent").setAttribute("data-count", "3");

    document.getElementById("dual_wield_specialization_talent").setAttribute("data-count", "0");
    document.getElementById("improved_execute_talent").setAttribute("data-count", "0");
    document.getElementById("improved_slam_talent").setAttribute("data-count", "4");
    document.getElementById("improved_overpower_talent").setAttribute("data-count", "1");

    displayTalentBootup();
    calcTotalTalents();
}

function recursiveChildCall(node, n_talent_points) {
    for (let i = 0; i < node.childNodes.length; i++) {
        let child = node.childNodes[i];
        if (child.className === "icon") {
            n_talent_points[0] += parseFloat(child.getAttribute("data-count"));
        }
        recursiveChildCall(child, n_talent_points);
    }
}

function calcTotalTalents() {
    let talent_trees = ["Arms", "Fury", "Prot"];
    let total_points = 0;
    for (let tree of talent_trees) {
        let tree_name = tree.toLowerCase();
        let arms_node = document.getElementsByClassName(tree_name)[0];
        let n_talent_points = [0];
        recursiveChildCall(arms_node, n_talent_points);
        total_points += n_talent_points[0];
        let title_elem = document.getElementById(tree_name + "_title");
        title_elem.innerHTML = tree + " (" + n_talent_points[0].toString() + ")"
    }
    let total_points_elem = document.getElementById("total_points");
    total_points_elem.innerHTML = "Total points: " + total_points.toString() + "/51";
    showHideTalentDiv();
}

function showHideTalentDiv() {
    let ms_talent = document.getElementById("mortal_strike_talent");
    let current_val = parseInt(ms_talent.getAttribute("data-count"));
    if (current_val === 1) {
        document.getElementById("mortal_strike_talent_div").style.display = "block";
    } else {
        document.getElementById("mortal_strike_talent_div").style.display = "none";
    }

    let bt_talent = document.getElementById("bloodthirst_talent");
    current_val = parseInt(bt_talent.getAttribute("data-count"));
    if (current_val === 1) {
        document.getElementById("bloodthirst_talent_div").style.display = "block";
    } else {
        document.getElementById("bloodthirst_talent_div").style.display = "none";
    }

    let ss_talent = document.getElementById("sweeping_strikes_talent");
    current_val = parseInt(ss_talent.getAttribute("data-count"));
    if (current_val === 1) {
        document.getElementById("sweeping_strikes_talent_div").style.display = "block";
    } else {
        document.getElementById("sweeping_strikes_talent_div").style.display = "none";
    }
}

function displayTalentBootup() {
    let icon_vec = document.getElementsByClassName("icon");
    for (let i = 0; i < icon_vec.length; i++) {
        let talent_button = icon_vec[i];
        let current_val = parseInt(talent_button.getAttribute("data-count"));
        let max_val = parseInt(talent_button.getAttribute("data-max"));
        let show_string = current_val.toString() + '/' + max_val.toString();
        if (current_val === 0) {
            show_string = "";
        }
        talent_button.setAttribute("data-disp", show_string);
        talent_button.setAttribute("onclick", "IncrementTalent(this.id);");
        drawTalentBorder(talent_button);
    }
}

function IncrementTalent(talent_name) {
    let talent_button = document.getElementById(talent_name);
    let next_val = parseFloat(talent_button.getAttribute("data-count")) + 1;
    let max_val = parseFloat(talent_button.getAttribute("data-max"));
    if (next_val > max_val) {
        next_val = 0
    }
    talent_button.setAttribute("data-count", next_val);
    let show_string = next_val.toString() + '/' + max_val.toString();
    if (next_val === 0) {
        show_string = "";
    }
    talent_button.setAttribute("data-disp", show_string);
    calcTotalTalents();
    drawTalentBorder(talent_button);
}

function drawTalentBorder(talent_button) {
    let val = parseInt(talent_button.getAttribute("data-count"));
    if (val === 0) {
        talent_button.style.border = "3px solid #000000";
    } else {
        talent_button.style.border = "3px solid #75ff78";
    }
    for (let talent_name of not_implemented_talents) {
        let talent_button = document.getElementById(talent_name);
        talent_button.style.border = "3px dashed #000000";
    }
}