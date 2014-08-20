/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * objectfile.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_OBJECTFILE
#define HEADER_OBJECTFILE

typedef struct {
	char* symbol;
	int pos;
} symbolusage;

typedef struct {
	int classusagecount;
	symbolusage** classusages;
	int propertyusagecount;
	symbolusage** propertyusages;
	char* filename;
} objectfile;

#endif
