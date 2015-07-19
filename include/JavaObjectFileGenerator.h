/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ObjectFileGenerator.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_JAVA_OBJECT_GENERATOR
#define HEADER_JAVA_OBJECT_GENERATOR

#include <iostream>

#include "tree.h"

#include "ScopeSymbolTable.h"
#include "ClassSpaceSymbolTable.h"
#include "TypeAnalyzer.h"

using namespace std;

class JavaObjectFileGenerator {

	public:
		JavaObjectFileGenerator(ostream& file, ClassSpaceSymbolTable* classes) : file(file) {this->classes = classes; forceArrayIdentifier = false; }
		void generate(Node* tree);

	private:
		void generateInterface(Node* tree);
		void generateEarlyBailoutTemporaries(Node* tree);
		string toJavaTypeInformation(PureType<wake::QUALIFIED> type, bool forceBoxedTypes = false);
		string toJavaTypeInformation(PureType<wake::UNQUALIFIED> type);
		string toJavaIdentifier(const VarRef& ref);
		string toJavaProvisionSymbol(string wakesymbol);
		bool forceArrayIdentifier;
		TypeAnalyzer typeanalyzer;
		ClassSpaceSymbolTable* classes;
		ScopeSymbolTable table;
		ostream& file;
		string classname;

};

#endif
