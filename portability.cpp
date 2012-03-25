#include "portability.h"

#ifdef WIN32

bool is_interactive() {
    return true;
}

#else

#include <unistd.h>
bool is_interactive() {
    return isatty(0);
}

#endif
// vim: set sw=4 sts=4 ts=8 noet:
