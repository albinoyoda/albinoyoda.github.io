import os


def remove_blank_spaces(string_list):
    washed_list = []
    for elem in string_list:
        if elem != '':
            washed_list.append(elem)
    return washed_list


def remove_underscores_and_capitalize(string):
    split_string = string.split("_")
    output = split_string[0].capitalize()
    for i in range(1, len(split_string)):
        output = output + " " + split_string[i]
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


file = "/../wow_library/include/Armory.hpp"
path = os.getcwd() + file
file1 = open(path, "r")
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

with open("index.html", "r") as f:
    lines = f.readlines()

for armors in armor_types:
    armors.items = list(set(armors.items))
    armors.items.sort()
# all_weapons.sort()

washed_lines = []
copy_line = True
armor_index = 0
found_start = False
found_stop = False
for index, line in enumerate(lines):
    l = line
    if copy_line:
        washed_lines.append(line)
    if line == "    <!--armor begin-->\n":
        found_start = True
        armor_index = index
        copy_line = False
    if line == "    <!--armor stop-->\n":
        found_stop = True
        washed_lines.append(line)
        copy_line = True

dropdowns_in_a_row = 0
extra_stuff = ' size="10" class="item-select" multiple'
if found_start and found_stop:
    first_part = washed_lines[0:armor_index + 1]
    second_part = washed_lines[armor_index + 1:]

    # Armor selection
    generated_1 = []
    for armor_type in armor_types:
        if armor_type.category == 'ring' or armor_type.category == 'trinket':
            for i in range(2):
                generated_1.append('<select id="' + armor_type.category + str(i + 1) + '_dd"' + extra_stuff + '>\n')
                generated_1.append(
                    '    <option value="none" selected disabled>' + armor_type.category + str(i + 1) + '</option>\n')
                for item in armor_type.items:
                    item_name = remove_underscores_and_capitalize(item)
                    generated_1.append('    <option value="' + item + '">' + item_name + '</option>\n')
                generated_1.append('</select>\n')
                generated_1.append('\n')
                dropdowns_in_a_row = dropdowns_in_a_row + 1
        else:
            generated_1.append('<select id="' + armor_type.category + '_dd"' + extra_stuff + '>\n')
            generated_1.append('    <option value="none" selected disabled>' + armor_type.category + '</option>\n')
            for item in armor_type.items:
                item_name = remove_underscores_and_capitalize(item)
                generated_1.append('    <option value="' + item + '">' + item_name + '</option>\n')
            generated_1.append('</select>\n')
            generated_1.append('\n')
            dropdowns_in_a_row = dropdowns_in_a_row + 1

        if dropdowns_in_a_row >= 3:
            generated_1.append('<p></p>\n')
            generated_1.append('\n')
            dropdowns_in_a_row = 0

    # Weapon selection
    alternatives = ["main_hand", "off_hand"]
    generated_2 = ['<b> Select weapons:</b><br/>\n']
    for i in range(2):
        generated_2.append('<select id="' + alternatives[i] + '_dd" >\n')
        generated_2.append('    <option value="none" selected disabled>' + alternatives[i] + '</option>\n')
        for wep_type in weapons:
            generated_2.append(
                '    <option value="none" disabled> --- ' + wep_type.category.upper() + ' --- </option>\n')
            for item in wep_type.items:
                split_item = item.split("*")
                if i == 0:
                    if split_item[1] == "one" or split_item[1] == "mai":
                        item_name = remove_underscores_and_capitalize(split_item[0])
                        generated_2.append('    <option value="' + split_item[0] + '">' + item_name + '</option>\n')
                if i == 1:
                    if split_item[1] == "one" or split_item[1] == "off":
                        item_name = remove_underscores_and_capitalize(split_item[0])
                        generated_2.append('    <option value="' + split_item[0] + '">' + item_name + '</option>\n')
        generated_2.append('</select>\n')
        generated_2.append('\n')

    file1 = open("index.html", "w")
    for line in first_part:
        file1.write(line)
    for line in generated_1:
        file1.write(line)
    for line in generated_2:
        file1.write(line)
    for line in second_part:
        file1.write(line)
    file1.close()
else:
    print("WARNING: no start of stop found")
