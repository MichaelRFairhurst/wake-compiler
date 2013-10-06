#include "AddressAllocator.h"

AddressAllocator::AddressAllocator() {
	stringgenerator = 0;
}

string AddressAllocator::allocate() {
	stringgenerator++;

	return string(1, static_cast<const char>(96 + stringgenerator));
}

void AddressAllocator::deallocate() {
	stringgenerator--;
}
