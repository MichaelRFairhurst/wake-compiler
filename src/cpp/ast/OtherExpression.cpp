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

PureType* wake::ast::OtherExpression::typeCheck(bool forceArrayIdentifier) {
	TypeAnalyzer* analyzer = classestable->getAnalyzer();
	PureType* ret = NULL;

	// read Printer as Printer[] from ARRAY_ACCESS nodes
	// but not if there are ANY nodes before the next TYPEDATA
	if(forceArrayIdentifier && !(node->node_type == NT_TYPEDATA || node->node_type == NT_MEMBER_ACCESS)) {
		forceArrayIdentifier = false;
	}

	switch(node->node_type) {
		// Here is where the recursion ends
		case NT_STRINGLIT:
			ret = new PureType(TYPE_CLASS);
			ret->typedata._class.classname = strdup("Text");
			break;

		case NT_NUMBERLIT:
			ret = new PureType(TYPE_CLASS);
			ret->typedata._class.classname = strdup("Num");
			break;

		case NT_BOOLLIT:
			ret = new PureType(TYPE_CLASS);
			ret->typedata._class.classname = strdup("Bool");
			break;

		case NT_ALIAS:
			{
				throw "this should be redundant now";
				char* name = node->node_data.string;
				boost::optional<PureType*> variable = scopesymtable->find(name);
				if(!variable && thiscontext != NULL) {
					throw "need to inject proptable";
					PropertySymbolTable* proptable = NULL; //classestable->findModifiable(thiscontext);
					variable = proptable->find(name);
					if(variable) {
						name = strdup(name);
						node->node_type = NT_MEMBER_ACCESS;
						addSubNode(node, makeEmptyNode(NT_THIS, node->loc));
						addSubNode(node, makeNodeFromString(NT_COMPILER_HINT, name, node->loc));
					}
				}
				if(!variable) {
					ret = new PureType(TYPE_MATCHALL);
					errors->addError(new SemanticError(SYMBOL_NOT_DEFINED, "Symbol by name of " + string(name) + " not found", node));
				} else {
					ret = new PureType(**variable);
				}
			}
			break;

		case NT_VAR_REF:
			{
				//TypeParameterizer parameterizer;
				//parameterizer.rewriteClasstypesToParameterizedtypeByLabel(node->node_data.pure_type, parameterizedtypes);

				VarRef ref = *node->node_data.var_ref;
				if(forceArrayIdentifier && ref.alias == NULL) {
					ref._class->arrayed = 1;
				}

				boost::optional<PureType*> variable = scopesymtable->find(&ref);
				if(!variable && thiscontext != NULL) {
					throw "need to inject proptable";
					PropertySymbolTable* proptable = NULL; //classestable->findModifiable(thiscontext);
					variable = proptable->find(ref.toString());
					if(variable) {
						char* propname = strdup(ref.toString().c_str());
						delete node->node_data.var_ref; node->node_data.nodes = NULL;
						node->node_type = NT_MEMBER_ACCESS;
						addSubNode(node, makeEmptyNode(NT_THIS, node->loc));
						addSubNode(node, makeNodeFromString(NT_COMPILER_HINT, propname, node->loc));
					}
				}
				if(!variable) {
					ret = new PureType(TYPE_MATCHALL);
					errors->addError(new SemanticError(SYMBOL_NOT_DEFINED, "Symbol by name of " + ref.toString() + " not found", node));
				} else {
					// detect if they wrote Printer[] instead of Printer[][]
					// Either way Printer[x] would work, so if forceArrayIdentifier skip this step
					if(!forceArrayIdentifier && (*variable)->type == TYPE_LIST && ref._class != NULL && ref._class->arrayed != analyzer->getArrayReferenceLevel(**variable))
						errors->addError(new SemanticError(SYMBOL_NOT_DEFINED, "Accessed arrayed variable " + ref.toString() + " with wrong number of [] brackets.", node));

					ret = new PureType(**variable);
				}
			}
			break;

		case NT_THIS:
			ret = new PureType(*thiscontext);
			break;

		case NT_NOTHING:
			ret = new PureType(TYPE_OPTIONAL);
			ret->typedata.optional.contained = new PureType(TYPE_MATCHALL);
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
				PureType additive = *auto_ptr<PureType>(children[1].typeCheck(false));

				// Ensure the left operand is never a matchall when the right is
				// this way, when ret is matchall, so we can run all typechecks by the
				// lefthand side
				if(ret->type == TYPE_MATCHALL) {
					auto_ptr<PureType> temp(ret);
					ret = new PureType(additive);
					additive = *temp;
				}

				if(analyzer->isPrimitiveTypeNum(ret) || ret->type == TYPE_MATCHALL) {
					if(!analyzer->isPrimitiveTypeNum(&additive) && ret->type != TYPE_MATCHALL) {
						EXPECTED	"Num"
						ERRONEOUS	additive.toString()
						THROW		("Addition with a non-numeral");
					}

				} else if(analyzer->isPrimitiveTypeText(ret)) {
					if(!analyzer->isPrimitiveTypeText(&additive) && ret->type != TYPE_MATCHALL) {
						EXPECTED	"Text"
						ERRONEOUS	additive.toString()
						THROW		("Concatenation with non-Text");
					}

				} else {
					string erroneousstring = ret->toString();
					delete ret;
					ret = new PureType(TYPE_ERROR);

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
		case NT_MOD:
		case NT_MODNATIVE:
		case NT_MODALT:
		case NT_BITSHIFTLEFT:
		case NT_BITSHIFTRIGHT:
		case NT_BITAND:
		case NT_BITXOR:
		case NT_BITOR:
		case NT_SUBTRACT:
			{
				ret = children[0].typeCheck(false);
				PureType factor = *auto_ptr<PureType>(children[1].typeCheck(false));

				if(!analyzer->isPrimitiveTypeNum(ret) || !analyzer->isPrimitiveTypeNum(&factor)) {
					string erroneousstring;

					if(analyzer->isPrimitiveTypeNum(ret)) {
						erroneousstring = factor.toString();
					} else {
						erroneousstring = ret->toString();
						delete ret;
						ret = new PureType(TYPE_CLASS);
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
					string erroneousstring = ret->toString();
					delete ret;
					ret = new PureType(TYPE_CLASS);
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
				ret = new PureType(TYPE_CLASS);
				ret->typedata._class.classname = strdup("Bool");

				PureType a = *auto_ptr<PureType>(children[0].typeCheck(false));
				PureType b = *auto_ptr<PureType>(children[1].typeCheck(false));

				if(!analyzer->isPrimitiveTypeNum(&a) || !analyzer->isPrimitiveTypeNum(&b)) {
					string erroneousstring;

					if(analyzer->isPrimitiveTypeNum(&a)) {
						erroneousstring = b.toString();
					} else {
						erroneousstring = a.toString();
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
				PureType a = *auto_ptr<PureType>(children[0].typeCheck(false));
				PureType b = *auto_ptr<PureType>(children[1].typeCheck(false));
				ret = new PureType(TYPE_CLASS);
				ret->typedata._class.classname = strdup("Bool");

				if(!analyzer->isASubtypeOfB(&a, &b) && !analyzer->isASubtypeOfB(&b, &a)) {
					EXPECTED	a.toString()
					ERRONEOUS	b.toString()
					THROW		("Uncomparable types");
				}
			}
			break;

		case NT_AND:
		case NT_OR:
			{
				ret = children[0].typeCheck(false);
				PureType cmp = *auto_ptr<PureType>(children[1].typeCheck(false));

				if(!analyzer->isPrimitiveTypeBool(ret) || !analyzer->isPrimitiveTypeBool(&cmp)) {
					string erroneousstring;

					if(analyzer->isPrimitiveTypeBool(ret)) {
						erroneousstring = cmp.toString();
					} else {
						erroneousstring = ret->toString();
						delete ret;
						ret = new PureType(TYPE_CLASS);
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
				string erroneousstring = ret->toString();
				delete ret;
				ret = new PureType(TYPE_CLASS);
				ret->typedata._class.classname = strdup("Bool");

				EXPECTED	"Bool"
				ERRONEOUS	erroneousstring
				THROW		("If conditions must be Bool");
			}
			break;

		case NT_TYPESAFE_ARRAY_ACCESS:
			{
				ret = children[0].typeCheck(true);
				PureType index = *auto_ptr<PureType>(children[1].typeCheck(false));
				if(ret->type != TYPE_LIST && ret->type != TYPE_MATCHALL) {
					EXPECTED	"A list"
					ERRONEOUS	ret->toString()
					THROW		("Getting index of non-array");
				} else if(ret->type == TYPE_LIST) { // Otherwise we're a matchall
					PureType temp = *ret->typedata.list.contained;
					delete ret;
					ret = new PureType(TYPE_OPTIONAL);
					ret->typedata.optional.contained = new PureType(temp);
				}

				if(!analyzer->isPrimitiveTypeNum(&index) && index.type != TYPE_MATCHALL) {
					EXPECTED	"Num"
					ERRONEOUS	index.toString()
					THROW		("Array indices must be numeric");
				}
			}
			break;

		case NT_ARRAY_ACCESS:
		case NT_ARRAY_ACCESS_LVAL:
			{
				ret = children[0].typeCheck(true);
				PureType index = *auto_ptr<PureType>(children[1].typeCheck(false));
				if(ret->type != TYPE_LIST && ret->type != TYPE_MATCHALL) {
					EXPECTED	"A list"
					ERRONEOUS	ret->toString()
					THROW		("Getting index of non-array");
				} else if(ret->type == TYPE_LIST) { // Otherwise we're a matchall
					PureType temp = *ret->typedata.list.contained;
					delete ret;
					ret = new PureType(temp);
				}

				if(!analyzer->isPrimitiveTypeNum(&index) && index.type != TYPE_MATCHALL) {
					EXPECTED	"Num"
					ERRONEOUS	index.toString()
					THROW		("Array indices must be numeric");
				}
			}
			break;

		case NT_VALUED_ASSIGNMENT:
		case NT_ASSIGNMENT:
			{
				if(!isValidLValue(node->node_data.nodes[0])) {
					errors->addError(new SemanticError(INVALID_ASSIGNMENT, "", node));
					ret = new PureType(node->node_type == NT_ASSIGNMENT ? TYPE_UNUSABLE : TYPE_MATCHALL);
				} else {
					PureType subject = *auto_ptr<PureType>(children[0].typeCheck(false));
					ret = node->node_type == NT_ASSIGNMENT ? new PureType(TYPE_UNUSABLE) : new PureType(subject);
					PureType assignment = *auto_ptr<PureType>(children[1].typeCheck(false));
					if(!analyzer->isASubtypeOfB(&assignment, &subject)) {
						EXPECTED	subject.toString()
						ERRONEOUS	assignment.toString()
						THROW		("Invalid type in assignment");
					}
				}
			}
			break;

		case NT_CAST:
			try {
				TypeParameterizer parameterizer;
				parameterizer.rewriteClasstypesToParameterizedtypeByLabel(node->node_data.nodes[0]->node_data.pure_type, parameterizedtypes);
				ret = new PureType(*node->node_data.nodes[0]->node_data.pure_type);
				classestable->assertTypeIsValid(ret);
				PureType casted = *auto_ptr<PureType>(children[1].typeCheck(false));
				if(!analyzer->isASubtypeOfB(&casted, ret)) {
					EXPECTED	ret->toString()
					ERRONEOUS	casted.toString()
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
				ret = new PureType(TYPE_LIST);
				ret->typedata.list.contained = new PureType(TYPE_MATCHALL);
			} else if(node->subnodes == 1) {
				ret = new PureType(TYPE_LIST);
				ret->typedata.list.contained = children[0].typeCheck(false);;
			} else {
				auto_ptr<PureType> first(children[0].typeCheck(false));
				auto_ptr<PureType> second(children[1].typeCheck(false));
				boost::optional<PureType*> common;

				int i = 1;
				while(true) {
					common = analyzer->getCommonSubtypeOf(first.get(), second.get());

					if(!common) {
						string expectedstring = "Types with a common ancestor";
						string erroneousstring = first->toString() + " and " + second->toString();
						ret = new PureType(TYPE_MATCHALL);

						EXPECTED	expectedstring
						ERRONEOUS	erroneousstring
						THROW		("No common type between items in array declaration");
					}

					i++;
					if(i == node->subnodes) break;
					first.reset(*common);
					second.reset(children[i].typeCheck(false));
				}

				ret = new PureType(TYPE_LIST);
				ret->typedata.list.contained = *common;
			}
			break;

		case NT_EARLYBAILOUT_MEMBER_ACCESS:
			{
				PureType subject = *auto_ptr<PureType>(children[0].typeCheck(false));
				if(subject.type == TYPE_MATCHALL) {
					ret = new PureType(subject);
					break;
				} else if(subject.type != TYPE_OPTIONAL) {
					errors->addError(new SemanticError(OPTIONAL_USE_OF_NONOPTIONAL_TYPE, "using .? on a nonoptional", node));
					ret = new PureType(TYPE_MATCHALL);
					break;
				} else {
					ret = new PureType(TYPE_OPTIONAL);

					PureType* nonoptional = subject.typedata.optional.contained;
					while(nonoptional->type == TYPE_OPTIONAL) {
						nonoptional = nonoptional->typedata.optional.contained;
					}

					ret->typedata.optional.contained = typeCheckMemberAccess(node, *nonoptional, forceArrayIdentifier);
				}

			}
			break;

		case NT_MEMBER_ACCESS:
			{
				PureType subject = *auto_ptr<PureType>(children[0].typeCheck(false));
				if(subject.type == TYPE_MATCHALL) {
					ret = new PureType(subject);
					break;
				}

				ret = typeCheckMemberAccess(node, subject, forceArrayIdentifier);
			}
			break;

		case NT_IF_THEN_ELSE:
			{
				auto_ptr<PureType> condition(children[1].typeCheck(false));
				auto_ptr<PureType> left(children[0].typeCheck(false));
				auto_ptr<PureType> right(children[2].typeCheck(false));
				boost::optional<PureType*> common = analyzer->getCommonSubtypeOf(left.get(), right.get());
				if(!common) {
					ret = new PureType(TYPE_MATCHALL);
					EXPECTED	"Two types with a single closest common ancestor"
					ERRONEOUS	left->toString() + " and " + right->toString()
					THROW		("Only booleans can go inside a ternary operator");
				}

				ret = *common;

				if(!analyzer->isPrimitiveTypeBool(condition.get()) && !analyzer->isPrimitiveTypeNum(condition.get())) {
					EXPECTED	"Bool or Num"
					ERRONEOUS	condition->toString()
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

PureType* wake::ast::OtherExpression::typeCheckMemberAccess(Node* tree, PureType& subject, bool forceArrayIdentifier) {
	TypeAnalyzer* analyzer = classestable->getAnalyzer();
	PureType* ret;
	PureType* boxedtype;

	if(analyzer->isAutoboxedType(&subject, &boxedtype)) {
		Node* node = tree->node_data.nodes[0];
		tree->node_data.nodes[0] = makeTwoBranchNode(NT_AUTOBOX, node, makeNodeFromString(NT_COMPILER_HINT, strdup(boxedtype->typedata._class.classname), tree->loc), tree->loc);
		subject = *boxedtype;
		delete boxedtype;
	}

	string name = tree->node_data.nodes[1]->node_type == NT_ALIAS
		? tree->node_data.nodes[1]->node_data.string
		: tree->node_data.nodes[1]->node_data.pure_type->toString();

	if(forceArrayIdentifier && tree->node_data.nodes[1]->node_type != NT_ALIAS) name += "[]";

	if(subject.type == TYPE_OPTIONAL) {
		errors->addError(new SemanticError(DIRECT_USE_OF_OPTIONAL_TYPE, "Accessing property " + name + " on nonoptional type " + subject.toString() + ". You must first wrap object in an exists { } clause.", tree));
		ret = new PureType(TYPE_MATCHALL);
		return ret;
	}

	// sanity...
	if(subject.type != TYPE_CLASS) {
		errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, "Accessing property " + name + ", the subject type " + subject.toString() + " could not be autoboxed into a class with methods.", tree));
		ret = new PureType(TYPE_MATCHALL);
		return ret;
	}

	auto_ptr<ReadOnlyPropertySymbolTable> proptable(classestable->findFullyQualified(subject.getFQClassname()));
	boost::optional<PureType*> variable = proptable->find(name);
	if(!variable) {
		ret = new PureType(TYPE_MATCHALL);
		errors->addError(new SemanticError(PROPERTY_OR_METHOD_NOT_FOUND, "Symbol by name of " + name + " not found", tree));
	} else {
		PureType* member = tree->node_data.nodes[1]->node_data.pure_type;
		if(!forceArrayIdentifier && tree->node_data.nodes[1]->node_type != NT_ALIAS && member->type == TYPE_LIST && analyzer->getArrayReferenceLevel(*member) != analyzer->getArrayReferenceLevel(**variable))
			errors->addError(new SemanticError(SYMBOL_NOT_DEFINED, "Accessed arrayed variable " + name + " with wrong number of [] brackets.", tree));

		if(tree->node_data.nodes[0]->node_type != NT_THIS) {
			int flags = proptable->getFlags(name);
			if(!(flags & PROPERTY_PUBLIC)) {
				errors->addError(new SemanticError(PRIVATE_ACCESS, "Accessing private member " + name, node));
			}
		}

		ret = new PureType(**variable);
		addSubNode(tree, makeNodeFromString(NT_COMPILER_HINT, strdup(proptable->getAddress(name).c_str()), tree->loc));
	}

	return ret;
}

