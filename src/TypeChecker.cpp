#include "TypeChecker.h"

TypeChecker::TypeChecker(ErrorTracker* errors, ObjectSymbolTable* objectsymtable, ScopeSymbolTable* scopesymtable, MethodSignatureParseTreeTraverser* methodanalyzer) {
	this->errors = errors;
	this->objectsymtable = objectsymtable;
	this->scopesymtable = scopesymtable;
	this->methodanalyzer = methodanalyzer;
}

void TypeChecker::check(Node* tree) {
	freeType(typeCheck(tree));

	if(!exhaustiveReturns(tree))
		return; // TODO
}

bool TypeChecker::exhaustiveReturns(Node* tree) {
	return true; // TODO
}

void TypeChecker::setReturnType(Type* returntype) {
	this->returntype = returntype;
}

void TypeChecker::setThisContext(string name) {
	thiscontext = name;
}

Type* TypeChecker::typeCheck(Node* tree) {
	TypeAnalyzer* analyzer = objectsymtable->getAnalyzer();
	Type* ret = NULL;
	string erroneousstring;
	string expectedstring;

	try {
		switch(tree->node_type) {
			// Here is where the recursion ends
			case NT_STRINGLIT:
				ret = MakeType(TYPE_CLASS);
				ret->typedata._class.classname = strdup("Text");
				break;

			case NT_NUMBERLIT:
				ret = MakeType(TYPE_CLASS);
				ret->typedata._class.classname = strdup("Int");
				break;

			case NT_TRUTHLIT:
				ret = MakeType(TYPE_CLASS);
				ret->typedata._class.classname = strdup("Truth");
				break;

			case NT_ALIAS:
				ret = copyType(scopesymtable->get(string("@") + tree->node_data.string));
				break;

			case NT_TYPEDATA:
				ret = copyType(scopesymtable->get(tree->node_data.type));
				break;

			case NT_THIS:
				ret = MakeType(TYPE_CLASS);
				ret->typedata._class.classname = strdup(thiscontext.c_str());
				break;

			case NT_PARENT:
				throw string("not yet implemented");
				break;

			/**
			 *
			 * Backtracking from the above endpoints
			 *
			 */
			case NT_ADD:
				{
					ret = typeCheck(tree->node_data.nodes[0]);
					Type* additive = typeCheck(tree->node_data.nodes[1]);

					if(analyzer->isPrimitiveTypeInt(ret)) {
						if(!analyzer->isPrimitiveTypeInt(additive)) {
							erroneousstring = analyzer->getNameForType(additive); freeType(additive);
							expectedstring = "Int";
							throw string("Addition with a non-numeral");
						}

					} else if(analyzer->isPrimitiveTypeText(ret)) {
						if(!analyzer->isPrimitiveTypeText(additive)) {
							erroneousstring = analyzer->getNameForType(additive); freeType(additive);
							expectedstring = "Text";
							throw string("Concatenation with non-Text");
						}

					} else {
						expectedstring = "Int' or 'Text";
						erroneousstring = analyzer->getNameForType(ret);
						freeType(additive); freeType(ret);
						ret = MakeType(TYPE_CLASS);
						ret->typedata._class.classname = strdup("Int");

						throw string("Only numerics or Texts can be added/concatenated");
					}

					freeType(additive);
				}
				break;

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
							erroneousstring = analyzer->getNameForType(ret); freeType(ret);
							ret = MakeType(TYPE_CLASS);
							ret->typedata._class.classname = strdup("Int");
						}

						freeType(factor);

						throw string("Mathematical operation performed on non-numerals");
					}

					freeType(factor);
				}
				break;

			case NT_LESSTHAN:
			case NT_LESSTHANEQUAL:
			case NT_GREATERTHAN:
			case NT_GREATERTHANEQUAL:
				{
					ret = MakeType(TYPE_CLASS);
					ret->typedata._class.classname = strdup("Truth");

					Type* a = typeCheck(tree->node_data.nodes[0]);
					Type* b = typeCheck(tree->node_data.nodes[1]);

					if(!analyzer->isPrimitiveTypeInt(a) || !analyzer->isPrimitiveTypeInt(b)) {
						expectedstring = "Int";

						if(analyzer->isPrimitiveTypeInt(a)) {
							erroneousstring = analyzer->getNameForType(b);
						} else {
							erroneousstring = analyzer->getNameForType(a);
						}

						freeType(a); freeType(b);

						throw string("Mathematical operation performed on non-numerals");
					}

					freeType(a);
					freeType(b);
				}
				break;

			case NT_EQUALITY:
			case NT_INEQUALITY:
				{
					Type* a = typeCheck(tree->node_data.nodes[0]);
					Type* b = typeCheck(tree->node_data.nodes[1]);
					ret = MakeType(TYPE_CLASS);
					ret->typedata._class.classname = strdup("Truth");

					if(!analyzer->isASubtypeOfB(a, b) && !analyzer->isASubtypeOfB(b, a)) {
						expectedstring = analyzer->getNameForType(a);
						erroneousstring = analyzer->getNameForType(b);
						freeType(a); freeType(b);
						throw string("Uncomparable types");
					}

					freeType(a); freeType(b);
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
							erroneousstring = analyzer->getNameForType(ret); freeType(ret);
							ret = MakeType(TYPE_CLASS);
							ret->typedata._class.classname = strdup("Truth");
						}
						freeType(cmp);

						expectedstring = "Truth";

						throw string("AND or OR comparison on non-Truth types");
					}

					freeType(cmp);
				}
				break;

			case NT_INVERT:
				ret = typeCheck(tree->node_data.nodes[0]);

				if(!analyzer->isPrimitiveTypeTruth(ret)) {
					expectedstring = "Truth";
					erroneousstring = analyzer->getNameForType(ret);
					freeType(ret);
					ret = MakeType(TYPE_CLASS);
					ret->typedata._class.classname = strdup("Truth");
					ret->arrayed = 0;
					throw string("If conditions must be Truths");
				}
				break;

			case NT_ARRAY_ACCESS:
				{
					ret = typeCheck(tree->node_data.nodes[0]);
					Type* index = typeCheck(tree->node_data.nodes[1]);
					--ret->arrayed;

					if(!analyzer->isPrimitiveTypeInt(index)) {
						erroneousstring = analyzer->getNameForType(index); freeType(index);
						expectedstring = "Int";
						throw string("Array indices must be numeric");
					} else if(ret->arrayed < 0) {
						erroneousstring = analyzer->getNameForType(ret); freeType(index);
						throw string("Getting index of non-array");
					}

					freeType(index);
				}
				break;

			case NT_IF_ELSE:
			case NT_WHILE:
				{
					ret = typeCheck(tree->node_data.nodes[0]);
					freeType(typeCheck(tree->node_data.nodes[1]));

					if(tree->subnodes > 2) {
						freeType(typeCheck(tree->node_data.nodes[2]));
					}

					if(!analyzer->isPrimitiveTypeTruth(ret)) {
						expectedstring = "Truth"; freeType(ret);
						ret = MakeType(TYPE_CLASS);
						ret->typedata._class.classname = strdup("Truth");
						throw string("If/While conditions must be Truths");
					}
				}
				break;

			case NT_FOR:
				{
					freeType(typeCheck(tree->node_data.nodes[0]));
					ret = typeCheck(tree->node_data.nodes[1]);
					freeType(typeCheck(tree->node_data.nodes[2]));
					freeType(typeCheck(tree->node_data.nodes[3]));

					if(!analyzer->isPrimitiveTypeTruth(ret)) {
						expectedstring = "Truth"; freeType(ret);
						ret = MakeType(TYPE_CLASS);
						ret->typedata._class.classname = strdup("Truth");
						throw string("For conditions must be Truths");
					}
				}
				break;

			case NT_RETURN:
				if(returntype == NULL) {
					if(tree->subnodes == 0) break;

					ret = typeCheck(tree->node_data.nodes[0]);
					expectedstring = "VOID";
					erroneousstring = analyzer->getNameForType(ret);
					throw string("Method is not allowed to return anything");
				}

				if(tree->subnodes == 0) {
					expectedstring = analyzer->getNameForType(returntype);
					erroneousstring = "VOID";
					throw string("Method is not allowed to return without a value");
				}

				ret = typeCheck(tree->node_data.nodes[0]);
				if(!analyzer->isASubtypeOfB(ret, returntype)) {
					expectedstring = analyzer->getNameForType(returntype);
					erroneousstring = analyzer->getNameForType(ret);
					throw string("Return type is incompatible with method signature");
				}
				break;

			case NT_ASSIGNMENT:
				{
					ret = typeCheck(tree->node_data.nodes[0]);
					Type* assignment = typeCheck(tree->node_data.nodes[1]);
					//TODO This leaks for invalid property names since analyzer throws a SymbolNotFoundException
					if(!analyzer->isASubtypeOfB(assignment, ret)) {
						expectedstring = analyzer->getNameForType(ret);
						erroneousstring = analyzer->getNameForType(assignment);
						freeType(assignment);
						throw string("Invalid type in assignment");
					}
					freeType(assignment);
				}
				break;

			case NT_LAMBDA_INVOCATION:
				{
					Type* lambda = typeCheck(tree->node_data.nodes[0]);
					Type* actual = MakeType(TYPE_LAMBDA);
					actual->typedata.lambda.arguments = MakeTypeArray();
					ret = copyType(actual->typedata.lambda.returntype = copyType(lambda->typedata.lambda.returntype));

					if(tree->subnodes == 2) {
						int i;
						for(i = 0; i < tree->node_data.nodes[1]->subnodes; i++) {
							AddTypeToTypeArray(typeCheck(tree->node_data.nodes[1]->node_data.nodes[i]), actual->typedata.lambda.arguments);
						}
					}

					if(!analyzer->isASubtypeOfB(actual, lambda)) {
						expectedstring = analyzer->getNameForType(lambda);
						erroneousstring = analyzer->getNameForType(actual);
						freeType(actual); freeType(lambda);
						throw string("Argument lists not compatible in function invocation");
					}

					freeType(actual); freeType(lambda);
				}
				break;

			case NT_METHOD_INVOCATION:
				{
					Type* subject = typeCheck(tree->node_data.nodes[0]);
					Node* methodname = tree->node_data.nodes[1];
					vector<pair<string, TypeArray*> > method_segments;

					int i = 0;
					while(i < methodname->subnodes) {
						string name = methodname->node_data.nodes[i]->node_data.string;
						TypeArray* args = MakeTypeArray(); // TODO must delete
						i++;

						if(methodname->subnodes > i) {
							int a;
							for(a = 0; a < methodname->node_data.nodes[i]->subnodes; a++)
								AddTypeToTypeArray(typeCheck(methodname->node_data.nodes[i]->node_data.nodes[a]), args);
						}

						method_segments.push_back(pair<string, TypeArray*>(name, args));
						i++;
					}

					PropertySymbolTable* methodtable = objectsymtable->find(subject->typedata._class.classname);
					Type* lambdatype = methodtable->get(methodtable->getSymbolNameOf(&method_segments));

					ret = copyType(lambdatype->typedata.lambda.returntype);
				}
				break;

			// Ignoring these for now
			case NT_SWITCH:
			case NT_CASE:
			case NT_CURRIED:
			case NT_INCREMENT:
			case NT_DECREMENT:
				throw string("Not supported yet");

			// These require a common-ancestor function
			case NT_ARRAY_DECLARATION:
			case NT_IF_THEN_ELSE:

			// These I will implement soon(ish)
			case NT_MEMBER_ACCESS:
			case NT_RETRIEVAL:

			// these might not be necessary
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
						if(i > 0) freeType(ret);
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

		errors->addError(new SemanticError(TYPE_ERROR, errormsg, tree));
	}

	return ret;
}
