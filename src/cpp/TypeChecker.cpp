/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TypeChecker.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By: Coleridge Williamson
 *
 **************************************************/

#include "TypeChecker.h"
#include "CompilationExceptions.h"
#include "TypeParameterizer.h"

TypeChecker::TypeChecker(ErrorTracker* errors, ClassSpaceSymbolTable* classestable, ScopeSymbolTable* scopesymtable, MethodSignatureParseTreeTraverser* methodanalyzer) {
	this->errors = errors;
	this->classestable = classestable;
	this->scopesymtable = scopesymtable;
	this->methodanalyzer = methodanalyzer;
	thiscontext = NULL;
}

void TypeChecker::check(Node* tree) {
	freeType(typeCheck(tree, false));

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

		case NT_EXISTS:
		case NT_IF_ELSE:
			// If no else, then not exhaustive
			if(tree->subnodes == 2) return false;
			// Returns are exhaustive if both the if and else statements are exhaustive
			return exhaustiveReturns(tree->node_data.nodes[1]) && exhaustiveReturns(tree->node_data.nodes[2]);

		case NT_TRY:
			// If no breok, then not exhaustive
			if(tree->subnodes == 1) return false;
			// Returns are exhaustive if both the try and catch statements are exhaustive
			return exhaustiveReturns(tree->node_data.nodes[0]) && exhaustiveReturns(tree->node_data.nodes[1]);

		case NT_RETURN:
		case NT_THROW:
			return true;

		case NT_CATCH:
		case NT_BLOCK:
		case NT_RETRIEVALS_STATEMENTS:
			{
				int i;
				// Statement lists which contain a return are exhaustive
				for(i = 0; i < tree->subnodes; i++)
					if(exhaustiveReturns(tree->node_data.nodes[i])) return true;
				return false;
			}

		case NT_ABSTRACT_METHOD:
			return true;

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

void TypeChecker::setThisContext(Type* context) {
	thiscontext = context;
}

void TypeChecker::setParameterizedTypes(const vector<Type*>& types) {
	parameterizedtypes = types;
}

Type* TypeChecker::typeCheck(Node* tree, bool forceArrayIdentifier) {
	TypeAnalyzer* analyzer = classestable->getAnalyzer();
	Type* ret = NULL;
	string erroneousstring;
	string expectedstring;

	// read Printer as Printer[] from ARRAY_ACCESS nodes
	// but not if there are ANY nodes before the next TYPEDATA
	if(forceArrayIdentifier && !(tree->node_type == NT_TYPEDATA || tree->node_type == NT_MEMBER_ACCESS)) {
		forceArrayIdentifier = false;
	}

	try {
		switch(tree->node_type) {
			// Here is where the recursion ends
			case NT_STRINGLIT:
				ret = MakeType(TYPE_CLASS);
				ret->typedata._class.classname = strdup("Text");
				break;

			case NT_NUMBERLIT:
				ret = MakeType(TYPE_CLASS);
				ret->typedata._class.classname = strdup("Num");
				break;

			case NT_BOOLLIT:
				ret = MakeType(TYPE_CLASS);
				ret->typedata._class.classname = strdup("Bool");
				break;

			case NT_ALIAS:
				{
					char* name = tree->node_data.string;
					boost::optional<Type*> variable = scopesymtable->find(name);
					if(!variable && thiscontext != NULL) {
						PropertySymbolTable* proptable = classestable->findModifiable(thiscontext);
						variable = proptable->find(name);
						if(variable) {
							name = strdup(name);
							tree->node_type = NT_MEMBER_ACCESS;
							AddSubNode(tree, MakeEmptyNode(NT_THIS));
							AddSubNode(tree, MakeNodeFromString(NT_COMPILER_HINT, name));
						}
					}
					if(!variable) {
						ret = MakeType(TYPE_MATCHALL);
						errors->addError(new SemanticError(SYMBOL_NOT_DEFINED, "Symbol by name of " + string(name) + " not found", tree));
					} else {
						ret = copyType(*variable);
					}
				}
				break;

			case NT_TYPEDATA:
				{
					TypeParameterizer parameterizer;
					parameterizer.writeInParameterizations(&tree->node_data.type, parameterizedtypes);

					Type* type = copyType(tree->node_data.type);
					if(forceArrayIdentifier) type->arrayed = 1;

					boost::optional<Type*> variable = scopesymtable->find(type);
					if(!variable && thiscontext != NULL) {
						PropertySymbolTable* proptable = classestable->findModifiable(thiscontext);
						variable = proptable->find(scopesymtable->getNameForType(type));
						if(variable) {
							char* propname = strdup(scopesymtable->getNameForType(type).c_str());
							tree->node_type = NT_MEMBER_ACCESS;
							AddSubNode(tree, MakeEmptyNode(NT_THIS));
							AddSubNode(tree, MakeNodeFromString(NT_COMPILER_HINT, propname));
						}
					}
					if(!variable) {
						ret = MakeType(TYPE_MATCHALL);
						errors->addError(new SemanticError(SYMBOL_NOT_DEFINED, "Symbol by name of " + scopesymtable->getNameForType(type) + " not found", tree));
					} else {
						// detect if they wrote Printer[] instead of Printer[][]
						// Either way Printer[x] would work, so if forceArrayIdentifier skip this step
						if(!forceArrayIdentifier && type->arrayed != (*variable)->arrayed)
							errors->addError(new SemanticError(SYMBOL_NOT_DEFINED, "Accessed arrayed variable " + scopesymtable->getNameForType(type) + " with wrong number of [] brackets.", tree));

						ret = copyType(*variable);
					}
				}
				break;

			case NT_THIS:
				ret = copyType(thiscontext);
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
			case NT_ADD_ASSIGNMENT:
				if(!isValidLValue(tree->node_data.nodes[0])) {
					errors->addError(new SemanticError(INVALID_ASSIGNMENT, "", tree));
						break;
				}
				// FALLTHROUGH
			case NT_ADD:
				{
					ret = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);
					Type* additive = typeCheckUsable(tree->node_data.nodes[1], forceArrayIdentifier);

					// Ensure the left operand is never a matchall when the right is
					// this way, when ret is matchall, so we can run all typechecks by the
					// lefthand side
					if(ret->type == TYPE_MATCHALL) {
						Type* temp = ret;
						ret = additive;
						additive = temp;
					}

					if(analyzer->isPrimitiveTypeNum(ret) || ret->type == TYPE_MATCHALL) {
						if(!analyzer->isPrimitiveTypeNum(additive) && ret->type != TYPE_MATCHALL) {
							erroneousstring = analyzer->getNameForType(additive); freeType(additive);
							expectedstring = "Num";
							throw string("Addition with a non-numeral");
						}

					} else if(analyzer->isPrimitiveTypeText(ret)) {
						if(!analyzer->isPrimitiveTypeText(additive) && ret->type != TYPE_MATCHALL) {
							erroneousstring = analyzer->getNameForType(additive); freeType(additive);
							expectedstring = "Text";
							throw string("Concatenation with non-Text");
						}

					} else {
						expectedstring = "Num' or 'Text";
						erroneousstring = analyzer->getNameForType(ret);
						freeType(additive); freeType(ret);
						ret = MakeType(TYPE_ERROR);

						throw string("Only numerics or Texts can be added/concatenated");
					}

					freeType(additive);
				}
				break;

			case NT_MULT_ASSIGNMENT:
			case NT_DIV_ASSIGNMENT:
			case NT_SUB_ASSIGNMENT:
				if(!isValidLValue(tree->node_data.nodes[0])) {
					errors->addError(new SemanticError(INVALID_ASSIGNMENT, "", tree));
						break;
				}
				// FALLTHROUGH
			case NT_MULTIPLY:
			case NT_DIVIDE:
			case NT_SUBTRACT:
				{
					ret = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);
					Type* factor = typeCheckUsable(tree->node_data.nodes[1], forceArrayIdentifier);

					if(!analyzer->isPrimitiveTypeNum(ret) || !analyzer->isPrimitiveTypeNum(factor)) {
						expectedstring = "Num";

						if(analyzer->isPrimitiveTypeNum(ret)) {
							erroneousstring = analyzer->getNameForType(factor);
						} else {
							erroneousstring = analyzer->getNameForType(ret); freeType(ret);
							ret = MakeType(TYPE_CLASS);
							ret->typedata._class.classname = strdup("Num");
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
					ret->typedata._class.classname = strdup("Bool");

					Type* a = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);
					Type* b = typeCheckUsable(tree->node_data.nodes[1], forceArrayIdentifier);

					if(!analyzer->isPrimitiveTypeNum(a) || !analyzer->isPrimitiveTypeNum(b)) {
						expectedstring = "Num";

						if(analyzer->isPrimitiveTypeNum(a)) {
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
					Type* a = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);
					Type* b = typeCheckUsable(tree->node_data.nodes[1], forceArrayIdentifier);
					ret = MakeType(TYPE_CLASS);
					ret->typedata._class.classname = strdup("Bool");

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
					ret = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);
					Type* cmp = typeCheckUsable(tree->node_data.nodes[1], forceArrayIdentifier);

					if(!analyzer->isPrimitiveTypeBool(ret) || !analyzer->isPrimitiveTypeBool(cmp)) {
						if(analyzer->isPrimitiveTypeBool(ret)) {
							erroneousstring = analyzer->getNameForType(cmp);
						} else {
							erroneousstring = analyzer->getNameForType(ret); freeType(ret);
							ret = MakeType(TYPE_CLASS);
							ret->typedata._class.classname = strdup("Bool");
						}
						freeType(cmp);

						expectedstring = "Bool";

						throw string("AND or OR comparison on non-Bool types");
					}

					freeType(cmp);
				}
				break;

			case NT_INVERT:
				ret = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);

				if(!analyzer->isPrimitiveTypeBool(ret)) {
					expectedstring = "Bool";
					erroneousstring = analyzer->getNameForType(ret);
					freeType(ret);
					ret = MakeType(TYPE_CLASS);
					ret->typedata._class.classname = strdup("Bool");
					ret->arrayed = 0;
					throw string("If conditions must be Bool");
				}
				break;

			case NT_ARRAY_ACCESS:
				{
					forceArrayIdentifier = true;
					ret = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);
					forceArrayIdentifier = false;
					Type* index = typeCheckUsable(tree->node_data.nodes[1], forceArrayIdentifier);
					--ret->arrayed;

					if(!analyzer->isPrimitiveTypeNum(index) && index->type != TYPE_MATCHALL) {
						erroneousstring = analyzer->getNameForType(index); freeType(index);
						expectedstring = "Num";
						throw string("Array indices must be numeric");
					} else if(ret->arrayed < 0 && ret->type != TYPE_MATCHALL) {
						erroneousstring = analyzer->getNameForType(ret); freeType(index);
						throw string("Getting index of non-array");
					}

					freeType(index);
				}
				break;

			case NT_THROW:
				{
					ret = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);
					if(!analyzer->isException(ret)) {
						expectedstring = "Exception";
						erroneousstring = analyzer->getNameForType(ret);
						throw string("Can only throw subclasses of exception");
					}
				}
				break;

			case NT_IF_ELSE:
			case NT_WHILE:
				{
					ret = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);
					freeType(typeCheck(tree->node_data.nodes[1], false));

					if(tree->subnodes > 2) {
						freeType(typeCheck(tree->node_data.nodes[2], forceArrayIdentifier));
					}

					if(!analyzer->isPrimitiveTypeNum(ret) && !analyzer->isPrimitiveTypeBool(ret)) {
						erroneousstring = analyzer->getNameForType(ret);
						expectedstring = "Bool"; freeType(ret);
						ret = MakeType(TYPE_CLASS);
						ret->typedata._class.classname = strdup("Bool");
						throw string("If/While conditions must be Bool or Num");
					}
				}
				break;

			case NT_FOR:
				{
					scopesymtable->pushScope();
					freeType(typeCheck(tree->node_data.nodes[0], forceArrayIdentifier));
					ret = typeCheckUsable(tree->node_data.nodes[1], forceArrayIdentifier);
					freeType(typeCheck(tree->node_data.nodes[2], forceArrayIdentifier));
					freeType(typeCheck(tree->node_data.nodes[3], forceArrayIdentifier));
					scopesymtable->popScope();

					if(!analyzer->isPrimitiveTypeBool(ret)) {
						expectedstring = "Bool"; freeType(ret);
						ret = MakeType(TYPE_CLASS);
						ret->typedata._class.classname = strdup("Bool");
						throw string("For conditions must be Bool");
					}
				}
				break;

			case NT_RETURN:
				if(returntype == NULL) {
					if(tree->subnodes == 0) break;

					ret = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);
					expectedstring = "VOID";
					erroneousstring = analyzer->getNameForType(ret);
					throw string("Method is not allowed to return anything");
				}

				if(tree->subnodes == 0) {
					expectedstring = analyzer->getNameForType(returntype);
					erroneousstring = "VOID";
					throw string("Method is not allowed to return without a value");
				}

				ret = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);
				if(!analyzer->isASubtypeOfB(ret, returntype)) {
					expectedstring = analyzer->getNameForType(returntype);
					erroneousstring = analyzer->getNameForType(ret);
					throw string("Return type is incompatible with method signature");
				}
				break;

			case NT_VALUED_ASSIGNMENT:
			case NT_ASSIGNMENT:
				{
					if(!isValidLValue(tree->node_data.nodes[0])) {
						errors->addError(new SemanticError(INVALID_ASSIGNMENT, "", tree));
						ret = MakeType(tree->node_type == NT_ASSIGNMENT ? TYPE_UNUSABLE : TYPE_MATCHALL);
					} else {
						Type* subject = typeCheck(tree->node_data.nodes[0], forceArrayIdentifier);
						ret = tree->node_type == NT_ASSIGNMENT ? MakeType(TYPE_UNUSABLE) : subject;
						if(tree->node_data.nodes[1]->node_type == NT_ARRAY_DECLARATION && tree->node_data.nodes[1]->subnodes == 0) {
							if(!subject->arrayed) {
								expectedstring = analyzer->getNameForType(subject);
								erroneousstring = "[]";
								if(tree->node_type == NT_ASSIGNMENT) freeType(subject);
								throw string("Invalid type in assignment");
							}
						} else {
							Type* assignment = typeCheckUsable(tree->node_data.nodes[1], forceArrayIdentifier);
							//TODO This leaks for invalid property names since analyzer throws a SymbolNotFoundException
							if(!analyzer->isASubtypeOfB(assignment, subject)) {
								expectedstring = analyzer->getNameForType(subject);
								erroneousstring = analyzer->getNameForType(assignment);
								if(tree->node_type == NT_ASSIGNMENT) freeType(subject);
								freeType(assignment);
								throw string("Invalid type in assignment");
							}
							if(tree->node_type == NT_ASSIGNMENT) freeType(subject);
							freeType(assignment);
						}
					}
				}
				break;

			case NT_LAMBDA_INVOCATION:
				{
					Type* lambda = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);
					Type* actual = MakeType(TYPE_LAMBDA);
					actual->typedata.lambda.arguments = MakeTypeArray();
					ret = copyType(actual->typedata.lambda.returntype = copyType(lambda->typedata.lambda.returntype));

					if(tree->subnodes == 2) {
						int i;
						for(i = 0; i < tree->node_data.nodes[1]->subnodes; i++) {
							AddTypeToTypeArray(typeCheckUsable(tree->node_data.nodes[1]->node_data.nodes[i], forceArrayIdentifier), actual->typedata.lambda.arguments);
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
					Node* methodname = tree->node_data.nodes[1];

					if(tree->node_data.nodes[0]->node_type == NT_THIS && methodname->subnodes == 2) {
						string name = methodname->node_data.nodes[0]->node_data.string;
						boost::optional<Type*> variable = scopesymtable->find(name);
						if(variable) {
							methodname->node_type = NT_LAMBDA_INVOCATION;
							methodname->node_data.nodes[0]->node_type = NT_ALIAS;
							ret = typeCheckUsable(methodname, forceArrayIdentifier);
							break;
						}
					}

					Type* subject = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);

					if(subject->type == TYPE_MATCHALL) {
						ret = subject;
						break;
					}

					Type* boxedtype;
					if(analyzer->isAutoboxedType(subject, &boxedtype)) {
						Node* node = tree->node_data.nodes[0];
						tree->node_data.nodes[0] = MakeTwoBranchNode(NT_AUTOBOX, node, MakeNodeFromString(NT_COMPILER_HINT, strdup(boxedtype->typedata._class.classname)));
						freeType(subject);
						subject = boxedtype;
					}

					ReadOnlyPropertySymbolTable* methodtable;
					try {
						methodtable = classestable->find(subject);
					} catch (SymbolNotFoundException* e) {
						ret = MakeType(TYPE_MATCHALL);
						errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, string("Class by name of ") + subject->typedata._class.classname + " returned by another expression has not been imported and cannot be resolved", tree));
						freeType(subject);
						break;
					}
					vector<pair<string, TypeArray*> > method_segments;

					int i = 0;
					while(i < methodname->subnodes) {
						string name = methodname->node_data.nodes[i]->node_data.string;
						TypeArray* args = MakeTypeArray();
						i++;

						if(methodname->subnodes > i) {
							int a;
							for(a = 0; a < methodname->node_data.nodes[i]->subnodes; a++)
								AddTypeToTypeArray(typeCheckUsable(methodname->node_data.nodes[i]->node_data.nodes[a], forceArrayIdentifier), args);
						}

						method_segments.push_back(pair<string, TypeArray*>(name, args));
						i++;
					}

					boost::optional<Type*> lambdatype = methodtable->find(methodtable->getSymbolNameOf(&method_segments));

					if(subject->optional) {
						errors->addError(new SemanticError(DIRECT_USE_OF_OPTIONAL_TYPE, "Calling " + methodtable->getSymbolNameOf(&method_segments) + " on optional type" + subject->typedata._class.classname + ". You must first wrap object in an exists { } clause.", tree));
						ret = MakeType(TYPE_MATCHALL);
					}

					if(lambdatype) {
						ret = copyType((*lambdatype)->typedata.lambda.returntype);

						AddSubNode(tree, MakeNodeFromString(NT_COMPILER_HINT, strdup(subject->typedata._class.classname)));
						AddSubNode(tree, MakeNodeFromString(NT_COMPILER_HINT, strdup(methodtable->getAddress(methodtable->getSymbolNameOf(&method_segments)).c_str())));
					} else {
						errors->addError(new SemanticError(PROPERTY_OR_METHOD_NOT_FOUND, "Couldn't find property " + methodtable->getSymbolNameOf(&method_segments) + " on class" + subject->typedata._class.classname, tree));
						ret = MakeType(TYPE_MATCHALL);
					}

					freeType(subject);
					for(vector<pair<string, TypeArray*> >::iterator it = method_segments.begin(); it != method_segments.end(); ++it)
						freeTypeArray(it->second);
				}
				break;

			case NT_DECLARATION:
				try {
					TypeParameterizer parameterizer;
					parameterizer.writeInParameterizations(&tree->node_data.nodes[0]->node_data.type, parameterizedtypes);
					Type* assignee = tree->node_data.nodes[0]->node_data.type;
					classestable->assertTypeIsValid(assignee);
					if(assignee->arrayed && tree->node_data.nodes[1]->subnodes == 0 && tree->node_data.nodes[1]->node_type == NT_ARRAY_DECLARATION) {
						// Nothing to do here but relax.
					} else {
						Type* assignment = typeCheck(tree->node_data.nodes[1], forceArrayIdentifier);
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
					TypeParameterizer parameterizer;
					parameterizer.writeInParameterizations(&tree->node_data.nodes[0]->node_data.type, parameterizedtypes);
					ret = copyType(tree->node_data.nodes[0]->node_data.type);
					classestable->assertTypeIsValid(ret);
					Type* casted = typeCheckUsable(tree->node_data.nodes[1], forceArrayIdentifier);
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
					TypeParameterizer parameterizer;
					parameterizer.writeInParameterizations(&tree->node_data.nodes[0]->node_data.type, parameterizedtypes);
					try {
						provider = typeCheckUsable(tree->node_data.nodes[2], forceArrayIdentifier);
						ret = copyType(tree->node_data.nodes[0]->node_data.type);
						//TODO index 1 is the arguments
						classestable->assertTypeIsValid(ret);
						try {
							// TODO This does more work than we need to since it recurses
							classestable->getAnalyzer()->assertClassCanProvide(provider, ret);
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
					Type* ret = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);

					if(!ret->optional) {
						errors->addError(new SemanticError(EXISTS_ON_NONOPTIONAL_TYPE, "exists { } statement uses a nonoptional type", tree)); // @todo better error message!
						break;
					}

					Type* real = copyType(ret);
					real->optional = 0;

					if(tree->node_data.nodes[0]->node_type == NT_MEMBER_ACCESS) {
						errors->addError(new SemanticError(TYPE_ERROR, "Calling exists { } on a property is illegal as it is a shared reference and therefore might be unset amid the scope"));
					} else {
						scopesymtable->addOverwriting(real);
					}

					freeType(typeCheck(tree->node_data.nodes[1], forceArrayIdentifier));

					if(tree->node_data.nodes[0]->node_type == NT_MEMBER_ACCESS) {
						scopesymtable->popScope();
					} else {
						scopesymtable->addOverwriting(ret);
					}

					freeType(real);

					if(tree->subnodes > 2) {
						freeType(typeCheck(tree->node_data.nodes[2], forceArrayIdentifier));
					}
				} catch(SymbolNotFoundException* e) {
					errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
					delete e;
				}
				break;

			case NT_MEMBER_ACCESS:
				{
					Type* subject = typeCheckUsable(tree->node_data.nodes[0], false);
					if(subject->type == TYPE_MATCHALL) {
						ret = subject;
						break;
					}

					Type* boxedtype;
					if(analyzer->isAutoboxedType(subject, &boxedtype)) {
						Node* node = tree->node_data.nodes[0];
						tree->node_data.nodes[0] = MakeTwoBranchNode(NT_AUTOBOX, node, MakeNodeFromString(NT_COMPILER_HINT, strdup(boxedtype->typedata._class.classname)));
						freeType(subject);
						subject = boxedtype;
					}

					string name = tree->node_data.nodes[1]->node_type == NT_ALIAS
						? tree->node_data.nodes[1]->node_data.string
						: scopesymtable->getNameForType(tree->node_data.nodes[1]->node_data.type);

					if(forceArrayIdentifier) name += "[]";

					if(subject->optional) {
						errors->addError(new SemanticError(DIRECT_USE_OF_OPTIONAL_TYPE, "Accessing member " + name + " on optional type " + subject->typedata._class.classname + ". You must first wrap object in an exists { } clause.", tree));
						ret = MakeType(TYPE_MATCHALL);
						break;
					}

					ReadOnlyPropertySymbolTable* proptable = classestable->find(subject);
					boost::optional<Type*> variable = proptable->find(name);
					if(!variable) {
						ret = MakeType(TYPE_MATCHALL);
						errors->addError(new SemanticError(PROPERTY_OR_METHOD_NOT_FOUND, "Symbol by name of " + name + " not found", tree));
					} else {
						if(!forceArrayIdentifier && tree->node_data.nodes[1]->node_type != NT_ALIAS && tree->node_data.nodes[1]->node_data.type->arrayed != (*variable)->arrayed)
							errors->addError(new SemanticError(SYMBOL_NOT_DEFINED, "Accessed arrayed variable " + name + " with wrong number of [] brackets.", tree));

						ret = copyType(*variable);
						AddSubNode(tree, MakeNodeFromString(NT_COMPILER_HINT, strdup(name.c_str())));
					}
				}
				break;

			case NT_FOREACH:
				{
					ret = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);

					if(ret->optional) {
						errors->addError(new SemanticError(DIRECT_USE_OF_OPTIONAL_TYPE, "Iterating over optional type. You must first wrap object in an exists { } clause.", tree));
						ret = MakeType(TYPE_MATCHALL);
						break;
					}

					if(!ret->arrayed && ret->type != TYPE_MATCHALL) {
						errors->addError(new SemanticError(TYPE_ERROR, "Calling foreach over something that is not a list", tree));
					} else {
						Type* lowered = copyType(ret);
						free(lowered->alias);
						lowered->alias = NULL;
						lowered->arrayed--;

						scopesymtable->pushScope();
						scopesymtable->add(lowered);
						freeType(typeCheck(tree->node_data.nodes[1], forceArrayIdentifier));
						scopesymtable->popScope();

						AddSubNode(tree, MakeNodeFromString(NT_COMPILER_HINT, strdup(scopesymtable->getNameForType(lowered).c_str())));
					}
				}
				break;

			case NT_CATCH:
				{
					TypeParameterizer parameterizer;
					parameterizer.writeInParameterizations(&tree->node_data.nodes[0]->node_data.type, parameterizedtypes);
					try {
						Type* exception = tree->node_data.nodes[0]->node_data.type;
						classestable->assertTypeIsValid(exception);
						if(!analyzer->isException(exception)) {
							expectedstring = "exception subclass";
							erroneousstring = analyzer->getNameForType(exception);
							throw string("Only exceptions can be thrown");
						}
						AddSubNode(tree, MakeNodeFromString(NT_COMPILER_HINT, strdup(exception->typedata._class.classname)));
						scopesymtable->pushScope();
						scopesymtable->add(exception);
						freeType(typeCheck(tree->node_data.nodes[1], forceArrayIdentifier));
						scopesymtable->popScope();
					} catch(SymbolNotFoundException* e) {
						errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
						delete e;
					}
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
			// these might not be necessary
			case NT_TYPE_ARRAY:
			case NT_VALUES:
				throw string("Not supported yet");

			// These have tests so can't throw, but won't compile anyway as long as switch throws
			case NT_DEFAULTCASE:
			case NT_CASE:

			// these don't have types
			case NT_BLOCK:
			case NT_TRY:
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
						ret = typeCheck(tree->node_data.nodes[i], forceArrayIdentifier);
						i++;
					}
					if(tree->node_type == NT_BLOCK || tree->node_type == NT_CATCH || tree->node_type == NT_TRY) scopesymtable->popScope();
				} catch(SemanticError *e) {
					if(tree->node_type == NT_BLOCK || tree->node_type == NT_CATCH || tree->node_type == NT_TRY) scopesymtable->popScope();
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
		case NT_MEMBER_ACCESS: return true;
		case NT_ARRAY_ACCESS:
			return isValidLValue(n->node_data.nodes[0]);
		default:
			return false;
	}
}

Type* TypeChecker::typeCheckUsable(Node* n, bool forceArrayIdentifier) {
	Type* t = typeCheck(n, forceArrayIdentifier);
	if(t == NULL) {
		return MakeType(TYPE_NOTHING);
	}

	if(t->type == TYPE_UNUSABLE) {
		errors->addError(new SemanticError(USE_OF_ASSIGNMENT_VALUE, "", n));
		t->type = TYPE_MATCHALL;
	}

	return t;
}
