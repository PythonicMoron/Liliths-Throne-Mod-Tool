#include "clothingmod.h"

#include <QFile>
#include <QTextStream>

// Constructors and Destructors

ClothingMod::BlockedParts::ConcealedSlots::ConcealedSlots(const QStringList &list)
{
    // This constructor takes a QStringList and sets the ConcealedSlots object flag to 'list'

    type = ClothingMod::BlockedParts::ConcealedSlots::Type::list;
    data = QVariant(list);
}

ClothingMod::BlockedParts::ConcealedSlots::ConcealedSlots(const QString &preset)
{
    // This constructor takes a QString and sets the ConcealedSlots object flag to 'preset'

    type = ClothingMod::BlockedParts::ConcealedSlots::Type::preset;
    data = QVariant(preset);
}

ClothingMod::BlockedParts::ConcealedSlots::ConcealedSlots()
{
    // Needs to have a default constructor. Calling this just creates the object as an empty list.

    type = ClothingMod::BlockedParts::ConcealedSlots::Type::list;
    data = QVariant(QStringList());
}

ClothingMod::BlockedParts::BlockedParts()
{
    // This MUST be initialized. Yes, I learned this the hard way.
    concealed_slots = QSharedPointer<ClothingMod::BlockedParts::ConcealedSlots>(new ClothingMod::BlockedParts::ConcealedSlots());
}

ClothingMod::BlockedParts::BlockedParts(const ClothingMod::BlockedParts &other)
{
    // The copy constructor must be explicitly defined because we want a copy of the data the pointers point to, not a copy of the pointer to the same data.

    displacement_type = other.displacement_type;
    access_required = other.access_required;
    blocked_bodyparts = other.blocked_bodyparts;
    access_blocked = other.access_blocked;
    concealed_slots = QSharedPointer<ClothingMod::BlockedParts::ConcealedSlots>(new ClothingMod::BlockedParts::ConcealedSlots(*other.concealed_slots.get()));
}

ClothingMod::InsertableToyAttributes::InsertableToyAttributes()
{
    enabled = false;
    length = 0;
    girth = 0;
    modifiers = QStringList();
}

ClothingMod::PenetrableToyAttributes::PenetrableToyAttributes()
{
    enabled = false;
    depth = 0;
    capacity = 0;
    elasticity = 0;
    plasticity = 0;
    wetness = 0;
    modifiers = QStringList();
}



// End Constructors and Destructors



// Start Get/Set

ClothingMod::BlockedParts::ConcealedSlots::Type ClothingMod::BlockedParts::ConcealedSlots::getType()
{
    // Return flag
    return type;
}

QStringList ClothingMod::BlockedParts::ConcealedSlots::getSlots()
{
    // If data is a list then return it. If not, return an empty list.

    if (type == ClothingMod::BlockedParts::ConcealedSlots::Type::list)
        return data.toStringList();
    return QStringList(); // Bad. Check type before calling accessor.
}

QString ClothingMod::BlockedParts::ConcealedSlots::getPreset()
{
    // If data is a preset then return it. Return a blank string otherwise.

    if (type == ClothingMod::BlockedParts::ConcealedSlots::Type::preset)
        return data.toString();
    return QString(); // Bad. Check type before calling accessor.
}

void ClothingMod::BlockedParts::ConcealedSlots::setSlots(QStringList &value)
{
    // Sets this object to a given list.

    type = ClothingMod::BlockedParts::ConcealedSlots::Type::list;
    data = QVariant(value);
}

void ClothingMod::BlockedParts::ConcealedSlots::setPreset(const QString &value)
{
    // Sets this object to a given preset.

    type = ClothingMod::BlockedParts::ConcealedSlots::Type::preset;
    data = QVariant(value);
}

// End Get/Set



// Functions

