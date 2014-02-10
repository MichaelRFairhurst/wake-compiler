#include "TypeChecker.h"
#include "CompilationExceptions.h"

TypeChecker::TypeChecker(ErrorTracker* errors, ObjectSymbolTable* objectsymtable, ScopeSymbolTable* scopesymtable, MethodSignatureParseTreeTraverser* methodanalyzer) {
	this->errors = errors;
	this->objectsymtable = objectsymtable;
	this->scopesymtable = scopesymtable;
	this->methodanalyzer = methodanalyzer;
}

void TypeChecker::check(Node* tree) {
	freeType(typeCheck(tree));

	flowAnalysis(tree, false, false, false);
	if(returntype != NULL && !exhaustiveReturns(tree))
		errors->addError(new SemanticError(INEXHAUSTIVE_RETURNS, "", tree));
}

bool TypeChecker::exhaustiveReturns(Node* tree) {
	switch(tree->node_type) {
		case NT_SWITCH:
			//not yet supported
			return false;

		case NT_FOR:
		case NT_WHILE:
			// Loops are always conditional and therefore never exhaustive
			return false;
		case NT_IF_ELSE:
			// If no else, then not exhaustive
			if(tree->subnodes == 2) return false;
			// Returns are exhaustive if both the if and else statements are exhaustive
			return exhaustiveReturns(tree->node_data.nodes[1]) && exhaustiveReturns(tree->node_data.nodes[2]);

		case NT_RETURN:
			return true;

		case NT_BLOCK:
		case NT_RETRIEVALS_STATEMENTS:
			{
				int i;
				// Statement lists which contain a return are exhaustive
				for(i = 0; i < tree->subnodes; i++)
					if(exhaustiveReturns(tree->node_data.nodes[i])) return true;
				return false;
			}

		default:
			return false;
	}
}

