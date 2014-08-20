/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * parseUtil.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_PARSEUTIL
#define HEADER_PARSEUTIL

#include <string.h>

char* unEscapeStringLiteral(char* in, char terminator);
char* keepInner(char* in);

#endif
