/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TableFileReader.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_TABLE_FILE_READER
#define HEADER_TABLE_FILE_READER

#include <istream>
#include <string>
#include "PropertySymbolTable.h"
#include "PureType.h"

class TableFileReader {

	public:
		void read(PropertySymbolTable* table, istream& s);

	private:
		std::string readString(istream& s);
		char* readCString(istream& s);
		unsigned char readUInt8(istream& s);
		double readNum64(istream& s);
		void readMethod(PropertySymbolTable* table, istream& s);
		void readInheritance(PropertySymbolTable* table, istream& s);
		PureType* readType(istream& s);
		void readTypeCommon(PureType* type, istream& s);
		PureType* readPureTypeByTag(int tag, istream& s);
		PureType* readClassType(istream& s);
		PureType* readLambdaType(istream& s);
		PureType* readParameterizedType(istream& s, int tag);
		PureType* readListType(istream& s);
		PureType* readOptionalType(istream& s);
		vector<Annotation*> readAnnotations(istream& s);
		Annotation* readAnnotation(istream& s);

};

#endif
