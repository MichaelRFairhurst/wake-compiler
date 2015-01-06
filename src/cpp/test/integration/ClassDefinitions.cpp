/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ClassDefinitions.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	MultipleClassDefinition,
	"every MyClass is: every MyClass is:",
	PTT_EXPECT(MULTIPLE_CLASS_DEFINITION)
)

PTT_TEST_CASE(
	CircularInheritance,
	"every MyClassA (a MyClassB) is: every MyClassB (a MyClassA) is:",
	PTT_EXPECT(CIRCULAR_INHERITANCE)
)

PTT_TEST_CASE(
	TransitiveCircularInheritance,
	"every MyClassA (a MyClassB) is: every MyClassB (a MyClassC) is: every MyClassC (a MyClassA) is:",
	PTT_EXPECT(CIRCULAR_INHERITANCE)
)

PTT_TEST_CASE(
	ChildClassNotExists,
	"every MyClassA (a MyClassB) is:",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	ExtendChildClassTwice,
	"every Parent is: every Child (a Parent, capable Parent) is:",
	PTT_EXPECT(MULTIPLE_INHERITANCE)
)

PTT_TEST_CASE(
	ExtendTwoClasses,
	"every ClassA is: every ClassB is: every ClassC (a ClassA, a ClassB) is:",
	PTT_EXPECT(MORE_THAN_ONE_SUBCLASS)
)

PTT_TEST_CASE(
	TestCircularNeedThroughInheritanceIsInvalid,
	"every SubClass (a MyClass) is: every MyClass is: needs SubClass;",
	PTT_EXPECT(CIRCULAR_DEPENDENCIES)
);

PTT_TEST_CASE(
	TestParentsNeedsAreVisibleToChild,
	"every MyClass is: needs Text:hey; every SubClass (a MyClass) is: Text -- getHey() { return Text; }",
	PTT_VALID
);

