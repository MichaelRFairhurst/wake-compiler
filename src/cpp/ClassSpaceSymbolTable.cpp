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

	string fqname = (module.size() ? module + "." : "") + addingclass_name;
	if(classes.count(fqname)) {
		return boost::optional<SemanticError*>(new SemanticError(MULTIPLE_CLASS_DEFINITION));
	}

	addingclass_symbol = new PropertySymbolTable(&analyzer, module);
	addingclass_symbol->classname = name;
	addingclass_hassubclass = false;

	classes[fqname] = pair<PropertySymbolTable*, bool>(addingclass_symbol, true);
	fullQualifications[addingclass_name] = fqname;

	return boost::optional<SemanticError*>();
}

void ClassSpaceSymbolTable::prepImport(string modulename, string classname) {
	if(modulename == "") {
		fullQualifications[classname] = classname;
	} else {
		fullQualifications[classname] = modulename + "." + classname;
	}
}

boost::optional<SemanticError*> ClassSpaceSymbolTable::importClass(PropertySymbolTable* table) {
	string fqname = (table->getModule().size() ? table->getModule() + "." : "") + table->classname;
	if(classes.count(fqname) /* || fullQualifications.count(table->classname) this is redundant? And prevents preloading the qualifications */) {
		return boost::optional<SemanticError*>(new SemanticError(MULTIPLE_CLASS_DEFINITION));
	}

	fullQualifications[table->classname] = fqname;
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
	string fqname = fullQualifications[addingclass_name];

	if(fullQualifications.count(childname)) {
		// careful! undefined behavior if we don't check this exists first
		childname = fullQualifications[childname];
	} else {
		// either they forgot to import it, or they are about to define it
		// which means its in the current module. Continue as if its just
		// not defined yet, it will fail later if they don't
		childname = module.size() ? module + "." + childname : childname;
	}

	if(addingclass_hassubclass && as_subclass) {
		return boost::optional<SemanticError*>(new SemanticError(MORE_THAN_ONE_SUBCLASS));
	}

	if(childname == fqname) {
		return boost::optional<SemanticError*>(new SemanticError(SELF_INHERITANCE));
	}

	if(analyzer.isASubtypeOfB(childname, fqname)) {
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

ReadOnlyPropertySymbolTable* ClassSpaceSymbolTable::findFullyQualified(string fqclassname, vector<PureType<wake::QUALIFIED>*> parameters) {
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
	return new TempPropertySymbolTable(*findFullyQualifiedModifiable(fqclassname));
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

void ClassSpaceSymbolTable::assertTypeIsValid(PureType<wake::QUALIFIED>* type) {
	if(type->type == TYPE_PARAMETERIZED) return;

	if(type->type == TYPE_CLASS) {
		if(classes.count(type->getFQClassname())) {
			std::map<string, pair<PropertySymbolTable*, bool> >::iterator searcher = classes.find(type->getFQClassname());
			PropertySymbolTable* table = searcher->second.first;
			vector<PureType<wake::QUALIFIED>*> parameterizations;

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
	for(vector<SpecializableVarDecl<wake::QUALIFIED>*>::iterator needit = it->second.first->getNeeds()->begin(); needit != it->second.first->getNeeds()->end(); ++needit)
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
	if(fullQualifications.count(classname)) {
		return fullQualifications[classname];
	} else {
		SymbolNotFoundException* error = new SymbolNotFoundException();
		error->errormsg = "Type " + classname + " was not found in loaded class space";;
		throw error;
	}
}

PureType<wake::QUALIFIED>* ClassSpaceSymbolTable::setModulesOnType(PureType<wake::UNQUALIFIED>* type) {
	if(type->type == TYPE_CLASS) {
		if(type->typedata._class.modulename == NULL) {
			try {
				PropertySymbolTable* fullSpec = findByImportedNameModifiable(type->typedata._class.classname);
				type->typedata._class.modulename = strdup(fullSpec->getModule().c_str());
			} catch(SymbolNotFoundException* e) {
				delete e;
			}
		}

		if(type->typedata._class.parameters != NULL) {
			for (int i = 0; i < type->typedata._class.parameters->typecount; i++) {
				setModulesOnType(type->typedata._class.parameters->types[i]);
			}
		}
	} else if(type->type == TYPE_OPTIONAL) {
		setModulesOnType(type->typedata.optional.contained);
	} else if(type->type == TYPE_LIST) {
		setModulesOnType(type->typedata.list.contained);
	} else if(type->type == TYPE_PARAMETERIZED || type->type == TYPE_PARAMETERIZED_ARG) {
		if(type->typedata.parameterized.upperbound != NULL) {
			setModulesOnType(type->typedata.parameterized.upperbound);
		}
		if(type->typedata.parameterized.lowerbound != NULL) {
			setModulesOnType(type->typedata.parameterized.lowerbound);
		}
	} else if(type->type == TYPE_LAMBDA) {
		if(type->typedata.lambda.returntype != NULL) {
			setModulesOnType(type->typedata.lambda.returntype);
		}

		if(type->typedata.lambda.arguments != NULL) {
			for (int i = 0; i < type->typedata.lambda.arguments->typecount; i++) {
				setModulesOnType(type->typedata.lambda.arguments->types[i]);
			}
		}
	}

	return (PureType<wake::QUALIFIED>*) type;
}

SpecializablePureType<wake::QUALIFIED>* ClassSpaceSymbolTable::setModulesOnType(SpecializablePureType<wake::UNQUALIFIED>* type) {
	setModulesOnType(&type->typedata);
	return (SpecializablePureType<wake::QUALIFIED>*) type;
}

SpecializableVarDecl<wake::QUALIFIED>* ClassSpaceSymbolTable::setModulesOnType(SpecializableVarDecl<wake::UNQUALIFIED>* type) {
	setModulesOnType(&type->decl);
	return (SpecializableVarDecl<wake::QUALIFIED>*) type;
}

VarDecl<wake::QUALIFIED>* ClassSpaceSymbolTable::setModulesOnType(VarDecl<wake::UNQUALIFIED>* type) {
	setModulesOnType(&type->typedata);
	return (VarDecl<wake::QUALIFIED>*) type;
}


PropertySymbolTable* ClassSpaceSymbolTable::findByImportedNameModifiable(string classname) {
	return findFullyQualifiedModifiable(getFullyQualifiedClassname(classname));
}

ReadOnlyPropertySymbolTable* ClassSpaceSymbolTable::findByImportedName(string classname) {
	return findFullyQualified(getFullyQualifiedClassname(classname));
}

ReadOnlyPropertySymbolTable* ClassSpaceSymbolTable::findByImportedName(string classname, vector<PureType<wake::QUALIFIED>*> args) {
	return findFullyQualified(getFullyQualifiedClassname(classname), args);

}
