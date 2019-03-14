#include "weaponmod.h"

#include <QFile>
#include <QTextStream>

bool WeaponMod::read_file(const QDomDocument &xml_doc, QString &error)
{
    // This Function utilizes the QtXml library to read existing mod xml files.
    // Note: I used a lot of 'this->' for readability. Helps get an idea of scope since a lot of different scopes are used here.

    // This is nightmare fuel.
    // A simple mistake here can either cause fatal problems during runtime or trickle all the way down into the xml save and possibly cause trouble outside of this tool.

    // Get root element from document
    QDomElement root = xml_doc.documentElement();

    // Looking through children of "weapon"
    for (QDomElement child = root.firstChildElement(); !child.isNull(); child = child.nextSiblingElement()) {

        if (child.tagName() == "coreAttributes") {

            // Looking through children of "coreAttributes"
            for (QDomElement element = child.firstChildElement(); !element.isNull(); element = element.nextSiblingElement()) {

                if (element.tagName() == "value") {
                    bool ok;
                    this->value = element.text().toInt(&ok);
                    if (!ok)
                        error.append("Value could not be converted to integer. Will default to 0!\n");
                    element.nextSiblingElement();
                }

                if (element.tagName() == "melee") {
                    if (element.text().toLower() == "true")
                        this->melee = true;
                    else
                        this->melee = false;
                    element.nextSiblingElement();
                }

                if (element.tagName() == "twoHanded") {
                    if (element.text().toLower() == "true")
                        this->two_handed = true;
                    else
                        this->two_handed = false;
                    element.nextSiblingElement();
                }

                if (element.tagName() == "determiner") {
                    this->determiner = element.text();
                    element.nextSiblingElement();
                }

                if (element.tagName() == "name") {
                    this->name = element.text();
                    element.nextSiblingElement();
                }

                if (element.tagName() == "namePlural") {
                    this->name_plural = element.text();
                    if (element.attribute("pluralByDefault").toLower() == "true")
                        this->plural_default = true;
                    else
                        this->plural_default = false;
                    element.nextSiblingElement();
                }

                if (element.tagName() == "description") {
                    this->description = element.text();
                    element.nextSiblingElement();
                }

                if (element.tagName() == "attackDescriptor") {
                    this->attack_descriptor = element.text();
                    element.nextSiblingElement();
                }

                if (element.tagName() == "attackTooltipDescription") {
                    this->attack_tooltip = element.text();
                    element.nextSiblingElement();
                }

                if (element.tagName() == "rarity") {
                    this->rarity = element.text();
                    element.nextSiblingElement();
                }

                if (element.tagName() == "equipText") {
                    this->equip_text = element.text();
                    element.nextSiblingElement();
                }

                if (element.tagName() == "unequipText") {
                    this->unequip_text = element.text();
                    element.nextSiblingElement();
                }

                if (element.tagName() == "imageName") {
                    this->image_name = element.text();
                    element.nextSiblingElement();
                }

                if (element.tagName() == "imageEquippedName") {
                    this->equipped_image_name = element.text();
                    element.nextSiblingElement();
                }

                if (element.tagName() == "damage") {
                    bool ok;
                    this->damage = element.text().toInt(&ok);
                    if (!ok)
                        error.append("Damage could not be converted to integer. Will default to 0!\n");
                    element.nextSiblingElement();
                }

                if (element.tagName() == "arcaneCost") {
                    bool ok;
                    this->arcane_cost = element.text().toInt(&ok);
                    if (!ok)
                        error.append("Arcane cost could not be converted to integer. Will default to 0!\n");
                    element.nextSiblingElement();
                }

                if (element.tagName() == "damageVariance") {
                    this->damage_variance = element.text();
                    element.nextSiblingElement();
                }

                if (element.tagName() == "availableDamageTypes") {
                    iterate_string_list(element, "damageType", this->damage_types);
                    element.nextSiblingElement();
                }

                if (element.tagName() == "spells") {
                    iterate_string_list(element, "spell", this->spells);
                    element.nextSiblingElement();
                }

                if (element.tagName() == "enchantmentLimit") {
                    QString number = element.text();
                    if (!number.isEmpty()) {
                        bool ok;
                        this->enchantment_limit = number.toInt(&ok);
                        if (!ok) {
                            error.append("Enchantment limit could not be converted to integer. Will default to -1!\n");
                            this->enchantment_limit = -1;
                        }
                    } else this->enchantment_limit = -1;
                    element.nextSiblingElement();
                }

                if (element.tagName() == "effects") {
                    QList<Effect> effect_list = QList<Effect>();
                    QDomNodeList list = element.elementsByTagName("effect");
                    for (int i = 0; i < list.size(); i++) {
                        if (list.at(i).isElement()) {
                            Effect effect;
                            QDomElement entry = list.at(i).toElement();
                            effect.effect_type = entry.attribute("itemEffectType");
                            effect.limit = entry.attribute("limit").toInt();
                            effect.potency = entry.attribute("potency");
                            effect.primary_modifier = entry.attribute("primaryModifier");
                            effect.secondary_modifier = entry.attribute("secondaryModifier");
                            effect.timer = entry.attribute("timer").toInt();
                            effect_list.append(effect);
                        }
                    }
                    this->effects = effect_list;
                    element.nextSiblingElement();
                }

                if (element.tagName() == "primaryColours") {
                    get_colours(element, this->primary_colour);
                    element.nextSiblingElement();
                }

                if (element.tagName() == "primaryColoursDye") {
                    get_colours(element, this->primary_colour_dye);
                    element.nextSiblingElement();
                }

                if (element.tagName() == "secondaryColours") {
                    get_colours(element, this->secondary_colour);
                    element.nextSiblingElement();
                }

                if (element.tagName() == "secondaryColoursDye") {
                    get_colours(element, this->secondary_colour_dye);
                    element.nextSiblingElement();
                }

                if (element.tagName() == "itemTags") {
                    iterate_string_list(element, "tag", this->item_tags);
                    element.nextSiblingElement();
                }

            } // End "coreAttributes"
        }

        if (child.tagName() == "hitDescriptions")
            iterate_string_list(child, "hitText", this->hit_text);

        if (child.tagName() == "missDescriptions")
            iterate_string_list(child, "missText", this->miss_text);

    } // End "weapon"

    return true;
}

