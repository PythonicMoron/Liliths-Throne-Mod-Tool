#ifndef DATACOMMON_H
#define DATACOMMON_H

#include <QDomElement>

#include "common/colour.h"

// This is a base class that defines common functions/variables/containers used across the different mod types.
// This will likely change as things become more/less spread out with new mods or new mod formats.

class DataCommon
{
public:
    struct Effect { // Stores enchantment effect data as a single object.
        Effect();
        QString effect_type, potency, primary_modifier, secondary_modifier;
        int limit, timer;
    };

    virtual ~DataCommon();

    // Common functions
    virtual bool read_file(const QDomDocument &xml_doc, QString &error);
    virtual bool save_file(const QString &path, QString &error);

    // Common data
    QString name, description;

protected:
    // These two pointers are solely used to save some typing for the 'write' and 'check_string' functions
    // They are managed elsewhere, do NOT delete them!
    QDomDocument *document; // I'm lazy
    QString *error; // I'm really lazy

    // These functions assist in reading/writing the xml files.
    void iterate_string_list(QDomElement &dom_element, const QString &tag, QStringList &result);
    void get_colours(QDomElement &dom_element, QSharedPointer<Colour> &colour);
    void check_string(const QString &value, const QString &error_string);
    QDomElement write(QDomNode &parent, const QString &name, const QString &data = nullptr, bool cdata = false);
    void write_colour(QDomNode &core, const QString &name, Colour *colour);
    QString bool_string(bool state);
};

#endif // DATACOMMON_H
