/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ForeachAt.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include <string.h>
#include "ast/ForeachAt.h"
#include "VarDecl.h"

using namespace wake::ast;

void ForeachAt::typeCheck() {
	VarDecl<QUALIFIED> atTypeDecl;
	atTypeDecl.typedata = PureType<QUALIFIED>(TYPE_CLASS);
	atTypeDecl.typedata.typedata._class.classname = strdup("Int");
	atTypeDecl.typedata.typedata._class.modulename = strdup("lang");

	if(var_ref.alias != NULL) {
		atTypeDecl.alias = strdup(var_ref.alias);
	} else {
		if(var_ref._class->classname != std::string("Int")) {
			errors->addError(new SemanticError(TYPE_ERROR, "The 'at' variable of this foreach loop must be of type Int", node));
			return;
		}

		if(var_ref._class->arrayed) {
			errors->addError(new SemanticError(TYPE_ERROR, "The 'at' variable of this foreach loop must be of type Int", node));
			return;
		}

		atTypeDecl.shadow = var_ref._class->shadow;
	}

	scopesymtable->pushScope();
	try {
		scopesymtable->add(&atTypeDecl);
		base->typeCheck();
	} catch(...) {
		scopesymtable->popScope();
		throw;
	}

	scopesymtable->popScope();

	addSubNode(node, makeNodeFromString(NT_COMPILER_HINT, strdup(var_ref.toString().c_str()), node->loc));
}

bool ForeachAt::exhaustiveReturns() {
	return false;
}
