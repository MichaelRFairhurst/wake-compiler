#include "ClassSpaceSymbolTable.h"
#include <algorithm>
#include "CompilationExceptions.h"

ClassSpaceSymbolTable::ClassSpaceSymbolTable() {
	analyzer.reference = this;
}

ClassSpaceSymbolTable::~ClassSpaceSymbolTable() {
	for(map<string, pair<PropertySymbolTable*, bool> >::iterator it = classes.begin(); it != classes.end(); ++it) {
		delete it->second.first;
	}
}

boost::optional<SemanticError*> ClassSpaceSymbolTable::addClass(string name) {
	addingclass_name = name;

	if(classes.count(addingclass_name)) {
		return boost::optional<SemanticError*>(new SemanticError(MULTIPLE_CLASS_DEFINITION));
	}

	addingclass_symbol = new PropertySymbolTable(&analyzer);
	addingclass_symbol->classname = name;
	addingclass_hassubclass = false;

	classes[addingclass_name] = pair<PropertySymbolTable*, bool>(addingclass_symbol, true);

	return boost::optional<SemanticError*>();
}

boost::optional<SemanticError*> ClassSpaceSymbolTable::importClass(PropertySymbolTable* table) {
	if(classes.count(addingclass_name)) {
		return boost::optional<SemanticError*>(new SemanticError(MULTIPLE_CLASS_DEFINITION));
	}

	classes[table->classname] = pair<PropertySymbolTable*, bool>(table, false);
	return boost::optional<SemanticError*>();
}

PropertySymbolTable* ClassSpaceSymbolTable::getEmptyPropertySymbolTable() {
	return new PropertySymbolTable(&analyzer);
}

vector<PropertySymbolTable*> ClassSpaceSymbolTable::getDefinedClasses() {
	vector<PropertySymbolTable*> response;

	for(map<string, pair<PropertySymbolTable*, bool> >::iterator it = classes.begin(); it != classes.end(); ++it) {
		if(it->second.second) response.push_back(it->second.first);
	}

	return response;
}

boost::optional<SemanticError*> ClassSpaceSymbolTable::addInheritance(string childname, bool as_subclass) {
	if(addingclass_hassubclass && as_subclass) {
		return boost::optional<SemanticError*>(new SemanticError(MORE_THAN_ONE_SUBCLASS));
	}

	if(childname == addingclass_name) {
		return boost::optional<SemanticError*>(new SemanticError(SELF_INHERITANCE));
	}

	if(analyzer.isASubtypeOfB(childname, addingclass_name)) {
		return boost::optional<SemanticError*>(new SemanticError(CIRCULAR_INHERITANCE));
	}

	if(addingclass_symbol->parentage.count(childname)) {
		return boost::optional<SemanticError*>(new SemanticError(MULTIPLE_INHERITANCE));
	}

	addingclass_hassubclass = addingclass_symbol->parentage[childname] = as_subclass;

	return boost::optional<SemanticError*>();
}

void ClassSpaceSymbolTable::propagateInheritance() {
	map<string, pair<PropertySymbolTable*, bool> > passed;
	for(map<string, pair<PropertySymbolTable*, bool> >::iterator it = classes.begin(); it != classes.end(); ++it) {
		inheritances_gathered[it->first] = pair<PropertySymbolTable*, bool>(it->second.first, false);
	}

	for(map<string, pair<PropertySymbolTable*, bool> >::iterator it = inheritances_gathered.begin(); it != inheritances_gathered.end(); ++it) {
		propagateInheritanceToParent(it->first);
	}

	//for(map<string, pair<PropertySymbolTable*, bool> >::iterator it = inheritances_gathered.begin(); it != inheritances_gathered.end(); ++it) {
		//delete it->second;
	//}
}

void ClassSpaceSymbolTable::propagateInheritanceToParent(string childname) {
	bool defined = classes.find(childname)->second.second;
	if(!defined) return;

	pair<PropertySymbolTable*, bool>* current = &inheritances_gathered.find(childname)->second;
	if(current->second) return; // Already propagated

	for(map<string, bool>::iterator it = current->first->parentage.begin(); it != current->first->parentage.end(); ++it) {
		propagateInheritanceToParent(it->first);
		propagateInheritanceTables(current->first, findModifiable(it->first), it->second);
	}

	current->second = true;
}

