#ifndef COLOURS_H
#define COLOURS_H

#include <QVariant>

// Stores either a vector of colours or a single colour preset.
class Colour
{
public:
    enum Type { // What is stored
        list, preset
    };

    Colour(QString &preset);
    Colour(QStringList &colours);

    // Get/Set

    QString getPreset();
    QStringList getColours();

    void setPreset(const QString &value);
    void setColours(QStringList &value);

    Type getType(); // Gonna be honest with you, I should have just wrote an isList function that returns bool...

    // End Get/Set

private:
    Type type; // Flag
    QVariant data; // Can be type QString or QStringList
};

#endif // COLOURS_H
