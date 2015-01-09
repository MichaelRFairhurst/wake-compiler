#include "ast/Catch.h"
#include "TypeError.h"
#include "tree.h"
#include "SemanticError.h"

void wake::ast::typeCheck() {
	try {
		classestable->assertTypeIsValid(exception);

		if(!analyzer->isException(exception)) {
			EXPECTED	"exception subclass"
			ERRONEOUS	analyzer->getNameForType(exception)
			THROW		("Only exceptions can be thrown");
		}

		AddSubNode(node, MakeNodeFromString(NT_COMPILER_HINT, strdup(exception->typedata._class.classname), node->loc));

		scopesymtable->pushScope();
		scopesymtable->add(exception);

		body->typeCheck();

		scopesymtable->popScope();
	} catch(SymbolNotFoundException* e) {
		errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, node));
		delete e;
	}
}
