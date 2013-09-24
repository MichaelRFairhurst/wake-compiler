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
	TypeAnalyzer* analyzer = objectsymtable.getAnalyzer();
	Type* ret;
	string erroneousstring;
	string expectedstring;

	try {
		switch(tree->node_type) {
			// Here is where the recursion ends
			case NT_STRINGLIT:
				ret = MakeType(TYPE_CLASS);
				ret->typedata._class.classname = "Text";
				break;

			case NT_NUMBERLIT:
				ret = MakeType(TYPE_CLASS);
				ret->typedata._class.classname = "Int";
				break;

			case NT_TRUTHLIT:
				ret = MakeType(TYPE_CLASS);
				ret->typedata._class.classname = "Truth";
				break;

			case NT_ALIAS:
				ret = MakeType(TYPE_CLASS);
				*ret = *scopesymtable.get(string("@") + tree->node_data.string); // COPY
				break;

			case NT_TYPEDATA:
				ret = MakeType(TYPE_CLASS);
				*ret = *scopesymtable.get(tree->node_data.type); // COPY
				break;

			// Backtracking from the above endpoints
			case NT_MULTIPLY:
			case NT_DIVIDE:
			case NT_SUBTRACT:
				{
					ret = typeCheck(tree->node_data.nodes[0]);
					Type* factor = typeCheck(tree->node_data.nodes[1]);

					if(!analyzer->isPrimitiveTypeInt(ret) || !analyzer->isPrimitiveTypeInt(factor)) {
						expectedstring = "Int";

						if(analyzer->isPrimitiveTypeInt(ret)) {
							erroneousstring = analyzer->getNameForType(factor);
						} else {
							erroneousstring = analyzer->getNameForType(ret); free(ret);
							ret = MakeType(TYPE_CLASS);
							ret->typedata._class.classname = "Int";
						}

						free(factor);

						throw string("Mathematical operation performed on non-numerals");
					}

					free(factor);
				}
				break;

			case NT_LESSTHAN:
			case NT_LESSTHANEQUAL:
			case NT_GREATERTHAN:
			case NT_GREATERTHANEQUAL:
				{
					ret = MakeType(TYPE_CLASS);
					ret->typedata._class.classname = "Truth";

					Type* a = typeCheck(tree->node_data.nodes[0]);
					Type* b = typeCheck(tree->node_data.nodes[1]);

					if(!analyzer->isPrimitiveTypeInt(a) || !analyzer->isPrimitiveTypeInt(b)) {
						expectedstring = "Int";

						if(analyzer->isPrimitiveTypeInt(a)) {
							erroneousstring = analyzer->getNameForType(b);
						} else {
							erroneousstring = analyzer->getNameForType(a); free(a);
						}

						free(b);

						throw string("Mathematical operation performed on non-numerals");
					}

					free(b);
				}
				break;

			case NT_ADD:
				{
					ret = typeCheck(tree->node_data.nodes[0]);
					Type* additive = typeCheck(tree->node_data.nodes[1]);

					if(analyzer->isPrimitiveTypeInt(ret)) {
						if(!analyzer->isPrimitiveTypeInt(additive)) {
							erroneousstring = analyzer->getNameForType(additive); free(additive);
							expectedstring = "Int";
							throw string("Addition with a non-numeral");
						}

					} else if(analyzer->isPrimitiveTypeText(ret)) {
						if(!analyzer->isPrimitiveTypeText(additive)) {
							erroneousstring = analyzer->getNameForType(additive); free(additive);
							expectedstring = "Text";
							throw string("Concatenation with non-Text");
						}

					} else {
						expectedstring = "Int' or 'Text";
						erroneousstring = analyzer->getNameForType(ret);
						free(additive); free(ret);
						ret = MakeType(TYPE_CLASS);
						ret->typedata._class.classname = "Int";

						throw string("Only numerics or Texts can be added/concatenated");
					}

					free(additive);
				}
				break;

			case NT_EQUALITY:
			case NT_INEQUALITY:
				{
					Type* a = typeCheck(tree->node_data.nodes[0]);
					Type* b = typeCheck(tree->node_data.nodes[1]);
					ret = MakeType(TYPE_CLASS);
					ret->typedata._class.classname = "Truth";

					if(!analyzer->isASubtypeOfB(a, b) && !analyzer->isASubtypeOfB(b, a)) {
						expectedstring = analyzer->getNameForType(a);
						erroneousstring = analyzer->getNameForType(b);
						free(a); free(b);
						throw string("Uncomparable types");
					}

					free(a);
				}
				break;

			case NT_AND:
			case NT_OR:
				{
					ret = typeCheck(tree->node_data.nodes[0]);
					Type* cmp = typeCheck(tree->node_data.nodes[0]);

					if(!analyzer->isPrimitiveTypeTruth(ret) || !analyzer->isPrimitiveTypeTruth(cmp)) {
						if(analyzer->isPrimitiveTypeTruth(ret)) {
							erroneousstring = analyzer->getNameForType(cmp);
						} else {
							erroneousstring = analyzer->getNameForType(ret); free(ret);
							ret = MakeType(TYPE_CLASS);
							ret->typedata._class.classname = "Truth";
						}
						free(cmp);

						expectedstring = "Truth";

						throw string("AND or OR comparison on non-Truth types");
					}

					free(cmp);
				}
				break;

			case NT_ARRAY_ACCESS:
				{
					ret = typeCheck(tree->node_data.nodes[0]);
					Type* index = typeCheck(tree->node_data.nodes[1]);
					--ret->arrayed;

					if(!analyzer->isPrimitiveTypeInt(index)) {
						erroneousstring = analyzer->getNameForType(index); free(index);
						expectedstring = "Int";
						throw string("Array indices must be numeric");
					} else if(ret->arrayed < 0) {
						erroneousstring = analyzer->getNameForType(ret); free(index);
						throw string("Getting index of non-array");
					}
				}
				break;

			case NT_IF_ELSE:
			case NT_WHILE:
				{
					ret = typeCheck(tree->node_data.nodes[0]);
					Type* temp = typeCheck(tree->node_data.nodes[1]);
					free(temp);

					if(tree->subnodes > 2) {
						Type* temp = typeCheck(tree->node_data.nodes[2]);
						free(temp);
					}

					if(!analyzer->isPrimitiveTypeTruth(ret)) {
						expectedstring = "Truth"; free(ret);
						ret = MakeType(TYPE_CLASS);
						ret->typedata._class.classname = "Truth";
						throw string("If/While conditions must be Truths");
					}
				}
				break;

			case NT_INVERT:
				ret = typeCheck(tree->node_data.nodes[0]);

				if(!analyzer->isPrimitiveTypeTruth(ret)) {
					expectedstring = "Truth";
					erroneousstring = analyzer->getNameForType(ret);
					free(ret);
					ret = MakeType(TYPE_CLASS);
					ret->typedata._class.classname = "Truth";
					ret->arrayed = 0;
					throw string("If conditions must be Truths");
				}
				break;

			case NT_RETURN: // TODO: void methods
				ret = typeCheck(tree->node_data.nodes[0]);
				if(!analyzer->isASubtypeOfB(ret, traversingmethod_return)) {
					expectedstring = analyzer->getNameForType(traversingmethod_return);
					erroneousstring = analyzer->getNameForType(ret);
					throw string("Return type is incompatible with method signature");
				}
				break;

			case NT_ASSIGNMENT:
				{
					ret = typeCheck(tree->node_data.nodes[0]);
					Type* assignment = typeCheck(tree->node_data.nodes[1]);
					if(!analyzer->isASubtypeOfB(assignment, ret)) {
						expectedstring = analyzer->getNameForType(ret);
						erroneousstring = analyzer->getNameForType(assignment);
						throw string("Invalid type in assignment");
					}
					free(assignment);
				}
				break;

			case NT_LAMBDA_INVOCATION:
				{
					Type* lambda = typeCheck(tree->node_data.nodes[0]);
					Type* actual = MakeType(TYPE_LAMBDA);
					ret = actual->typedata.lambda.returntype = lambda->typedata.lambda.returntype;

					if(tree->subnodes == 2) {
						int i;
						for(i = 0; i < tree->node_data.nodes[1]->subnodes; i++) {
							AddTypeToTypeArray(typeCheck(tree->node_data.nodes[1]->node_data.nodes[i]), actual->typedata.lambda.arguments);
						}
					}

					if(!analyzer->isASubtypeOfB(actual, lambda)) {
						expectedstring = analyzer->getNameForType(lambda);
						erroneousstring = analyzer->getNameForType(actual);
						free(actual); // TODO: must be recursive (this is a guaranteed memleak)
						throw string("Argument lists not compatible in function invocation");
					}

					free(actual); // TODO: must be recursive (this is a guaranteed memleak)
				}
				break;

			// These need to have typechecking implemented
			case NT_CURRIED:
			case NT_MEMBER_ACCESS:
			case NT_METHOD_INVOCATION:
			case NT_SWITCH:
			case NT_FOR:
			case NT_RETRIEVAL:
			case NT_ARRAY_DECLARATION:
			case NT_INCREMENT:
			case NT_DECREMENT:
			case NT_IF_THEN_ELSE:
			case NT_CASE:

			// these might not be necessary
			case NT_ARGUMENTS:
			case NT_ARGUMENT:
			case NT_TYPE_ARRAY:
			case NT_VALUES:

			// these don't have types
			case NT_BLOCK:
			case NT_EXPRESSIONS:
			case NT_RETRIEVALS_STATEMENTS:
			case NT_DEFAULTCASE:
			case NT_BREAK:
			case NT_CONTINUE:
				{
					int i = 0;
					while(i < tree->subnodes) {
						ret = typeCheck(tree->node_data.nodes[i]);
						i++;
					}
				}
		}
	} catch(string errormsg) {
		errormsg += "; expected '";
		errormsg += expectedstring;
		errormsg += "'; actual was '";
		errormsg += erroneousstring;
		errormsg += "'";

		errors.push_back(new SemanticError(TYPE_ERROR, errormsg, tree, errorcontext));
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