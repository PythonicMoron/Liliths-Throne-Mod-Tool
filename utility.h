#ifndef UTILITY_H
#define UTILITY_H

// Macro to mark function parameters as unused.
#define UNUSED(expr) /*expr*/

#include <QString>

// Just some generic function(s) commonly used.
namespace Utility
{
    void error(const QString& error);
};

#endif // UTILITY_H