bool ClothingMod::read_file(const QDomDocument &xml_doc, QString &error)
{
    // This Function utilizes the QtXml library to read existing mod xml files.
    // Note: I used a lot of 'this->' for readability. Helps get an idea of scope since a lot of different scopes are used here.

    // This is nightmare fuel.
    // A simple mistake here can either cause fatal problems during runtime or trickle all the way down into the xml save and possibly cause trouble outside of this tool.

    // Old format dialogue warning vars
    bool old_dialogue = false, new_dialogue = false, dialogue_slot_warning = false, dialogue_slot_failure = false;
    const QStringList old_strings = {"playerSelf", "playerNPC", "playerNPCRough", "NPCSelf", "NPCPlayer", "NPCPlayerRough", "NPCOtherNPC", "NPCOtherNPCRough"}, new_strings = {"self", "other", "otherRough"};

    // Get root element from document
    QDomElement root = xml_doc.documentElement();

    // Initialize some variables. Most are already initialized by operations below.
    this->slot_dialogue = QMap<QString, QList<XPlacementText>>();
    this->slot_blocked_parts = QMap<QString, QList<BlockedParts>>();

    // Looking through children of "clothing"
    for (QDomElement child = root.firstChildElement(); !child.isNull(); child = child.nextSiblingElement()) {

        if (child.tagName() == "coreAttributes") {

            // Looking through children of "coreAttributes"
            for (QDomElement element = child.firstChildElement(); !element.isNull(); element = element.nextSiblingElement()) {

                if (element.tagName() == "authorTag") {
                    this->author_tag = element.text();
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "value") {
                    bool ok;
                    this->value = element.text().toInt(&ok);
                    if (!ok)
                        error.append("Value could not be converted to integer. Will default to 0!\n");
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "determiner") {
                    this->determiner = element.text();
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "name") {
                    this->name = element.text();
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "namePlural") {
                    this->plural_name = element.text();
                    if (element.attribute("pluralByDefault").toLower() == "true")
                        this->plural_default = true;
                    else
                        this->plural_default = false;
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "description") {
                    this->description = element.text();
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "physicalResistance") {
                    bool ok;
                    this->physical_resistance = element.text().toDouble(&ok);
                    if (!ok)
                        error.append("Physical resistance could not be converted to integer. Will default to 0!\n");
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "femininity") {
                    this->femininity = element.text();
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "slot") { // Old format catch
                    // We don't show a warning about the old style here because it has no significant effect.
                    this->equippable_slots = QStringList(element.text());
                    element = element.nextSiblingElement();
                } else if (element.tagName() == "equipSlots") {
                    iterate_string_list(element, "slot", this->equippable_slots);
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "rarity") {
                    this->rarity = element.text();
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "clothingSet") {
                    this->clothing_set = element.text();
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "imageName") {
                    this->image_name = element.text();
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "imageEquippedName") {
                    QList<QPair<QString, QString>> list = QList<QPair<QString, QString>>();
                    bool message_shown = false;
                    while (element.tagName() == "imageEquippedName") { // There can be multiple elements with this name. Hold the outer loop until we go through all of them.
                        QString slot = element.attribute("slot");
                        if (slot.isEmpty() && !element.text().isEmpty()) // Old style catch
                            if (!this->equippable_slots.isEmpty()) { // Make sure we have a slot
                                list.append(QPair<QString, QString>(element.text(), this->equippable_slots.first()));
                                if (!message_shown)
                                    error.append("Old equipped image field updated to new format and set to the first available slot.\n");
                            } else // Fuck
                                error.append("No initial slot set! Cannot convert old style equipped image. Will be null!\n");
                        else if (!element.text().isEmpty())
                            list.append(QPair<QString, QString>(element.text(), slot));
                        element = element.nextSiblingElement();
                    }
                    this->equipped_image_name_slot = list;
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
                    element = element.nextSiblingElement();
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
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "blockedPartsList") {
                    bool message_shown = false;
                    while (element.tagName() == "blockedPartsList") { // There can be multiple elements with this name. Hold the outer loop until we go through all of them.
                        QString slot = element.attribute("slot");
                        QList<BlockedParts> blocked_parts_list = QList<BlockedParts>();
                        QDomNodeList list = element.elementsByTagName("blockedParts");
                        for (int i = 0; i < list.size(); i++) {
                            if (list.at(i).isElement()) {
                                BlockedParts blocked_parts_entry;

                                // Looking through children of "blockedPartsList/blockedParts"
                                for (QDomElement entry = list.at(i).firstChildElement(); !entry.isNull(); entry = entry.nextSiblingElement()) {

                                    if (entry.tagName() == "displacementType") {
                                        blocked_parts_entry.displacement_type = entry.text();
                                        entry = entry.nextSiblingElement();
                                    }

                                    if (entry.tagName() == "clothingAccessRequired") {
                                        iterate_string_list(entry, "clothingAccess", blocked_parts_entry.access_required);
                                        entry = entry.nextSiblingElement();
                                    }

                                    if (entry.tagName() == "blockedBodyParts") {
                                        iterate_string_list(entry, "bodyPart", blocked_parts_entry.blocked_bodyparts);
                                        entry = entry.nextSiblingElement();
                                    }

                                    if (entry.tagName() == "clothingAccessBlocked") {
                                        iterate_string_list(entry, "clothingAccess", blocked_parts_entry.access_blocked);
                                        entry = entry.nextSiblingElement();
                                    }

                                    if (entry.tagName() == "concealedSlots") {
                                        QString attr = entry.attribute("values");
                                        if (!attr.isEmpty()) { // Preset
                                            blocked_parts_entry.concealed_slots = QSharedPointer<BlockedParts::ConcealedSlots>(new BlockedParts::ConcealedSlots(attr));
                                        } else { // List
                                            QStringList concealed_parts = QStringList();
                                            iterate_string_list(entry, "slot", concealed_parts);
                                            blocked_parts_entry.concealed_slots = QSharedPointer<BlockedParts::ConcealedSlots>(new BlockedParts::ConcealedSlots(concealed_parts));
                                        }
                                        entry = entry.nextSiblingElement();
                                    }

                                } // End "blockedPartsList/blockedParts"

                                blocked_parts_list.append(blocked_parts_entry);
                            }
                        }
                        if (slot.isEmpty()) // Old style catch
                            if (!this->equippable_slots.isEmpty()) { // Must have at least one slot
                                append_map_list<BlockedParts>(this->slot_blocked_parts, this->equippable_slots.first(), blocked_parts_list);
                                if (!message_shown)
                                    error.append("Old blocked parts fields updated to new format and set to the first available slot.\n");
                            } else // Oh shit
                                error.append("No initial slot set! Cannot convert old style blocked parts list. Will be null!\n");
                        else
                            append_map_list<BlockedParts>(this->slot_blocked_parts, slot, blocked_parts_list);
                        element = element.nextSiblingElement();
                    }
                }

                if (element.tagName() == "incompatibleSlots") {
                    QList<QPair<QString, QString>> list = QList<QPair<QString, QString>>();
                    bool message_shown = false;
                    while (element.tagName() == "incompatibleSlots") { // There can be multiple elements with this name. Hold the outer loop until we go through all of them.
                        QString slot = element.attribute("slot");
                        if (slot.isEmpty()) // Old style catch
                            if (!this->equippable_slots.isEmpty()) { // Make sure we have a slot
                                QStringList string_list = QStringList();
                                iterate_string_list(element, "slot", string_list);
                                for (const QString &str : string_list)
                                    list.append(QPair<QString, QString>(str, this->equippable_slots.first()));
                                if (!message_shown)
                                    error.append("Old incompatible slot entry updated to new format and set to the first available slot.\n");
                            } else // Fuck
                                error.append("No initial slot set! Cannot convert old style incompatible slots. Will be null!\n");
                        else { // Has slot defined
                            QStringList string_list = QStringList();
                            iterate_string_list(element, "slot", string_list);
                            for (const QString &str : string_list)
                                if (!str.isEmpty())
                                    list.append(QPair<QString, QString>(str, slot));
                        }
                        element = element.nextSiblingElement();
                    }
                    this->incompatible_slots = list;
                }

                if (element.tagName() == "primaryColours") {
                    get_colours(element, this->primary_colour);
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "primaryColoursDye") {
                    get_colours(element, this->primary_colour_dye);
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "secondaryColours") {
                    get_colours(element, this->secondary_colour);
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "secondaryColoursDye") {
                    get_colours(element, this->secondary_colour_dye);
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "tertiaryColours") {
                    get_colours(element, this->tertiary_colour);
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "tertiaryColoursDye") {
                    get_colours(element, this->tertiary_colour_dye);
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "defaultPatterns") {
                    this->pattern_chance = element.attribute("patternChance", "0.0").toDouble() * 100;
                    if (element.attribute("colourNameDerivedFromPattern").toLower() == "true")
                        this->pattern_name_derived = true;
                    else
                        this->pattern_name_derived = false;
                    iterate_string_list(element, "pattern", this->default_patterns);
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "patternPrimaryColours") {
                    get_colours(element, this->pattern_primary_colour);
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "patternSecondaryColours") {
                    get_colours(element, this->pattern_secondary_colour);
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "patternTertiaryColours") {
                    get_colours(element, this->pattern_tertiary_colour);
                    element = element.nextSiblingElement();
                }

                if (element.tagName() == "itemTags") {
                    QList<QPair<QString, QString>> list = QList<QPair<QString, QString>>();
                    while (element.tagName() == "itemTags") { // There can be multiple elements with this name. Hold the outer loop until we go through all of them.
                        QString slot = element.attribute("slot");
                        if (slot.isEmpty()) // Regular tags
                            iterate_string_list(element, "tag", this->item_tags);
                        else { // Slot tags
                            QStringList tags;
                            iterate_string_list(element, "tag", tags);
                            for (const QString &tag : tags) {
                                if (!tag.isEmpty())
                                    list.append(QPair<QString, QString>(tag, slot));
                            }
                        }
                        element = element.nextSiblingElement();
                    }

                    // Quick check for old sex toy flags
                    for (QPair<QString, QString> slot_tag : list)
                        if (slot_tag.second.toUpper().contains("DILDO")) {
                            error.append("Old 'DILDO' tag detected! This item has to manually be converted to the new toy attribute system! Use the 'Sex Toy' tab.\n");
                            break;
                        }

                    this->slot_tags = list;
                }

            } // End "coreAttributes"
        }

        if (child.tagName() == "sexAttributesSelf") {
            // Looking through "sexAttributesSelf"
            for (QDomElement element = child.firstChildElement(); !element.isNull(); element = element.nextSiblingElement()) {

                // Looking through "penetration"
                if (element.tagName() == "penetration") {
                    for (QDomElement innerChild = element.firstChildElement(); !innerChild.isNull(); innerChild = innerChild.nextSiblingElement()) {
                        if (!this->dildoSelf.enabled) this->dildoSelf.enabled = true; // So we know that it is valid later

                        if (innerChild.tagName() == "length") {
                            bool ok;
                            this->dildoSelf.length = innerChild.text().toInt(&ok);
                            if (!ok)
                                error.append("Self insertable toy length could not be converted to integer. Will default to 0!\n");
                            innerChild = innerChild.nextSiblingElement();
                        }

                        if (innerChild.tagName() == "girth") {
                            bool ok;
                            this->dildoSelf.girth = innerChild.text().toInt(&ok);
                            if (!ok)
                                error.append("Self insertable toy girth could not be converted to integer. Will default to 0!\n");
                            innerChild = innerChild.nextSiblingElement();
                        }

                        if (innerChild.tagName() == "modifiers")
                            iterate_string_list(innerChild, "mod", this->dildoSelf.modifiers);
                    }
                }

                if (element.tagName() == "orifice") {
                    for (QDomElement innerChild = element.firstChildElement(); !innerChild.isNull(); innerChild = innerChild.nextSiblingElement()) {
                        if (!this->holeSelf.enabled) this->holeSelf.enabled = true; // So we know that it is valid later

                        if (innerChild.tagName() == "depth") {
                            bool ok;
                            this->holeSelf.depth = innerChild.text().toInt(&ok);
                            if (!ok)
                                error.append("Self penetrable toy depth could not be converted to integer. Will default to 0!\n");
                            innerChild = innerChild.nextSiblingElement();
                        }

                        if (innerChild.tagName() == "capacity") {
                            bool ok;
                            this->holeSelf.capacity = innerChild.text().toInt(&ok);
                            if (!ok)
                                error.append("Self penetrable toy capacity could not be converted to integer. Will default to 0!\n");
                            innerChild = innerChild.nextSiblingElement();
                        }

                        if (innerChild.tagName() == "elasticity") {
                            bool ok;
                            this->holeSelf.elasticity = innerChild.text().toInt(&ok);
                            if (!ok)
                                error.append("Self penetrable toy elasticity could not be converted to integer. Will default to 0!\n");
                            innerChild = innerChild.nextSiblingElement();
                        }

                        if (innerChild.tagName() == "plasticity") {
                            bool ok;
                            this->holeSelf.plasticity = innerChild.text().toInt(&ok);
                            if (!ok)
                                error.append("Self penetrable toy plasticity could not be converted to integer. Will default to 0!\n");
                            innerChild = innerChild.nextSiblingElement();
                        }

                        if (innerChild.tagName() == "wetness") {
                            bool ok;
                            this->holeSelf.wetness = innerChild.text().toInt(&ok);
                            if (!ok)
                                error.append("Self penetrable toy wetness could not be converted to integer. Will default to 0!\n");
                            innerChild = innerChild.nextSiblingElement();
                        }

                        if (innerChild.tagName() == "modifiers")
                            iterate_string_list(innerChild, "mod", this->holeSelf.modifiers);
                    }
                }
            }
        }

        if (child.tagName() == "sexAttributesOther") {
            // Looking through "sexAttributesSelf"
            for (QDomElement element = child.firstChildElement(); !element.isNull(); element = element.nextSiblingElement()) {

                // Looking through "penetration"
                if (element.tagName() == "penetration") {
                    for (QDomElement innerChild = element.firstChildElement(); !innerChild.isNull(); innerChild = innerChild.nextSiblingElement()) {
                        if (!this->dildoOther.enabled) this->dildoOther.enabled = true; // So we know that it is valid later

                        if (innerChild.tagName() == "length") {
                            bool ok;
                            this->dildoOther.length = innerChild.text().toInt(&ok);
                            if (!ok)
                                error.append("Other insertable toy length could not be converted to integer. Will default to 0!\n");
                            innerChild = innerChild.nextSiblingElement();
                        }

                        if (innerChild.tagName() == "girth") {
                            bool ok;
                            this->dildoOther.girth = innerChild.text().toInt(&ok);
                            if (!ok)
                                error.append("Other insertable toy girth could not be converted to integer. Will default to 0!\n");
                            innerChild = innerChild.nextSiblingElement();
                        }

                        if (innerChild.tagName() == "modifiers")
                            iterate_string_list(innerChild, "mod", this->dildoOther.modifiers);
                    }
                }

                if (element.tagName() == "orifice") {
                    for (QDomElement innerChild = element.firstChildElement(); !innerChild.isNull(); innerChild = innerChild.nextSiblingElement()) {
                        if (!this->holeOther.enabled) this->holeOther.enabled = true; // So we know that it is valid later

                        if (innerChild.tagName() == "depth") {
                            bool ok;
                            this->holeOther.depth = innerChild.text().toInt(&ok);
                            if (!ok)
                                error.append("Other penetrable toy depth could not be converted to integer. Will default to 0!\n");
                            innerChild = innerChild.nextSiblingElement();
                        }

                        if (innerChild.tagName() == "capacity") {
                            bool ok;
                            this->holeOther.capacity = innerChild.text().toInt(&ok);
                            if (!ok)
                                error.append("Other penetrable toy capacity could not be converted to integer. Will default to 0!\n");
                            innerChild = innerChild.nextSiblingElement();
                        }

                        if (innerChild.tagName() == "elasticity") {
                            bool ok;
                            this->holeOther.elasticity = innerChild.text().toInt(&ok);
                            if (!ok)
                                error.append("Other penetrable toy elasticity could not be converted to integer. Will default to 0!\n");
                            innerChild = innerChild.nextSiblingElement();
                        }

                        if (innerChild.tagName() == "plasticity") {
                            bool ok;
                            this->holeOther.plasticity = innerChild.text().toInt(&ok);
                            if (!ok)
                                error.append("Other penetrable toy plasticity could not be converted to integer. Will default to 0!\n");
                            innerChild = innerChild.nextSiblingElement();
                        }

                        if (innerChild.tagName() == "wetness") {
                            bool ok;
                            this->holeOther.wetness = innerChild.text().toInt(&ok);
                            if (!ok)
                                error.append("Other penetrable toy wetness could not be converted to integer. Will default to 0!\n");
                            innerChild = innerChild.nextSiblingElement();
                        }

                        if (innerChild.tagName() == "modifiers")
                            iterate_string_list(innerChild, "mod", this->holeOther.modifiers);
                    }
                }
            }
        }

        if (child.tagName() == "displacementText") {
            XPlacementText xplacement;
            xplacement.flag = XPlacementText::Flag::displacement;
            xplacement.type = child.attribute("type");
            QString slot = child.attribute("slot");

            QDomElement element = child.firstChildElement();

            // This check is here because I don't want to compare strings on every loop cycle.
            if (!old_dialogue && !new_dialogue) { // We need to determine whether or not a warning must be displayed
                if (old_strings.contains(element.text())) {
                    old_dialogue = true; // Old format, set var and show warning
                    error.append("Old dialogue format detected. Please note that the following fields will be ignored:\n\t<playerSelf>, <playerNPC>, <playerNPCRough>, <NPCPlayer>, <NPCPlayerRough>\n");
                    error.append("The <NPCSelf>, <NPCOtherNPC>, and <NPCOtherNPCRough> fields will be changed to <self>, <other>, and <otherRough> respectively.\n");
                } else if (new_strings.contains(element.text()))
                    new_dialogue = true; // New format, set var and forget this ever happened
            }

            // Looking through "displacementText"
            while (!element.isNull()) {
                // This section includes old format catches

                if (element.tagName() == "self" || element.tagName() == "NPCSelf")
                    xplacement.self = element.text();

                if (element.tagName() == "other" || element.tagName() == "NPCOtherNPC")
                    xplacement.other = element.text();

                if (element.tagName() == "otherRough" || element.tagName() == "NPCOtherNPCRough")
                    xplacement.other_rough = element.text();

                element = element.nextSiblingElement();

            } // End "displacementText"

            if (slot.isEmpty()) { // Old format catch
                if (!this->equippable_slots.isEmpty()) { // Need to have a slot to assign
                    if (!dialogue_slot_warning) { // Show the missing slot warning if not already shown
                        dialogue_slot_warning = true;
                        error.append("No slot defined for dialogue! All sections without a slot defined will be set to the first available slot.\n");
                    }
                    append_map_list<XPlacementText>(this->slot_dialogue, this->equippable_slots.first(), xplacement);
                } else if (!dialogue_slot_failure) { // Shot the missing slot warning if not already shown
                    dialogue_slot_failure = true;
                    error.append("No initial slot set! Could not set slot for dialogue. Dialogue will be null!\n");
                }
            } else
                append_map_list<XPlacementText>(this->slot_dialogue, slot, xplacement);
        }

        if (child.tagName() == "replacementText") {
            XPlacementText xplacement;
            xplacement.flag = XPlacementText::Flag::replacement;
            xplacement.type = child.attribute("type");
            QString slot = child.attribute("slot");

            QDomElement element = child.firstChildElement();

            // This check is here because I don't want to compare strings on every loop cycle.
            if (!old_dialogue && !new_dialogue) { // We need to determine whether or not a warning must be displayed
                if (old_strings.contains(element.text())) {
                    old_dialogue = true; // Old format, set var and show warning
                    error.append("Old dialogue fields detected. Please note that the following fields will be ignored:\n\t<playerSelf>, <playerNPC>, <playerNPCRough>, <NPCPlayer>, <NPCPlayerRough>\n");
                    error.append("The <NPCSelf>, <NPCOtherNPC>, and <NPCOtherNPCRough> fields will be changed to <self>, <other>, and <otherRough> respectively.\n");
                } else if (new_strings.contains(element.text()))
                    new_dialogue = true; // New format, set var and forget this ever happened
            }

            // Looking through "replacementText"
            while (!element.isNull()) {
                // This section includes old format catches

                if (element.tagName() == "self" || element.tagName() == "NPCSelf")
                    xplacement.self = element.text();

                if (element.tagName() == "other" || element.tagName() == "NPCOtherNPC")
                    xplacement.other = element.text();

                if (element.tagName() == "otherRough" || element.tagName() == "NPCOtherNPCRough")
                    xplacement.other_rough = element.text();

                element = element.nextSiblingElement();

            } // End "replacementText"

            if (slot.isEmpty()) { // Old format catch
                if (!this->equippable_slots.isEmpty()) { // Need to have a slot to assign
                    if (!dialogue_slot_warning) { // Show the missing slot warning if not already shown
                        dialogue_slot_warning = true;
                        error.append("No slot defined for dialogue! All sections without a slot defined will be set to the first available slot.\n");
                    }
                    append_map_list<XPlacementText>(this->slot_dialogue, this->equippable_slots.first(), xplacement);
                } else if (!dialogue_slot_failure) { // Shot the missing slot warning if not already shown
                    dialogue_slot_failure = true;
                    error.append("No initial slot set! Could not set slot for dialogue. Dialogue will be null!\n");
                }
            } else
                append_map_list<XPlacementText>(this->slot_dialogue, slot, xplacement);
        }

    } // End "clothing"

    // Last minute checks for null values
    QString blank = "NULL"; // I have no fucking clue why this has to be here. Don't remove it. That pisses the compiler off.
    if (this->pattern_primary_colour.isNull())
        this->pattern_primary_colour = QSharedPointer<Colour>(new Colour(blank));
    if (this->pattern_secondary_colour.isNull())
        this->pattern_secondary_colour = QSharedPointer<Colour>(new Colour(blank));
    if (this->pattern_tertiary_colour.isNull())
        this->pattern_tertiary_colour = QSharedPointer<Colour>(new Colour(blank));

    return true;
}