void TypeChecker::flowAnalysis(Node* tree, bool breakable, bool caseable, bool continuable) {
	int i;
	switch(tree->node_type) {
		// These can contain switches, loops, switches, and breaks
		case NT_SWITCH:
			for(i = 0; i < tree->subnodes; i++)
				flowAnalysis(tree->node_data.nodes[i], true, true, continuable);
			break;
		case NT_FOR:
		case NT_WHILE:
			for(i = 0; i < tree->subnodes; i++)
				flowAnalysis(tree->node_data.nodes[i], true, caseable, true);
			break;
		case NT_BLOCK:
		case NT_IF_ELSE:
		case NT_RETRIEVALS_STATEMENTS:
			for(i = 0; i < tree->subnodes; i++)
				flowAnalysis(tree->node_data.nodes[i], breakable, caseable, continuable);
			break;

		// These can't! don't waste time recursing on them or anything else
		case NT_CASE:
		case NT_DEFAULTCASE:
			if(!caseable)
				errors->addError(new SemanticError(ILLEGAL_CASE, "", tree));
			break;
		case NT_BREAK:
			if(!breakable)
				errors->addError(new SemanticError(ILLEGAL_BREAK, "", tree));
			break;
		case NT_CONTINUE:
			if(!continuable)
				errors->addError(new SemanticError(ILLEGAL_CONTINUE, "", tree));
			break;
	}
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
				{
					boost::optional<Type*> variable = scopesymtable->find(string("@") + tree->node_data.string);
					if(!variable) {
						PropertySymbolTable* proptable = objectsymtable->find(thiscontext);
						variable = proptable->find(string("@") + tree->node_data.string);
					}
					if(!variable) {
						ret = MakeType(TYPE_MATCHALL);
						errors->addError(new SemanticError(SYMBOL_NOT_DEFINED, "Symbol by name of @" + string(tree->node_data.string) + " not found", tree));
					} else {
						ret = copyType(*variable);
					}
				}
				break;

			case NT_TYPEDATA:
				{
					boost::optional<Type*> variable = scopesymtable->find(tree->node_data.type);
					if(!variable) {
						PropertySymbolTable* proptable = objectsymtable->find(thiscontext);
						variable = proptable->find(scopesymtable->getNameForType(tree->node_data.type));
					}
					if(!variable) {
						ret = MakeType(TYPE_MATCHALL);
						errors->addError(new SemanticError(SYMBOL_NOT_DEFINED, "Symbol by name of " + scopesymtable->getNameForType(tree->node_data.type) + " not found", tree));
					} else {
						ret = copyType(*variable);
					}
				}
				break;

			case NT_THIS:
				ret = MakeType(TYPE_CLASS);
				ret->typedata._class.classname = strdup(thiscontext.c_str());
				break;

			case NT_NOTHING:
				ret = MakeType(TYPE_NOTHING);
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
						ret = MakeType(TYPE_ERROR);

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
					Type* cmp = typeCheck(tree->node_data.nodes[1]);

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
					if(!isValidLValue(tree->node_data.nodes[0])) {
						errors->addError(new SemanticError(INVALID_ASSIGNMENT, "", tree));
					} else {
						ret = typeCheck(tree->node_data.nodes[0]);
						if(tree->node_data.nodes[1]->node_type == NT_ARRAY_DECLARATION && tree->node_data.nodes[1]->subnodes == 0) {
							if(!ret->arrayed) {
								expectedstring = analyzer->getNameForType(ret);
								erroneousstring = "[]";
								throw string("Invalid type in assignment");
							}
						} else {
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
					}
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
						TypeArray* args = MakeTypeArray();
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
					boost::optional<Type*> lambdatype = methodtable->find(methodtable->getSymbolNameOf(&method_segments));

					if(subject->optional) {
						errors->addError(new SemanticError(DIRECT_USE_OF_OPTIONAL_TYPE, "Calling " + methodtable->getSymbolNameOf(&method_segments) + " on optional type" + subject->typedata._class.classname + ". You must first wrap object in an exists { } clause.", tree));
						ret = MakeType(TYPE_MATCHALL);
					}

					if(lambdatype) {
						ret = copyType((*lambdatype)->typedata.lambda.returntype);
					} else {
						errors->addError(new SemanticError(PROPERTY_OR_METHOD_NOT_FOUND, "Couldn't find property " + methodtable->getSymbolNameOf(&method_segments) + " on class" + subject->typedata._class.classname, tree));
						ret = MakeType(TYPE_MATCHALL);
					}

					AddSubNode(tree, MakeNodeFromString(NT_COMPILER_HINT, strdup(subject->typedata._class.classname)));
					AddSubNode(tree, MakeNodeFromString(NT_COMPILER_HINT, strdup(methodtable->getSymbolNameOf(&method_segments).c_str())));

					freeType(subject);
					for(vector<pair<string, TypeArray*> >::iterator it = method_segments.begin(); it != method_segments.end(); ++it)
						freeTypeArray(it->second);
				}
				break;

			case NT_DECLARATION:
				try {
					Type* assignee = tree->node_data.nodes[0]->node_data.type;
					objectsymtable->assertTypeIsValid(assignee);
					if(assignee->arrayed && tree->node_data.nodes[1]->subnodes == 0 && tree->node_data.nodes[1]->node_type == NT_ARRAY_DECLARATION) {
						// Nothing to do here but relax.
					} else {
						Type* assignment = typeCheck(tree->node_data.nodes[1]);
						if(!analyzer->isASubtypeOfB(assignment, assignee)) {
							expectedstring = analyzer->getNameForType(assignee);
							erroneousstring = analyzer->getNameForType(assignment);
							freeType(assignment);
							throw string("Invalid value in declaration of variable");
						}
						freeType(assignment);
					}
					scopesymtable->add(tree->node_data.nodes[0]->node_data.type);
				} catch(SymbolNotFoundException* e) {
					errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
					delete e;
				}
				break;

			case NT_CAST:
				try {
					ret = copyType(tree->node_data.nodes[0]->node_data.type);
					objectsymtable->assertTypeIsValid(ret);
					Type* casted = typeCheck(tree->node_data.nodes[1]);
					if(!analyzer->isASubtypeOfB(casted, ret)) {
						expectedstring = analyzer->getNameForType(ret);
						erroneousstring = analyzer->getNameForType(casted);
						freeType(casted);
						throw string("Casting an object that is not a valid subtype");
					}
					freeType(casted);
				} catch(SymbolNotFoundException* e) {
					errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
					delete e;
				}
				break;

			case NT_RETRIEVAL:
				{
					Type* provider;
					try {
						provider = typeCheck(tree->node_data.nodes[2]);
						ret = copyType(tree->node_data.nodes[0]->node_data.type);
						//TODO index 1 is the arguments
						objectsymtable->assertTypeIsValid(ret);
						try {
							// TODO This does more work than we need to since it recurses
							objectsymtable->getAnalyzer()->assertClassCanProvide(provider, ret);
						} catch(SemanticError *e) {
							e->token = tree;
							errors->addError(e);
						}
						AddSubNode(tree, MakeNodeFromString(NT_COMPILER_HINT, strdup(provider->typedata._class.classname)));
					} catch(SymbolNotFoundException* e) {
						errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
						delete e;
					}
					freeType(provider);
				}
				break;

			case NT_ARRAY_DECLARATION:
				// These should all be specially handled in the other cases
				if(!tree->subnodes) {
					errors->addError(new SemanticError(TYPE_ERROR, "Invalid use of empty list", tree));
				} else {
					errors->addError(new SemanticError(WARNING, "Not supported yet", tree));
				}
				break;

			case NT_EXISTS:
				try {
					Type* ret = typeCheck(tree->node_data.nodes[0]);

					if(!ret->optional) {
						errors->addError(new SemanticError(EXISTS_ON_NONOPTIONAL_TYPE, "exists { } statement uses a nonoptional type", tree)); // @todo better error message!
						break;
					}

					objectsymtable->assertTypeIsValid(ret);

					Type* real = copyType(ret);
					real->optional = 0;

					scopesymtable->addOverwriting(real);
					freeType(typeCheck(tree->node_data.nodes[1]));
					scopesymtable->addOverwriting(ret);
					freeType(real);
				} catch(SymbolNotFoundException* e) {
					errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
					delete e;
				}
				break;

			// Ignoring these for now
			case NT_SWITCH:
			case NT_CURRIED:
			case NT_INCREMENT:
			case NT_DECREMENT:
			// These require a common-ancestor function
			case NT_IF_THEN_ELSE:
			// These I will implement soon(ish)
			case NT_MEMBER_ACCESS:
			// these might not be necessary
			case NT_TYPE_ARRAY:
			case NT_VALUES:
				throw string("Not supported yet");

			// These have tests so can't throw, but won't compile anyway as long as switch throws
			case NT_DEFAULTCASE:
			case NT_CASE:

			// these don't have types
			case NT_BLOCK:
				scopesymtable->pushScope();
				// FALL THROUGH!
			case NT_EXPRESSIONS:
			case NT_RETRIEVALS_STATEMENTS:
			case NT_BREAK:
			case NT_CONTINUE:
				try {
					int i = 0;
					while(i < tree->subnodes) {
						if(i > 0) freeType(ret);
						ret = typeCheck(tree->node_data.nodes[i]);
						i++;
					}
					if(tree->node_type == NT_BLOCK) scopesymtable->popScope();
				} catch(SemanticError *e) {
					if(tree->node_type == NT_BLOCK) scopesymtable->popScope();
					throw e;
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

bool TypeChecker::isValidLValue(Node* n) {
	switch(n->node_type) {
		case NT_ALIAS: return true;
		case NT_TYPEDATA: return true;
		case NT_ARRAY_ACCESS:
			return isValidLValue(n->node_data.nodes[0]);
		default:
			return false;
	}
}
