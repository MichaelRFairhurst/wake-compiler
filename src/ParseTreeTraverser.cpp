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
				Node* methodname;
				Node* methodbody;
				vector<pair<string, TypeArray*> > arguments_segments;
				int i;

				if(tree->node_data.nodes[0]->node_type == NT_METHOD_RETURN_TYPE) {
					traversingmethod_return = tree->node_data.nodes[0]->node_data.nodes[0]->node_data.type;
					objectsymtable.assertTypeIsValid(traversingmethod_return);
					methodname = tree->node_data.nodes[1];
					methodbody = tree->node_data.nodes[2];
				} else {
					traversingmethod_return = NULL;
					methodname = tree->node_data.nodes[0];
					methodbody = tree->node_data.nodes[1];
				}

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

				string name = traversingclass_data->addMethod(traversingmethod_return, &arguments_segments, methodbody);

				string prevcontext = errorcontext;
				errorcontext = "In declaration of 'every " + traversingclass_name + "' method " + name;
				Type* method = traversingclass_data->get(name);

				// Begin Method Scope For Type Analysis
				scopesymtable.pushScope();
				for(i = 0; i < method->typedata.lambda.arguments->typecount; i++) {
					scopesymtable.add(method->typedata.lambda.arguments->types[i]);
				}

				// Run Type Analysis
				typeCheck(methodbody);

				// Pop Method Scope
				scopesymtable.popScope();
				errorcontext = prevcontext;

			} catch(SemanticError* e) {
				e->token = tree; e->context = errorcontext;
				errors.push_back(e);
			}

	}
}

