#include "ast/Throw.h"
#include "TypeError.h"
#include <memory>

void wake::ast::Throw::typeCheck() {
	auto_ptr<Type*> exceptionType(exception->typeCheck());

	if(!analyzer->isException(exceptionType.get())) {
		EXPECTED	"Exception"
		ERRONEOUS	analyzer->getNameForType(ret)
		THROW		("Can only throw subclasses of exception");
	}
}
