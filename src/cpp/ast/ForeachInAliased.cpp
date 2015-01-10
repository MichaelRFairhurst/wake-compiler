#include "ast/ForeachInAliased.h"

Type* wake::ast::ForeachInAliased::getIterationType(Type* iterableType) {
	iterableType.alias = strdup(alias);
	return *iterableType;
}
