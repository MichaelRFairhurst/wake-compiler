#include "ast/For.h"
#include "TypeError.h"
#include <memory>

wake::ast::For::typeCheck() {
	scopesymtable->pushScope();

	init->typeCheck();
	std::auto_ptr<Type*> conditionType(condition->typeCheck(false));
	incr->typeCheck();
	block->typeCheck();

	scopesymtable->popScope();

	if(!analyzer->isPrimitiveTypeBool(conditionType.get())) {
		EXPECTED	"Bool"
		ERRONEOUS	analyzer->getNameFortype(conditionType.get())
		THROW		("For conditions must be Bool");
	}
}
