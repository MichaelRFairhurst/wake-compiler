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

using namespace wake;

PureType<QUALIFIED>* ast::OtherExpression::typeCheck(bool forceArrayIdentifier) {
	TypeAnalyzer* analyzer = classestable->getAnalyzer();
	PureType<QUALIFIED>* ret = NULL;

	// read Printer as Printer[] from ARRAY_ACCESS nodes
	// but not if there are ANY nodes before the next TYPEDATA
	if(forceArrayIdentifier && !(node->node_type == NT_VAR_REF || node->node_type == NT_MEMBER_ACCESS)) {
		forceArrayIdentifier = false;
	}

	switch(node->node_type) {
		// Here is where the recursion ends
		case NT_STRINGLIT:
			ret = new PureType<QUALIFIED>(TYPE_CLASS);
			ret->typedata._class.classname = strdup("Text");
			ret->typedata._class.modulename = strdup("lang");
			break;

		case NT_NUMBERLIT:
			ret = new PureType<QUALIFIED>(TYPE_CLASS);
			ret->typedata._class.classname = strdup("Num");
			ret->typedata._class.modulename = strdup("lang");
			break;

		case NT_INTEGERLIT:
			ret = new PureType<QUALIFIED>(TYPE_CLASS);
			ret->typedata._class.classname = strdup("Int");
			ret->typedata._class.modulename = strdup("lang");
			break;

		case NT_BOOLLIT:
			ret = new PureType<QUALIFIED>(TYPE_CLASS);
			ret->typedata._class.classname = strdup("Bool");
			ret->typedata._class.modulename = strdup("lang");
			break;

		case NT_VAR_DECL_DATA:
			ret = new PureType<QUALIFIED>(*classestable->setModulesOnType(&node->node_data.var_decl->typedata));
			break;

		case NT_VAR_REF:
			{
				//TypeParameterizer parameterizer;
				//parameterizer.rewriteClasstypesToParameterizedtypeByLabel(node->node_data.pure_type, parameterizedtypes);

				VarRef ref = *node->node_data.var_ref;
				if(forceArrayIdentifier && ref.alias == NULL) {
					ref._class->arrayed = 1;
				}

				boost::optional<PureType<QUALIFIED>*> variable = scopesymtable->find(&ref);
				if(!variable && thiscontext != NULL) {
					auto_ptr<ReadOnlyPropertySymbolTable> proptable(classestable->findFullyQualified(thiscontext->getFQClassname()));
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
					ret = new PureType<QUALIFIED>(TYPE_MATCHALL);
					errors->addError(new SemanticError(SYMBOL_NOT_DEFINED, "Symbol by name of " + ref.toString() + " not found", node));
				} else {
					// detect if they wrote Printer[] instead of Printer[][]
					// Either way Printer[x] would work, so if forceArrayIdentifier skip this step
					if(!forceArrayIdentifier && (*variable)->type == TYPE_LIST && ref._class != NULL && ref._class->arrayed != analyzer->getArrayReferenceLevel(**variable))
						errors->addError(new SemanticError(SYMBOL_NOT_DEFINED, "Accessed arrayed variable " + ref.toString() + " with wrong number of [] brackets.", node));

					ret = new PureType<QUALIFIED>(**variable);
				}
			}
			break;

		case NT_THIS:
			ret = new PureType<QUALIFIED>(*thiscontext);
			break;

		case NT_NOTHING:
			ret = new PureType<QUALIFIED>(TYPE_OPTIONAL);
			ret->typedata.optional.contained = new PureType<QUALIFIED>(TYPE_MATCHALL);
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
				PureType<QUALIFIED> additive = *auto_ptr<PureType<QUALIFIED> >(children[1].typeCheck(false));

				if(node->node_type == NT_ADD_ASSIGNMENT && analyzer->isPrimitiveTypeInt(ret) && analyzer->isPrimitiveTypeNum(&additive)) {
					EXPECTED	"Int"
					ERRONEOUS	additive.toString()
					THROW		("An Int cannot use += with a Num, as an Int doesn't have enough precision to store all possible results.");
				}

				// Ensure the left operand is never a matchall when the right is
				// this way, when ret is matchall, so we can run all typechecks by the
				// lefthand side
				// Do the same for Int vs Num so the left side represents the highest precision
				if(ret->type == TYPE_MATCHALL || analyzer->isPrimitiveTypeNum(&additive)) {
					auto_ptr<PureType<QUALIFIED> > temp(ret);
					ret = new PureType<QUALIFIED>(additive);
					additive = *temp;
				}

				if(analyzer->isPrimitiveTypeNum(ret) || analyzer->isPrimitiveTypeInt(ret) || ret->type == TYPE_MATCHALL) {
					if(!analyzer->isPrimitiveTypeNum(&additive) && !analyzer->isPrimitiveTypeInt(&additive) && ret->type != TYPE_MATCHALL) {
						EXPECTED	"Num' or 'Int"
						ERRONEOUS	additive.toString()
						THROW		("Addition with a non-numeral");
					}

				} else if(analyzer->isPrimitiveTypeText(ret) || analyzer->isPrimitiveTypeChar(ret)) {
					if(!analyzer->isPrimitiveTypeText(&additive) && !analyzer->isPrimitiveTypeChar(&additive) && ret->type != TYPE_MATCHALL) {
						EXPECTED	"Text' or 'Char"
						ERRONEOUS	additive.toString()
						THROW		("Concatenation with non-Text");
					}

					if(analyzer->isPrimitiveTypeChar(ret)) {
						free(ret->typedata._class.classname);
						ret->typedata._class.classname = strdup("Text");
					}
				} else {
					string erroneousstring = ret->toString();
					delete ret;
					ret = new PureType<QUALIFIED>(TYPE_ERROR);

					EXPECTED	"Num' or 'Text' or 'Char"
					ERRONEOUS	erroneousstring
					THROW		("Only numerics or Texts can be added/concatenated");
				}

			}
			break;

		case NT_MULT_ASSIGNMENT:
		case NT_SUB_ASSIGNMENT:
			if(!isValidLValue(node->node_data.nodes[0])) {
				errors->addError(new SemanticError(INVALID_ASSIGNMENT, "", node));
					break;
			}
			// FALLTHROUGH
		case NT_MULTIPLY:
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
				PureType<QUALIFIED> factor = *auto_ptr<PureType<QUALIFIED> >(children[1].typeCheck(false));

				if(node->node_type == NT_MULT_ASSIGNMENT
					|| node->node_type == NT_SUB_ASSIGNMENT
					&& analyzer->isPrimitiveTypeInt(ret)
					&& analyzer->isPrimitiveTypeNum(&factor)
				) {
					EXPECTED	"Int"
					ERRONEOUS	factor.toString()
					THROW		("An Int cannot use -= or *= with a Num, as an Int doesn't have enough precision to store all possible results.");
				}

				// If a num is on the right side, move it to the left. That way 'ret' is our
				// highest precision
				if(analyzer->isPrimitiveTypeNum(&factor)) {
					auto_ptr<PureType<QUALIFIED> > temp(ret);
					ret = new PureType<QUALIFIED>(factor);
					factor = *temp;
				}

				string erroneousstring;
				bool broken = false;

				if(!analyzer->isPrimitiveTypeNum(&factor) && !analyzer->isPrimitiveTypeInt(&factor)) {
					erroneousstring = factor.toString();
					broken = true;
				} else if(!analyzer->isPrimitiveTypeNum(ret) && !analyzer->isPrimitiveTypeInt(ret)) {
					broken = true;
					erroneousstring = ret->toString();
					delete ret;
					ret = new PureType<QUALIFIED>(TYPE_CLASS);
					ret->typedata._class.classname = strdup("Num");
					ret->typedata._class.modulename = strdup("lang");
				}

				if(broken) {
					EXPECTED	"Num"
					ERRONEOUS	erroneousstring
					THROW		("Mathematical operation performed on non-numerals");
				}
			}
			break;
		case NT_DIV_ASSIGNMENT:
			if(!isValidLValue(node->node_data.nodes[0])) {
				errors->addError(new SemanticError(INVALID_ASSIGNMENT, "", node));
					break;
			}
			// FALLTHROUGH
		case NT_DIVIDE:
			{
				ret = children[0].typeCheck(false);
				PureType<QUALIFIED> factor = *auto_ptr<PureType<QUALIFIED> >(children[1].typeCheck(false));

				if(node->node_type == NT_DIV_ASSIGNMENT
					&& analyzer->isPrimitiveTypeInt(ret)
				) {
					EXPECTED	"Num"
					ERRONEOUS	ret->toString()
					THROW		("Cannot use /= with an Int, as it requires using integer division. Use \\= instead.");
				}

				string erroneousstring;
				bool broken = false;

				if(!analyzer->isPrimitiveTypeNum(&factor) && !analyzer->isPrimitiveTypeInt(&factor)) {
					erroneousstring = factor.toString();
					broken = true;
				} else if(!analyzer->isPrimitiveTypeNum(ret) && !analyzer->isPrimitiveTypeInt(ret)) {
					broken = true;
					erroneousstring = ret->toString();
					delete ret;
					ret = new PureType<QUALIFIED>(TYPE_CLASS);
					ret->typedata._class.classname = strdup("Num");
					ret->typedata._class.modulename = strdup("lang");
				}

				if(broken) {
					EXPECTED	"Num"
					ERRONEOUS	erroneousstring
					THROW		("Mathematical operation performed on non-numerals");
				}

				if(!analyzer->isPrimitiveTypeNum(ret)) {
					delete ret;
					ret = new PureType<QUALIFIED>(TYPE_CLASS);
					ret->typedata._class.classname = strdup("Num");
					ret->typedata._class.modulename = strdup("lang");
				}
			}
			break;

		case NT_BITNOT:
			{
				ret = children[0].typeCheck(false);

				if(!analyzer->isPrimitiveTypeNum(ret) && !analyzer->isPrimitiveTypeInt(ret)) {
					string erroneousstring = ret->toString();
					delete ret;
					ret = new PureType<QUALIFIED>(TYPE_CLASS);
					ret->typedata._class.classname = strdup("Num");
					ret->typedata._class.modulename = strdup("lang");

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
				ret = new PureType<QUALIFIED>(TYPE_CLASS);
				ret->typedata._class.classname = strdup("Bool");
				ret->typedata._class.modulename = strdup("lang");

				PureType<QUALIFIED> a = *auto_ptr<PureType<QUALIFIED> >(children[0].typeCheck(false));
				PureType<QUALIFIED> b = *auto_ptr<PureType<QUALIFIED> >(children[1].typeCheck(false));

				if(!(analyzer->isPrimitiveTypeNum(&a) || analyzer->isPrimitiveTypeInt(&a)) || !(analyzer->isPrimitiveTypeNum(&b) || analyzer->isPrimitiveTypeInt(&b))) {
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
				PureType<QUALIFIED> a = *auto_ptr<PureType<QUALIFIED> >(children[0].typeCheck(false));
				PureType<QUALIFIED> b = *auto_ptr<PureType<QUALIFIED> >(children[1].typeCheck(false));
				ret = new PureType<QUALIFIED>(TYPE_CLASS);
				ret->typedata._class.classname = strdup("Bool");
				ret->typedata._class.modulename = strdup("lang");

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
				PureType<QUALIFIED> cmp = *auto_ptr<PureType<QUALIFIED> >(children[1].typeCheck(false));

				if(!analyzer->isPrimitiveTypeBool(ret) || !analyzer->isPrimitiveTypeBool(&cmp)) {
					string erroneousstring;

					if(analyzer->isPrimitiveTypeBool(ret)) {
						erroneousstring = cmp.toString();
					} else {
						erroneousstring = ret->toString();
						delete ret;
						ret = new PureType<QUALIFIED>(TYPE_CLASS);
						ret->typedata._class.classname = strdup("Bool");
						ret->typedata._class.modulename = strdup("lang");
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
				ret = new PureType<QUALIFIED>(TYPE_CLASS);
				ret->typedata._class.classname = strdup("Bool");
				ret->typedata._class.modulename = strdup("lang");

				EXPECTED	"Bool"
				ERRONEOUS	erroneousstring
				THROW		("If conditions must be Bool");
			}
			break;

		case NT_TYPESAFE_ARRAY_ACCESS:
			{
				ret = children[0].typeCheck(true);
				PureType<QUALIFIED> index = *auto_ptr<PureType<QUALIFIED> >(children[1].typeCheck(false));
				if(ret->type != TYPE_LIST && ret->type != TYPE_MATCHALL) {
					EXPECTED	"A list"
					ERRONEOUS	ret->toString()
					THROW		("Getting index of non-array");
				} else if(ret->type == TYPE_LIST) { // Otherwise we're a matchall
					PureType<QUALIFIED> temp = *ret->typedata.list.contained;
					delete ret;
					ret = new PureType<QUALIFIED>(TYPE_OPTIONAL);
					ret->typedata.optional.contained = new PureType<QUALIFIED>(temp);
				}

				if(!analyzer->isPrimitiveTypeInt(&index) && index.type != TYPE_MATCHALL) {
					EXPECTED	"Int"
					ERRONEOUS	index.toString()
					THROW		("Array indices must be integers");
				}
			}
			break;

		case NT_ARRAY_ACCESS:
		case NT_ARRAY_ACCESS_LVAL:
			{
				ret = children[0].typeCheck(true);
				PureType<QUALIFIED> index = *auto_ptr<PureType<QUALIFIED> >(children[1].typeCheck(false));
				if(ret->type != TYPE_LIST && ret->type != TYPE_MATCHALL) {
					EXPECTED	"A list"
					ERRONEOUS	ret->toString()
					THROW		("Getting index of non-array");
				} else if(ret->type == TYPE_LIST) { // Otherwise we're a matchall
					PureType<QUALIFIED> temp = *ret->typedata.list.contained;
					delete ret;
					ret = new PureType<QUALIFIED>(temp);
				}

				if(!analyzer->isPrimitiveTypeInt(&index) && index.type != TYPE_MATCHALL) {
					EXPECTED	"Int"
					ERRONEOUS	index.toString()
					THROW		("Array indices must be integers");
				}
			}
			break;

		case NT_VALUED_ASSIGNMENT:
		case NT_ASSIGNMENT:
			{
				if(!isValidLValue(node->node_data.nodes[0])) {
					errors->addError(new SemanticError(INVALID_ASSIGNMENT, "", node));
					ret = new PureType<QUALIFIED>(node->node_type == NT_ASSIGNMENT ? TYPE_UNUSABLE : TYPE_MATCHALL);
				} else {
					PureType<QUALIFIED> subject = *auto_ptr<PureType<QUALIFIED> >(children[0].typeCheck(false));
					ret = node->node_type == NT_ASSIGNMENT ? new PureType<QUALIFIED>(TYPE_UNUSABLE) : new PureType<QUALIFIED>(subject);
					PureType<QUALIFIED> assignment = *auto_ptr<PureType<QUALIFIED> >(children[1].typeCheck(false));
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
				PureType<QUALIFIED>* castType = classestable->setModulesOnType(node->node_data.nodes[0]->node_data.pure_type);
				parameterizer.rewriteClasstypesToParameterizedtypeByLabel(castType, parameterizedtypes);
				ret = new PureType<QUALIFIED>(*castType);
				classestable->assertTypeIsValid(ret);
				PureType<QUALIFIED> casted = *auto_ptr<PureType<QUALIFIED> >(children[1].typeCheck(false));
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
				ret = new PureType<QUALIFIED>(TYPE_LIST);
				ret->typedata.list.contained = new PureType<QUALIFIED>(TYPE_MATCHALL);
			} else if(node->subnodes == 1) {
				ret = new PureType<QUALIFIED>(TYPE_LIST);
				ret->typedata.list.contained = children[0].typeCheck(false);;
			} else {
				auto_ptr<PureType<QUALIFIED> > first(children[0].typeCheck(false));
				auto_ptr<PureType<QUALIFIED> > second(children[1].typeCheck(false));
				boost::optional<PureType<QUALIFIED>*> common;

				int i = 1;
				while(true) {
					common = analyzer->getCommonSubtypeOf(first.get(), second.get());

					if(!common) {
						string expectedstring = "Types with a common ancestor";
						string erroneousstring = first->toString() + " and " + second->toString();
						ret = new PureType<QUALIFIED>(TYPE_MATCHALL);

						EXPECTED	expectedstring
						ERRONEOUS	erroneousstring
						THROW		("No common type between items in array declaration");
					}

					i++;
					if(i == node->subnodes) break;
					first.reset(*common);
					second.reset(children[i].typeCheck(false));
				}

				ret = new PureType<QUALIFIED>(TYPE_LIST);
				ret->typedata.list.contained = *common;
			}
			break;

		case NT_EARLYBAILOUT_MEMBER_ACCESS:
			{
				PureType<QUALIFIED> subject = *auto_ptr<PureType<QUALIFIED> >(children[0].typeCheck(false));
				if(subject.type == TYPE_MATCHALL) {
					ret = new PureType<QUALIFIED>(subject);
					break;
				} else if(subject.type != TYPE_OPTIONAL) {
					errors->addError(new SemanticError(OPTIONAL_USE_OF_NONOPTIONAL_TYPE, "using .? on a nonoptional", node));
					ret = new PureType<QUALIFIED>(TYPE_MATCHALL);
					break;
				} else {
					ret = new PureType<QUALIFIED>(TYPE_OPTIONAL);

					PureType<QUALIFIED>* nonoptional = subject.typedata.optional.contained;
					while(nonoptional->type == TYPE_OPTIONAL) {
						nonoptional = nonoptional->typedata.optional.contained;
					}

					ret->typedata.optional.contained = typeCheckMemberAccess(node, *nonoptional, forceArrayIdentifier);
				}

			}
			break;

		case NT_MEMBER_ACCESS:
			{
				PureType<QUALIFIED> subject = *auto_ptr<PureType<QUALIFIED> >(children[0].typeCheck(false));
				if(subject.type == TYPE_MATCHALL) {
					ret = new PureType<QUALIFIED>(subject);
					break;
				}

				ret = typeCheckMemberAccess(node, subject, forceArrayIdentifier);
			}
			break;

		case NT_IF_THEN_ELSE:
			{
				auto_ptr<PureType<QUALIFIED> > condition(children[1].typeCheck(false));
				auto_ptr<PureType<QUALIFIED> > left(children[0].typeCheck(false));
				auto_ptr<PureType<QUALIFIED> > right(children[2].typeCheck(false));
				boost::optional<PureType<QUALIFIED>*> common = analyzer->getCommonSubtypeOf(left.get(), right.get());
				if(!common) {
					ret = new PureType<QUALIFIED>(TYPE_MATCHALL);
					EXPECTED	"Two types with a single closest common ancestor"
					ERRONEOUS	left->toString() + " and " + right->toString()
					THROW		("Only booleans can go inside a ternary operator");
				}

				ret = *common;

				if(!analyzer->isPrimitiveTypeBool(condition.get()) && !analyzer->isPrimitiveTypeNum(condition.get()) && !analyzer->isPrimitiveTypeInt(condition.get())) {
					EXPECTED	"Bool' or 'Int' or 'Num"
					ERRONEOUS	condition->toString()
					THROW		("Only Bools, Ints, and Nums can go inside a ternary operator condition");
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

bool ast::OtherExpression::isValidLValue(Node* n) {
	switch(n->node_type) {
		case NT_ALIAS:
		case NT_VAR_REF:
		case NT_VAR_DECL_DATA:
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

PureType<QUALIFIED>* ast::OtherExpression::typeCheckMemberAccess(Node* tree, PureType<QUALIFIED>& subject, bool forceArrayIdentifier) {
	TypeAnalyzer* analyzer = classestable->getAnalyzer();
	PureType<QUALIFIED>* ret;
	PureType<QUALIFIED>* boxedtype;

	if(analyzer->isAutoboxedType(&subject, &boxedtype)) {
		Node* node = tree->node_data.nodes[0];
		tree->node_data.nodes[0] = makeTwoBranchNode(NT_AUTOBOX, node, makeNodeFromString(NT_COMPILER_HINT, strdup(boxedtype->typedata._class.classname), tree->loc), tree->loc);
		subject = *boxedtype;
		delete boxedtype;
	}

	VarRef ref = *tree->node_data.nodes[1]->node_data.var_ref;
	if(forceArrayIdentifier && ref.alias == NULL) {
		ref._class->arrayed = 1;
	}

	string name = ref.toString();

	if(subject.type == TYPE_OPTIONAL) {
		errors->addError(new SemanticError(DIRECT_USE_OF_OPTIONAL_TYPE, "Accessing property " + name + " on nonoptional type " + subject.toString() + ". You must first wrap object in an exists { } clause.", tree));
		ret = new PureType<QUALIFIED>(TYPE_MATCHALL);
		return ret;
	}

	// sanity...
	if(subject.type != TYPE_CLASS) {
		errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, "Accessing property " + name + ", the subject type " + subject.toString() + " could not be autoboxed into a class with methods.", tree));
		ret = new PureType<QUALIFIED>(TYPE_MATCHALL);
		return ret;
	}

	auto_ptr<ReadOnlyPropertySymbolTable> proptable(classestable->findFullyQualified(subject.getFQClassname(), subject.getClassParametersAsVector()));
	boost::optional<PureType<QUALIFIED>*> variable = proptable->find(name);
	if(!variable) {
		ret = new PureType<QUALIFIED>(TYPE_MATCHALL);
		errors->addError(new SemanticError(PROPERTY_OR_METHOD_NOT_FOUND, "Symbol by name of " + name + " not found", tree));
	} else {
		PureType<QUALIFIED>* member = *variable;
		if(!forceArrayIdentifier && ref.alias == NULL && member->type == TYPE_LIST && analyzer->getArrayReferenceLevel(**variable) != ref._class->arrayed)
			errors->addError(new SemanticError(SYMBOL_NOT_DEFINED, "Accessed arrayed variable " + name + " with wrong number of [] brackets.", tree));

		if(tree->node_data.nodes[0]->node_type != NT_THIS) {
			int flags = proptable->getFlags(name);
			if(!(flags & PROPERTY_PUBLIC)) {
				errors->addError(new SemanticError(PRIVATE_ACCESS, "Accessing private member " + name, node));
			}
		}

		ret = new PureType<QUALIFIED>(**variable);
		addSubNode(tree, makeNodeFromString(NT_COMPILER_HINT, strdup(proptable->getAddress(name).c_str()), tree->loc));
	}

	return ret;
}

