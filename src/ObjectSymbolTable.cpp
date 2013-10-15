#include "ObjectSymbolTable.h"
#include <algorithm>
#include "CompilationExceptions.h"

ObjectSymbolTable::ObjectSymbolTable() {
	analyzer.reference = this;
}

ObjectSymbolTable::~ObjectSymbolTable() {
	for(map<string, pair<PropertySymbolTable*, string> >::iterator it = classes.begin(); it != classes.end(); ++it) {
		delete it->second.first;
	}
}

void ObjectSymbolTable::addClass(string name) {
	addingclass_name = name;

	if(classes.count(addingclass_name)) {
		throw new SemanticError(MULTIPLE_CLASS_DEFINITION);
	}

	addingclass_symbol = new PropertySymbolTable(&analyzer, &alloc);
	addingclass_hassubclass = false;

	classes[addingclass_name] = pair<PropertySymbolTable*, string>(addingclass_symbol, "a");
}

void ObjectSymbolTable::addInheritance(string childname, bool as_subclass) {
	if(addingclass_hassubclass && as_subclass) {
		throw new SemanticError(MORE_THAN_ONE_SUBCLASS);
	}

	if(childname == addingclass_name) {
		throw new SemanticError(SELF_INHERITANCE);
	}

	if(analyzer.isASubtypeOfB(childname, addingclass_name)) {
		throw new SemanticError(CIRCULAR_INHERITANCE);
	}

	if(addingclass_symbol->parentage.count(childname)) {
		throw new SemanticError(MULTIPLE_INHERITANCE);
	}

	addingclass_hassubclass = addingclass_symbol->parentage[childname] = as_subclass;
}

void ObjectSymbolTable::propagateInheritance() {
	map<string, pair<PropertySymbolTable*, bool> > passed;
	for(map<string, pair<PropertySymbolTable*, string> >::iterator it = classes.begin(); it != classes.end(); ++it) {
		inheritances_gathered[it->first] = pair<PropertySymbolTable*, bool>(it->second.first, false);
	}

	for(map<string, pair<PropertySymbolTable*, bool> >::iterator it = inheritances_gathered.begin(); it != inheritances_gathered.end(); ++it) {
		propagateInheritanceToParent(it->first);
	}

	//for(map<string, pair<PropertySymbolTable*, bool> >::iterator it = inheritances_gathered.begin(); it != inheritances_gathered.end(); ++it) {
		//delete it->second;
	//}
}

void ObjectSymbolTable::propagateInheritanceToParent(string childname) {
	pair<PropertySymbolTable*, bool>* current = &inheritances_gathered.find(childname)->second;
	if(current->second) return; // Already propagated

	for(map<string, bool>::iterator it = current->first->parentage.begin(); it != current->first->parentage.end(); ++it) {
		propagateInheritanceToParent(it->first);
		propagateInheritanceTables(current->first, find(it->first), it->second);
	}

	current->first->assignAddresses();

	current->second = true;
}

PropertySymbolTable* ObjectSymbolTable::find(string name) {
	std::map<string, pair<PropertySymbolTable*, string> >::iterator searcher = classes.find(name);
	if(!classes.count(name)) {
		SymbolNotFoundException* error = new SymbolNotFoundException();
		error->errormsg = "Could not find symbol ";
		error->errormsg += name;
		error->errormsg += " in object table";
		throw error;
	}

	return searcher->second.first;
}

void ObjectSymbolTable::assertTypeIsValid(Type* type) {
	if(type->type == TYPE_CLASS) {
		if(classes.count(type->typedata._class.classname)) return;

		throw new SemanticError(CLASSNAME_NOT_FOUND, type->typedata._class.classname + string(" is not a valid type"));
	} else {
		if(type->typedata.lambda.returntype != NULL)
			assertTypeIsValid(type->typedata.lambda.returntype);
		if(type->typedata.lambda.arguments == NULL) return;

		int i;
		for(i = 0; i < type->typedata.lambda.arguments->typecount; i++)
			assertTypeIsValid(type->typedata.lambda.arguments->types[i]);
	}
}

void ObjectSymbolTable::printEntryPoints(EntryPointAnalyzer* entryanalyzer) {
	for(map<string, pair<PropertySymbolTable*, string> >::iterator it = classes.begin(); it != classes.end(); ++it) {
		if(!entryanalyzer->checkClassNeedsCanBeMain(it->second.first->getNeeds())) continue;
		entryanalyzer->printClass(it->first);
		it->second.first->printEntryPoints(entryanalyzer);
	}
}

TypeAnalyzer* ObjectSymbolTable::getAnalyzer() {
	return &analyzer;
}
