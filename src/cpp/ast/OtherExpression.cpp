/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * OtherExpression.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "type.h"
#include "TypeAnalyzer.h"
#include "TypeParameterizer.h"
#include "ClassSpaceSymbolTable.h"
#include "ast/ExpressionNode.h"
#include "ast/OtherExpression.h"

Type* OtherExpression:typeCheck(bool forceArrayIdentifiers) {
	TypeAnalyzer* analyzer = classestable->getAnalyzer();
	Type* ret = NULL;
	string erroneousstring;
	string expectedstring;

	// read Printer as Printer[] from ARRAY_ACCESS nodes
	// but not if there are ANY nodes before the next TYPEDATA
	if(forceArrayIdentifier && !(tree->node_type == NT_TYPEDATA || tree->node_type == NT_MEMBER_ACCESS)) {
		forceArrayIdentifier = false;
	}

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
						AddSubNode(tree, MakeEmptyNode(NT_THIS, tree->loc));
						AddSubNode(tree, MakeNodeFromString(NT_COMPILER_HINT, name, tree->loc));
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

				Type type = *tree->node_data.type;
				if(forceArrayIdentifier) {
					Type* wrapped = new Type(type);
					type = Type(TYPE_LIST);
					type.typedata.list.contained = wrapped;
				}

				boost::optional<Type*> variable = scopesymtable->find(&type);
				if(!variable && thiscontext != NULL) {
					PropertySymbolTable* proptable = classestable->findModifiable(thiscontext);
					variable = proptable->find(scopesymtable->getNameForType(&type));
					if(variable) {
						char* propname = strdup(scopesymtable->getNameForType(&type).c_str());
						delete tree->node_data.type; tree->node_data.nodes = NULL;
						tree->node_type = NT_MEMBER_ACCESS;
						AddSubNode(tree, MakeEmptyNode(NT_THIS, tree->loc));
						AddSubNode(tree, MakeNodeFromString(NT_COMPILER_HINT, propname, tree->loc));
					}
				}
				if(!variable) {
					ret = MakeType(TYPE_MATCHALL);
					errors->addError(new SemanticError(SYMBOL_NOT_DEFINED, "Symbol by name of " + scopesymtable->getNameForType(&type) + " not found", tree));
				} else {
					// detect if they wrote Printer[] instead of Printer[][]
					// Either way Printer[x] would work, so if forceArrayIdentifier skip this step
					if(!forceArrayIdentifier && type.type == TYPE_LIST && analyzer->getArrayReferenceLevel(type) != analyzer->getArrayReferenceLevel(**variable))
						errors->addError(new SemanticError(SYMBOL_NOT_DEFINED, "Accessed arrayed variable " + scopesymtable->getNameForType(&type) + " with wrong number of [] brackets.", tree));

					ret = new Type(**variable);
				}
			}
			break;

		case NT_THIS:
			ret = copyType(thiscontext);
			break;

		case NT_NOTHING:
			ret = new Type(TYPE_OPTIONAL);
			ret->typedata.optional.contained = new Type(TYPE_MATCHALL);
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
				Type additive = *auto_ptr<Type>(typeCheckUsable(tree->node_data.nodes[1], forceArrayIdentifier));

				// Ensure the left operand is never a matchall when the right is
				// this way, when ret is matchall, so we can run all typechecks by the
				// lefthand side
				if(ret->type == TYPE_MATCHALL) {
					auto_ptr<Type> temp(ret);
					ret = new Type(additive);
					additive = *temp;
				}

				if(analyzer->isPrimitiveTypeNum(ret) || ret->type == TYPE_MATCHALL) {
					if(!analyzer->isPrimitiveTypeNum(&additive) && ret->type != TYPE_MATCHALL) {
						erroneousstring = analyzer->getNameForType(&additive);
						expectedstring = "Num";
						throw string("Addition with a non-numeral");
					}

				} else if(analyzer->isPrimitiveTypeText(ret)) {
					if(!analyzer->isPrimitiveTypeText(&additive) && ret->type != TYPE_MATCHALL) {
						erroneousstring = analyzer->getNameForType(&additive);
						expectedstring = "Text";
						throw string("Concatenation with non-Text");
					}

				} else {
					expectedstring = "Num' or 'Text";
					erroneousstring = analyzer->getNameForType(ret);
					delete ret;
					ret = new Type(TYPE_ERROR);

					throw string("Only numerics or Texts can be added/concatenated");
				}
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
				Type factor = *auto_ptr<Type>(typeCheckUsable(tree->node_data.nodes[1], forceArrayIdentifier));

				if(!analyzer->isPrimitiveTypeNum(ret) || !analyzer->isPrimitiveTypeNum(&factor)) {
					expectedstring = "Num";

					if(analyzer->isPrimitiveTypeNum(ret)) {
						erroneousstring = analyzer->getNameForType(&factor);
					} else {
						erroneousstring = analyzer->getNameForType(ret);
						delete ret;
						ret = MakeType(TYPE_CLASS);
						ret->typedata._class.classname = strdup("Num");
					}

					throw string("Mathematical operation performed on non-numerals");
				}
			}
			break;

		case NT_LESSTHAN:
		case NT_LESSTHANEQUAL:
		case NT_GREATERTHAN:
		case NT_GREATERTHANEQUAL:
			{
				ret = MakeType(TYPE_CLASS);
				ret->typedata._class.classname = strdup("Bool");

				Type a = *auto_ptr<Type>(typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier));
				Type b = *auto_ptr<Type>(typeCheckUsable(tree->node_data.nodes[1], forceArrayIdentifier));

				if(!analyzer->isPrimitiveTypeNum(&a) || !analyzer->isPrimitiveTypeNum(&b)) {
					expectedstring = "Num";

					if(analyzer->isPrimitiveTypeNum(&a)) {
						erroneousstring = analyzer->getNameForType(&b);
					} else {
						erroneousstring = analyzer->getNameForType(&a);
					}

					throw string("Mathematical operation performed on non-numerals");
				}
			}
			break;

		case NT_EQUALITY:
		case NT_INEQUALITY:
			{
				Type a = *auto_ptr<Type>(typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier));
				Type b = *auto_ptr<Type>(typeCheckUsable(tree->node_data.nodes[1], forceArrayIdentifier));
				ret = MakeType(TYPE_CLASS);
				ret->typedata._class.classname = strdup("Bool");

				if(!analyzer->isASubtypeOfB(&a, &b) && !analyzer->isASubtypeOfB(&b, &a)) {
					expectedstring = analyzer->getNameForType(&a);
					erroneousstring = analyzer->getNameForType(&b);
					throw string("Uncomparable types");
				}
			}
			break;

		case NT_AND:
		case NT_OR:
			{
				ret = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);
				Type cmp = *auto_ptr<Type>(typeCheckUsable(tree->node_data.nodes[1], forceArrayIdentifier));

				if(!analyzer->isPrimitiveTypeBool(ret) || !analyzer->isPrimitiveTypeBool(&cmp)) {
					if(analyzer->isPrimitiveTypeBool(ret)) {
						erroneousstring = analyzer->getNameForType(&cmp);
					} else {
						erroneousstring = analyzer->getNameForType(ret);
						delete ret;
						ret = MakeType(TYPE_CLASS);
						ret->typedata._class.classname = strdup("Bool");
					}

					expectedstring = "Bool";

					throw string("AND or OR comparison on non-Bool types");
				}
			}
			break;

		case NT_INVERT:
			ret = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);

			if(!analyzer->isPrimitiveTypeBool(ret)) {
				expectedstring = "Bool";
				erroneousstring = analyzer->getNameForType(ret);
				delete ret;
				ret = new Type(TYPE_CLASS);
				ret->typedata._class.classname = strdup("Bool");
				throw string("If conditions must be Bool");
			}
			break;

		case NT_TYPESAFE_ARRAY_ACCESS:
			{
				forceArrayIdentifier = false;
				ret = typeCheckUsable(tree->node_data.nodes[0], true);
				Type index = *auto_ptr<Type>(typeCheckUsable(tree->node_data.nodes[1], forceArrayIdentifier));
				if(ret->type != TYPE_LIST && ret->type != TYPE_MATCHALL) {
					erroneousstring = analyzer->getNameForType(ret);
					throw string("Getting index of non-array");
				} else if(ret->type == TYPE_LIST) { // Otherwise we're a matchall
					Type temp = *ret->typedata.list.contained;
					delete ret;
					ret = new Type(TYPE_OPTIONAL);
					ret->typedata.optional.contained = new Type(temp);
				}

				if(!analyzer->isPrimitiveTypeNum(&index) && index.type != TYPE_MATCHALL) {
					erroneousstring = analyzer->getNameForType(&index);
					expectedstring = "Num";
					throw string("Array indices must be numeric");
				}
			}
			break;

		case NT_ARRAY_ACCESS:
		case NT_ARRAY_ACCESS_LVAL:
			{
				forceArrayIdentifier = false;
				ret = typeCheckUsable(tree->node_data.nodes[0], true);
				Type index = *auto_ptr<Type>(typeCheckUsable(tree->node_data.nodes[1], forceArrayIdentifier));
				if(ret->type != TYPE_LIST && ret->type != TYPE_MATCHALL) {
					erroneousstring = analyzer->getNameForType(ret);
					throw string("Getting index of non-array");
				} else if(ret->type == TYPE_LIST) { // Otherwise we're a matchall
					Type temp = *ret->typedata.list.contained;
					delete ret;
					ret = new Type(temp);
				}

				if(!analyzer->isPrimitiveTypeNum(&index) && index.type != TYPE_MATCHALL) {
					erroneousstring = analyzer->getNameForType(&index);
					expectedstring = "Num";
					throw string("Array indices must be numeric");
				}
			}
			break;

		case NT_VALUED_ASSIGNMENT:
		case NT_ASSIGNMENT:
			{
				if(!isValidLValue(tree->node_data.nodes[0])) {
					errors->addError(new SemanticError(INVALID_ASSIGNMENT, "", tree));
					ret = new Type(tree->node_type == NT_ASSIGNMENT ? TYPE_UNUSABLE : TYPE_MATCHALL);
				} else {
					Type subject = *auto_ptr<Type>(typeCheck(tree->node_data.nodes[0], forceArrayIdentifier));
					ret = tree->node_type == NT_ASSIGNMENT ? new Type(TYPE_UNUSABLE) : new Type(subject);
					Type assignment = *auto_ptr<Type>(typeCheckUsable(tree->node_data.nodes[1], forceArrayIdentifier));
					if(!analyzer->isASubtypeOfB(&assignment, &subject)) {
						expectedstring = analyzer->getNameForType(&subject);
						erroneousstring = analyzer->getNameForType(&assignment);
						throw string("Invalid type in assignment");
					}
				}
			}
			break;

		case NT_LAMBDA_INVOCATION:
			{
				Type lambda = *auto_ptr<Type>(typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier));
				Type actual(TYPE_LAMBDA);
				actual.typedata.lambda.arguments = MakeTypeArray();
				if(lambda.typedata.lambda.returntype != NULL) {
					actual.typedata.lambda.returntype = new Type(*lambda.typedata.lambda.returntype);
					ret = new Type(*lambda.typedata.lambda.returntype);
				} else {
					ret = new Type(TYPE_UNUSABLE);
				}

				if(tree->subnodes == 2) {
					int i;
					for(i = 0; i < tree->node_data.nodes[1]->subnodes; i++) {
						AddTypeToTypeArray(typeCheckUsable(tree->node_data.nodes[1]->node_data.nodes[i], forceArrayIdentifier), actual.typedata.lambda.arguments);
					}
				}

				if(!analyzer->isASubtypeOfB(&actual, &lambda)) {
					expectedstring = analyzer->getNameForType(&lambda);
					erroneousstring = analyzer->getNameForType(&actual);
					throw string("Argument lists not compatible in function invocation");
				}
			}
			break;

		case NT_EARLYBAILOUT_METHOD_INVOCATION:
			{
				Type subject = *auto_ptr<Type>(typeCheckUsable(tree->node_data.nodes[0], false));
				if(subject.type == TYPE_MATCHALL) {
					ret = new Type(subject);
					break;
				} else if(subject.type != TYPE_OPTIONAL) {
					errors->addError(new SemanticError(OPTIONAL_USE_OF_NONOPTIONAL_TYPE, "using .? on a nonoptional", tree));
					ret = new Type(TYPE_MATCHALL);
					break;
				} else {
					ret = new Type(TYPE_OPTIONAL);
					Type* nonoptional = subject.typedata.optional.contained;

					while(nonoptional->type == TYPE_OPTIONAL) {
						nonoptional = nonoptional->typedata.optional.contained;
					}

					ret->typedata.optional.contained = typeCheckMethodInvocation(tree, *nonoptional);
				}

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
						ret = typeCheck(methodname, forceArrayIdentifier);
						break;
					}
				}

				Type subject = *auto_ptr<Type>(typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier));

				if(subject.type == TYPE_MATCHALL) {
					ret = new Type(subject);
					break;
				}

				ret = typeCheckMethodInvocation(tree, subject);
			}
			break;

		case NT_CAST:
			try {
				TypeParameterizer parameterizer;
				parameterizer.writeInParameterizations(&tree->node_data.nodes[0]->node_data.type, parameterizedtypes);
				ret = copyType(tree->node_data.nodes[0]->node_data.type);
				classestable->assertTypeIsValid(ret);
				Type casted = *auto_ptr<Type>(typeCheckUsable(tree->node_data.nodes[1], forceArrayIdentifier));
				if(!analyzer->isASubtypeOfB(&casted, ret)) {
					expectedstring = analyzer->getNameForType(ret);
					erroneousstring = analyzer->getNameForType(&casted);
					throw string("Casting an object that is not a valid subtype");
				}
			} catch(SymbolNotFoundException* e) {
				errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
				delete e;
			}
			break;

		case NT_RETRIEVAL:
			{
				TypeParameterizer parameterizer;
				parameterizer.writeInParameterizations(&tree->node_data.nodes[0]->node_data.type, parameterizedtypes);
				try {
					Type provider = *auto_ptr<Type>(typeCheckUsable(tree->node_data.nodes[2], forceArrayIdentifier));
					ret = new Type(*tree->node_data.nodes[0]->node_data.type);
					classestable->assertTypeIsValid(ret);
					vector<Type*> arguments;
					boost::ptr_vector<Type> arguments_latch;

					if(tree->node_data.nodes[1]->node_type != NT_EMPTY) {
						for(int i = 0; i < tree->node_data.nodes[1]->subnodes; i++) {
							Type* argtype = typeCheckUsable(tree->node_data.nodes[1]->node_data.nodes[i], forceArrayIdentifier);
							arguments.push_back(argtype);
							arguments_latch.push_back(argtype);
						}
					}

					string name = analyzer->getProvisionSymbol(ret, arguments);

					try {
						auto_ptr<ReadOnlyPropertySymbolTable> table(classestable->find(&provider));
						boost::optional<Type*> provision = table->find(name);
						if(!provision) {
							errors->addError(new SemanticError(PROPERTY_OR_METHOD_NOT_FOUND, "Provision " + name + " does not exist on class " + analyzer->getNameForType(&provider), tree));
						}
					} catch(SymbolNotFoundException *e) {
						delete e;
					}
					AddSubNode(tree, MakeNodeFromString(NT_COMPILER_HINT, strdup(name.c_str()), tree->loc));
				} catch(SymbolNotFoundException* e) {
					errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
					delete e;
				}
			}
			break;

		case NT_ARRAY_DECLARATION:
			// These should all be specially handled in the other cases
			if(!tree->subnodes) {
				ret = new Type(TYPE_LIST);
				ret->typedata.list.contained = new Type(TYPE_MATCHALL);
			} else if(tree->node_data.nodes[0]->subnodes == 1) {
				ret = new Type(TYPE_LIST);
				ret->typedata.list.contained = typeCheckUsable(tree->node_data.nodes[0]->node_data.nodes[0], false);
			} else {
				auto_ptr<Type> first(typeCheckUsable(tree->node_data.nodes[0]->node_data.nodes[0], false));
				auto_ptr<Type> second(typeCheckUsable(tree->node_data.nodes[0]->node_data.nodes[1], false));
				boost::optional<Type*> common;

				int i = 1;
				while(true) {
					common = analyzer->getCommonSubtypeOf(first.get(), second.get());

					if(!common) {
						expectedstring = "Types with a common ancestor";
						erroneousstring = analyzer->getNameForType(first.get()) + " and " + analyzer->getNameForType(second.get());
						ret = new Type(TYPE_MATCHALL);
						throw string("No common type between items in array declaration");
					}

					i++;
					if(i == tree->node_data.nodes[0]->subnodes) break;
					first.reset(*common);
					second.reset(typeCheckUsable(tree->node_data.nodes[0]->node_data.nodes[i], false));
				}

				ret = new Type(TYPE_LIST);
				ret->typedata.list.contained = *common;
			}
			break;

		case NT_EARLYBAILOUT_MEMBER_ACCESS:
			{
				Type subject = *auto_ptr<Type>(typeCheckUsable(tree->node_data.nodes[0], false));
				if(subject.type == TYPE_MATCHALL) {
					ret = new Type(subject);
					break;
				} else if(subject.type != TYPE_OPTIONAL) {
					errors->addError(new SemanticError(OPTIONAL_USE_OF_NONOPTIONAL_TYPE, "using .? on a nonoptional", tree));
					ret = new Type(TYPE_MATCHALL);
					break;
				} else {
					ret = new Type(TYPE_OPTIONAL);

					Type* nonoptional = subject.typedata.optional.contained;
					while(nonoptional->type == TYPE_OPTIONAL) {
						nonoptional = nonoptional->typedata.optional.contained;
					}

					ret->typedata.optional.contained = typeCheckMemberAccess(tree, *nonoptional, forceArrayIdentifier);
				}

			}
			break;

		case NT_MEMBER_ACCESS:
			{
				Type subject = *auto_ptr<Type>(typeCheckUsable(tree->node_data.nodes[0], false));
				if(subject.type == TYPE_MATCHALL) {
					ret = new Type(subject);
					break;
				}

				ret = typeCheckMemberAccess(tree, subject, forceArrayIdentifier);
			}
			break;

		case NT_IF_THEN_ELSE:
			{
				auto_ptr<Type> condition(typeCheckUsable(tree->node_data.nodes[1], false));
				auto_ptr<Type> left(typeCheckUsable(tree->node_data.nodes[0], false));
				auto_ptr<Type> right(typeCheckUsable(tree->node_data.nodes[2], false));
				boost::optional<Type*> common = analyzer->getCommonSubtypeOf(left.get(), right.get());
				if(!common) {
					ret = MakeType(TYPE_MATCHALL);
					expectedstring = "Two types with a single closest common ancestor";
					erroneousstring = analyzer->getNameForType(left.get()) + " and " + analyzer->getNameForType(right.get());
					throw string("Only booleans can go inside a ternary operator");
				}

				ret = *common;

				if(!analyzer->isPrimitiveTypeBool(condition.get()) && !analyzer->isPrimitiveTypeNum(condition.get())) {
					expectedstring = "Bool or Num";
					erroneousstring = analyzer->getNameForType(condition.get());
					throw string("Only booleans and nums can go inside a ternary operator condition");
				}
			}
			break;

		// Ignoring these for now
		case NT_SWITCH:
		case NT_CURRIED:
		case NT_INCREMENT:
		case NT_DECREMENT:
		// These require a common-ancestor function
		case NT_TYPE_ARRAY:
		case NT_VALUES:
			throw string("Not supported yet");

		// These have tests so can't throw, but won't compile anyway as long as switch throws
		case NT_EXPRESSIONS:
			try {
				int i = 0;
				while(i < tree->subnodes) {
					if(i > 0) delete ret;
					ret = typeCheck(tree->node_data.nodes[i], forceArrayIdentifier);
					i++;
				}
			} catch(SemanticError *e) {
				throw e;
			}
	}

	return ret;
}
