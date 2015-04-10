/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ClassSpaceSymbolTable.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ClassSpaceSymbolTable.h"
#include <algorithm>
#include "CompilationExceptions.h"
#include "TempPropertySymbolTable.h"

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

	addingclass_symbol = new PropertySymbolTable(&analyzer, module);
	addingclass_symbol->classname = name;
	addingclass_hassubclass = false;

	string fqname = (module.size() ? module + "." : "") + addingclass_name;
	classes[fqname] = pair<PropertySymbolTable*, bool>(addingclass_symbol, true);
	fullQualifications[addingclass_name] = fqname;

	return boost::optional<SemanticError*>();
}

boost::optional<SemanticError*> ClassSpaceSymbolTable::importClass(PropertySymbolTable* table) {
	string fqname = (table->getModule().size() ? table->getModule() + "." : "") + table->classname;
	if(classes.count(fqname) || fullQualifications.count(table->classname)) {
		return boost::optional<SemanticError*>(new SemanticError(MULTIPLE_CLASS_DEFINITION));
	}

	fullQualifications[fqname] = table->classname;
	classes[fqname] = pair<PropertySymbolTable*, bool>(table, false);
	return boost::optional<SemanticError*>();
}

PropertySymbolTable* ClassSpaceSymbolTable::getEmptyPropertySymbolTable() {
	return new PropertySymbolTable(&analyzer, module);
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

	childname = fullQualifications[childname];
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

void ClassSpaceSymbolTable::propagateInheritance(ErrorTracker& errors) {
	map<string, pair<PropertySymbolTable*, bool> > passed;
	for(map<string, pair<PropertySymbolTable*, bool> >::iterator it = classes.begin(); it != classes.end(); ++it) {
		inheritances_gathered[it->first] = pair<PropertySymbolTable*, bool>(it->second.first, false);
	}

	for(map<string, pair<PropertySymbolTable*, bool> >::iterator it = inheritances_gathered.begin(); it != inheritances_gathered.end(); ++it) {
		propagateInheritanceToParent(it->first, errors);
	}

	//for(map<string, pair<PropertySymbolTable*, bool> >::iterator it = inheritances_gathered.begin(); it != inheritances_gathered.end(); ++it) {
		//delete it->second;
	//}
}

void ClassSpaceSymbolTable::propagateInheritanceToParent(string childname, ErrorTracker& errors) {
	bool defined = classes.find(childname)->second.second;
	if(!defined) return;

	pair<PropertySymbolTable*, bool>* current = &inheritances_gathered.find(childname)->second;
	if(current->second) return; // Already propagated

	for(map<string, bool>::iterator it = current->first->parentage.begin(); it != current->first->parentage.end(); ++it) {
		propagateInheritanceToParent(it->first, errors);
		propagateInheritanceTables(current->first, findFullyQualifiedModifiable(it->first), it->second, errors);
	}

	current->second = true;
}

ReadOnlyPropertySymbolTable* ClassSpaceSymbolTable::findFullyQualified(string fqclassname, vector<PureType*> parameters) {
	PropertySymbolTable* table = findFullyQualifiedModifiable(fqclassname);
	if(!parameters.size()) {
		if(table->getParameters().size()) {
			SymbolNotFoundException* error = new SymbolNotFoundException();
			error->errormsg = "Type " + fqclassname + " cannot be used without type parameters";;
			throw error;
		}
		return new TempPropertySymbolTable(*table);
	} else {
		if(table->getParameters().size() != parameters.size()) {
			SymbolNotFoundException* error = new SymbolNotFoundException();
			error->errormsg = "Type " + fqclassname + " is used with the wrong number of type parameters";;
			throw error;
		} else {
			// TODO check lower and upper bounds on type parameters
			return table->resolveParameters(parameters);
		}
	}
}

ReadOnlyPropertySymbolTable* ClassSpaceSymbolTable::findFullyQualified(string fqclassname) {
	return findFullyQualifiedModifiable(fqclassname);
}

PropertySymbolTable* ClassSpaceSymbolTable::findFullyQualifiedModifiable(string fqclassname) {
	std::map<string, pair<PropertySymbolTable*, bool> >::iterator searcher = classes.find(fqclassname);
	if(!classes.count(fqclassname)) {
		SymbolNotFoundException* error = new SymbolNotFoundException();
		error->errormsg = "Could not find symbol ";
		error->errormsg += fqclassname;
		error->errormsg += " in class space";
		throw error;
	}

	return searcher->second.first;
}

void ClassSpaceSymbolTable::assertTypeIsValid(PureType* type) {
	if(type->type == TYPE_PARAMETERIZED) return;

	if(type->type == TYPE_CLASS) {
		if(classes.count(type->typedata._class.classname)) {
			std::map<string, pair<PropertySymbolTable*, bool> >::iterator searcher = classes.find(type->typedata._class.classname);
			PropertySymbolTable* table = searcher->second.first;
			vector<PureType*> parameterizations;

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
	} else if(type->type == TYPE_LAMBDA) {
		if(type->typedata.lambda.returntype != NULL)
			assertTypeIsValid(type->typedata.lambda.returntype);
		if(type->typedata.lambda.arguments == NULL) return;

		int i;
		for(i = 0; i < type->typedata.lambda.arguments->typecount; i++)
			assertTypeIsValid(type->typedata.lambda.arguments->types[i]);
	} else if(type->type == TYPE_LIST) {
		assertTypeIsValid(type->typedata.list.contained);
	} else if(type->type == TYPE_OPTIONAL) {
		assertTypeIsValid(type->typedata.optional.contained);
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
	for(map<string, pair<PropertySymbolTable*, bool> >::iterator it = classes.begin(); it != classes.end(); ++it)
	for(vector<SpecializableVarDecl*>::iterator needit = it->second.first->getNeeds()->begin(); needit != it->second.first->getNeeds()->end(); ++needit)
	if((*needit)->decl.typedata.type == TYPE_CLASS) {
		analyzer.assertFQNeedIsNotCircular(it->first, (*needit)->decl.typedata.getFQClassname());
	}
}

void ClassSpaceSymbolTable::setModule(string module) {
	this->module = module;
}

string ClassSpaceSymbolTable::getModule() {
	return module;
}

string ClassSpaceSymbolTable::getFullyQualifiedClassname(string classname) {
	PropertySymbolTable* table = findFullyQualifiedModifiable(classname);
	if(table->getModule().size()) {
		return table->getModule() + "." + classname;
	}

	return classname;
}
