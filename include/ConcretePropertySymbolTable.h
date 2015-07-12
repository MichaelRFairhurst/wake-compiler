/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ConcretePropertySymbolTable.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_CONCRETE_PROPERTY_SYMBOL
#define HEADER_CONCRETE_PROPERTY_SYMBOL
#include "PropertySymbolTable.h"

using namespace std;

class ConcretePropertySymbolTable : public PropertySymbolTable {
	public:
		ConcretePropertySymbolTable(TypeAnalyzer* tanalyzer, string module) : analyzer(tanalyzer), module(module), abstract(false), declaredtypeparameters(new vector<PureType<wake::QUALIFIED>*>()), needs(new vector<SpecializableVarDecl<wake::QUALIFIED>*>()) {};
		~ConcretePropertySymbolTable();
		boost::optional<PureType<wake::QUALIFIED>*> find(string name);
		boost::optional<ObjectProperty*> findByCasing(string casing);
		bool isPublic(string name);
		string getAddress(string name);
		int getFlags(string name);
		boost::optional<SemanticError*> addMethod(PureType<wake::QUALIFIED>* returntype, vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >* segments_arguments, int flags, vector<Annotation*> annotations);
		boost::optional<SemanticError*> addProperty(VarDecl<wake::QUALIFIED>* property, int flags, vector<Annotation*> annotations);
		boost::optional<SemanticError*> addProvision(SpecializablePureType<wake::QUALIFIED>* provision, vector<PureType<wake::QUALIFIED>*> &arguments, int flags, vector<Annotation*> annotations);
		bool isBehavioralProvision(string name);
		void addNeed(SpecializableVarDecl<wake::QUALIFIED>* need, int flags, vector<Annotation*> annotations);
		void printEntryPoints(EntryPointAnalyzer* entryanalyzer);
		vector<SpecializableVarDecl<wake::QUALIFIED>*>* getNeeds();
		string getSymbolNameOf(vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >* segments_arguments);
		string getCasingNameOf(vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >* segments_arguments);
		bool isAbstract();
		void setIsAbstract(bool newvalue);
		const map<string, bool>& getParentage();
		map<string, bool>& getParentageModifiable();
		void setParentage(map<string, bool> newparentage);
		void setParameters(vector<PureType<wake::QUALIFIED>*>* parameters);
		const vector<PureType<wake::QUALIFIED>*>& getParameters();
		PureType<wake::QUALIFIED>* getAsPureType();
		void setAnnotations(vector<Annotation*> annotations);
		const boost::ptr_vector<Annotation>& getAnnotations();
		void setModule(string newmodule);
		string getModule();
		string getClassname();
		void setClassname(string classname);

		TypeAnalyzer* getAnalyzer();

		ReadOnlyPropertySymbolTable* resolveParameters(vector<PureType<wake::QUALIFIED>*>& parameters);

		map<string, ObjectProperty*>& getProperties();
		//void setProperties(const map<string, ObjectProperty*>& newproperties);

	private:
		map<string, ObjectProperty*> properties;
		bool abstract;
		map<string, bool> parentage;
		string classname;
		TypeAnalyzer* analyzer;
		vector<SpecializableVarDecl<wake::QUALIFIED>*>* needs;
		vector<PureType<wake::QUALIFIED>*>* declaredtypeparameters;
		boost::ptr_vector<Annotation> annotations;
		string module;
};

#endif
