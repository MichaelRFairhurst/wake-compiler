#include "SimpleAddressTable.h"

bool SimpleAddressTable::symbolExists(std::string symbol) {
	std::map<std::string, std::string>::iterator it = addresses.find(symbol);
	return it != addresses.end();
}

std::string SimpleAddressTable::getAddress(std::string symbol) {
	return addresses[symbol];
}

void SimpleAddressTable::addSymbol(std::string symbol) {
	addresses[symbol] = allocator.allocate();
}