bool WeaponMod::save_file(const QString &path, QString &error)
{
    // Does the polar opposite of above.
    // Note: I used a lot of 'this->' for readability. Helps get an idea of scope since a lot of different scopes are used here.

    // First, we must validate the data.

    // This sets the error string pointer.
    this->error = &error;

    // Default widget stuff
    check_string(this->name, "Name");
    check_string(this->damage_variance, "Damage variance");
    check_string(this->name_plural, "Plural name");
    check_string(this->determiner, "Determiner");
    check_string(this->description, "Description");
    check_string(this->attack_descriptor, "Attack descriptor");
    check_string(this->attack_tooltip, "Attack tooltip");
    check_string(this->equip_text, "Equip text");
    check_string(this->unequip_text, "Unequip text");
    check_string(this->image_name, "Image name");
    check_string(this->rarity, "Rarity");

    // Hit/miss text
    if (this->hit_text.isEmpty())
        error.append("You need at least one entry in hit text!");
    if (this->miss_text.isEmpty())
        error.append("You need at least one entry in miss text!");
    for (const QString &text : this->hit_text)
        check_string(text, "Hit text -> Entry");
    for (const QString &text : this->miss_text)
        check_string(text, "Miss text -> Entry");

    // Enchantment widget stuff
    for (const DataCommon::Effect &obj : this->effects) {
        check_string(obj.effect_type, "Effect -> Effect type");
        check_string(obj.primary_modifier, "Effect -> Primary modifier");
        check_string(obj.secondary_modifier, "Effect -> Secondary modifier");
        check_string(obj.potency, "Effect -> Potency");
    }

    // If we got an error, then something was invalid and we need to stop.
    if (!error.isEmpty())
        return false;

    // Done validating data. All empty or null fields from here on out will be omitted or set as a none flag

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        error.append("Failed to create file!");
        return false;
    }

    // Set the document pointer. Needs to be done for the write functions
    document = new QDomDocument();

    // Set the processing instructions. Also called the xml declaration. This is used to define output encoding.
    document->appendChild(document->createProcessingInstruction("xml", R"(version="1.0" encoding="UTF-8" standalone="no")"));

    // Root nodes
    auto root = write(*document, "weapon");
    auto core = write(root, "coreAttributes");

    // Core attributes
    write(core, "value", QString::number(this->value));
    write(core, "melee", bool_string(this->melee));
    write(core, "twoHanded", bool_string(this->two_handed));
    write(core, "determiner", this->determiner, true);
    write(core, "name", this->name, true);

    auto plural = write(core, "namePlural", this->name_plural, true);
    plural.setAttribute("pluralByDefault", bool_string(this->plural_default));

    write(core, "description", this->description, true);
    write(core, "attackDescriptor", this->attack_descriptor);
    write(core, "attackTooltipDescription", this->attack_tooltip);
    write(core, "rarity", this->rarity);
    write(core, "equipText", this->equip_text, true);
    write(core, "unequipText", this->unequip_text, true);
    write(core, "imageName", this->image_name);
    write(core, "imageEquippedName", this->equipped_image_name);
    write(core, "damage", QString::number(this->damage));
    write(core, "arcaneCost", QString::number(this->arcane_cost));
    write(core, "damageVariance", this->damage_variance);

    // Damage types
    auto types = write(core, "availableDamageTypes");
    for (const QString &type : this->damage_types)
        write(types, "damageType", type);

    // Spells
    auto magic = write(core, "spells");
    for (const QString &spell : this->spells)
        write(magic, "spell", spell);

    // Enchantment stuff
    if (this->enchantment_limit < 0)
        write(core, "enchantmentLimit");
    else
        write(core, "enchantmentLimit", QString::number(this->enchantment_limit));

    auto effects_node = write(core, "effects");
    for (const DataCommon::Effect &effect : this->effects) {
        auto node = write(effects_node, "effect");
        node.setAttribute("itemEffectType", effect.effect_type);
        node.setAttribute("limit", QString::number(effect.limit));
        node.setAttribute("potency", effect.potency);
        node.setAttribute("primaryModifier", effect.primary_modifier);
        node.setAttribute("secondaryModifier", effect.secondary_modifier);
        node.setAttribute("timer", QString::number(effect.timer));
    }

    // Colours
    write_colour(core, "primaryColours", this->primary_colour.get());
    write_colour(core, "primaryColoursDye", this->primary_colour_dye.get());
    write_colour(core, "secondaryColours", this->secondary_colour.get());
    write_colour(core, "secondaryColoursDye", this->secondary_colour_dye.get());

    // Item tags
    auto tags = write(core, "itemTags");
    for (const QString &tag : this->item_tags)
        write(tags, "tag", tag);

    // Hit/miss text
    auto hit = write(root, "hitDescriptions");
    for (const QString &text : this->hit_text)
        write(hit, "hitText", text, true);

    auto miss = write(root, "missDescriptions");
    for (const QString &text : this->miss_text)
        write(miss, "missText", text, true);



    // Attempt to write to file. Has undefined behaviour on fail. We can only hope it doesn't at this point.
    QTextStream out(&file);
    document->save(out, 4);
    out.flush();
    file.close();
    delete document;
    return true;
}