ReadOnlyPropertySymbolTable* ClassSpaceSymbolTable::find(Type* type) {
	PropertySymbolTable* table = findModifiable(type);
	if(type->typedata._class.parameters == NULL) {
		if(table->getParameters().size()) {
			SymbolNotFoundException* error = new SymbolNotFoundException();
			error->errormsg = "Type " + string(type->typedata._class.classname) + " cannot be used without type parameters";;
			throw error;
		}
		return table;
	} else {
		if(table->getParameters().size() != type->typedata._class.parameters->typecount) {
			SymbolNotFoundException* error = new SymbolNotFoundException();
			error->errormsg = "Type " + string(type->typedata._class.classname) + " is used with the wrong number of type parameters";;
			throw error;
		} else {
			// TODO check lower and upper bounds on type parameters
			vector<Type*> temp;
			for(int i = 0; i < type->typedata._class.parameters->typecount; i++) {
				temp.push_back(type->typedata._class.parameters->types[i]);
			}
			return table->resolveParameters(temp);
		}
	}
}

ReadOnlyPropertySymbolTable* ClassSpaceSymbolTable::find(string name) {
	return findModifiable(name);
}

PropertySymbolTable* ClassSpaceSymbolTable::findModifiable(Type* type) {
	return findModifiable(type->typedata._class.classname);
}

PropertySymbolTable* ClassSpaceSymbolTable::findModifiable(string name) {
	std::map<string, pair<PropertySymbolTable*, bool> >::iterator searcher = classes.find(name);
	if(!classes.count(name)) {
		SymbolNotFoundException* error = new SymbolNotFoundException();
		error->errormsg = "Could not find symbol ";
		error->errormsg += name;
		error->errormsg += " in object table";
		throw error;
	}

	return searcher->second.first;
}

void ClassSpaceSymbolTable::assertTypeIsValid(Type* type) {
	if(type->type == TYPE_PARAMETERIZED) return;

	if(type->type == TYPE_CLASS) {
		if(classes.count(type->typedata._class.classname)) {
			std::map<string, pair<PropertySymbolTable*, bool> >::iterator searcher = classes.find(type->typedata._class.classname);
			PropertySymbolTable* table = searcher->second.first;
			vector<Type*> parameterizations;

			if(type->typedata._class.parameters != NULL) {
				int i;
				for(i = 0; i < type->typedata._class.parameters->typecount; i++) {
					assertTypeIsValid(type->typedata._class.parameters->types[i]);
					parameterizations.push_back(type->typedata._class.parameters->types[i]);
				}
			}

			// @TODO check upper and lower bounds against parameterizations

			if(table->getParameters().size() != parameterizations.size()) {
				throw new SemanticError(INVALID_GENERIC_TYPE, type->typedata._class.classname + string(" requires fewer type parameters, or more, or none."));
			}
			return;
		}

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

void ClassSpaceSymbolTable::printEntryPoints(EntryPointAnalyzer* entryanalyzer) {
	for(map<string, pair<PropertySymbolTable*, bool> >::iterator it = classes.begin(); it != classes.end(); ++it) {
		if(!entryanalyzer->checkClassNeedsCanBeMain(it->second.first->getNeeds())) continue;
		entryanalyzer->printClass(it->first);
		it->second.first->printEntryPoints(entryanalyzer);
	}
}

TypeAnalyzer* ClassSpaceSymbolTable::getAnalyzer() {
	return &analyzer;
}

void ClassSpaceSymbolTable::assertNoNeedsAreCircular() {
	for(map<string, pair<PropertySymbolTable*, bool> >::iterator it = classes.begin(); it != classes.end(); ++it) {
		for(vector<Type*>::iterator needit = it->second.first->getNeeds()->begin(); needit != it->second.first->getNeeds()->end(); ++needit) {
			analyzer.assertNeedIsNotCircular(it->first, *needit);
		}
	}
}
