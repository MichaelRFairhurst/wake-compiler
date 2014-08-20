/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * AddressAllocator.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

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
