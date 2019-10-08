#include "notimplementedexception.h"

NotImplementedException::NotImplementedException() : std::logic_error("Call to unimplemented member function!") {}
