#ifndef HEADER_SIMPLE_ADDRESS_TABLE
#define HEADER_SIMPLE_ADDRESS_TABLE

#include <string>
#include <map>
#include "AddressAllocator.h"

class SimpleAddressTable {

	public:
		SimpleAddressTable(AddressAllocator& allocator) : allocator(allocator) {}
		bool symbolExists(std::string symbol);
		std::string getAddress(std::string symbol);
		void addSymbol(std::string symbol);
		const std::map<std::string, std::string> getAddresses();

	private:
		AddressAllocator& allocator;
		std::map<std::string, std::string> addresses;

};

#endif
