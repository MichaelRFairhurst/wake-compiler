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
	@AnnotatedParams('test', 123, 123.5, \\n)								\n\
	every MyClass is:											\n\
		@Annotated												\n\
		@AnnotatedVal(true)										\n\
		@AnnotatedParams('test', 123, 123.5, \\n)							\n\
		myMethod() {											\n\
		}														\n\
																\n\
		@Annotated												\n\
		@AnnotatedVal(true)										\n\
		@AnnotatedParams('test', 123, 123.5, \\n)							\n\
		myMethod() {											\n\
		}",
	PTT_EXPECT(MULTIPLE_METHOD_DEFINITION)
);

PTT_TEST_CASE(
	TestAnnotatedMethodsNeedsPropertiesClassesParseAndTypeCheck,
	"@Annotated													\n\
	@AnnotatedVal(true)											\n\
	@AnnotatedParams('test', 123, 123.5, \\n)								\n\
	every MyClass is:											\n\
		needs													\n\
			@Annotated											\n\
			@AnnotatedVal(true)									\n\
			@AnnotatedParams('test', 123, 123.5, \\n)						\n\
			Num;												\n\
																\n\
		@Annotated												\n\
		@AnnotatedVal(true)										\n\
		@AnnotatedParams('test', 123, 123.5, \\n)							\n\
		with Text = 'hey';										\n\
																\n\
		@Annotated												\n\
		@AnnotatedVal(true)										\n\
		@AnnotatedParams('test', 123, 123.5, \\n)							\n\
		with public $Num = 0;									\n\
																\n\
		@Annotated												\n\
		@AnnotatedVal(true)										\n\
		@AnnotatedParams('test', 123, 123.5, \\n)							\n\
		myTypecheckedMethod() {									\n\
			Num + Text;											\n\
			Text + $Num;										\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestAnnotatedProvisions,
	"@Annotated													\n\
	@AnnotatedVal(true)											\n\
	@AnnotatedParams('test', 123, 123.5, \\n)								\n\
	every MyClass is:											\n\
		provides												\n\
			@Annotated											\n\
			@AnnotatedVal(true)									\n\
			@AnnotatedParams('test', 123, 123.5, \\n)						\n\
			Int:hello <- 4;										\n\
																\n\
		@Annotated												\n\
		@AnnotatedVal(true)										\n\
		@AnnotatedParams('test', 123, 123.5, \\n)							\n\
		myMethod() {											\n\
			var Int:hello from this;							\n\
		}",
	PTT_VALID
);
