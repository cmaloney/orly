/* <base/not_implemented.h>

   A replacement for <base/not_implemented_error.h>

   Use NOT_IMPLEMENTED() as a stub implementation for things which are not implemented. Allows you to get to full
   compilation of a module more quickly and easily determine where you need to work next to make something implemented.

   Uses std::logic_error instead of a custom exception. */

#include <stdexcept>

#include <base/code_location.h>

#define NOT_IMPLEMENTED()   throw std::logic_error("Not implemented at " HERE_STRING);
