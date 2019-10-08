#ifndef WEAPONMOD_H
#define WEAPONMOD_H

#include <QSharedPointer>

#include "common/datacommon.h"

// This class does three things:
// 1 - Defines containers for related data inside of the weapon mod. (None here right now, but the option is there)
// 2 - Stores the actual data for the weapon mod.
// 3 - Does the heavy lifting for reading/writing the xml files.

class WeaponMod : public DataCommon
{
public:
    // This class is pretty easily understood.

    // Functions
    bool read_file(const QDomDocument &xml_doc, QString &error) override;
    bool save_file(const QString &path, QString &error) override;

    // Data
    bool melee, two_handed, plural_default;
    QString author_tag, determiner, name_plural, attack_descriptor, attack_tooltip, rarity, weapon_set, equip_text, unequip_text, image_name, equipped_image_name, damage_variance;
    int damage, arcane_cost, value, enchantment_limit;

    // Data lists
    QStringList damage_types, spells, item_tags, hit_text, miss_text;
    QList<Effect> effects;

    // Data pointers
    QSharedPointer<Colour> primary_colour, secondary_colour, primary_colour_dye, secondary_colour_dye;
};

#endif // WEAPONMOD_H
