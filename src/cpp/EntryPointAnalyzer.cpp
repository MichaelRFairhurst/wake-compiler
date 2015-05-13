/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * EntryPointAnalyzer.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "EntryPointAnalyzer.h"
#include <iostream>
#include "ClassSpaceSymbolTable.h"
#include "CompilationExceptions.h"

using namespace wake;

bool EntryPointAnalyzer::checkClassNeedsCanBeMain(vector<SpecializableVarDecl<QUALIFIED>*>* needs) {
	for(vector<SpecializableVarDecl<QUALIFIED>*>::iterator it = needs->begin(); it != needs->end(); ++it) {
		PureType<QUALIFIED>* type = &(*it)->decl.typedata;
		if(type->type == TYPE_LAMBDA) return false;
		if(type->type == TYPE_OPTIONAL) return false;
		if(type->type == TYPE_LIST) return false;
		if(type->type == TYPE_PARAMETERIZED) return false;
		if(type->type == TYPE_PARAMETERIZED_ARG) return false;
		if(type->typedata._class.classname == string("Int")) return false;
		if(type->typedata._class.classname == string("Text")) return false;
		if(type->typedata._class.classname == string("Truth")) return false;
	}

	return true;
}

bool EntryPointAnalyzer::checkFQClassMethodCanBeMain(string methodname, PureType<QUALIFIED>* method) {
	if(methodname.find("<-") != string::npos) return false;
	return method->typedata.lambda.arguments->typecount == 0;
}

bool EntryPointAnalyzer::checkFQClassMethodCanBeMain(string classname, string methodname, ClassSpaceSymbolTable* table) {
	try {
		boost::optional<PureType<QUALIFIED>*> method = table->findFullyQualified(classname)->find(methodname);
		return method ? checkFQClassMethodCanBeMain(methodname, *method) : false;
	} catch(SymbolNotFoundException* e) {
		delete e;
		return false;
	}
}

bool EntryPointAnalyzer::printClass(string classname) {
	cout << "Methods available for " << classname << endl;
}

bool EntryPointAnalyzer::printMethod(string methodname) {
	cout << "                            " << methodname << endl;
}
