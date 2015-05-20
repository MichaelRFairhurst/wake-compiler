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
	"every MyClass: every MyClass:",
	PTT_EXPECT(MULTIPLE_CLASS_DEFINITION)
)

PTT_TEST_CASE(
	CircularInheritance,
	"every MyClassA (a MyClassB): every MyClassB (a MyClassA):",
	PTT_EXPECT(CIRCULAR_INHERITANCE)
)

PTT_TEST_CASE(
	TransitiveCircularInheritance,
	"every MyClassA (a MyClassB): every MyClassB (a MyClassC): every MyClassC (a MyClassA):",
	PTT_EXPECT(CIRCULAR_INHERITANCE)
)

PTT_TEST_CASE(
	ChildClassNotExists,
	"every MyClassA (a MyClassB):",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	ExtendChildClassTwice,
	"every Parent: every Child (a Parent, capable Parent):",
	PTT_EXPECT(MULTIPLE_INHERITANCE)
)

PTT_TEST_CASE(
	ExtendTwoClasses,
	"every ClassA: every ClassB: every ClassC (a ClassA, a ClassB):",
	PTT_EXPECT(MORE_THAN_ONE_SUBCLASS)
)

PTT_TEST_CASE(
	TestCircularNeedThroughInheritanceIsInvalid,
	"every SubClass (a MyClass): every MyClass: needs SubClass;",
	PTT_EXPECT(CIRCULAR_DEPENDENCIES)
);

PTT_TEST_CASE(
	TestParentsNeedsAreVisibleToChild,
	"every MyClass: needs Text:hey; every SubClass (a MyClass): Text -- getHey() { return Text; }",
	PTT_VALID
);

PTT_TEST_CASE(
	TestExternClassWithMethodsIsRejected,
	"extern MyClass:						\n\
		provides MyClass:test <- MyClass,	\n\
			MyClass <- { return this; };	\n\
		myMethod() {						\n\
		}",
	PTT_EXPECT(EXTERN_METHOD_BODY)
	PTT_EXPECT(EXTERN_METHOD_BODY)
	PTT_EXPECT(EXTERN_METHOD_BODY)
);


PTT_TEST_CASE(
	TestExternClassValid,
	"extern MyClass:			\n\
		needs Num;				\n\
		provides MyClass;		\n\
		myMethod();",
	PTT_VALID
);
