/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TempPropertySymbolTable.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "TempPropertySymbolTable.h"

using namespace wake;

boost::optional<PureType<QUALIFIED>*> TempPropertySymbolTable::find(string name) {
	return table.find(name);
}

boost::optional<ObjectProperty*> TempPropertySymbolTable::findByCasing(string casing) {
	return table.findByCasing(casing);
}

bool TempPropertySymbolTable::isPublic(string name) {
	return table.isPublic(name);
}

string TempPropertySymbolTable::getAddress(string name) {
	return table.getAddress(name);
}

int TempPropertySymbolTable::getFlags(string name) {
	return table.getFlags(name);
}

vector<SpecializableVarDecl<QUALIFIED>*>* TempPropertySymbolTable::getNeeds() {
	return table.getNeeds();
}

string TempPropertySymbolTable::getSymbolNameOf(vector<pair<string, PureTypeArray<QUALIFIED>*> >* segments_arguments) {
	return table.getSymbolNameOf(segments_arguments);
}

bool TempPropertySymbolTable::isAbstract() {
	return table.isAbstract();
}

const map<string, bool>& TempPropertySymbolTable::getParentage() {
	return table.getParentage();
}

string TempPropertySymbolTable::getModule() {
	return table.getModule();
}

string TempPropertySymbolTable::getClassname() {
	return table.getClassname();
}

boost::optional<SemanticError*> TempPropertySymbolTable::addMethod(PureType<wake::QUALIFIED>* returntype, vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >* segments_arguments, int flags, vector<Annotation*> annotations) {
	return table.addMethod(returntype, segments_arguments, flags, annotations);
}

boost::optional<SemanticError*> TempPropertySymbolTable::addProperty(VarDecl<wake::QUALIFIED>* property, int flags, vector<Annotation*> annotations) {
	return table.addProperty(property, flags, annotations);
}

boost::optional<SemanticError*> TempPropertySymbolTable::addProvision(SpecializablePureType<wake::QUALIFIED>* provision, vector<PureType<wake::QUALIFIED>*> &arguments, int flags, vector<Annotation*> annotations) {
	return table.addProvision(provision, arguments, flags, annotations);
}

bool TempPropertySymbolTable::isBehavioralProvision(string name) {
	return table.isBehavioralProvision(name);
}

void TempPropertySymbolTable::addNeed(SpecializableVarDecl<wake::QUALIFIED>* need, int flags, vector<Annotation*> annotations) {
	table.addNeed(need, flags, annotations);
}

void TempPropertySymbolTable::printEntryPoints(EntryPointAnalyzer* entryanalyzer) {
	table.printEntryPoints(entryanalyzer);
}

string TempPropertySymbolTable::getCasingNameOf(vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >* segments_arguments) {
	return table.getCasingNameOf(segments_arguments);
}

void TempPropertySymbolTable::setIsAbstract(bool newvalue) {
	table.setIsAbstract(newvalue);
}

map<string, bool>& TempPropertySymbolTable::getParentageModifiable() {
	return table.getParentageModifiable();
}

void TempPropertySymbolTable::setParentage(map<string, bool> newparentage) {
	table.setParentage(newparentage);
}

void TempPropertySymbolTable::setParameters(vector<PureType<wake::QUALIFIED>*>* parameters) {
	table.setParameters(parameters);
}

const vector<PureType<wake::QUALIFIED>*>& TempPropertySymbolTable::getParameters() {
	return table.getParameters();
}

PureType<wake::QUALIFIED>* TempPropertySymbolTable::getAsPureType() {
	return table.getAsPureType();
}

void TempPropertySymbolTable::setAnnotations(vector<Annotation*> annotations) {
	table.setAnnotations(annotations);
}

const boost::ptr_vector<Annotation>& TempPropertySymbolTable::getAnnotations() {
	return table.getAnnotations();
}

void TempPropertySymbolTable::setModule(string newmodule) {
	table.setModule(newmodule);
}

void TempPropertySymbolTable::setClassname(string classname) {
	table.setClassname(classname);
}

ReadOnlyPropertySymbolTable* TempPropertySymbolTable::resolveParameters(vector<PureType<wake::QUALIFIED>*>& parameters) {
	return table.resolveParameters(parameters);
}

map<string, ObjectProperty*>& TempPropertySymbolTable::getProperties() {
	return table.getProperties();
}

TypeAnalyzer* TempPropertySymbolTable::getAnalyzer() {
	return table.getAnalyzer();
}
