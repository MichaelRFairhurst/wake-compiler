/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * AddressAllocatorTest.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "boost/test/unit_test.hpp"
#include "AddressAllocator.h"

BOOST_AUTO_TEST_SUITE(AddressAllocatorTest)

BOOST_AUTO_TEST_CASE(TestGoesThroughAlphabet)
{
	AddressAllocator alloc;
	BOOST_REQUIRE(alloc.allocate() == "a");
	BOOST_REQUIRE(alloc.allocate() == "b");
	BOOST_REQUIRE(alloc.allocate() == "c");
	BOOST_REQUIRE(alloc.allocate() == "d");
	BOOST_REQUIRE(alloc.allocate() == "e");
	BOOST_REQUIRE(alloc.allocate() == "f");
	BOOST_REQUIRE(alloc.allocate() == "g");
	BOOST_REQUIRE(alloc.allocate() == "h");
	BOOST_REQUIRE(alloc.allocate() == "i");
	BOOST_REQUIRE(alloc.allocate() == "j");
}

BOOST_AUTO_TEST_CASE(TestGoesUpAndDown)
{
	AddressAllocator alloc;
	BOOST_REQUIRE(alloc.allocate() == "a");
	BOOST_REQUIRE(alloc.allocate() == "b");
	alloc.deallocate();
	BOOST_REQUIRE(alloc.allocate() == "b");
	BOOST_REQUIRE(alloc.allocate() == "c");
	alloc.deallocate();
	BOOST_REQUIRE(alloc.allocate() == "c");
	alloc.deallocate();
	alloc.deallocate();
	BOOST_REQUIRE(alloc.allocate() == "b");
	alloc.deallocate();
	alloc.deallocate();
	BOOST_REQUIRE(alloc.allocate() == "a");
}

BOOST_AUTO_TEST_CASE(TestGoesToCapsAndBack)
{
	AddressAllocator alloc;
	int i;
	for(i = 0; i < 26; i++)
		alloc.allocate();

	string allocation = alloc.allocate();
	BOOST_REQUIRE_MESSAGE(allocation == "A", allocation + " was recieved instead of A");
	alloc.deallocate();
	alloc.deallocate();
	allocation = alloc.allocate();
	BOOST_REQUIRE_MESSAGE(allocation == "z", allocation + " was recieved instead of z");
}

BOOST_AUTO_TEST_CASE(TestGoesToDoubleAndBack)
{
	AddressAllocator alloc;
	int i;
	for(i = 0; i < 52; i++)
		alloc.allocate();

	string allocation = alloc.allocate();
	BOOST_REQUIRE_MESSAGE(allocation == "aa", allocation + " was recieved instead of aa");
	alloc.deallocate();
	alloc.deallocate();
	allocation = alloc.allocate();
	BOOST_REQUIRE_MESSAGE(allocation == "Z", allocation + " was recieved instead of Z");
}

BOOST_AUTO_TEST_CASE(TestCountsUpLikeBackwardsDigits)
{
	AddressAllocator alloc;
	int i;
	for(i = 0; i < 52; i++)
		alloc.allocate();

	string allocation = alloc.allocate();
	BOOST_REQUIRE_MESSAGE(allocation == "aa", allocation + " was recieved instead of aa");
	allocation = alloc.allocate();
	BOOST_REQUIRE_MESSAGE(allocation == "ba", allocation + " was recieved instead of ba");
	allocation = alloc.allocate();
	BOOST_REQUIRE_MESSAGE(allocation == "ca", allocation + " was recieved instead of ca");
	allocation = alloc.allocate();
	BOOST_REQUIRE_MESSAGE(allocation == "da", allocation + " was recieved instead of da");
}

BOOST_AUTO_TEST_CASE(TestCountsUpSecondDigitToo)
{
	AddressAllocator alloc;
	int i;
	for(i = 0; i < 104; i++)
		alloc.allocate();

	string allocation = alloc.allocate();
	BOOST_REQUIRE_MESSAGE(allocation == "ab", allocation + " was recieved instead of ab");
	allocation = alloc.allocate();
	BOOST_REQUIRE_MESSAGE(allocation == "bb", allocation + " was recieved instead of bb");
	allocation = alloc.allocate();
	BOOST_REQUIRE_MESSAGE(allocation == "cb", allocation + " was recieved instead of cb");
	allocation = alloc.allocate();
	BOOST_REQUIRE_MESSAGE(allocation == "db", allocation + " was recieved instead of db");
}

BOOST_AUTO_TEST_SUITE_END()
