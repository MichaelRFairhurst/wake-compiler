/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * PropertySymbolTable.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_BASE_PROPERTY_SYMBOL
#define HEADER_BASE_PROPERTY_SYMBOL
#include <map>
#include <string>
#include <vector>
#include <utility>
#include <boost/optional/optional.hpp>

#include "tree.h"

#include "ReadOnlyPropertySymbolTable.h"
#include "ObjectProperty.h"
#include "TypeAnalyzer.h"
#include "SemanticError.h"
#include "ErrorTracker.h"
#include "EntryPointAnalyzer.h"
#include "Annotation.h"

using namespace std;

class PropertySymbolTable : public ReadOnlyPropertySymbolTable {
	public:
		virtual ~PropertySymbolTable(){};
		virtual boost::optional<PureType<wake::QUALIFIED>*> find(string name)=0;
		virtual boost::optional<ObjectProperty*> findByCasing(string casing)=0;
		virtual bool isPublic(string name)=0;
		virtual string getAddress(string name)=0;
		virtual int getFlags(string name)=0;
		virtual boost::optional<SemanticError*> addMethod(PureType<wake::QUALIFIED>* returntype, vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >* segments_arguments, int flags, vector<Annotation*> annotations)=0;
		virtual boost::optional<SemanticError*> addProperty(VarDecl<wake::QUALIFIED>* property, int flags, vector<Annotation*> annotations)=0;
		virtual boost::optional<SemanticError*> addProvision(SpecializablePureType<wake::QUALIFIED>* provision, vector<PureType<wake::QUALIFIED>*> &arguments, int flags, vector<Annotation*> annotations)=0;
		virtual bool isBehavioralProvision(string name)=0;
		virtual void addNeed(SpecializableVarDecl<wake::QUALIFIED>* need, int flags, vector<Annotation*> annotations)=0;
		virtual void printEntryPoints(EntryPointAnalyzer* entryanalyzer)=0;
		virtual vector<SpecializableVarDecl<wake::QUALIFIED>*>* getNeeds()=0;
		virtual string getSymbolNameOf(vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >* segments_arguments)=0;
		virtual string getCasingNameOf(vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >* segments_arguments)=0;
		virtual bool isAbstract()=0;
		virtual void setIsAbstract(bool newvalue)=0;
		virtual const map<string, bool>& getParentage()=0;
		virtual map<string, bool>& getParentageModifiable()=0;
		virtual void setParentage(map<string, bool> newparentage)=0;
		virtual void setParameters(vector<PureType<wake::QUALIFIED>*>* parameters)=0;
		virtual const vector<PureType<wake::QUALIFIED>*>& getParameters()=0;
		virtual PureType<wake::QUALIFIED>* getAsPureType()=0;
		virtual void setAnnotations(vector<Annotation*> annotations)=0;
		virtual const boost::ptr_vector<Annotation>& getAnnotations()=0;
		virtual void setModule(string newmodule)=0;
		virtual string getModule()=0;
		virtual string getClassname()=0;
		virtual void setClassname(string classname)=0;
		virtual TypeAnalyzer* getAnalyzer()=0;

		virtual ReadOnlyPropertySymbolTable* resolveParameters(vector<PureType<wake::QUALIFIED>*>& parameters)=0;

		virtual map<string, ObjectProperty*>& getProperties()=0;

		friend void propagateInheritanceTables(PropertySymbolTable* parent, PropertySymbolTable* child, bool extend, ErrorTracker& errors);
};

#endif