Type* ParseTreeTraverser::typeCheck(Node* tree) {
	Type* ret;
	switch(tree->node_type) {
		// Here is where the recursion ends
		case NT_STRINGLIT:
			ret = MakeType(NT_CLASS);
			ret->typedata._class.classname = "Text";
			break;

		case NT_NUMBERLIT:
			ret = MakeType(NT_CLASS);
			ret->typedata._class.classname = "Int";
			break;

		case NT_TRUTHLIT:
			ret = MakeType(NT_CLASS);
			ret->typedata._class.classname = "Truth";
			break;

		case NT_ALIAS:
			ret = MakeType(NT_CLASS);
			*ret = *scopesymtable.get(string("@") + tree->node_data.string);
			break;

		case NT_TYPEDATA:
			ret = MakeType(NT_CLASS);
			*ret = *scopesymtable.get(tree->node_data.type);
			break;

		// Backtracking from the above endpoints
		case NT_MULTIPLY:
		case NT_DIVIDE:
		case NT_SUBTRACT:
		case NT_LESSTHAN:
		case NT_LESSTHANEQUAL:
		case NT_GREATERTHAN:
		case NT_GREATERTHANEQUAL:
			{
				ret = typeCheck(tree->node_data.nodes[0]);
				Type* factor = typeCheck(tree->node_data.nodes[1]);
				if(ret->type == TYPE_LAMBDA || factor->type == TYPE_LAMBDA) {
					errors.push_back(new SemanticError(TYPE_ERROR, "Expected Ints. Cannot multiply lambdas.", tree, errorcontext));

					ret = MakeType(NT_CLASS);
					ret->typedata._class.classname = "Int";

				} else if(ret->typedata._class.classname == "Int" && ret->arrayed == 0) {
					if(factor->typedata._class.classname != "Int" || factor->arrayed != 0) {
						errors.push_back(new SemanticError(TYPE_ERROR, "Second operand of * must be an int", tree, errorcontext));
					}

				} else {
					ret->typedata._class.classname = "Int";
					errors.push_back(new SemanticError(TYPE_ERROR, "First operand of * must be an int", tree, errorcontext));
				}
				delete factor;
			}
			break;

		case NT_ADD:
			{
				ret = typeCheck(tree->node_data.nodes[0]);
				Type* factor = typeCheck(tree->node_data.nodes[1]);
				if(ret->type == TYPE_LAMBDA || factor->type == TYPE_LAMBDA) {
					errors.push_back(new SemanticError(TYPE_ERROR, "Expected Ints. Cannot add lambdas.", tree, errorcontext));

					ret = MakeType(NT_CLASS);
					ret->typedata._class.classname = "Int";

				} else if(ret->typedata._class.classname == "Int" && ret->arrayed == 0) {
					if(factor->typedata._class.classname != "Int" || factor->arrayed != 0) {
						errors.push_back(new SemanticError(TYPE_ERROR, "Second operand of + must be an int", tree, errorcontext));
					}

				} else if(ret->typedata._class.classname == "Text" && ret->arrayed == 0) {
					if(factor->typedata._class.classname != "Text" || factor->arrayed != 0) {
						errors.push_back(new SemanticError(TYPE_ERROR, "Second operand of + must be a Text", tree, errorcontext));
					}

				} else {
					ret->typedata._class.classname = "Int";
					errors.push_back(new SemanticError(TYPE_ERROR, "First operand of + must be an int or text", tree, errorcontext));
				}
				delete factor;
			}
			break;

		case NT_EQUALITY:
		case NT_INEQUALITY:
			{
				ret = typeCheck(tree->node_data.nodes[0]);
				Type* cmp = typeCheck(tree->node_data.nodes[0]);

				if(ret->type == TYPE_LAMBDA || cmp->type == TYPE_LAMBDA) {
					errors.push_back(new SemanticError(TYPE_ERROR, "Expected Ints. Cannot compare equality of lambdas.", tree, errorcontext));

					ret = MakeType(NT_CLASS);

				} else if(ret->typedata._class.classname != cmp->typedata._class.classname) {
					errors.push_back(new SemanticError(TYPE_ERROR, "Equating different types", tree, errorcontext));

				} else if(ret->arrayed != cmp->arrayed) {
					errors.push_back(new SemanticError(TYPE_ERROR, "Unequal array (as in YourClass[]) amounts", tree, errorcontext));
				}


				ret->typedata._class.classname = "Truth";
			}
			break;

		case NT_AND:
		case NT_OR:
			{
				ret = typeCheck(tree->node_data.nodes[0]);
				Type* cmp = typeCheck(tree->node_data.nodes[0]);

				if(ret->type == TYPE_LAMBDA || cmp->type == TYPE_LAMBDA) {
					errors.push_back(new SemanticError(TYPE_ERROR, "Expected Truths. Cannot compare equality of lambdas.", tree, errorcontext));

					ret->typedata._class.classname = "Truth";
					ret = MakeType(NT_CLASS);
				} else if(ret->typedata._class.classname != "Truth" || cmp->typedata._class.classname != "Truth" || ret->arrayed + cmp->arrayed > 0) {
					errors.push_back(new SemanticError(TYPE_ERROR, "Cannot compare anything but Truth values with && or ||", tree, errorcontext));
				}
			}
			break;

		case NT_ARGUMENTS:
		case NT_ARGUMENT:
		case NT_TYPE_ARRAY:
		case NT_CLASSNAME:
		case NT_CURRIED:
		case NT_ARRAY_ACCESS:
		case NT_MEMBER_ACCESS:
		case NT_METHOD_INVOCATION:
		case NT_LAMBDA_INVOCATION:
		case NT_BLOCK:
		case NT_EXPRESSIONS:
		case NT_RETRIEVALS_STATEMENTS:
		case NT_RETRIEVAL:
		case NT_ASSIGNMENT:
		case NT_CASE:
		case NT_DEFAULTCASE:
		case NT_IF_ELSE:
		case NT_SWITCH:
		case NT_WHILE:
		case NT_FOR:
		case NT_BREAK:
		case NT_CONTINUE:
		case NT_RETURN:
		case NT_IF_THEN_ELSE:
		case NT_VALUES:
		case NT_ARRAY_DECLARATION:
		case NT_INCREMENT:
		case NT_DECREMENT:
		case NT_INVERT:
			{
				int i = 0;
				while(i < tree->subnodes) {
					ret = typeCheck(tree->node_data.nodes[i]);
					i++;
				}
			}
	}

	return ret;
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

