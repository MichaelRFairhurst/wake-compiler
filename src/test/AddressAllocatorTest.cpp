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

BOOST_AUTO_TEST_SUITE_END()
