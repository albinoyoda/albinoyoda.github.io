import os


def remove_blank_spaces(string_list):
    washed_list = []
    for elem in string_list:
        if elem != '':
            washed_list.append(elem)
    return washed_list


def remove_underscores_and_capitalize(string):
    split_string = string.split("_")
    output = ""
    for i, word in enumerate(split_string):
        if (i == 0) or ((word != 'of') and (word != 'the')):
            output += " " + word.capitalize()
        else:
            output += " " + word
    return output


def is_string_left_curly_bracket(string):
    split_string = string.split(' ')
    washed_string = remove_blank_spaces(split_string)
    return washed_string[0] == "{"


class Armor_category:
    def __init__(self, category):
        self.category = category
        self.items = []

    def add_item(self, item):
        self.items.append(item)


class Weapon_category:
    def __init__(self, category):
        self.category = category
        self.items = []

    def add_item(self, wep_name, wep_hand):
        self.items.append(wep_name + "*" + wep_hand)


armory_location = os.path.abspath(
    os.path.join(os.path.dirname(__file__), '..', '..', 'wow_library', 'include', 'Armory.hpp'))
index_html_location = os.path.abspath(
    os.path.join(os.path.dirname(__file__), '..', 'index.html'))
file1 = open(armory_location, "r")
lines = file1.readlines()
file1.close()

armor_types = []
found_item_vector = False
for line in lines:
    line1 = line.split(' ')
    line1 = remove_blank_spaces(line1)
    if line1[0] == 'std::vector<Armor>':
        if line1[1][-3:-1] == '_t':
            armor = Armor_category(line1[1][0:-3])
            found_item_vector = True

    if found_item_vector:
        line2 = line.split('"')
        if len(line2) == 3:
            if is_string_left_curly_bracket(line2[0]):
                armor.add_item(line2[1])

    if line1[0] == '};\n' and found_item_vector:
        found_item_vector = False
        armor_types.append(armor)

weapons = []
found_item_vector = False
found_item_socket = False
found_item_name = False
for line in lines:
    line1 = line.split(' ')
    line1 = remove_blank_spaces(line1)
    if line1[0] == 'std::vector<Weapon>':
        if line1[1][-3:-1] == '_t':
            weapon = Weapon_category(line1[1][0:-3])
            found_item_vector = True

    if found_item_vector:
        line2 = line.split('"')
        if len(line2) == 3:
            if is_string_left_curly_bracket(line2[0]):
                weapon_name = line2[1]
                found_item_name = True

        line3 = line.split('Weapon_socket')
        if len(line3) == 2:
            weapon_hand = line3[1][2:5]
            found_item_socket = True

    if found_item_name and found_item_socket:
        weapon.add_item(weapon_name, weapon_hand)
        found_item_socket = False
        found_item_name = False

    if line1[0] == '};\n' and found_item_vector:
        found_item_vector = False
        weapons.append(weapon)

# all_weapons = []
for wep_type in weapons:
    wep_type.items = list(set(wep_type.items))
    wep_type.items.sort()

with open(index_html_location, "r") as f:
    lines = f.readlines()

for armors in armor_types:
    armors.items = list(set(armors.items))
    armors.items.sort()
# all_weapons.sort()

copy_line_chunks = []
copy_lines = []
copy_line = True
string_begin = "<!--armor begin{}-->"
string_stop = "<!--armor stop{}-->"
n_found = 0
start_indexes = []
stop_indexes = []
for index, line in enumerate(lines):
    if copy_line:
        copy_lines.append(line)
    for i in range(5):
        if line.find(string_begin.format(i)) != -1:
            start_indexes.append(index)
            copy_line = False
            copy_line_chunks.append(copy_lines)
        if line.find(string_stop.format(i)) != -1:
            stop_indexes.append(index)
            copy_lines = [line]
            copy_line = True
copy_line_chunks.append(copy_lines)

if (len(start_indexes) != 4) or (len(stop_indexes) != 4):
    print("Did not find all begin and stop indexes. aborting!")
    exit()

dropdowns_in_a_row = 0
extra_stuff_armor = '" class="item-select"'
extra_stuff_armor_mult = '_mult" size="10" class="item-select" multiple'
extra_stuff_weapon = '" class="weapon-select"'
extra_stuff_weapon_mult = '_mult" size="10" class="item-select" multiple'
extra_stuff_weapon_mult_two_hand = '_mult" size="10" class="weapon-select" multiple'

div_title_string = \
    '<span class="left_select">{}</span><span class="middle_select">{}</span><span class="right_select">{}</span><br>\n'
div_title_string_double = '<span class="left_select">{}</span><span class="middle_select">{}</span><br>\n'

armor_names = []
for armor_type in armor_types:
    if armor_type.category == 'ring' or armor_type.category == 'trinket':
        armor_names.append(armor_type.category + "1")
        armor_names.append(armor_type.category + "2")
    else:
        armor_names.append(armor_type.category)

armor_names_mult = []
for armor_type in armor_types:
    if armor_type.category == 'ring' or armor_type.category == 'trinket':
        armor_names_mult.append((armor_type.category + "s").capitalize())
    else:
        armor_names_mult.append(armor_type.category.capitalize())
armor_names_mult.append("Weapons")
armor_names_mult.append("Two-Handed Weapons")

