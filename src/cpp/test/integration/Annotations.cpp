/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Annotations.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	TestAnnotatedMethodsClassesParseAndCheckSemantics,
	"@Annotated													\n\
	@AnnotatedVal(true)											\n\
	@AnnotatedParams('test', 123)								\n\
	every MyClass is:											\n\
		@Annotated												\n\
		@AnnotatedVal(true)										\n\
		@AnnotatedParams('test', 123)							\n\
		myMethod() {											\n\
		}														\n\
																\n\
		@Annotated												\n\
		@AnnotatedVal(true)										\n\
		@AnnotatedParams('test', 123)							\n\
		myMethod() {											\n\
		}",
	PTT_EXPECT(MULTIPLE_METHOD_DEFINITION)
);

PTT_TEST_CASE(
	TestAnnotatedMethodsNeedsClassesParseAndTypeCheck,
	"@Annotated													\n\
	@AnnotatedVal(true)											\n\
	@AnnotatedParams('test', 123)								\n\
	every MyClass is:											\n\
		needs													\n\
			@Annotated											\n\
			@AnnotatedVal(true)									\n\
			@AnnotatedParams('test', 123)						\n\
			Num;												\n\
																\n\
		@Annotated												\n\
		@AnnotatedVal(true)										\n\
		@AnnotatedParams('test', 123)							\n\
		myTypecheckedMethod() {									\n\
			Num + 'test';										\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

