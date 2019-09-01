#ifndef OUTFITMOD_H
#define OUTFITMOD_H

#include <QSharedPointer>

#include "common/datacommon.h"

class OutfitMod : public DataCommon
{
public:
    struct ClothingConditional {
        bool constant;
        QStringList conditionals;
    };

    struct PresetColourGroup {
        bool single_colour;
        QSharedPointer<Colour> colours;
        QList<int> referenced_groups;
    };

    struct Weapon {
        QString type;
        QStringList conditionals, damage_types;
        QSharedPointer<Colour> primary_colour, secondary_colour;
    };

    struct UniqueClothing {
        // The documentation for this part of the mod was very confusing...
        QString id, name, pattern;
        QStringList conditionals;
        QSharedPointer<Colour> primary_colour, secondary_colour, tertiary_colour, pattern_primary_colour, pattern_secondary_colour, patter_tertiary_colour;
        bool enchantment_known, is_dirty;
        QList<DataCommon::Effect> effects;
    };

    struct GenericClothingType {
        QString slot, rarity;
        QStringList conditionals, item_tags, acceptable_femininities;
        QSharedPointer<Colour> primary_colour, secondary_colour, tertiary_colour;
    };

    struct ClothingType {
        QStringList conditionals, types;
        QSharedPointer<Colour> primary_colour, secondary_colour, tertiary_colour;
    };

    // Functions
    bool read_file(const QDomDocument &xml_doc, QString &error) override;
    bool save_file(const QString &path, QString &error) override;

    // Data
    QString femininity;
    int weight;

    // Data Lists
    QStringList conditionals, world_types, outfit_types, acceptable_configurations;
    QList<ClothingConditional> clothing_conditionals;
    QList<PresetColourGroup> colour_groups;
    QList<Weapon> main_weapons, offhand_weapons;
    QList<UniqueClothing> guaranteed_equips;
    QList<GenericClothingType> generic_clothing_types;
    QList<ClothingType> clothing_types;

    // Data Pointers
    QStringList *possible_colours; // Managed elsewhere. For the love of God, don't delete it.
};

#endif // OUTFITMOD_H
