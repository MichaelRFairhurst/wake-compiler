/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TempPropertySymbolTable.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_TEMP_PROPERTY_SYMBOL
#define HEADER_TEMP_PROPERTY_SYMBOL
#include "PropertySymbolTable.h"

using namespace std;

class TempPropertySymbolTable : public PropertySymbolTable {
	public:
		TempPropertySymbolTable(PropertySymbolTable& table) : table(table) {};
		~TempPropertySymbolTable() {};

		boost::optional<PureType<wake::QUALIFIED>*> find(string name);
		boost::optional<ObjectProperty*> findByCasing(string casing);
		bool isPublic(string name);
		string getAddress(string name);
		int getFlags(string name);
		vector<SpecializableVarDecl<wake::QUALIFIED>*>* getNeeds();
		string getSymbolNameOf(vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >* segments_arguments);
		bool isAbstract();
		string getModule();
		string getClassname();
		const map<string, bool>& getParentage();

		boost::optional<SemanticError*> addMethod(PureType<wake::QUALIFIED>* returntype, vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >* segments_arguments, int flags, vector<Annotation*> annotations);
		boost::optional<SemanticError*> addProperty(VarDecl<wake::QUALIFIED>* property, int flags, vector<Annotation*> annotations);
		boost::optional<SemanticError*> addProvision(SpecializablePureType<wake::QUALIFIED>* provision, vector<PureType<wake::QUALIFIED>*> &arguments, int flags, vector<Annotation*> annotations);
		bool isBehavioralProvision(string name);
		void addNeed(SpecializableVarDecl<wake::QUALIFIED>* need, int flags, vector<Annotation*> annotations);
		void printEntryPoints(EntryPointAnalyzer* entryanalyzer);
		string getCasingNameOf(vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >* segments_arguments);
		void setIsAbstract(bool newvalue);
		map<string, bool>& getParentageModifiable();
		void setParentage(map<string, bool> newparentage);
		void setParameters(vector<PureType<wake::QUALIFIED>*>* parameters);
		const vector<PureType<wake::QUALIFIED>*>& getParameters();
		PureType<wake::QUALIFIED>* getAsPureType();
		void setAnnotations(vector<Annotation*> annotations);
		const boost::ptr_vector<Annotation>& getAnnotations();
		void setModule(string newmodule);
		void setClassname(string classname);
		ReadOnlyPropertySymbolTable* resolveParameters(vector<PureType<wake::QUALIFIED>*>& parameters);
		map<string, ObjectProperty*>& getProperties();
		TypeAnalyzer* getAnalyzer();

	private:
		PropertySymbolTable& table;

};

#endif
