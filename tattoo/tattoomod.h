#ifndef TATTOOMOD_H
#define TATTOOMOD_H

#include "common/datacommon.h"

// This class does three things:
// 1 - Defines containers for related data inside of the tattoo mod. (None here right now, but the option is there)
// 2 - Stores the actual data for the tattoo mod.
// 3 - Does the heavy lifting for reading/writing the xml files.

class TattooMod : public DataCommon
{
public:
    // This class is pretty easily understood.

    // Functions
    bool read_file(const QDomDocument &xml_doc, QString &error) override;
    bool save_file(const QString &path, QString &error) override;

    // Data lists
    QStringList slot_availability;

    // Data pointers
    QSharedPointer<Colour> tertiary_colour;
};

#endif // TATTOOMOD_H
