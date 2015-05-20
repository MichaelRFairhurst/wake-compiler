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
#include "VarDecl.h"
#include "SpecializableVarDecl.h"

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
		VarDecl<wake::QUALIFIED>* readVarDecl(istream& s);
		SpecializableVarDecl<wake::QUALIFIED>* readSpecializableVarDecl(istream& s);
		PureType<wake::QUALIFIED>* readType(istream& s);
		PureType<wake::QUALIFIED>* readTypeByTag(int tag, istream& s);
		PureType<wake::QUALIFIED>* readPureTypeByTag(int tag, istream& s);
		PureType<wake::QUALIFIED>* readClassType(istream& s);
		PureType<wake::QUALIFIED>* readLambdaType(istream& s);
		PureType<wake::QUALIFIED>* readParameterizedType(istream& s, int tag);
		PureType<wake::QUALIFIED>* readListType(istream& s);
		PureType<wake::QUALIFIED>* readOptionalType(istream& s);
		vector<Annotation*> readAnnotations(istream& s);
		Annotation* readAnnotation(istream& s);

};

#endif
