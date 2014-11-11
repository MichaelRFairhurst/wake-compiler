/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * PropertySymbolTable.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "PropertySymbolTable.h"
#include "SemanticError.h"
#include "DerivedPropertySymbolTable.h"
#include "TypeParameterizer.h"

PropertySymbolTable::PropertySymbolTable(TypeAnalyzer* analyzer) {
	this->analyzer = analyzer;
	abstract = false;
	declaredtypeparameters = new vector<Type*>(); // @TODO this is a hack
	needs = new vector<Type*>(); // @TODO so is this (kind of)
}

const map<string, bool>& PropertySymbolTable::getParentage() {
	return parentage;
}

boost::optional<SemanticError*> PropertySymbolTable::addMethod(Type* returntype, vector<pair<string, TypeArray*> >* segments_arguments, int flags, vector<Annotation*> annotations) {
	string name = getSymbolNameOf(segments_arguments);

	if(properties.count(name)) {
		string temp = "duplicate method signature is " + name;
		return boost::optional<SemanticError*>(new SemanticError(MULTIPLE_METHOD_DEFINITION, temp));
	}

	if(flags & PROPERTY_ABSTRACT) {
		abstract = true;
	}

	Type* method = MakeType(TYPE_LAMBDA);
	method->typedata.lambda.returntype = returntype == NULL ? NULL : new Type(*returntype);

	TypeArray* conglomerate = MakeTypeArray();
	for(vector<pair<string, TypeArray*> >::iterator it = segments_arguments->begin(); it != segments_arguments->end(); ++it) {
		int i;
		for(i = 0; i < it->second->typecount; i++)
			AddTypeToTypeArray(copyType(it->second->types[i]), conglomerate);
	}

	method->typedata.lambda.arguments = conglomerate;

	ObjectProperty* prop = new ObjectProperty;
	prop->flags = flags;
	prop->type = method;
	prop->casing = getCasingNameOf(segments_arguments);
	prop->address = name;
	prop->annotations = boost::ptr_vector<Annotation*>(annotations.begin(), annotations.end());

	properties[name] = prop;

	return boost::optional<SemanticError*>();
}

boost::optional<SemanticError*> PropertySymbolTable::addProperty(Type* property, int flags, vector<Annotation*> annotations) {
	string name = analyzer->getNameForTypeAsProperty(property);

	if(properties.count(name)) {
		string temp = "duplicate property name is " + name;
		return boost::optional<SemanticError*>(new SemanticError(DUPLICATE_PROPERTY_DEFINITION, temp));
	}

	ObjectProperty* prop = new ObjectProperty;
	prop->casing = name;
	prop->flags = flags;
	prop->type = property;
	prop->address = name;
	prop->annotations = boost::ptr_vector<Annotation*>(annotations.begin(), annotations.end());

	properties[name] = prop;
	return boost::optional<SemanticError*>();
}

boost::optional<SemanticError*> PropertySymbolTable::addProvision(Type* provided, vector<Type*> &arguments, int flags) {
	string name = getProvisionSymbol(provided, arguments);

	if(properties.count(name)) {
		string temp = "duplicate provision is " + name;
		return boost::optional<SemanticError*>(new SemanticError(DUPLICATE_PROPERTY_DEFINITION, temp));
	}

	Type* method = MakeType(TYPE_LAMBDA);
	method->typedata.lambda.returntype = copyType(provided);

	method->typedata.lambda.arguments = MakeTypeArray(); //TODO injections with curried ctors or arguments!

	ObjectProperty* prop = new ObjectProperty;
	prop->type = method;
	prop->casing = name;
	prop->address = name;
	prop->flags = flags;

	properties[name] = prop;
	return boost::optional<SemanticError*>();
}

boost::optional<Type*> PropertySymbolTable::find(string name) {
	if(!properties.count(name))
		//throw new SemanticError(PROPERTY_OR_METHOD_NOT_FOUND, "Cannot find method with signature " + name);
		return boost::optional<Type*>();

	return boost::optional<Type*>(properties.find(name)->second->type);
}

string PropertySymbolTable::getAddress(string name) {
	return properties.find(name)->second->address;
}

string PropertySymbolTable::getProvisionSymbol(Type* provided, vector<Type*> &arguments) {
	return analyzer->getProvisionSymbol(provided, arguments);
}

string PropertySymbolTable::getCasingNameOf(vector<pair<string, TypeArray*> >* segments_arguments) {
	string name;
	for(vector<pair<string, TypeArray*> >::iterator it = segments_arguments->begin(); it != segments_arguments->end(); ++it) {
		name += it->first;
		name += "(";
		int i;
		for(i = 0; i < it->second->typecount; i++) {
			if(i) name += ",";
			name += '#';
		}
		name += ")";
	}

	return name;
}

string PropertySymbolTable::getSymbolNameOf(vector<pair<string, TypeArray*> >* segments_arguments) {
	string name;
	for(vector<pair<string, TypeArray*> >::iterator it = segments_arguments->begin(); it != segments_arguments->end(); ++it) {
		name += it->first;
		name += "(";
		int i;
		for(i = 0; i < it->second->typecount; i++) {
			if(i) name += ",";
			name += analyzer->getNameForType(it->second->types[i]);
		}
		name += ")";
	}

	return name;
}

void PropertySymbolTable::printEntryPoints(EntryPointAnalyzer* entryanalyzer) {
	for(map<string, ObjectProperty*>::iterator it = properties.begin(); it != properties.end(); ++it) {
		if(entryanalyzer->checkMethodCanBeMain(it->first, it->second->type))
			entryanalyzer->printMethod(it->first);
	}
}

void PropertySymbolTable::addNeed(Type* needed, int flags, vector<Annotation*> annotations) {
	needs->push_back(needed);
	addProperty(needed, flags | PROPERTY_NEED, annotations);
}

vector<Type*>* PropertySymbolTable::getNeeds() {
	return needs;
}

bool PropertySymbolTable::isAbstract() {
	return abstract;
}

PropertySymbolTable::~PropertySymbolTable() {
	for(map<string, ObjectProperty*>::iterator it = properties.begin(); it != properties.end(); ++it) {
		delete it->second;
	}

	delete needs;
	delete declaredtypeparameters;
}

void propagateInheritanceTables(PropertySymbolTable* child, PropertySymbolTable* parent, bool extend, ErrorTracker& errors) {
	for(map<string, ObjectProperty*>::iterator it = parent->properties.begin(); it != parent->properties.end(); ++it) {
		if(!extend && !(it->second->flags & PROPERTY_PUBLIC)) continue;
		map<string, ObjectProperty*>::iterator searcher = child->properties.find(it->first);
		if(searcher == child->properties.end()) {
			ObjectProperty* propagate = new ObjectProperty;
			propagate->type = copyType(it->second->type);
			propagate->flags = it->second->flags;
			propagate->casing = it->second->casing;
			propagate->address = it->second->address;
			if(!extend) {
				propagate->flags |= PROPERTY_ABSTRACT;
				child->abstract = true;
			} else if(propagate->flags & PROPERTY_ABSTRACT) {
				child->abstract = true;
			}
			child->properties[it->first] = propagate;

			if(extend && propagate->flags & PROPERTY_NEED) {
				child->analyzer->assertNeedIsNotCircular(child->classname, propagate->type);
				child->getNeeds()->push_back(propagate->type);
			}

		} else {
			searcher->second->address = it->second->address;
			if(searcher->second->type->type == TYPE_LAMBDA)
			if(it->second->type->typedata.lambda.returntype != NULL)
			if(searcher->second->type->typedata.lambda.returntype == NULL
				|| !child->analyzer->isASubtypeOfB(
					searcher->second->type->typedata.lambda.returntype,	// this is contravariance
					it->second->type->typedata.lambda.returntype 		// Check parent return is a subtype of child return
				)
			) {
				errors.addError(new SemanticError(INVALID_CHILD_RETURN_TYPE, "method " + searcher->second->address + " on class " + child->classname, NULL, ""));
			}
		}
	}
}

bool PropertySymbolTable::isPublic(string name) {
	return properties.find(name)->second->flags & PROPERTY_PUBLIC;
}

void PropertySymbolTable::setParameters(vector<Type*>* parameters) {
	delete declaredtypeparameters;
	declaredtypeparameters = parameters;
}

const vector<Type*>& PropertySymbolTable::getParameters() {
	return *declaredtypeparameters;
}

ReadOnlyPropertySymbolTable* PropertySymbolTable::resolveParameters(vector<Type*>& parameters) {
	TypeParameterizer parameterizer;
	map<string, ObjectProperty*>* newprops = new map<string, ObjectProperty*>();
	for(map<string, ObjectProperty*>::iterator it = properties.begin(); it != properties.end(); ++it) {
		string casing, oldcasing = it->second->casing;
		ObjectProperty* newprop = new ObjectProperty();
		newprop->type = copyType(it->second->type);
		newprop->flags = it->second->flags;
		newprop->address = it->second->address;
		parameterizer.applyParameterizations(&newprop->type, getParameters(), parameters);
		int i = 0, c = 0;
		while(c < oldcasing.length()) {
			if(oldcasing.at(c) == '#') {
				casing += analyzer->getNameForType(newprop->type->typedata.lambda.arguments->types[i]);
				i++;
			} else {
				casing += oldcasing.at(c);
			}
			c++;
		}
		newprop->casing = casing;
		(*newprops)[casing] = newprop;
	}

	vector<Type*>* newneeds = new vector<Type*>();
	for(vector<Type*>::iterator it = needs->begin(); it != needs->end(); ++it) {
		Type* newneed = copyType(*it);
		parameterizer.applyParameterizations(&newneed, getParameters(), parameters);
		newneeds->push_back(newneed);
	}

	return new DerivedPropertySymbolTable(*analyzer, newneeds, newprops, parentage);
}

Type* PropertySymbolTable::getAsType() {
	Type* ret = MakeType(TYPE_CLASS);
	ret->typedata._class.classname = strdup(classname.c_str());
	if(getParameters().size()) {
		ret->typedata._class.parameters = MakeTypeArray();
		for(vector<Type*>::const_iterator it = getParameters().begin(); it != getParameters().end(); ++it) {
			AddTypeToTypeArray(*it, ret->typedata._class.parameters);
		}
	}

	return ret;
}

bool PropertySymbolTable::isBehavioralProvision(string name) {
	return properties.find(name)->second->flags & PROPERTY_BLOCKPROVISION;
}
