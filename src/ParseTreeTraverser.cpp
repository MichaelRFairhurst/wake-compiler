#include "ParseTreeTraverser.h"
#include "CompilationExceptions.h"
#include "ClassParseTreeTraverser.h"
#include <string.h>

void ParseTreeTraverser::traverse(Node* tree) {
	switch(tree->node_type) {
		case NT_PROGRAM:
			traverse(tree->node_data.nodes[0]);
			if(tree->subnodes > 1) traverse(tree->node_data.nodes[1]);
			secondPass(tree->node_data.nodes[0]);
			if(tree->subnodes > 1) secondPass(tree->node_data.nodes[1]);
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

		case NT_CLASS:
			traverse(tree->node_data.nodes[0]);
			traverse(tree->node_data.nodes[1]);
			break;

		case NT_CLASSNAME:
			try {
				errors.pushContext("In declaration of 'every " + string(tree->node_data.string) + "'");
				objectsymtable.addClass(tree->node_data.string);
			} catch(SemanticError* e) {
				e->token = tree;
				errors.addError(e);
			}
			break;

		case NT_INTERFACE:
		case NT_SUBCLASS:
			try {
				objectsymtable.addInheritance(tree->node_data.string, tree->node_type == NT_SUBCLASS);
			} catch(SemanticError* e) {
				e->token = tree;
				errors.addError(e);
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

		case NT_CLASS:
			{
				string classname = tree->node_data.nodes[0]->node_data.string;
				errors.pushContext("In declaration of 'every " + classname + "'");
				ClassParseTreeTraverser classtraverser(&errors, &objectsymtable, &scopesymtable, classname, &typechecker);

				secondPass(tree->node_data.nodes[1]);
				if(tree->subnodes > 2) classtraverser.traverse(tree->node_data.nodes[2]);

				errors.popContext();
			}
			break;

		case NT_INTERFACE:
		case NT_SUBCLASS:
			try {
				objectsymtable.find(tree->node_data.string);
			} catch(SymbolNotFoundException* e) {
				errors.addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
				delete e;
			}
			break;

	}
}

void ParseTreeTraverser::printErrors(SemanticErrorPrinter& printer) {
	errors.printErrors(printer);
}
