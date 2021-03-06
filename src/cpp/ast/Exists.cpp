/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Exists.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/Exists.h"
#include "CompilationExceptions.h"
#include "tree.h"
#include <memory>

using namespace wake;

void ast::Exists::typeCheck() {
	try {
		auto_ptr<VarRef> existableRef(new VarRef(*node->node_data.nodes[0]->node_data.var_ref));
		auto_ptr<PureType<QUALIFIED> > existableType(existable->typeCheck(false));

		if(node->node_data.nodes[0]->node_type == NT_MEMBER_ACCESS) {
			errors->addError(new SemanticError(TYPE_ERROR, "Calling exists { } on a property is illegal as it is a shared reference and therefore might be unset amid the scope", node));
			return;
		}

		if(existableType->type != TYPE_OPTIONAL) {
			errors->addError(new SemanticError(EXISTS_ON_NONOPTIONAL_TYPE, "exists { } statement uses a nonoptional type", node)); // @todo better error message!
			return;
		}

		VarDecl<QUALIFIED> realDecl;
		VarDecl<QUALIFIED> origDecl;
		realDecl.typedata = *existableType->typedata.optional.contained;
		origDecl.typedata = *existableType.get();
		if(existableRef->alias != NULL) {
			realDecl.alias = strdup(existableRef->alias);
			origDecl.alias = strdup(existableRef->alias);
		} else {
			realDecl.shadow = existableRef->_class->shadow;
			origDecl.shadow = existableRef->_class->shadow;
		}

		scopesymtable->addOverwriting(new VarDecl<QUALIFIED> (realDecl));

		block->typeCheck();

		scopesymtable->addOverwriting(new VarDecl<QUALIFIED> (origDecl));

		if(otherwise.get() != NULL) {
			otherwise->typeCheck();
		}
	} catch(SymbolNotFoundException* e) {
		errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, node));
		delete e;
	}
}

bool ast::Exists::exhaustiveReturns() {
	if(otherwise.get() == NULL) {
		return false;
	}

	return block->exhaustiveReturns() && otherwise->exhaustiveReturns();
}
