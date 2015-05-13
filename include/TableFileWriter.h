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

class TableFileWriter {

	public:
		void write(ostream& out, PropertySymbolTable* table);
	private:
		void writeType(ostream& out, PureType<wake::QUALIFIED>* type);
		void writeProperty(ostream& out, ObjectProperty* property);
		void writeVarDecl(ostream& out, VarDecl<wake::QUALIFIED>& decl);
		void writeAnnotations(ostream& out, const boost::ptr_vector<Annotation>& annotations);
};

#endif
