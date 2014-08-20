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

bool EntryPointAnalyzer::checkClassNeedsCanBeMain(vector<Type*>* needs) {
	for(vector<Type*>::iterator it = needs->begin(); it != needs->end(); ++it) {
		if((*it)->type == TYPE_LAMBDA) return false;
		if((*it)->arrayed) return false;
		if((*it)->typedata._class.classname == string("Int")) return false;
		if((*it)->typedata._class.classname == string("Text")) return false;
		if((*it)->typedata._class.classname == string("Truth")) return false;
	}

	return true;
}

bool EntryPointAnalyzer::checkMethodCanBeMain(string methodname, Type* method) {
	if(methodname.find("<-") != string::npos) return false;
	return method->typedata.lambda.arguments->typecount == 0;
}

bool EntryPointAnalyzer::checkMethodCanBeMain(string classname, string methodname, ClassSpaceSymbolTable* table) {
	try {
		boost::optional<Type*> method = table->find(classname)->find(methodname);
		return method ? checkMethodCanBeMain(methodname, *method) : false;
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
