#include "portability.h"

#ifdef WIN32

bool is_interactive() {
    return true;
}

const char * eof_keystroke = "Ctrl-Z, enter";

#else

#include <unistd.h>
bool is_interactive() {
    return isatty(0);
}

const char * eof_keystroke = "Ctrl-D";

#endif
// vim: set sw=4 sts=4 ts=8 noet:
