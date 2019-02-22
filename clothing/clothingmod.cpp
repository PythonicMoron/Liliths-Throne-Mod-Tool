#include "clothingmod.h"

#include <QtXml>

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

    // Get root element from document
    QDomElement root = xml_doc.documentElement();

    // Initialize some variables. Most are already initialized by operations below.
    this->dialogue = QList<XPlacementText>();

    // This clusterfuck is volatile. Be ready for a world of pain if you change anything.
    QDomElement child = root.firstChildElement();

    // Looking through children of "clothing"
    while (!child.isNull()) {

        if (child.tagName() == "coreAttributes") {
            QDomElement element = child.firstChildElement();

            // Looking through children of "coreAttributes"
            while (!element.isNull()) {

                if (element.tagName() == "value") {
                    bool ok;
                    this->value = element.text().toInt(&ok);
                    if (!ok)
                        error.append("Value could not be converted to integer. Will default to 0!\n");
                }

                if (element.tagName() == "determiner")
                    this->determiner = element.text();

                if (element.tagName() == "name")
                    this->name = element.text();

                if (element.tagName() == "namePlural") {
                    this->plural_name = element.text();
                    if (element.attribute("pluralByDefault").toLower() == "true")
                        this->plural_default = true;
                    else
                        this->plural_default = false;
                }

                if (element.tagName() == "description")
                    this->description = element.text();

                if (element.tagName() == "physicalResistance") {
                    bool ok;
                    this->physical_resistance = element.text().toInt(&ok);
                    if (!ok)
                        error.append("Physical resistance could not be converted to integer. Will default to 0!\n");
                }

                if (element.tagName() == "femininity")
                    this->femininity = element.text();

                if (element.tagName() == "slot")
                    this->slot = element.text();

                if (element.tagName() == "rarity")
                    this->rarity = element.text();

                if (element.tagName() == "clothingSet")
                    this->clothing_set = element.text();

                if (element.tagName() == "imageName")
                    this->image_name = element.text();

                if (element.tagName() == "imageEquippedName")
                    this->equipped_image_name = element.text();

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
                }

                if (element.tagName() == "blockedPartsList") {
                    QList<BlockedParts> blocked_parts_list = QList<BlockedParts>();
                    QDomNodeList list = element.elementsByTagName("blockedParts");
                    for (int i = 0; i < list.size(); i++) {
                        if (list.at(i).isElement()) {
                            BlockedParts blocked_parts_entry;
                            QDomElement entry = list.at(i).firstChildElement();

                            // Looking through children of "blockedPartsList/blockedParts"
                            while (!entry.isNull()) {

                                if (entry.tagName() == "displacementType")
                                    blocked_parts_entry.displacement_type = entry.text();

                                if (entry.tagName() == "clothingAccessRequired")
                                    iterate_string_list(entry, "clothingAccess", blocked_parts_entry.access_required);

                                if (entry.tagName() == "blockedBodyParts")
                                    iterate_string_list(entry, "bodyPart", blocked_parts_entry.blocked_bodyparts);

                                if (entry.tagName() == "clothingAccessBlocked")
                                    iterate_string_list(entry, "clothingAccess", blocked_parts_entry.access_blocked);

                                if (entry.tagName() == "concealedSlots") {
                                    QString attr = entry.attribute("values");
                                    if (!attr.isEmpty()) { // Preset
                                        blocked_parts_entry.concealed_slots = QSharedPointer<BlockedParts::ConcealedSlots>(new BlockedParts::ConcealedSlots(attr));
                                    } else { // List
                                        QStringList concealed_parts = QStringList();
                                        iterate_string_list(entry, "slot", concealed_parts);
                                        blocked_parts_entry.concealed_slots = QSharedPointer<BlockedParts::ConcealedSlots>(new BlockedParts::ConcealedSlots(concealed_parts));
                                    }
                                }

                                entry = entry.nextSiblingElement();

                            } // End "blockedPartsList/blockedParts"

                            blocked_parts_list.append(blocked_parts_entry);
                        }
                    }
                    this->blocked_parts = blocked_parts_list;
                }

                if (element.tagName() == "incompatibleSlots")
                    iterate_string_list(element, "slot", this->incompatible_slots);

                if (element.tagName() == "primaryColours")
                    get_colours(element, this->primary_colour);

                if (element.tagName() == "primaryColoursDye")
                    get_colours(element, this->primary_colour_dye);

                if (element.tagName() == "secondaryColours")
                    get_colours(element, this->secondary_colour);

                if (element.tagName() == "secondaryColoursDye")
                    get_colours(element, this->secondary_colour_dye);

                if (element.tagName() == "tertiaryColours")
                    get_colours(element, this->tertiary_colour);

                if (element.tagName() == "tertiaryColoursDye")
                    get_colours(element, this->tertiary_colour_dye);

                if (element.tagName() == "itemTags")
                    iterate_string_list(element, "tag", this->item_tags);

                element = element.nextSiblingElement();

            } // End "coreAttributes"
        }

        if (child.tagName() == "displacementText") {
            XPlacementText xplacement;
            xplacement.flag = XPlacementText::Flag::displacement;
            xplacement.type = child.attribute("type");

            QDomElement element = child.firstChildElement();

            // Looking through "displacementText"
            while (!element.isNull()) {

                if (element.tagName() == "playerSelf")
                    xplacement.player_self = element.text();

                if (element.tagName() == "playerNPC")
                    xplacement.player_npc = element.text();

                if (element.tagName() == "playerNPCRough")
                    xplacement.player_npc_rough = element.text();

                if (element.tagName() == "NPCSelf")
                    xplacement.npc_self = element.text();

                if (element.tagName() == "NPCPlayer")
                    xplacement.npc_player = element.text();

                if (element.tagName() == "NPCPlayerRough")
                    xplacement.npc_player_rough = element.text();

                if (element.tagName() == "NPCOtherNPC")
                    xplacement.npc_other_npc = element.text();

                if (element.tagName() == "NPCOtherNPCRough")
                    xplacement.npc_other_npc_rough = element.text();

                element = element.nextSiblingElement();

            } // End "displacementText"

            this->dialogue.append(xplacement);
        }

        if (child.tagName() == "replacementText") {
            XPlacementText xplacement;
            xplacement.flag = XPlacementText::Flag::replacement;
            xplacement.type = child.attribute("type");

            QDomElement element = child.firstChildElement();

            // Looking through "replacementText"
            while (!element.isNull()) {

                if (element.tagName() == "playerSelf")
                    xplacement.player_self = element.text();

                if (element.tagName() == "playerNPC")
                    xplacement.player_npc = element.text();

                if (element.tagName() == "playerNPCRough")
                    xplacement.player_npc_rough = element.text();

                if (element.tagName() == "NPCSelf")
                    xplacement.npc_self = element.text();

                if (element.tagName() == "NPCPlayer")
                    xplacement.npc_player = element.text();

                if (element.tagName() == "NPCPlayerRough")
                    xplacement.npc_player_rough = element.text();

                if (element.tagName() == "NPCOtherNPC")
                    xplacement.npc_other_npc = element.text();

                if (element.tagName() == "NPCOtherNPCRough")
                    xplacement.npc_other_npc_rough = element.text();

                element = element.nextSiblingElement();

            } // End "replacementText"

            this->dialogue.append(xplacement);
        }

        child = child.nextSiblingElement();

    } // End "clothing"

    return true;
}

