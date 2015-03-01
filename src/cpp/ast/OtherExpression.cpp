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

#include "ast/ExpressionNode.h"
#include "ast/OtherExpression.h"
#include "TypeParameterizer.h"
#include "TypeError.h"
#include <iostream>
#include "CompilationExceptions.h"

Type* wake::ast::OtherExpression::typeCheck(bool forceArrayIdentifier) {
	TypeAnalyzer* analyzer = classestable->getAnalyzer();
	Type* ret = NULL;

	// read Printer as Printer[] from ARRAY_ACCESS nodes
	// but not if there are ANY nodes before the next TYPEDATA
	if(forceArrayIdentifier && !(node->node_type == NT_TYPEDATA || node->node_type == NT_MEMBER_ACCESS)) {
		forceArrayIdentifier = false;
	}

	switch(node->node_type) {
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
				char* name = node->node_data.string;
				boost::optional<Type*> variable = scopesymtable->find(name);
				if(!variable && thiscontext != NULL) {
					PropertySymbolTable* proptable = classestable->findModifiable(thiscontext);
					variable = proptable->find(name);
					if(variable) {
						name = strdup(name);
						node->node_type = NT_MEMBER_ACCESS;
						AddSubNode(node, MakeEmptyNode(NT_THIS, node->loc));
						AddSubNode(node, MakeNodeFromString(NT_COMPILER_HINT, name, node->loc));
					}
				}
				if(!variable) {
					ret = MakeType(TYPE_MATCHALL);
					errors->addError(new SemanticError(SYMBOL_NOT_DEFINED, "Symbol by name of " + string(name) + " not found", node));
				} else {
					ret = copyType(*variable);
				}
			}
			break;

		case NT_TYPEDATA:
			{
				TypeParameterizer parameterizer;
				parameterizer.rewriteClasstypesToParameterizedtypeByLabel(&node->node_data.type, parameterizedtypes);

				Type type = *node->node_data.type;
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
						delete node->node_data.type; node->node_data.nodes = NULL;
						node->node_type = NT_MEMBER_ACCESS;
						AddSubNode(node, MakeEmptyNode(NT_THIS, node->loc));
						AddSubNode(node, MakeNodeFromString(NT_COMPILER_HINT, propname, node->loc));
					}
				}
				if(!variable) {
					ret = MakeType(TYPE_MATCHALL);
					errors->addError(new SemanticError(SYMBOL_NOT_DEFINED, "Symbol by name of " + scopesymtable->getNameForType(&type) + " not found", node));
				} else {
					// detect if they wrote Printer[] instead of Printer[][]
					// Either way Printer[x] would work, so if forceArrayIdentifier skip this step
					if(!forceArrayIdentifier && type.type == TYPE_LIST && analyzer->getArrayReferenceLevel(type) != analyzer->getArrayReferenceLevel(**variable))
						errors->addError(new SemanticError(SYMBOL_NOT_DEFINED, "Accessed arrayed variable " + scopesymtable->getNameForType(&type) + " with wrong number of [] brackets.", node));

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
			if(!isValidLValue(node->node_data.nodes[0])) {
				errors->addError(new SemanticError(INVALID_ASSIGNMENT, "", node));
					break;
			}
			// FALLTHROUGH
		case NT_ADD:
			{
				ret = children[0].typeCheck(false);
				Type additive = *auto_ptr<Type>(children[1].typeCheck(false));

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
						EXPECTED	"Num"
						ERRONEOUS	analyzer->getNameForType(&additive)
						THROW		("Addition with a non-numeral");
					}

				} else if(analyzer->isPrimitiveTypeText(ret)) {
					if(!analyzer->isPrimitiveTypeText(&additive) && ret->type != TYPE_MATCHALL) {
						EXPECTED	"Text"
						ERRONEOUS	analyzer->getNameForType(&additive)
						THROW		("Concatenation with non-Text");
					}

				} else {
					string erroneousstring = analyzer->getNameForType(ret);
					delete ret;
					ret = new Type(TYPE_ERROR);

					EXPECTED	"Num' or 'Text"
					ERRONEOUS	erroneousstring
					THROW		("Only numerics or Texts can be added/concatenated");
				}
			}
			break;

		case NT_MULT_ASSIGNMENT:
		case NT_DIV_ASSIGNMENT:
		case NT_SUB_ASSIGNMENT:
			if(!isValidLValue(node->node_data.nodes[0])) {
				errors->addError(new SemanticError(INVALID_ASSIGNMENT, "", node));
					break;
			}
			// FALLTHROUGH
		case NT_MULTIPLY:
		case NT_DIVIDE:
		case NT_BITSHIFTLEFT:
		case NT_BITSHIFTRIGHT:
		case NT_BITAND:
		case NT_BITXOR:
		case NT_BITOR:
		case NT_SUBTRACT:
			{
				ret = children[0].typeCheck(false);
				Type factor = *auto_ptr<Type>(children[1].typeCheck(false));

				if(!analyzer->isPrimitiveTypeNum(ret) || !analyzer->isPrimitiveTypeNum(&factor)) {
					string erroneousstring;

					if(analyzer->isPrimitiveTypeNum(ret)) {
						erroneousstring = analyzer->getNameForType(&factor);
					} else {
						erroneousstring = analyzer->getNameForType(ret);
						delete ret;
						ret = MakeType(TYPE_CLASS);
						ret->typedata._class.classname = strdup("Num");
					}

					EXPECTED	"Num"
					ERRONEOUS	erroneousstring
					THROW		("Mathematical operation performed on non-numerals");
				}
			}
			break;
		
		case NT_BITNOT:
			{
				ret = children[0].typeCheck(false);
				
				if(!analyzer->isPrimitiveTypeNum(ret)) {
					string erroneousstring = analyzer->getNameForType(ret);
					delete ret;
					ret = MakeType(TYPE_CLASS);
					ret->typedata._class.classname = strdup("Num");
				   
					EXPECTED	"Num"
					ERRONEOUS	erroneousstring
					THROW		("Bitwise Not operation performed on non-numerals");
				
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

				Type a = *auto_ptr<Type>(children[0].typeCheck(false));
				Type b = *auto_ptr<Type>(children[1].typeCheck(false));

				if(!analyzer->isPrimitiveTypeNum(&a) || !analyzer->isPrimitiveTypeNum(&b)) {
					string erroneousstring;

					if(analyzer->isPrimitiveTypeNum(&a)) {
						erroneousstring = analyzer->getNameForType(&b);
					} else {
						erroneousstring = analyzer->getNameForType(&a);
					}

					EXPECTED	"Num"
					ERRONEOUS	erroneousstring
					THROW		("Mathematical operation performed on non-numerals");
				}
			}
			break;

		case NT_EQUALITY:
		case NT_INEQUALITY:
			{
				Type a = *auto_ptr<Type>(children[0].typeCheck(false));
				Type b = *auto_ptr<Type>(children[1].typeCheck(false));
				ret = MakeType(TYPE_CLASS);
				ret->typedata._class.classname = strdup("Bool");

				if(!analyzer->isASubtypeOfB(&a, &b) && !analyzer->isASubtypeOfB(&b, &a)) {
					EXPECTED	analyzer->getNameForType(&a)
					ERRONEOUS	analyzer->getNameForType(&b)
					THROW		("Uncomparable types");
				}
			}
			break;

		case NT_AND:
		case NT_OR:
			{
				ret = children[0].typeCheck(false);
				Type cmp = *auto_ptr<Type>(children[1].typeCheck(false));

				if(!analyzer->isPrimitiveTypeBool(ret) || !analyzer->isPrimitiveTypeBool(&cmp)) {
					string erroneousstring;

					if(analyzer->isPrimitiveTypeBool(ret)) {
						erroneousstring = analyzer->getNameForType(&cmp);
					} else {
						erroneousstring = analyzer->getNameForType(ret);
						delete ret;
						ret = MakeType(TYPE_CLASS);
						ret->typedata._class.classname = strdup("Bool");
					}

					EXPECTED	"Bool"
					ERRONEOUS	erroneousstring
					THROW		("AND or OR comparison on non-Bool types");
				}
			}
			break;

		case NT_INVERT:
			ret = children[0].typeCheck(false);

			if(!analyzer->isPrimitiveTypeBool(ret)) {
				string erroneousstring = analyzer->getNameForType(ret);
				delete ret;
				ret = new Type(TYPE_CLASS);
				ret->typedata._class.classname = strdup("Bool");

				EXPECTED	"Bool"
				ERRONEOUS	erroneousstring
				THROW		("If conditions must be Bool");
			}
			break;

		case NT_TYPESAFE_ARRAY_ACCESS:
			{
				ret = children[0].typeCheck(true);
				Type index = *auto_ptr<Type>(children[1].typeCheck(false));
				if(ret->type != TYPE_LIST && ret->type != TYPE_MATCHALL) {
					EXPECTED	"A list"
					ERRONEOUS	analyzer->getNameForType(ret)
					THROW		("Getting index of non-array");
				} else if(ret->type == TYPE_LIST) { // Otherwise we're a matchall
					Type temp = *ret->typedata.list.contained;
					delete ret;
					ret = new Type(TYPE_OPTIONAL);
					ret->typedata.optional.contained = new Type(temp);
				}

				if(!analyzer->isPrimitiveTypeNum(&index) && index.type != TYPE_MATCHALL) {
					EXPECTED	"Num"
					ERRONEOUS	analyzer->getNameForType(&index)
					THROW		("Array indices must be numeric");
				}
			}
			break;

		case NT_ARRAY_ACCESS:
		case NT_ARRAY_ACCESS_LVAL:
			{
				ret = children[0].typeCheck(true);
				Type index = *auto_ptr<Type>(children[1].typeCheck(false));
				if(ret->type != TYPE_LIST && ret->type != TYPE_MATCHALL) {
					EXPECTED	"A list"
					ERRONEOUS	analyzer->getNameForType(ret)
					THROW		("Getting index of non-array");
				} else if(ret->type == TYPE_LIST) { // Otherwise we're a matchall
					Type temp = *ret->typedata.list.contained;
					delete ret;
					ret = new Type(temp);
				}

				if(!analyzer->isPrimitiveTypeNum(&index) && index.type != TYPE_MATCHALL) {
					EXPECTED	"Num"
					ERRONEOUS	analyzer->getNameForType(&index)
					THROW		("Array indices must be numeric");
				}
			}
			break;

		case NT_VALUED_ASSIGNMENT:
		case NT_ASSIGNMENT:
			{
				if(!isValidLValue(node->node_data.nodes[0])) {
					errors->addError(new SemanticError(INVALID_ASSIGNMENT, "", node));
					ret = new Type(node->node_type == NT_ASSIGNMENT ? TYPE_UNUSABLE : TYPE_MATCHALL);
				} else {
					Type subject = *auto_ptr<Type>(children[0].typeCheck(false));
					ret = node->node_type == NT_ASSIGNMENT ? new Type(TYPE_UNUSABLE) : new Type(subject);
					Type assignment = *auto_ptr<Type>(children[1].typeCheck(false));
					if(!analyzer->isASubtypeOfB(&assignment, &subject)) {
						EXPECTED	analyzer->getNameForType(&subject)
						ERRONEOUS	analyzer->getNameForType(&assignment)
						THROW		("Invalid type in assignment");
					}
				}
			}
			break;

		case NT_CAST:
			try {
				TypeParameterizer parameterizer;
				parameterizer.rewriteClasstypesToParameterizedtypeByLabel(&node->node_data.nodes[0]->node_data.type, parameterizedtypes);
				ret = copyType(node->node_data.nodes[0]->node_data.type);
				classestable->assertTypeIsValid(ret);
				Type casted = *auto_ptr<Type>(children[1].typeCheck(false));
				if(!analyzer->isASubtypeOfB(&casted, ret)) {
					EXPECTED	analyzer->getNameForType(ret)
					ERRONEOUS	analyzer->getNameForType(&casted)
					THROW		("Casting an object that is not a valid subtype");
				}
			} catch(SymbolNotFoundException* e) {
				errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, node));
				delete e;
			}
			break;

		case NT_ARRAY_DECLARATION:
			// These should all be specially handled in the other cases
			if(!node->subnodes) {
				ret = new Type(TYPE_LIST);
				ret->typedata.list.contained = new Type(TYPE_MATCHALL);
			} else if(node->subnodes == 1) {
				ret = new Type(TYPE_LIST);
				ret->typedata.list.contained = children[0].typeCheck(false);;
			} else {
				auto_ptr<Type> first(children[0].typeCheck(false));
				auto_ptr<Type> second(children[1].typeCheck(false));
				boost::optional<Type*> common;

				int i = 1;
				while(true) {
					common = analyzer->getCommonSubtypeOf(first.get(), second.get());

					if(!common) {
						string expectedstring = "Types with a common ancestor";
						string erroneousstring = analyzer->getNameForType(first.get()) + " and " + analyzer->getNameForType(second.get());
						ret = new Type(TYPE_MATCHALL);

						EXPECTED	expectedstring
						ERRONEOUS	erroneousstring
						THROW		("No common type between items in array declaration");
					}

					i++;
					if(i == node->subnodes) break;
					first.reset(*common);
					second.reset(children[i].typeCheck(false));
				}

				ret = new Type(TYPE_LIST);
				ret->typedata.list.contained = *common;
			}
			break;

		case NT_EARLYBAILOUT_MEMBER_ACCESS:
			{
				Type subject = *auto_ptr<Type>(children[0].typeCheck(false));
				if(subject.type == TYPE_MATCHALL) {
					ret = new Type(subject);
					break;
				} else if(subject.type != TYPE_OPTIONAL) {
					errors->addError(new SemanticError(OPTIONAL_USE_OF_NONOPTIONAL_TYPE, "using .? on a nonoptional", node));
					ret = new Type(TYPE_MATCHALL);
					break;
				} else {
					ret = new Type(TYPE_OPTIONAL);

					Type* nonoptional = subject.typedata.optional.contained;
					while(nonoptional->type == TYPE_OPTIONAL) {
						nonoptional = nonoptional->typedata.optional.contained;
					}

					ret->typedata.optional.contained = typeCheckMemberAccess(node, *nonoptional, forceArrayIdentifier);
				}

			}
			break;

		case NT_MEMBER_ACCESS:
			{
				Type subject = *auto_ptr<Type>(children[0].typeCheck(false));
				if(subject.type == TYPE_MATCHALL) {
					ret = new Type(subject);
					break;
				}

				ret = typeCheckMemberAccess(node, subject, forceArrayIdentifier);
			}
			break;

		case NT_IF_THEN_ELSE:
			{
				auto_ptr<Type> condition(children[1].typeCheck(false));
				auto_ptr<Type> left(children[0].typeCheck(false));
				auto_ptr<Type> right(children[2].typeCheck(false));
				boost::optional<Type*> common = analyzer->getCommonSubtypeOf(left.get(), right.get());
				if(!common) {
					ret = MakeType(TYPE_MATCHALL);
					EXPECTED	"Two types with a single closest common ancestor"
					ERRONEOUS	analyzer->getNameForType(left.get()) + " and " + analyzer->getNameForType(right.get())
					THROW		("Only booleans can go inside a ternary operator");
				}

				ret = *common;

				if(!analyzer->isPrimitiveTypeBool(condition.get()) && !analyzer->isPrimitiveTypeNum(condition.get())) {
					EXPECTED	"Bool or Num"
					ERRONEOUS	analyzer->getNameForType(condition.get())
					THROW		("Only booleans and nums can go inside a ternary operator condition");
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

		/*case NT_EXPRESSIONS:
			try {
				int i = 0;
				while(i < node->subnodes) {
					if(i > 0) delete ret;
					ret = typeCheck(node->node_data.nodes[i], forceArrayIdentifier);
					i++;
				}
			} catch(SemanticError *e) {
				throw e;
			}*/
	}

	return ret;
}

