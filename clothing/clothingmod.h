#ifndef CLOTHINGMOD_H
#define CLOTHINGMOD_H

#include <QSharedPointer>

#include "common/datacommon.h"

// This class does three things:
// 1 - Defines containers for related data inside of the clothing mod.
// 2 - Stores the actual data for the clothing mod.
// 3 - Does the heavy lifting for reading/writing the xml files.

class ClothingMod : public DataCommon // Take note of the members/objects inherited from DataCommon!
{
public:
    struct BlockedParts { // Stores clothing access and visibility data as single object.
        class ConcealedSlots { // Stores either a list of slots or a preset definition as a single object.
        public:
            enum Type {
                list, preset
                // list = QStringList, preset = QString
            };

            ConcealedSlots(const QStringList &list);
            ConcealedSlots(const QString &preset);
            ConcealedSlots();

            // Functions
            Type getType(); // Gonna be honest with you, I should have just wrote an isList function that returns bool...
            QStringList getSlots();
            QString getPreset();
            void setSlots(QStringList &value);
            void setPreset(const QString &value);

        private:
            Type type; // Flag
            QVariant data; // Can be type QString or QStringList
        };

        BlockedParts();

        // These were made to be fairly self-explanatory
        QString displacement_type;
        QStringList access_required, blocked_bodyparts, access_blocked;
        QSharedPointer<ConcealedSlots> concealed_slots; // Stored as a smart pointer to the object
    };

    struct XPlacementText { // Stores the dialogue for either 'displacementText' or 'replacementText' as a single object.
        enum Flag {
            displacement, replacement
            // As far as this tool goes, this only affects the text shown in editor and the xml flag used during saving.
        };

        Flag flag; // Flag
        // A metric fuckton of strings. These are the dialogue lines and you should be able to tell which is which through the names.
        QString type, player_self, player_npc, player_npc_rough, npc_self, npc_player, npc_player_rough, npc_other_npc, npc_other_npc_rough;
        // Please don't type essays into the editor.
    };

    // The rest of the class is easily understood.

    // Functions
    bool read_file(const QDomDocument &xml_doc, QString &error) override;
    bool save_file(const QString &path, QString &error) override;

    // Data
    int physical_resistance, value, enchantment_limit;
    bool plural_default;
    QString determiner, plural_name, femininity, slot, rarity, clothing_set, image_name, equipped_image_name;

    // Data lists
    QStringList incompatible_slots, item_tags;
    QList<Effect> effects;
    QList<BlockedParts> blocked_parts;
    QList<XPlacementText> dialogue;

    // Data pointers
    QSharedPointer<Colour> primary_colour, primary_colour_dye, secondary_colour, secondary_colour_dye, tertiary_colour, tertiary_colour_dye;
};

#endif // CLOTHINGMOD_H