bool ClothingMod::save_file(const QString &path, QString &error)
{
    // Does the polar opposite of above.
    // Note: I used a lot of 'this->' for readability. Helps get an idea of scope since a lot of different scopes are used here.

    // First, we must validate the data.

    // This sets the error string pointer.
    this->error = &error;

    // Default widget stuff
    check_string(this->name, "Name");
    check_string(this->plural_name, "Plural name");
    check_string(this->determiner, "Determiner");
    check_string(this->description, "Description");
    check_string(this->image_name, "Image name");
    check_string(this->femininity, "Femininity");
    check_string(this->slot, "Slot");
    check_string(this->rarity, "Rarity");

    // Default widget blocked parts
    for (const ClothingMod::BlockedParts &obj : this->blocked_parts)
        check_string(obj.displacement_type, "Blocked parts list -> Displacement type");

    // Enchantment widget stuff
    for (const DataCommon::Effect &obj : this->effects) {
        check_string(obj.effect_type, "Effect -> Effect type");
        check_string(obj.primary_modifier, "Effect -> Primary modifier");
        check_string(obj.secondary_modifier, "Effect -> Secondary modifier");
        check_string(obj.potency, "Effect -> Potency");
    }

    // Dialogue editor stuff
    for (const ClothingMod::XPlacementText &obj : this->dialogue) {
        check_string(obj.type, "Dialogue -> Type");
        check_string(obj.player_self, "Dialogue -> Player to self");
        check_string(obj.player_npc, "Dialogue -> Player to NPC");
        check_string(obj.player_npc_rough, "Dialogue -> Player to NPC rough");
        check_string(obj.npc_self, "Dialogue -> NPC to self");
        check_string(obj.npc_player, "Dialogue -> NPC to player");
        check_string(obj.npc_player_rough, "Dialogue -> NPC to player rough");
        check_string(obj.npc_other_npc, "Dialogue -> NPC to NPC");
        check_string(obj.npc_other_npc_rough, "Dialogue -> NPC to NPC rough");
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

    // Root nodes
    auto root = write(*document, "clothing");
    auto core = write(root, "coreAttributes");

    // Core attributes
    write(core, "value", QString::number(this->value));
    write(core, "determiner", this->determiner, true);
    write(core, "name", this->name, true);

    auto temp = write(core, "namePlural", this->plural_name, true);
    temp.setAttribute("pluralByDefault", bool_string(this->plural_default));

    write(core, "description", this->description, true);
    write(core, "physicalResistance", QString::number(this->physical_resistance));
    write(core, "femininity", this->femininity);
    write(core, "slot", this->slot);
    write(core, "rarity", this->rarity);
    write(core, "clothingSet", this->clothing_set);
    write(core, "imageName", this->image_name);
    write(core, "imageEquippedName", this->equipped_image_name);

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
    auto blocked_parts_node = write(core, "blockedPartsList");
    for (const ClothingMod::BlockedParts &parts : this->blocked_parts) {
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

    // Incompatible slots
    auto incompatible = write(core, "incompatibleSlots");
    for (const QString &islot : this->incompatible_slots)
        write(incompatible, "slot", islot);

    // Colours
    write_colour(core, "primaryColours", this->primary_colour.get());
    write_colour(core, "primaryColoursDye", this->primary_colour_dye.get());
    write_colour(core, "secondaryColours", this->secondary_colour.get());
    write_colour(core, "secondaryColoursDye", this->secondary_colour_dye.get());
    write_colour(core, "tertiaryColours", this->tertiary_colour.get());
    write_colour(core, "tertiaryColoursDye", this->tertiary_colour_dye.get());

    // Item tags
    auto tags = write(core, "itemTags");
    for (const QString &tag : this->item_tags)
        write(tags, "tag", tag);

    // Dialogue
    for (ClothingMod::XPlacementText &text : this->dialogue) {
        QDomElement xplacement;
        if (text.flag == ClothingMod::XPlacementText::Flag::replacement)
            xplacement = write(root, "replacementText");
        else
            xplacement = write(root, "displacementText");

        xplacement.setAttribute("type", text.type);
        write(xplacement, "playerSelf", text.player_self, true);
        write(xplacement, "playerNPC", text.player_npc, true);
        write(xplacement, "playerNPCRough", text.player_npc_rough, true);
        write(xplacement, "NPCSelf", text.npc_self, true);
        write(xplacement, "NPCPlayer", text.npc_player, true);
        write(xplacement, "NPCPlayerRough", text.npc_player_rough, true);
        write(xplacement, "NPCOtherNPC", text.npc_other_npc, true);
        write(xplacement, "NPCOtherNPCRough", text.npc_other_npc_rough, true);
    }



    // Attempt to write to file.
    bool state = true;
    if (file.write(document->toByteArray()) == -1) {
        error.append("Error occured while writing file");
        state = false;
    }
    file.close();
    delete document;
    return state;
}

// End Functions
