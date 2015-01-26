/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Optionals.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	UsingAOptionalTypeIsError,
	"every MyClass is:					\n\
		myMethod(MyClass?) {			\n\
			MyClass.myMethod(MyClass);	\n\
		}",
	PTT_EXPECT(DIRECT_USE_OF_OPTIONAL_TYPE)
);

PTT_TEST_CASE(
	AssignmentOfOptionalTypeAsRealTypeIsError,
	"every MyClass is:				\n\
		assignment(MyClass?) {		\n\
			var $MyClass = MyClass;	\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	AssignmentOfOptionalTypeToOptionalTypeIsValid,
	"every MyClass is:				\n\
		assignment(MyClass?) {		\n\
			var $MyClass? = MyClass;	\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	ReturnOfOptionalTypeAsRealTypeIsError,
	"every MyClass is:						\n\
		MyClass -- returnment(MyClass?) {	\n\
			return MyClass;					\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	ReturnOfOptionalTypeAsOptionalTypeIsValid,
	"every MyClass is:						\n\
		MyClass? -- returnment(MyClass?) {	\n\
			return MyClass;					\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	ArgumentOfOptionalTypeAsRealTypeIsError,
	"every MyClass is:			\n\
		optional(MyClass?) {	\n\
			real(MyClass);		\n\
		}						\n\
		real(MyClass) {			\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	ArgumentOfOptionalTypeAsRealTypeOnOverloadedMethodIsError,
	"every MyClass is:			\n\
		optional(MyClass?) {	\n\
			real(MyClass);		\n\
		}						\n\
		real(Num) {				\n\
		}						\n\
		real(MyClass) {			\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	ArgumentOfOptionalTypeAsOptionalTypeIsValid,
	"every MyClass is:			\n\
		optional(MyClass?) {	\n\
			real(MyClass);		\n\
		}						\n\
		real(MyClass?) {		\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	NullAssignmentToRealTypeIsError,
	"every MyClass is:			\n\
		myMethod(MyClass) {		\n\
			MyClass = nothing;	\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	NullAssignmentToOptionalTypeIsValid,
	"every MyClass is:			\n\
		myMethod(MyClass?) {	\n\
			MyClass = nothing;	\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	NullReturnOfRealTypeIsError,
	"every MyClass is:			\n\
		MyClass -- myMethod() { \n\
			return nothing;		\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	NullReturnOfOptionalTypeIsValid,
	"every MyClass is:				\n\
		MyClass? -- myMethod() {	\n\
			return nothing;			\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	NullArgumentOfRealArgumentIsError,
	"every MyClass is:		\n\
		real(MyClass) {		\n\
			real(nothing);	\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	NullArgumentOfOverloadedRealArgumentIsError,
	"every MyClass is:		\n\
		real(MyClass) {		\n\
			real(nothing);	\n\
		}					\n\
		real(Num) {		\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	ExistsClauseOnRealTypeNotAllowed,
	"every MyClass is:				\n\
		myMethod(MyClass) {			\n\
			if MyClass exists {}	\n\
		}",
	PTT_EXPECT(EXISTS_ON_NONOPTIONAL_TYPE)
);

PTT_TEST_CASE(
	ExistsClauseMakesOptionalTypeUsable,
	"every MyClass is:				\n\
		method() {}					\n\
		myMethod(MyClass?) {		\n\
			if MyClass exists {		\n\
				MyClass.method();	\n\
			}						\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	ExistsClauseMakesOptionalTypeAssignable,
	"every MyClass is:					\n\
		myMethod(MyClass?) {			\n\
			if MyClass exists {			\n\
				var $MyClass = MyClass;	\n\
			}							\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	ExistsClauseMakesOptionalTypeReturnable,
	"every MyClass is:			\n\
		needs $Num;				\n\
		Num -- myMethod(Num?) {	\n\
			if Num exists {		\n\
				return Num;		\n\
			}					\n\
			return $Num;		\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	ExistsClauseMakesOptionalTypeValidAsArgument,
	"every MyClass is:			\n\
		optional(MyClass?) {	\n\
			if MyClass exists {	\n\
				real(MyClass);	\n\
			}					\n\
		}						\n\
		real(MyClass) {			\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	AfterExistClauseOptionalsRemainOptional,
	"every MyClass is:					\n\
		print(Text) {}					\n\
		usage(MyClass?) {				\n\
			if MyClass exists {}		\n\
			MyClass.print(\"a\");		\n\
		}								\n\
		assignment(MyClass?) {			\n\
			if MyClass exists {}		\n\
			var $MyClass = MyClass;		\n\
		}								\n\
		MyClass -- returnin(MyClass?) {	\n\
			if MyClass exists {}		\n\
			return MyClass;				\n\
		}								\n\
		optional(MyClass?) {			\n\
			if MyClass exists {}		\n\
			real(MyClass);				\n\
		}								\n\
		real(MyClass) {					\n\
		}",
	PTT_EXPECT(DIRECT_USE_OF_OPTIONAL_TYPE)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	OptionalNumsBoolsAndTextsCantBeUsed,
	"every MyClass is:										\n\
		addOptionalNums(Num?, $Num?) { Num + $Num; }		\n\
		subtractOptionalNums(Num?, $Num?) { Num - $Num; }	\n\
		divideOptionalNums(Num?, $Num?) { Num / $Num; }		\n\
		multiplyOptionalNums(Num?, $Num?) { Num * $Num; }	\n\
		concatOptionalTexts(Text?, $Text?) { Text * Text; }	\n\
		notOptionalBool(Bool?) { !Bool; }					\n\
		andOptionalBools(Bool?, $Bool?) { Bool && $Bool; }	\n\
		orOptionalBools(Bool?, $Bool?) { Bool || $Bool; }	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestExistsClauseOnAlias,
	"every MyClass is: method() {		\n\
		var Num? aliased = nothing;		\n\
		if aliased exists {				\n\
			aliased + 2;				\n\
		}								\n\
	}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestExistsInvalidClauseOnAlias,
	"every MyClass is: method() {		\n\
		var Num aliased = 4;			\n\
		if aliased exists { }			\n\
	}",
	PTT_EXPECT(EXISTS_ON_NONOPTIONAL_TYPE)
);

PTT_TEST_CASE(
	TestEndOfExistsScopeDoesntRemoveVariableFromTable,
	"every MyClass is: method() {			\n\
		var Num? = nothing;					\n\
		if(true) {							\n\
			if Num exists { }				\n\
		}									\n\
		Num = nothing;						\n\
	}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestExistsOnAPropertyIsDisallowed,
	"every MyClass is:						\n\
		with Num? = nothing;				\n\
		callExistsOnProperty() {			\n\
			if Num exists {}				\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

