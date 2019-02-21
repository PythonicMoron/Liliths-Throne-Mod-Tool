#include "datacommon.h"

#include "utility.h"

bool DataCommon::read_file(const QString& UNUSED(path), QString& UNUSED(error))
{
    // This function MUST be overridden!
    // This is supposed to use QtXml to handle the hard part of reading xml files.
    return false;
}

bool DataCommon::save_file(const QString& UNUSED(path), QString& UNUSED(error))
{
    // This function MUST be overridden!
    // This is supposed to use QtXml to handle the hard part of writing xml files.
    return false;
}

void DataCommon::iterate_string_list(QDomElement &dom_element, const QString &tag, QStringList &result)
{
    // This is used while reading xml files.
    // It iterates through a given element while looking for a given tag and adds everything it finds to a given list.

    QStringList temp_string_list = QStringList();
    QDomNodeList temp_list = dom_element.elementsByTagName(tag);
    for (int var = 0; var < temp_list.size(); var++) {
        if (temp_list.at(var).isElement())
            temp_string_list.append(temp_list.at(var).toElement().text());
    }
    result = temp_string_list;
}

void DataCommon::get_colours(QDomElement &dom_element, QSharedPointer<Colour> &colour)
{
    // This is used while reading xml files.
    // It retrieves the colour data from the given element, whether it's a list or a preset, and passes the information into a given smart pointer to a colour object.

    QString attr = dom_element.attribute("values");
    if (!attr.isEmpty()) // Colour is a preset
        colour = QSharedPointer<Colour>(new Colour(attr));
    else { // Colour is a list of colours
        QStringList colours;
        iterate_string_list(dom_element, "colour", colours);
        colour = QSharedPointer<Colour>(new Colour(colours));
    }
}

void DataCommon::check_string(const QString &value, const QString &error_string)
{
    // This is used while writing xml files for validation.
    // Short and simple. Just checks if the passed string is empty or contains a null flag. It if it does, appends an error string to the error object.

    if (value.isNull() || value.isEmpty() || value == "NULL")
        this->error->append(error_string + " can not be empty!\n");
}

QDomElement DataCommon::write(QDomNode &parent, const QString &name, const QString &data, bool cdata)
{
    // This is used while writing xml files.
    // This is basically the write logic for the xml document.
    // It creates an element with the given name and appends it to the given parent.
    // Optionally, it also appends given data to the element and will wrap it in a CDATA node if cdata is true (defaults to false).

    auto element = document->createElement(name);
    if (!data.isNull() && !data.isEmpty() && data != "NULL") {
        if (cdata)
            element.appendChild(document->createCDATASection(data));
        else
            element.appendChild(document->createTextNode(data));
    }
    parent.appendChild(element);
    return element;
}

void DataCommon::write_colour(QDomNode &core, const QString &name, Colour *colour)
{
    // This is used while writing xml files.
    // It writes given colour data to an element with given name and adds it to the given core node.

    auto node = write(core, name);
    if (colour->getType() == Colour::Type::list)
        for (const QString &col : colour->getColours())
            write(node, "colour", col);
    else
        node.setAttribute("values", colour->getPreset());
}

QString DataCommon::bool_string(bool state)
{
    // This is used while writing xml files.
    // This simply converts a bool value to a string.

    if (state)
        return "true";
    return "false";
}

DataCommon::~DataCommon() = default;

DataCommon::Effect::Effect()
{
    effect_type + QString();
    potency = QString();
    primary_modifier = QString();
    secondary_modifier = QString();
    limit = 0;
    timer = 0;
}
