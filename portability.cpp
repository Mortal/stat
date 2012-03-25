#include <iostream>
#include <string>
#include "portability.h"

#ifdef WIN32

bool is_interactive() {
    return true;
}

const char * eof_keystroke = "Ctrl-Z, enter";

void display_results() {
    std::cout << "Bye bye." << std::endl;
    std::string line;
    std::getline(std::cin, line);
}

#else

#include <unistd.h>
bool is_interactive() {
    return isatty(0);
}

const char * eof_keystroke = "Ctrl-D";

void display_results() {
    std::cout << "Bye bye." << std::endl;
}

#endif
// vim: set sw=4 sts=4 ts=8 noet:
