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
#include "VarRef.h"
#include "VarDecl.h"
#include "SpecializableVarDecl.h"
#include "SpecializablePureType.h"

#include <iostream>

const map<string, bool>& PropertySymbolTable::getParentage() {
	return parentage;
}

boost::optional<SemanticError*> PropertySymbolTable::addMethod(PureType<wake::QUALIFIED>* returntype, vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >* segments_arguments, int flags, vector<Annotation*> annotations) {
	string name = getSymbolNameOf(segments_arguments);

	if(properties.count(name)) {
		string temp = "duplicate method signature is " + name;
		return boost::optional<SemanticError*>(new SemanticError(MULTIPLE_METHOD_DEFINITION, temp));
	}

	if(flags & PROPERTY_ABSTRACT) {
		abstract = true;
	}

	PureType<wake::QUALIFIED>* method = new PureType<wake::QUALIFIED>(TYPE_LAMBDA);
	method->typedata.lambda.returntype = returntype == NULL ? NULL : new PureType<wake::QUALIFIED>(*returntype);

	PureTypeArray<wake::QUALIFIED>* conglomerate = new PureTypeArray<wake::QUALIFIED>();
	for(vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >::iterator it = segments_arguments->begin(); it != segments_arguments->end(); ++it) {
		int i;
		for(i = 0; i < it->second->typecount; i++)
			// the templates are just a typesafety thing, we can safely cross cast them
			addPureTypeToPureTypeArray(
				new PureType<wake::UNQUALIFIED>(*(PureType<wake::UNQUALIFIED>*) it->second->types[i]),
				(PureTypeArray<wake::UNQUALIFIED>*) conglomerate
			);
	}

	method->typedata.lambda.arguments = conglomerate;

	ObjectProperty* prop = new ObjectProperty;
	prop->flags = flags;
	prop->decl.typedata = *method;
	prop->casing = getCasingNameOf(segments_arguments);
	prop->address = name;
	for(vector<Annotation*>::iterator annit = annotations.begin(); annit != annotations.end(); ++annit)
		prop->annotations.push_back(*annit);

	properties[name] = prop;

	return boost::optional<SemanticError*>();
}

boost::optional<SemanticError*> PropertySymbolTable::addProperty(VarDecl<wake::QUALIFIED>* property, int flags, vector<Annotation*> annotations) {
	VarRef refOfDecl = property->createVarRef();
	string name = refOfDecl.toString();

	if(properties.count(name)) {
		string temp = "duplicate property name is " + name;
		return boost::optional<SemanticError*>(new SemanticError(DUPLICATE_PROPERTY_DEFINITION, temp));
	}

	ObjectProperty* prop = new ObjectProperty;
	prop->casing = name;
	prop->flags = flags;
	prop->decl = *property;
	prop->address = name;
	for(vector<Annotation*>::iterator annit = annotations.begin(); annit != annotations.end(); ++annit)
		prop->annotations.push_back(*annit);

	properties[name] = prop;
	return boost::optional<SemanticError*>();
}

boost::optional<SemanticError*> PropertySymbolTable::addProvision(SpecializablePureType<wake::QUALIFIED>* provided, vector<PureType<wake::QUALIFIED>*> &arguments, int flags, vector<Annotation*> annotations) {
	string name = provided->toProvisionSymbol(arguments);

	if(properties.count(name)) {
		string temp = "duplicate provision is " + name;
		return boost::optional<SemanticError*>(new SemanticError(DUPLICATE_PROPERTY_DEFINITION, temp));
	}

	PureType<wake::QUALIFIED>* method = new PureType<wake::QUALIFIED>(TYPE_LAMBDA);
	method->typedata.lambda.returntype = new PureType<wake::QUALIFIED>(provided->typedata);

	method->typedata.lambda.arguments = new PureTypeArray<wake::QUALIFIED>(); //TODO injections with curried ctors or arguments!

	ObjectProperty* prop = new ObjectProperty;
	prop->decl.typedata = *method;
	prop->casing = name;
	prop->address = name;
	prop->flags = flags;
	for(vector<Annotation*>::iterator annit = annotations.begin(); annit != annotations.end(); ++annit)
		prop->annotations.push_back(*annit);

	properties[name] = prop;
	return boost::optional<SemanticError*>();
}

boost::optional<PureType<wake::QUALIFIED>*> PropertySymbolTable::find(string name) {
	if(!properties.count(name))
		//throw new SemanticError(PROPERTY_OR_METHOD_NOT_FOUND, "Cannot find method with signature " + name);
		return boost::optional<PureType<wake::QUALIFIED>*>();

	return boost::optional<PureType<wake::QUALIFIED>*>(&properties.find(name)->second->decl.typedata);
}

boost::optional<ObjectProperty*> PropertySymbolTable::findByCasing(string casing) {
	ObjectProperty* found = NULL;

	for(std::map<string, ObjectProperty*>::iterator it = properties.begin(); it != properties.end(); ++it)
	if(it->second->casing == casing)
	if(found == NULL) {
		found = it->second;
	} else {
		return boost::optional<ObjectProperty*>();
	}

	if(found) {
		return boost::optional<ObjectProperty*>(found);
	} else {
		return boost::optional<ObjectProperty*>();
	}
}

string PropertySymbolTable::getAddress(string name) {
	return properties.find(name)->second->address;
}

int PropertySymbolTable::getFlags(string name) {
	return properties.find(name)->second->flags;
}

string PropertySymbolTable::getCasingNameOf(vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >* segments_arguments) {
	string name;
	for(vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >::iterator it = segments_arguments->begin(); it != segments_arguments->end(); ++it) {
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

string PropertySymbolTable::getSymbolNameOf(vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >* segments_arguments) {
	string name;
	for(vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >::iterator it = segments_arguments->begin(); it != segments_arguments->end(); ++it) {
		name += it->first;
		name += "(";
		int i;
		for(i = 0; i < it->second->typecount; i++) {
			if(i) name += ",";
			name += it->second->types[i]->toString();
		}
		name += ")";
	}

	return name;
}

void PropertySymbolTable::printEntryPoints(EntryPointAnalyzer* entryanalyzer) {
	for(map<string, ObjectProperty*>::iterator it = properties.begin(); it != properties.end(); ++it) {
		if(entryanalyzer->checkFQClassMethodCanBeMain(module.size() ? module + "." + classname : classname, &it->second->decl.typedata))
			entryanalyzer->printMethod(it->first);
	}
}

void PropertySymbolTable::addNeed(SpecializableVarDecl<wake::QUALIFIED>* needed, int flags, vector<Annotation*> annotations) {
	needs->push_back(needed);
	addProperty(&needed->decl, flags | PROPERTY_NEED, annotations);
}

vector<SpecializableVarDecl<wake::QUALIFIED>*>* PropertySymbolTable::getNeeds() {
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
			propagate->decl = it->second->decl;
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

			if(!extend && propagate->flags & PROPERTY_NEED) {
				propagate->flags = propagate->flags & ~PROPERTY_NEED;
			}

		} else {
			searcher->second->address = it->second->address;
			if(searcher->second->decl.typedata.type == TYPE_LAMBDA)
			if(it->second->decl.typedata.typedata.lambda.returntype != NULL)
			if(searcher->second->decl.typedata.typedata.lambda.returntype == NULL
				|| !child->analyzer->isASubtypeOfB(
					searcher->second->decl.typedata.typedata.lambda.returntype,	// this is contravariance
					it->second->decl.typedata.typedata.lambda.returntype 		// Check parent return is a subtype of child return
				)
			) {
				errors.addError(new SemanticError(INVALID_CHILD_RETURN_TYPE, "method " + searcher->second->address + " on class " + child->classname, NULL, ""));
			}
		}
	}

	if(extend) {
		for(vector<SpecializableVarDecl<wake::QUALIFIED>*>::iterator it = parent->getNeeds()->begin(); it != parent->getNeeds()->end(); ++it) {
			if((*it)->decl.typedata.type == TYPE_CLASS) {
				string fqname = (child->getModule().size() ? child->getModule() + "." : "") + child->classname;
				child->analyzer->assertFQNeedIsNotCircular(fqname, (*it)->decl.typedata.getFQClassname());
			}

			child->getNeeds()->push_back(*it);
		}
	}
}

bool PropertySymbolTable::isPublic(string name) {
	return properties.find(name)->second->flags & PROPERTY_PUBLIC;
}

void PropertySymbolTable::setParameters(vector<PureType<wake::QUALIFIED>*>* parameters) {
	delete declaredtypeparameters;
	declaredtypeparameters = parameters;
}

const vector<PureType<wake::QUALIFIED>*>& PropertySymbolTable::getParameters() {
	return *declaredtypeparameters;
}

ReadOnlyPropertySymbolTable* PropertySymbolTable::resolveParameters(vector<PureType<wake::QUALIFIED>*>& parameters) {
	TypeParameterizer parameterizer;
	map<string, ObjectProperty*>* newprops = new map<string, ObjectProperty*>();
	for(map<string, ObjectProperty*>::iterator it = properties.begin(); it != properties.end(); ++it) {
		string newname, oldcasing = it->second->casing;
		ObjectProperty* newprop = new ObjectProperty();
		newprop->decl = it->second->decl;
		newprop->flags = it->second->flags;
		newprop->address = it->second->address;
		parameterizer.applyParameterizations(&newprop->decl.typedata, getParameters(), parameters);
		if(newprop->decl.typedata.type == TYPE_LAMBDA) {
			int i = 0, c = 0;
			while(c < oldcasing.length()) {
				if(oldcasing.at(c) == '#') {
					newname += newprop->decl.typedata.typedata.lambda.arguments->types[i]->toString();
					i++;
				} else {
					newname += oldcasing.at(c);
				}
				c++;
			}
		} else {
			VarRef ref(newprop->decl.createVarRef());
			newname = ref.toString();
		}
		newprop->casing = oldcasing;
		(*newprops)[newname] = newprop;
	}

	vector<SpecializableVarDecl<wake::QUALIFIED>*>* newneeds = new vector<SpecializableVarDecl<wake::QUALIFIED>*>();
	for(vector<SpecializableVarDecl<wake::QUALIFIED>*>::iterator it = needs->begin(); it != needs->end(); ++it) {
		SpecializableVarDecl<wake::QUALIFIED>* newneed = new SpecializableVarDecl<wake::QUALIFIED>(**it);
		parameterizer.applyParameterizations(&newneed->decl.typedata, getParameters(), parameters);
		newneeds->push_back(newneed);
	}

	return new DerivedPropertySymbolTable(*analyzer, newneeds, newprops, parentage);
}

PureType<wake::QUALIFIED>* PropertySymbolTable::getAsPureType() {
	PureType<wake::QUALIFIED>* ret = new PureType<wake::QUALIFIED>(TYPE_CLASS);
	ret->typedata._class.classname = strdup(classname.c_str());
	ret->typedata._class.modulename = strdup(module.c_str());
	if(getParameters().size()) {
		ret->typedata._class.parameters = new PureTypeArray<wake::QUALIFIED>();
		for(vector<PureType<wake::QUALIFIED>*>::const_iterator it = getParameters().begin(); it != getParameters().end(); ++it) {
			// these template params are for type safety only, we can freely cast them back and forth
			addPureTypeToPureTypeArray(
				(PureType<wake::UNQUALIFIED>*) *it,
				(PureTypeArray<wake::UNQUALIFIED>*) ret->typedata._class.parameters
			);
		}
	}

	return ret;
}

bool PropertySymbolTable::isBehavioralProvision(string name) {
	return properties.find(name)->second->flags & PROPERTY_BLOCKPROVISION;
}

void PropertySymbolTable::setAnnotations(vector<Annotation*> annotations) {
	for(vector<Annotation*>::iterator annit = annotations.begin(); annit != annotations.end(); ++annit)
		this->annotations.push_back(*annit);
}

const boost::ptr_vector<Annotation>& PropertySymbolTable::getAnnotations() {
	return annotations;
}

void PropertySymbolTable::setModule(string module) {
	this->module = module;
}

string PropertySymbolTable::getModule() {
	return module;
}
