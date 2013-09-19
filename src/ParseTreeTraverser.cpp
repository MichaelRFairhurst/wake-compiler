#include "ParseTreeTraverser.h"
#include "CompilationExceptions.h"

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
				errorcontext = "In declaration of 'every " + string(tree->node_data.string) + "'";
				objectsymtable.addClass(tree->node_data.string);
			} catch(SemanticError* e) {
				e->token = tree; e->context = errorcontext;
				errors.push_back(e);
			}
			break;

		case NT_INTERFACE:
		case NT_SUBCLASS:
			try {
				objectsymtable.addInheritance(tree->node_data.string, tree->node_type == NT_SUBCLASS);
			} catch(SemanticError* e) {
				e->token = tree; e->context = errorcontext;
				errors.push_back(e);
			}
	}
}

void ParseTreeTraverser::secondPass(Node* tree) {
	switch(tree->node_type) {
		case NT_CLASSBODY:
		case NT_PROVISIONS:
		case NT_INHERITANCESET:
		case NT_CLASSSET:
		case NT_INJECTED_CTOR_ARG:
			{
				int i = 0;
				while(i < tree->subnodes) {
					secondPass(tree->node_data.nodes[i]);
					i++;
				}
			}
			break;

		case NT_CLASS:
			traversingclass_name = tree->node_data.nodes[0]->node_data.string;
			errorcontext = "In declaration of 'every " + traversingclass_name + "'";
			traversingclass_data = objectsymtable.find(traversingclass_name);

			secondPass(tree->node_data.nodes[1]);
			if(tree->subnodes > 2) secondPass(tree->node_data.nodes[2]);
			break;

		case NT_INTERFACE:
		case NT_SUBCLASS:
			try {
				objectsymtable.find(tree->node_data.string);
			} catch(SymbolNotFoundException* e) {
				errors.push_back(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree, errorcontext));
				delete e;
			}
			break;

		case NT_PROVISION:
		case NT_PROVIDING:
			try {
				secondPass(tree->node_data.nodes[0]);
				if(tree->subnodes > 1) secondPass(tree->node_data.nodes[1]);
			} catch(SymbolNotFoundException* e) {
				errors.push_back(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree, errorcontext));
				delete e;
			}
			break;

		case NT_TYPEDATA:
			{
				Type* injection = tree->node_data.type;
				if(injection->type == TYPE_CLASS) objectsymtable.find(injection->typedata._class.classname);
			}
			break;

		case NT_INJECTED_CTOR:
			try {
				objectsymtable.find(tree->node_data.nodes[0]->node_data.string);
				secondPass(tree->node_data.nodes[1]);
			} catch(SymbolNotFoundException* e) {
				errors.push_back(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree, errorcontext));
				delete e;
			}
			break;

		case NT_METHOD_DECLARATION:
			try {
				Type* rettype;
				Node* methodname;
				vector<pair<string, TypeArray*> > arguments_segments;
				if(tree->node_data.nodes[0]->node_type == NT_METHOD_RETURN_TYPE) {
					rettype = tree->node_data.nodes[0]->node_data.nodes[0]->node_data.type;
					objectsymtable.assertTypeIsValid(rettype);
					methodname = tree->node_data.nodes[1];
				} else {
					rettype = NULL;
					methodname = tree->node_data.nodes[0];
				}

				int i;
				for(i = 0; i < methodname->subnodes; i++) {
					TypeArray* argshere;
					string namesegment = methodname->node_data.nodes[i]->node_data.string;
					i++;

					if(i < methodname->subnodes) {
						argshere = methodname->node_data.nodes[i]->node_data.typearray;
						for(int b = 0; b < argshere->typecount; b++) {
							objectsymtable.assertTypeIsValid(argshere->types[b]);
						}
					} else {
						argshere = MakeTypeArray();
					}

					arguments_segments.push_back(pair<string, TypeArray*>(namesegment, argshere));
				}

				errorcontext = "In declaration of 'every " + traversingclass_name + "' method " + traversingclass_data->addMethod(rettype, &arguments_segments, tree);
			} catch(SemanticError* e) {
				e->token = tree; e->context = errorcontext;
				errors.push_back(e);
			}

	}
}

vector<SemanticError*>* ParseTreeTraverser::getErrors() {
	return &errors;
}

void ParseTreeTraverser::printErrors(SemanticErrorPrinter& printer) {
	if(errors.size()) {
		for(std::vector<SemanticError*>::iterator vit = errors.begin(); vit != errors.end(); vit++)
			printer.print(*vit);
	}
}

ParseTreeTraverser::~ParseTreeTraverser() {
	for(vector<SemanticError*>::iterator it = errors.begin(); it != errors.end(); ++it) {
		delete *it;
	}
}
