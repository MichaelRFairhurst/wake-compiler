/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ObjectFileHeaderRenderer.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_OBJECT_FILE_HEADER_RENDERER
#define HEADER_OBJECT_FILE_HEADER_RENDERER

#include <iostream>
#include "ObjectFileHeaderData.h"

class ObjectFileHeaderRenderer {

	public:
		void writeOut(std::ostream& out, ObjectFileHeaderData* data);
};

#endif
