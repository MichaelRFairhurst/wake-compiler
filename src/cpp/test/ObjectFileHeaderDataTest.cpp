/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ObjectFileHeaderDataTest.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "boost/test/unit_test.hpp"
#include "ObjectFileHeaderData.h"

BOOST_AUTO_TEST_SUITE(ObjectFileHeaderDataTest)

BOOST_AUTO_TEST_CASE(TestBeginsWithNoClasses)
{
	ObjectFileHeaderData data;
	BOOST_REQUIRE(data.getClasses().size() == 0);
}

BOOST_AUTO_TEST_CASE(TestBeginsWithNoProperties)
{
	ObjectFileHeaderData data;
	BOOST_REQUIRE(data.getProperties().size() == 0);
}

BOOST_AUTO_TEST_CASE(TestBeginsWithNoClassUsages)
{
	ObjectFileHeaderData data;
	BOOST_REQUIRE(data.getClassUsages().size() == 0);
}

BOOST_AUTO_TEST_CASE(TestBeginsWithNoPropertyUsages)
{
	ObjectFileHeaderData data;
	BOOST_REQUIRE(data.getPropertyUsages().size() == 0);
}

BOOST_AUTO_TEST_CASE(TestBeginsWithNoNeeds)
{
	ObjectFileHeaderData data;
	BOOST_REQUIRE(data.getNeeds().size() == 0);
}

BOOST_AUTO_TEST_CASE(TestAddingClassUsageAddsClass)
{
	ObjectFileHeaderData data;
	data.addClassUsage(0, "test");
	BOOST_REQUIRE(data.getClasses().size() == 1);
	BOOST_REQUIRE(data.getClasses()[0] == "test");
}

BOOST_AUTO_TEST_CASE(TestAddingClassUsageAddsClassUsage)
{
	ObjectFileHeaderData data;
	data.addClassUsage(10, "test");
	BOOST_REQUIRE(data.getClassUsages().size() == 1);
	BOOST_REQUIRE(data.getClassUsages()[0].first == 10);
	BOOST_REQUIRE(data.getClassUsages()[0].second == "test");
}

BOOST_AUTO_TEST_CASE(TestAddingClassUsageTwiceAddsClassOnce)
{
	ObjectFileHeaderData data;
	data.addClassUsage(0, "test");
	data.addClassUsage(1, "test");
	BOOST_REQUIRE(data.getClasses().size() == 1);
	BOOST_REQUIRE(data.getClasses()[0] == "test");
}

BOOST_AUTO_TEST_CASE(TestAddingPropertyUsageAddsClass)
{
	ObjectFileHeaderData data;
	data.addPropertyUsage(0, "test");
	BOOST_REQUIRE(data.getProperties().size() == 1);
	BOOST_REQUIRE(data.getProperties()[0] == "test");
}

BOOST_AUTO_TEST_CASE(TestAddingPropertyUsageAddsPropertyUsage)
{
	ObjectFileHeaderData data;
	data.addPropertyUsage(10, "test");
	BOOST_REQUIRE(data.getPropertyUsages().size() == 1);
	BOOST_REQUIRE(data.getPropertyUsages()[0].first == 10);
	BOOST_REQUIRE(data.getPropertyUsages()[0].second == "test");
}

BOOST_AUTO_TEST_CASE(TestAddingPropertyUsageTwiceAddsPropertyOnce)
{
	ObjectFileHeaderData data;
	data.addPropertyUsage(0, "test");
	data.addPropertyUsage(1, "test");
	BOOST_REQUIRE(data.getProperties().size() == 1);
	BOOST_REQUIRE(data.getProperties()[0] == "test");
}

BOOST_AUTO_TEST_SUITE_END()
