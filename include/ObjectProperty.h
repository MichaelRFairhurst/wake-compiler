#ifndef HEADER_OBJECT_PROPERTY
#define HEADER_OBJECT_PROPERTY

#include <string>

extern "C" {
	#include "type.h"
}

using namespace std;

struct ObjectProperty {
	string address;
	string casing;
	int flags;
	Type* type;
};

#endif
