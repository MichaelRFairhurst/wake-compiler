/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TableFileWriter.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_TABLE_FILE_WRITER
#define HEADER_TABLE_FILE_WRITER

#include <iostream>
#include "PropertySymbolTable.h"
extern "C" {
	#include "type.h"
}

class TableFileWriter {

	public:
		void write(ostream& out, PropertySymbolTable* table);
	private:
		void writeType(ostream& out, Type* type);
		void writeProperty(ostream& out, ObjectProperty* property);
};

#endif
