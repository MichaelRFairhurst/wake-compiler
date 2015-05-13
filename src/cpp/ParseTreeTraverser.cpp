/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ParseTreeTraverser.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ParseTreeTraverser.h"
#include "CompilationExceptions.h"
#include "ClassParseTreeTraverser.h"
#include "AnnotationTreeTraverser.h"
#include <string.h>

void ParseTreeTraverser::traverse(Node* tree) {
	switch(tree->node_type) {
		case NT_PROGRAM:
			traverse(tree->node_data.nodes[0]);
			if(tree->subnodes > 1) traverse(tree->node_data.nodes[1]);
			if(tree->subnodes > 2) traverse(tree->node_data.nodes[2]);

			if(!passesForCompilation()) return;
			secondPass(tree->node_data.nodes[0]);
			if(tree->subnodes > 1) secondPass(tree->node_data.nodes[1]);
			if(tree->subnodes > 2) secondPass(tree->node_data.nodes[2]);

			if(!passesForCompilation()) return;
			try {
				objectsymtable->propagateInheritance(errors);
			} catch(SemanticError* e) {
				e->token = tree;
				errors.addError(e);
				return;
			}

			thirdPass(tree->node_data.nodes[0]);
			if(tree->subnodes > 1) thirdPass(tree->node_data.nodes[1]);
			if(tree->subnodes > 2) thirdPass(tree->node_data.nodes[2]);
			break;

		case NT_INHERITANCESET:
		case NT_CLASSSET:
			{
				int i = 0;
				while(i < tree->subnodes) {
					traverse(tree->node_data.nodes[i]);
					i++;
				}
			}
			break;

		case NT_ANNOTATED_CLASS:
			traverse(tree->node_data.nodes[0]);
			break;

		case NT_CLASS_EXTERN:
		case NT_CLASS:
			{
				errors.pushContext("In declaration of 'every " + string(tree->node_data.nodes[0]->node_data.pure_type->typedata._class.classname) + "'");
				boost::optional<SemanticError*> error = objectsymtable->addClass(tree->node_data.nodes[0]->node_data.pure_type->typedata._class.classname);

				if(error) {
					(*error)->token = tree;
					errors.addError(*error);
				}

				PropertySymbolTable* proptable = objectsymtable->findByImportedNameModifiable(tree->node_data.nodes[0]->node_data.pure_type->typedata._class.classname);

				PureType<wake::UNQUALIFIED>* classtype = tree->node_data.nodes[0]->node_data.pure_type;
				vector<PureType<wake::QUALIFIED>*>* parameters = new vector<PureType<wake::QUALIFIED>*>();

				vector<string> usedGenericNames;
				if(classtype->typedata._class.parameters != NULL)
				for(int i = 0; i < classtype->typedata._class.parameters->typecount; i++)
				if(std::find(usedGenericNames.begin(), usedGenericNames.end(), classtype->typedata._class.parameters->types[i]->typedata.parameterized.label) == usedGenericNames.end()) {
					parameters->push_back(objectsymtable->setModulesOnType(classtype->typedata._class.parameters->types[i]));
					usedGenericNames.push_back(classtype->typedata._class.parameters->types[i]->typedata.parameterized.label);
				} else {
					errors.addError(new SemanticError(GENERIC_TYPE_COLLISION, string("Generic type with label ") + classtype->typedata._class.parameters->types[i]->typedata.parameterized.label + " is declared more than once", tree));
				}

				proptable->setParameters(parameters);

				traverse(tree->node_data.nodes[1]);
			}
			break;

		case NT_INTERFACE:
		case NT_SUBCLASS:
			{
				boost::optional<SemanticError*> error = objectsymtable->addInheritance(tree->node_data.nodes[0]->node_data.pure_type->typedata._class.classname, tree->node_type == NT_SUBCLASS);
				if(error) {
					(*error)->token = tree;
					errors.addError((*error));
				}
			}
	}
}

void ParseTreeTraverser::secondPass(Node* tree) {
	switch(tree->node_type) {
		case NT_INHERITANCESET:
		case NT_CLASSSET:
			{
				int i = 0;
				while(i < tree->subnodes) {
					secondPass(tree->node_data.nodes[i]);
					i++;
				}
			}
			break;

		case NT_ANNOTATED_CLASS:
		case NT_CLASS:
		case NT_CLASS_EXTERN:
			{
				vector<Annotation*> annotations;
				if(tree->node_type == NT_ANNOTATED_CLASS) {
					AnnotationTreeTraverser traverser;
					annotations = traverser.getAnnotations(tree->node_data.nodes[1]);
					tree = tree->node_data.nodes[0];
				}

				PureType<wake::UNQUALIFIED>* classtype = tree->node_data.nodes[0]->node_data.pure_type;
				string classname = classtype->typedata._class.classname;
				errors.pushContext("In declaration of '" + string(tree->node_type == NT_CLASS ? "every" : "extern") + " " + classname + "'");

				PropertySymbolTable* proptable = objectsymtable->findByImportedNameModifiable(classname);
				proptable->setAnnotations(annotations);

				ClassParseTreeTraverser classtraverser(&errors, objectsymtable, &scopesymtable, classname, &typechecker, &methodanalyzer, proptable, tree->node_type == NT_CLASS_EXTERN);

				secondPass(tree->node_data.nodes[1]);
				if(tree->subnodes > 2) classtraverser.firstPass(tree->node_data.nodes[2]);

				errors.popContext();
			}
			break;

		case NT_INTERFACE:
		case NT_SUBCLASS:
			try {
				objectsymtable->findByImportedName(tree->node_data.nodes[0]->node_data.pure_type->typedata._class.classname);
			} catch(SymbolNotFoundException* e) {
				errors.addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
				delete e;
			}
			break;

	}
}

void ParseTreeTraverser::thirdPass(Node* tree) {
	switch(tree->node_type) {
		case NT_CLASSSET:
			{
				int i = 0;
				while(i < tree->subnodes) {
					thirdPass(tree->node_data.nodes[i]);
					i++;
				}
			}
			break;

		case NT_ANNOTATED_CLASS:
			thirdPass(tree->node_data.nodes[0]);
			break;

		case NT_CLASS:
		case NT_CLASS_EXTERN:
			{
				PureType<wake::UNQUALIFIED>* classtype = tree->node_data.nodes[0]->node_data.pure_type;
				string classname = classtype->typedata._class.classname;
				errors.pushContext("In declaration of '" + string(tree->node_type == NT_CLASS ? "every" : "extern") + " " + classname + "'");

				ClassParseTreeTraverser classtraverser(&errors, objectsymtable, &scopesymtable, classname, &typechecker, &methodanalyzer, objectsymtable->findByImportedNameModifiable(classname), tree->node_type == NT_CLASS_EXTERN);

				thirdPass(tree->node_data.nodes[1]);
				if(tree->subnodes > 2) classtraverser.secondPass(tree->node_data.nodes[2]);

				errors.popContext();
			}
			break;
	}
}

bool ParseTreeTraverser::passesForCompilation() {
	return errors.passesForCompilation();
}

void ParseTreeTraverser::printErrors(SemanticErrorPrinter& printer) {
	errors.printErrors(printer);
}