bool wake::ast::OtherExpression::isValidLValue(Node* n) {
	switch(n->node_type) {
		case NT_ALIAS:
		case NT_TYPEDATA:
		case NT_MEMBER_ACCESS:
		case NT_ARRAY_ACCESS_LVAL:
			return true;

		case NT_ARRAY_ACCESS:
			if(isValidLValue(n->node_data.nodes[0])) {
				n->node_type = NT_ARRAY_ACCESS_LVAL;
				return true;
			}
			// fall through
		default:
			return false;
	}
}

Type* wake::ast::OtherExpression::typeCheckMemberAccess(Node* tree, Type& subject, bool forceArrayIdentifier) {
	TypeAnalyzer* analyzer = classestable->getAnalyzer();
	Type* ret;
	Type* boxedtype;

	if(analyzer->isAutoboxedType(&subject, &boxedtype)) {
		Node* node = tree->node_data.nodes[0];
		tree->node_data.nodes[0] = MakeTwoBranchNode(NT_AUTOBOX, node, MakeNodeFromString(NT_COMPILER_HINT, strdup(boxedtype->typedata._class.classname), tree->loc), tree->loc);
		subject = *boxedtype;
		delete boxedtype;
	}

	string name = tree->node_data.nodes[1]->node_type == NT_ALIAS
		? tree->node_data.nodes[1]->node_data.string
		: scopesymtable->getNameForType(tree->node_data.nodes[1]->node_data.type);

	if(forceArrayIdentifier && tree->node_data.nodes[1]->node_type != NT_ALIAS) name += "[]";

	if(subject.type == TYPE_OPTIONAL) {
		errors->addError(new SemanticError(DIRECT_USE_OF_OPTIONAL_TYPE, "Accessing property " + name + " on nonoptional type " + analyzer->getNameForType(&subject) + ". You must first wrap object in an exists { } clause.", tree));
		ret = new Type(TYPE_MATCHALL);
		return ret;
	}

	auto_ptr<ReadOnlyPropertySymbolTable> proptable(classestable->find(&subject));
	boost::optional<Type*> variable = proptable->find(name);
	if(!variable) {
		ret = MakeType(TYPE_MATCHALL);
		errors->addError(new SemanticError(PROPERTY_OR_METHOD_NOT_FOUND, "Symbol by name of " + name + " not found", tree));
	} else {
		Type* member = tree->node_data.nodes[1]->node_data.type;
		if(!forceArrayIdentifier && tree->node_data.nodes[1]->node_type != NT_ALIAS && member->type == TYPE_LIST && analyzer->getArrayReferenceLevel(*member) != analyzer->getArrayReferenceLevel(**variable))
			errors->addError(new SemanticError(SYMBOL_NOT_DEFINED, "Accessed arrayed variable " + name + " with wrong number of [] brackets.", tree));

		ret = copyType(*variable);
		AddSubNode(tree, MakeNodeFromString(NT_COMPILER_HINT, strdup(name.c_str()), tree->loc));
	}

	return ret;
}

