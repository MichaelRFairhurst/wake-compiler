/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * AbstractClasses.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	AbstractMethodMakesClassAbstract,
	"every MyClass is:		\n\
		provides MyClass;	\n\
		myMethod();",
	PTT_EXPECT(ABSTRACT_PROVISION)
);

PTT_TEST_CASE(
	AbstractParentMethodMakesClassAbstract,
	"every MyClass (a ParentClass) is:	\n\
		provides MyClass;				\n\
	every ParentClass is:				\n\
		myMethod();",
	PTT_EXPECT(ABSTRACT_PROVISION)
);

PTT_TEST_CASE(
	UnimplementedNumerfaceMethodMakesClassAbstract,
	"every MyClass (capable ParentClass) is:	\n\
		provides MyClass;						\n\
	every ParentClass is:						\n\
		myMethod() { ; }",
	PTT_EXPECT(ABSTRACT_PROVISION)
);

PTT_TEST_CASE(
	AbstractMethodMakesClassAbstractIndirectProvision,
	"every MyClass is:					\n\
		provides MyClass <- MyClass;	\n\
		myMethod();",
	PTT_EXPECT(ABSTRACT_PROVISION)
);

PTT_TEST_CASE(
	AbstractParentMethodMakesClassAbstractIndirectProvision,
	"every MyClass (a ParentClass) is:	\n\
		provides MyClass <- MyClass;	\n\
	every ParentClass is:				\n\
		myMethod();",
	PTT_EXPECT(ABSTRACT_PROVISION)
);

PTT_TEST_CASE(
	UnimplementedNumerfaceMethodMakesClassAbstractIndirectProvision,
	"every MyClass (capable ParentClass) is:	\n\
		provides MyClass <- MyClass;			\n\
	every ParentClass is:						\n\
		myMethod() { ; }",
	PTT_EXPECT(ABSTRACT_PROVISION)
);
