#ifndef HEADER_PARSEUTIL
#define HEADER_PARSEUTIL

#include <string.h>

char* unEscapeStringLiteral(char* in, char terminator);
char* keepInner(char* in);

#endif