bool ClothingMod::save_file(const QString &path, QString &error)
{
    // Does the polar opposite of above.
    // Note: I used a lot of 'this->' for readability. Helps get an idea of scope since a lot of different scopes are used here. A lot of shit gets allocated in this scope.

    // First, we must validate the data.

    // This sets the error string pointer.
    this->error = &error;

    // String list check lambda
    auto check_string_list = [this] (const QStringList &list, const QString &field, bool can_be_empty = true) {
        if (list.isEmpty()) { // Is the list empty?
            if (can_be_empty) // Is it allowed to be empty?
                return; // No need to continue evaluating this if we already know it's empty.
            else { // Uh-oh
                this->error->append(field + " list must contain at least one value!");
            }
        }
        for (int i = 0; i < list.length(); i++) { // Start actual data validation
            if (list.at(i).isNull() || list.at(i).isEmpty() || list.at(i) == "NULL") {
                QString num = QString::number(i + 1); // Normal people don't start counting from 0. Fucking weird, I know.
                this->error->append(field + " list entry " + num + " can not be empty!\n");
            }
        }
    };

    // Default widget stuff
    check_string(this->name, "Name");
    check_string(this->plural_name, "Plural name");
    check_string(this->determiner, "Determiner");
    check_string(this->description, "Description");
    check_string(this->image_name, "Image name");
    check_string(this->femininity, "Femininity");
    check_string(this->rarity, "Rarity");
    check_string_list(this->equippable_slots, "Slots", false);
    check_string_list(this->default_patterns, "Default patterns");

    // TODO verify slot_tags, equipped_image_name_slot, incompatible_slots

    // Default widget blocked parts
    for (const QString &key : this->slot_blocked_parts.keys())
        for (const ClothingMod::BlockedParts &obj : this->slot_blocked_parts[key])
            check_string(obj.displacement_type, "Blocked parts list [" + key + "] -> Displacement type");

    // Enchantment widget stuff
    for (const DataCommon::Effect &obj : this->effects) {
        check_string(obj.effect_type, "Effect -> Effect type");
        check_string(obj.primary_modifier, "Effect -> Primary modifier");
        check_string(obj.secondary_modifier, "Effect -> Secondary modifier");
        check_string(obj.potency, "Effect -> Potency");
    }

    // Dialogue editor stuff
    for (const QString &key : this->slot_dialogue.keys()) {
        for (const ClothingMod::XPlacementText &obj : this->slot_dialogue[key]) {
            check_string(obj.self, "Dialogue [" + key + "] -> Self");
            check_string(obj.other, "Dialogue [" + key + "] -> Other");
            check_string(obj.other_rough, "Dialogue [" + key + "] -> Other Rough");
        }
    }

    // If we got an error, then something was invalid and we need to stop.
    if (!error.isEmpty())
        return false;

    // Done validating data. All empty or null fields from here on out will be omitted or set as a none flag

    // We need to turn the 'QList<QPair<QString, QString>>' objects into a 'QMap<QString, X>' object for easy writing.

    QMap<QString, QList<QString>> mapped_item_tags, mapped_incompatible_slots;
    QMap<QString, QString> mapped_equipped_images; // Only defines a single object. No list needed.

    // Item tags
    for (const QPair<QString, QString> &pair : this->slot_tags)
        append_map_list<QString>(mapped_item_tags, pair.second, pair.first);

    // Equipped images
    for (const QPair<QString, QString> &pair : this->equipped_image_name_slot)
        mapped_equipped_images.insert(pair.second, pair.first);

    // Incompatible slots
    for (const QPair<QString, QString> &pair : this->incompatible_slots)
        append_map_list<QString>(mapped_incompatible_slots, pair.second, pair.first);

    // Conversions done. Let's get on with writing this thing.

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
    auto root = write(*document, "clothing");
    auto core = write(root, "coreAttributes");

    // Core attributes
    write(core, "authorTag", this->author_tag, true);
    write(core, "value", QString::number(this->value));
    write(core, "determiner", this->determiner, true);
    write(core, "name", this->name, true);

    auto temp = write(core, "namePlural", this->plural_name, true);
    temp.setAttribute("pluralByDefault", bool_string(this->plural_default));

    write(core, "description", this->description, true);
    write(core, "physicalResistance", QString::number(this->physical_resistance));
    write(core, "femininity", this->femininity);

    auto equip_slots_node = write(core, "equipSlots");
    for (const QString &slot : this->equippable_slots)
        write(equip_slots_node, "slot", slot);

    write(core, "rarity", this->rarity);
    write(core, "clothingSet", this->clothing_set);
    write(core, "imageName", this->image_name);

    for (const QString &slot : this->equippable_slots)
        if (mapped_equipped_images.contains(slot))
            write(core, "imageEquippedName", mapped_equipped_images[slot]).setAttribute("slot", slot);
        else
            write(core, "imageEquippedName").setAttribute("slot", slot);

    if (this->enchantment_limit < 0)
        write(core, "enchantmentLimit");
    else
        write(core, "enchantmentLimit", QString::number(this->enchantment_limit));

    // Enchantments
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

    // Blocked parts list
    if (this->slot_blocked_parts.isEmpty())
        write(core, "blockedPartsList");
    else
        for (const QString &key : this->slot_blocked_parts.keys()) {
            auto blocked_parts_node = write(core, "blockedPartsList");
            blocked_parts_node.setAttribute("slot", key);
            for (const ClothingMod::BlockedParts &parts : this->slot_blocked_parts[key]) {

                auto node = write(blocked_parts_node, "blockedParts");
                write(node, "displacementType", parts.displacement_type);

                auto access_req = write(node, "clothingAccessRequired");
                for (const QString &req : parts.access_required)
                    write(access_req, "clothingAccess", req);

                auto blocked = write(node, "blockedBodyParts");
                for (const QString &part : parts.blocked_bodyparts)
                    write(blocked, "bodyPart", part);

                auto access_block = write(node, "clothingAccessBlocked");
                for (const QString &block : parts.access_blocked)
                    write(access_block, "clothingAccess", block);

                auto concealed = write(node, "concealedSlots");
                if (parts.concealed_slots->getType() == ClothingMod::BlockedParts::ConcealedSlots::Type::list)
                    for (const QString &concealed_slot : parts.concealed_slots->getSlots())
                        write(concealed, "slot", concealed_slot);
                else
                    concealed.setAttribute("values", parts.concealed_slots->getPreset());
            }
        }

    // Incompatible slots
    for (const QString &slot : this->equippable_slots)
        if (mapped_incompatible_slots.contains(slot)) {
            auto node = write(core, "incompatibleSlots");
            node.setAttribute("slot", slot);
            for (const QString &value : mapped_incompatible_slots[slot])
                write(node, "slot", value);
        } else
            write(core, "incompatibleSlots").setAttribute("slot", slot);

    // Colours
    write_colour(core, "primaryColours", this->primary_colour.get());
    write_colour(core, "primaryColoursDye", this->primary_colour_dye.get());
    write_colour(core, "secondaryColours", this->secondary_colour.get());
    write_colour(core, "secondaryColoursDye", this->secondary_colour_dye.get());
    write_colour(core, "tertiaryColours", this->tertiary_colour.get());
    write_colour(core, "tertiaryColoursDye", this->tertiary_colour_dye.get());

    // Patterns
    auto pattern_node = write(core, "defaultPatterns");
    if (this->pattern_chance < 0.01) // It seems pointless to have a spawn chance of less than 1/1000th
        pattern_node.setAttribute("patternChance", QString::number(0));
    else {
        // There exists some stupid logic where the program will save INCREDIBLY small numbers if you don't truncate the number first. This is my fix
        double number = static_cast<int>(this->pattern_chance * 100) * 0.0001;
        pattern_node.setAttribute("patternChance", QString::number(number));
    }
    pattern_node.setAttribute("colourNameDerivedFromPattern", bool_string(this->pattern_name_derived));
    for (const QString &entry : this->default_patterns)
        write(pattern_node, "pattern", entry);

    // Pattern colours
    write_colour(core, "patternPrimaryColours", this->pattern_primary_colour.get());
    write_colour(core, "patternSecondaryColours", this->pattern_secondary_colour.get());
    write_colour(core, "patternTertiaryColours", this->pattern_tertiary_colour.get());

    // Item tags
    auto tags = write(core, "itemTags");
    for (const QString &tag : this->item_tags)
        write(tags, "tag", tag);

    // Slot item tags
    for (const QString &slot : this->equippable_slots)
        if (mapped_item_tags.contains(slot)) {
            auto node = write(core, "itemTags");
            node.setAttribute("slot", slot);
            for (const QString &value : mapped_item_tags[slot])
                write(node, "tag", value);
        } else
            write(core, "itemTags").setAttribute("slot", slot);

    // Self toy attributes
    if (this->dildoSelf.enabled || this->holeSelf.enabled) {
        auto node = write(root, "sexAttributesSelf");

        // Penetration toy
        if (this->dildoSelf.enabled) {
            auto toy = write(node, "penetration");
            write(toy, "length", QString::number(this->dildoSelf.length));
            write(toy, "girth", QString::number(this->dildoSelf.girth));
            auto mods = write(toy, "modifiers");
            for (QString mod : this->dildoSelf.modifiers)
                write(mods, "mod", mod);
        }

        // Orifice toy
        if (this->holeSelf.enabled) {
            auto toy = write(node, "orifice");
            write(toy, "depth", QString::number(this->holeSelf.depth));
            write(toy, "capacity", QString::number(this->holeSelf.capacity));
            write(toy, "elasticity", QString::number(this->holeSelf.elasticity));
            write(toy, "plasticity", QString::number(this->holeSelf.plasticity));
            write(toy, "wetness", QString::number(this->holeSelf.wetness));
            auto mods = write(toy, "modifiers");
            for (QString mod : this->holeSelf.modifiers)
                write(mods, "mod", mod);
        }
    }

    // Other toy attributes
    if (this->dildoOther.enabled || this->holeOther.enabled) {
        auto node = write(root, "sexAttributesOther");

        // Penetration toy
        if (this->dildoOther.enabled) {
            auto toy = write(node, "penetration");
            write(toy, "length", QString::number(this->dildoOther.length));
            write(toy, "girth", QString::number(this->dildoOther.girth));
            auto mods = write(toy, "modifiers");
            for (QString mod : this->dildoOther.modifiers)
                write(mods, "mod", mod);
        }

        // Orifice toy
        if (this->holeOther.enabled) {
            auto toy = write(node, "orifice");
            write(toy, "depth", QString::number(this->holeOther.depth));
            write(toy, "capacity", QString::number(this->holeOther.capacity));
            write(toy, "elasticity", QString::number(this->holeOther.elasticity));
            write(toy, "plasticity", QString::number(this->holeOther.plasticity));
            write(toy, "wetness", QString::number(this->holeOther.wetness));
            auto mods = write(toy, "modifiers");
            for (QString mod : this->holeOther.modifiers)
                write(mods, "mod", mod);
        }
    }

    // Dialogue
    for (const QString &slot : this->equippable_slots) {
        if (this->slot_dialogue.contains(slot)) {
            for (const XPlacementText &text : this->slot_dialogue[slot]) {
                // Setup element
                QDomElement xplacement;
                if (text.flag == ClothingMod::XPlacementText::Flag::replacement)
                    xplacement = write(root, "replacementText");
                else
                    xplacement = write(root, "displacementText");

                // Attributes
                xplacement.setAttribute("slot", slot);
                xplacement.setAttribute("type", text.type);

                // Text
                write(xplacement, "self", text.self, true);
                write(xplacement, "other", text.other, true);
                write(xplacement, "otherRough", text.other_rough, true);
            }
        }
    }



    // Attempt to write to file. Has undefined behaviour on fail. We can only hope it doesn't at this point.
    QTextStream out(&file);
    document->save(out, 4);
    out.flush();
    file.close();
    delete document;
    return true;
}

template<class T>
void ClothingMod::append_map_list(QMap<QString, QList<T>> &map, const QString &key, const T &value)
{
    if (map.contains(key)) // Item exists, append it to existing item.
        map[key].append(value);
    else // Item doesn't exist. Make it exist.
        map.insert(key, QList<T>() << value);
}

// End Functions

template<class T>
void ClothingMod::append_map_list(QMap<QString, QList<T> > &map, const QString &key, const QList<T> &value)
{
    if (map.contains(key)) // Item exists, append it to existing item.
        map[key].append(value);
    else // Item doesn't exist. Make it exist.
        map.insert(key, value);
}
