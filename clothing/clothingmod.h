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
        BlockedParts(const BlockedParts &other);

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
        QString type, self, other, other_rough;
        // Please don't type essays into the editor.
    };

    // The rest of the class is easily understood.

    // Functions
    bool read_file(const QDomDocument &xml_doc, QString &error) override;
    bool save_file(const QString &path, QString &error) override;

    // Data
    int value, enchantment_limit;
    double physical_resistance, pattern_chance;
    bool plural_default, pattern_name_derived;
    QString author_tag, determiner, plural_name, femininity, rarity, clothing_set, image_name;

    // Data lists
    QStringList equippable_slots, item_tags, default_patterns;
    QList<QPair<QString, QString>> slot_tags, /* first = item tag, second = slot */ equipped_image_name_slot, /* first = filename, second = slot */ incompatible_slots; /* first = incompatible slot, second = equipped slot */
    QList<Effect> effects;

    // Data maps
    QMap<QString, QList<BlockedParts>> slot_blocked_parts; // Key = slot, value = list of blocked parts
    QMap<QString, QList<XPlacementText>> slot_dialogue; // Key = slot, value = list of xplacement text

    // Data pointers
    QSharedPointer<Colour> primary_colour, primary_colour_dye, secondary_colour, secondary_colour_dye, tertiary_colour, tertiary_colour_dye, pattern_primary_colour, pattern_secondary_colour, pattern_tertiary_colour;

private:
    template<class T> // I would have made this an in function lambda since it isn't used anywhere else but C++11 doesn't allow templated lambdas. I think that's C++14.
    void append_map_list(QMap<QString, QList<T>> &map, const QString &key, const T &value); // I didn't want to type this several times in the read_file function... I'm lazy.
    template<class T>
    void append_map_list(QMap<QString, QList<T>> &map, const QString &key, const QList<T> &value); // For when I need a list of reasons to be lazy.
};

#endif // CLOTHINGMOD_H
