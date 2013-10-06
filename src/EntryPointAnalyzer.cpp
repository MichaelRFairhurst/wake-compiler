#include "EntryPointAnalyzer.h"
#include <iostream>
#include "ObjectSymbolTable.h"
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

bool EntryPointAnalyzer::checkMethodCanBeMain(string classname, string methodname, ObjectSymbolTable* table) {
	try {
		return checkMethodCanBeMain(methodname, table->find(classname)->get(methodname));
	} catch(SemanticError* e) {
		delete e;
		return false;
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
