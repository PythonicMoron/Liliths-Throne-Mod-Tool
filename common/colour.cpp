#include "colour.h"

// Constructors

Colour::Colour(QString &preset)
{
    // This constructor takes a QString and sets the Colour object flag to 'preset'

    type = Colour::Type::preset;
    data = QVariant(preset);
}

Colour::Colour(QStringList &colours)
{
    // This constructor takes a QStringList and sets the Colour object flag to 'list'

    type = Colour::Type::list;
    data = QVariant(colours);
}

// Get/Set

QString Colour::getPreset()
{
    // If data is a preset then return it. Return a blank string otherwise.

    if (type == Colour::Type::preset)
        return data.toString();
    return QString(); // Bad. Check type before calling accessor.
}

QStringList Colour::getColours()
{
    // If data is a list then return it. If not, return an empty list.

    if (type == Colour::Type::list)
        return data.toStringList();
    return QStringList(); // Bad. Check type before calling accessor.
}

void Colour::setPreset(const QString &value)
{
    // Sets this object to a given preset.

    type = Colour::Type::preset;
    data = QVariant(value);
}

void Colour::setColours(QStringList &value)
{
    // Sets this object to a given list.

    type = Colour::Type::list;
    data = QVariant(value);
}

Colour::Type Colour::getType()
{
    // Return flag
    return type;
}
