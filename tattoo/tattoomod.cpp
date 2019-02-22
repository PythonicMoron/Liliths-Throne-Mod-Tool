#include "tattoomod.h"

#include <QFile>

bool TattooMod::read_file(const QDomDocument &xml_doc, QString &error)
{
    // This Function utilizes the QtXml library to read existing mod xml files.
    // Note: I used a lot of 'this->' for readability. Helps get an idea of scope since a lot of different scopes are used here.

    // This is nightmare fuel.
    // A simple mistake here can either cause fatal problems during runtime or trickle all the way down into the xml save and possibly cause trouble outside of this tool.

    // Get root element from document
    QDomElement root = xml_doc.documentElement();

    // This clusterfuck is volatile. Be ready for a world of pain if you change anything.
    QDomElement child = root.firstChildElement();

    // Looking through children of "tattoo"
    while (!child.isNull()) {
        // This loop is kind of pointless. Left here in case the tattoo mod xml format changes...
        // And because I copied this from my other read_file functions and don't want to figure out how to change it.

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

                if (element.tagName() == "name")
                    this->name = element.text();

                if (element.tagName() == "description")
                    this->description = element.text();

                if (element.tagName() == "imageName")
                    this->image_name = element.text();

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

                if (element.tagName() == "slotAvailability")
                    iterate_string_list(element, "slot", this->slot_availability);

                if (element.tagName() == "primaryColours")
                    get_colours(element, this->primary_colour);

                if (element.tagName() == "secondaryColours")
                    get_colours(element, this->secondary_colour);

                if (element.tagName() == "tertiaryColours")
                    get_colours(element, this->tertiary_colour);

                element = element.nextSiblingElement();

            } // End "coreAttributes"
        }

        child = child.nextSiblingElement();

    } // End "tattoo"

    return true;
}

bool TattooMod::save_file(const QString &path, QString &error)
{
    // Does the polar opposite of above.
    // Note: I used a lot of 'this->' for readability. Helps get an idea of scope since a lot of different scopes are used here.

    // First, we must validate the data.

    // This sets the error string pointer.
    this->error = &error;

    // Default widget stuff
    check_string(this->name, "Name");
    check_string(this->description, "Description");
    check_string(this->image_name, "Image name");

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
    auto root = write(*document, "tattoo");
    auto core = write(root, "coreAttributes");

    // Core attributes
    write(core, "value", QString::number(this->value));
    write(core, "name", this->name, true);
    write(core, "description", this->description, true);
    write(core, "imageName", this->image_name);

    // Enchantment stuff
    if (this->enchantment_limit < 0)
        write(core, "enchantmentLimit");
    else
        write(core, "enchantmentLimit", QString::number(this->enchantment_limit));

    // Slot availability
    auto avail = write(core, "slotAvailability");
    for (const QString &slot : this->slot_availability)
        write(avail, "slot", slot);

    // Colours
    write_colour(core, "primaryColours", this->primary_colour.get());
    write_colour(core, "secondaryColours", this->secondary_colour.get());
    write_colour(core, "tertiaryColours", this->tertiary_colour.get());



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
