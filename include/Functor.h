#ifndef HEADER_FUNCTOR
#define HEADER_FUNCTOR

#include "type.h"
#include "tree.h"

struct Functor {
	Type* returntype;
	TypeArray* arguments;
	Node* body;
};

#endif