# ------------------------ STANDARD -----------------------
# ------------------------ ARMOR -----------------------
tot_generated = []
generated = []
index = 0
for armor_type in armor_types:
    if index % 3 == 0:
        generated.append(div_title_string.format(armor_names[index], armor_names[index + 1],
                                                 armor_names[index + 2]))
    index = index + 1
    if armor_type.category == 'ring' or armor_type.category == 'trinket':
        index = index + 1
        for i in range(2):
            generated.append(
                '<select id="' + armor_type.category + str(i + 1) + '_dd' + extra_stuff_armor + '>\n')
            generated.append('    <option value="none" selected disabled></option>\n')
            for item in armor_type.items:
                item_name = remove_underscores_and_capitalize(item)
                generated.append('    <option value="' + item + '">' + item_name + '</option>\n')
            generated.append('</select>\n')
            generated.append('\n')
            dropdowns_in_a_row = dropdowns_in_a_row + 1
    else:
        generated.append('<select id="' + armor_type.category + '_dd' + extra_stuff_armor + '>\n')
        generated.append('    <option value="none" selected disabled></option>\n')
        for item in armor_type.items:
            item_name = remove_underscores_and_capitalize(item)
            generated.append('    <option value="' + item + '">' + item_name + '</option>\n')
        generated.append('</select>\n')
        generated.append('\n')
        dropdowns_in_a_row = dropdowns_in_a_row + 1

    if dropdowns_in_a_row >= 3:
        generated.append('<p></p>\n')
        generated.append('\n')
        dropdowns_in_a_row = 0

tot_generated.append(generated)

# ------------------------ WEAPON -----------------------
alternatives = ["main_hand", "off_hand"]
generated = []
for i in range(2):
    generated.append('<select id="' + alternatives[i] + '_dd' + extra_stuff_weapon + '>\n')
    generated.append('    <option value="none" selected disabled></option>\n')
    for wep_type in weapons:
        if wep_type.category[0:3] == 'two':
            continue
        generated.append('    <option value="none" disabled> --- ' + wep_type.category.upper() + ' --- </option>\n')
        for item in wep_type.items:
            split_item = item.split("*")
            if i == 0:
                if split_item[1] == "one" or split_item[1] == "mai":
                    item_name = remove_underscores_and_capitalize(split_item[0])
                    generated.append('    <option value="' + split_item[0] + '">' + item_name + '</option>\n')
            if i == 1:
                if split_item[1] == "one" or split_item[1] == "off":
                    item_name = remove_underscores_and_capitalize(split_item[0])
                    generated.append('    <option value="' + split_item[0] + '">' + item_name + '</option>\n')
    generated.append('</select>\n')
tot_generated.append(generated)

alternative = "two_hand"
generated = []
generated.append('<select id="' + alternative + '_dd' + extra_stuff_weapon + '>\n')
generated.append('    <option value="none" selected disabled></option>\n')
for wep_type in weapons:
    if wep_type.category[0:3] != 'two':
        continue
    generated.append('    <option value="none" disabled> --- ' + wep_type.category.upper() + ' --- </option>\n')
    for item in wep_type.items:
        split_item = item.split("*")
        item_name = remove_underscores_and_capitalize(split_item[0])
        generated.append('    <option value="' + split_item[0] + '">' + item_name + '</option>\n')

generated.append('</select>\n')
tot_generated.append(generated)

# ------------------------ MULTI -----------------------
# ------------------------ ARMOR -----------------------
generated = []
index = 0
for armor_type in armor_types:
    if index % 3 == 0:
        generated.append(div_title_string.format(armor_names_mult[index], armor_names_mult[index + 1],
                                                 armor_names_mult[index + 2]))
    index = index + 1
    generated.append('<select id="' + armor_type.category + '_dd' + extra_stuff_armor_mult + '>\n')
    for item in armor_type.items:
        item_name = remove_underscores_and_capitalize(item)
        generated.append('    <option value="' + item + '">' + item_name + '</option>\n')
    generated.append('</select>\n')
    generated.append('\n')
    dropdowns_in_a_row = dropdowns_in_a_row + 1

    if dropdowns_in_a_row >= 3:
        generated.append('<p></p>\n')
        generated.append('\n')
        dropdowns_in_a_row = 0
tot_generated.append(generated)

# ------------------------ WEAPON -----------------------
generated = []
generated.append('<select id="weapons_dd' + extra_stuff_weapon_mult + '>\n')
for wep_type in weapons:
    if wep_type.category[0:3] == 'two':
        continue
    generated.append('    <option value="none" disabled> --- ' + wep_type.category.upper() + ' --- </option>\n')
    for item in wep_type.items:
        split_item = item.split("*")
        item_name = remove_underscores_and_capitalize(split_item[0])
        generated.append('    <option value="' + split_item[0] + '">' + item_name + '</option>\n')
generated.append('</select>\n')
tot_generated.append(generated)

generated.append('<select id="two_hand_weapons_dd' + extra_stuff_weapon_mult_two_hand + '>\n')
for wep_type in weapons:
    if wep_type.category[0:3] != 'two':
        continue
    generated.append('    <option value="none" disabled> --- ' + wep_type.category.upper() + ' --- </option>\n')
    for item in wep_type.items:
        split_item = item.split("*")
        item_name = remove_underscores_and_capitalize(split_item[0])
        generated.append('    <option value="' + split_item[0] + '">' + item_name + '</option>\n')
generated.append('</select>\n')
tot_generated.append(generated)

file1 = open(index_html_location, "w")
all_lines = copy_line_chunks[0] + tot_generated[0] + copy_line_chunks[1] + tot_generated[1] + copy_line_chunks[2] + \
            tot_generated[2] + copy_line_chunks[3] + tot_generated[3] + tot_generated[4] + copy_line_chunks[4]
for line in all_lines:
    file1.write(line)

file1.close()
