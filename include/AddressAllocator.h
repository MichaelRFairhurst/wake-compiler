#ifndef HEADER_ADDRESS_ALLOCATOR
#define HEADER_ADDRESS_ALLOCATOR

#include <string>

using namespace std;

class AddressAllocator {

	public:
		AddressAllocator();
		string allocate();
		void deallocate();

	private:
		int stringgenerator;

};

#endif
