/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Lists.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	ListIndexAccessTypeErrors,
	"every MyClass is:											\n\
		arrayAccessOnNum() { 9[1]; }							\n\
		arrayAccessOnString() { 'abcd'[1]; }					\n\
		arrayAccessOnBool() { true[1]; }						\n\
		arrayAccessOnNum(Num alias) { alias[1]; }				\n\
		arrayAccessOnString(Text alias) { alias[1]; }			\n\
		arrayAccessOnBool(Bool alias) { alias[1]; }				\n\
		arrayAccessOnClass(MyClass my) { my[1]; }				\n\
		arrayAccessWithClass(MyClass, Num[]) { Num[MyClass]; }	\n\
		arrayAccessWithString(Num[]) { Num['test']; }			\n\
		safeListAccessOnNum() { 9[?1]; }							\n\
		safeListAccessOnString() { 'abcd'[?1]; }					\n\
		safeListAccessOnBool() { true[?1]; }						\n\
		safeListAccessOnNum(Num alias) { alias[?1]; }				\n\
		safeListAccessOnString(Text alias) { alias[?1]; }			\n\
		safeListAccessOnBool(Bool alias) { alias[?1]; }				\n\
		safeListAccessOnClass(MyClass my) { my[?1]; }				\n\
		safeListAccessWithClass(MyClass, Num[]) { Num[?MyClass]; }	\n\
		safeListAccessWithString(Num[]) { Num[?'test']; }			\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	ListIndexSymbolNotFoundErrors,
	"every MyClass is:									\n\
		arrayAccessOnClass(Text) { Text[1]; }			\n\
		arrayAccessOnClass(Num) { Num[1]; }				\n\
		arrayAccessOnClass(Bool) { Bool[1]; }			\n\
		arrayAccessOnClass(MyClass) { MyClass[1]; }		\n\
	",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
);

PTT_TEST_CASE(
	ValidListIndexAccessAndReturningValidTypes,
	"every MyClass is:														\n\
		intListBecomesNum(Num[]) { Num[0] + 3; }							\n\
		stringListBecomesString(Text[]) { Text[1] + 'test'; }				\n\
		stringListListBecomesString(Text[][]) { Text[1][1] + 'test'; }	\n\
		intListListBecomesNum(Num[][]) { Num[1][1] + 5; }					\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	DeclareListsInvalidValues,
	"every MyClass is:					\n\
		assignMeToList() {				\n\
			var MyClass[] = this;		\n\
		}								\n\
		assignNumberToClassList() {	\n\
			var MyClass[] = 4;			\n\
		}								\n\
		assignTextToClassList() {		\n\
			var MyClass[] = 'test';		\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	DeclareEmptyListsValid,
	"every MyClass is:								\n\
		declareEmptyClassListAndListList() {		\n\
			var MyClass[] = [];						\n\
			var $MyClass[][] = [];					\n\
		}											\n\
		declareEmptyNumListAndListList() {		\n\
			var Num[] = [];							\n\
			var $Num[][] = [];						\n\
		}											\n\
		declareEmptyStringListAndListList() {	\n\
			var Text[] = [];						\n\
			var $Text[][] = [];						\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	AssignIndexesWrongTypes,
	"every MyClass is:										\n\
		with MyClass[] = [];								\n\
		with Num[] = [];									\n\
		with Text[] = [];									\n\
		with Bool[] = [];									\n\
		assignNumToMyClasses() { MyClass[1] = 5; }			\n\
		assignTextToMyClasses() { MyClass[1] = 'test'; }	\n\
		assignBoolToMyClasses() { MyClass[1] = true; }		\n\
		assignMyClassToNums() { Num[1] = this; }			\n\
		assignTextToNums() { Num[1] = 'test'; }				\n\
		assignBoolToNums() { Num[1] = true; }				\n\
		assignMyClassToTexts() { Text[1] = this; }			\n\
		assignNumToTexts() { Text[1] = 4; }					\n\
		assignBoolToTexts() { Text[1] = true; }				\n\
		assignMyClassToBools() { Bool[1] = this; }			\n\
		assignNumToBools() { Bool[1] = 4; }					\n\
		assignTextToBools() { Bool[1] = 'test'; }			\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
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
	ValidEmptyListAssignments,
	"every MyClass is:										\n\
		with MyClass[][] = [];								\n\
		with Num[][] = [];									\n\
		with Text[][] = [];									\n\
		with Bool[][] = [];								\n\
		assignToMyClasses() { MyClass[][] = []; }			\n\
		assignToMyClassesIndex() { MyClass[1] = []; }		\n\
		assignToNums() { Num[][] = []; }					\n\
		assignToNumsIndex() { Num[1] = []; }				\n\
		assignToTexts() { Text[][] = []; }					\n\
		assignToTextsIndex() { Text[1] = []; }				\n\
		assignToBools() { Bool[][] = []; }				\n\
		assignToBoolsIndex() { Bool[1] = []; }			\n\
	",
	PTT_VALID
);

PTT_TEST_CASE(
	InvalidEmptyListAssignments,
	"every MyClass is:										\n\
		with $Num = [];										\n\
		with $Text = [];									\n\
		with $MyClass = [];									\n\
		with $Bool = [];									\n\
		with Num[] = []; //valid							\n\
		with Text[] = []; //valid							\n\
		with Bool[] = []; //valid							\n\
		with MyClass[] = []; //valid						\n\
		assignToNumsIndex() { Num[1] = []; }				\n\
		assignToTextsIndex() { Text[1] = []; }				\n\
		assignToBoolsIndex() { Bool[1] = []; }			\n\
		assignToMyClassesIndex() { MyClass[1] = []; }		\n\
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
	TestValidListLiterals,
	"every MyClass is:											\n\
		method() {												\n\
			var Text[] = ['hey'];								\n\
			var $Text[] = ['hey', 'man'];						\n\
			var $$Text[] = [];									\n\
			var Num[] = [];										\n\
			var $$Num[] = [1];									\n\
			var $$$Num[] = [1, 2];								\n\
			var $$$$Num[][] = [[1]];							\n\
			var $$$Text[][] = [['hey']];						\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestInvalidListLiterals,
	"every MyClass is:											\n\
		method() {												\n\
			var Num[] = ['hey'];								\n\
			var Text[] = ['hey', 3];							\n\
			var $Text[] = [true];								\n\
			var $Num[] = [nothing];								\n\
			var $$Num[] = [this];								\n\
			var $$$Num[] = [1, [2]];							\n\
			var $$$$Num[][] = [['hey']];						\n\
			var $$Text[][] = [[['hey']]];						\n\
		}",
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
	TestStrangeValidUsagesOfEmptyList,
	"every MyClass is:											\n\
		with Num[][][] = [];									\n\
		with Num len = [].getSize();							\n\
		MyClass[] -- myMethod() {								\n\
			var Text[][] = [[], [], []];						\n\
			var Text = [[], [], []][0][0];						\n\
			var $Text = [][0][0][0]; // technically valid!		\n\
			return [];											\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestStrangeInvalidUsagesOfEmptyList,
	"every MyClass is:											\n\
		with Num[][][] = [[[[]]]];								\n\
		with Num len = [];										\n\
		MyClass -- myMethod() {									\n\
			var Text[][] = [[3], [], []];						\n\
			return [];											\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestNestedListAndOptionals,
	"every MyClass is:											\n\
		needs Num??;											\n\
		with Num?[] = [nothing];								\n\
		with $Num?[] = [];										\n\
		with $$Num[]? = [];										\n\
		with $$$Num[]? = nothing;								\n\
		with $$$$Num?[][][]?[]?[] = [nothing, [nothing, [[[nothing]]]]];",
	PTT_VALID
);

PTT_TEST_CASE(
	TestSetOptionalOptionalToValues,
	"every MyClass is:											\n\
		myMethod() {											\n\
			var Num? = nothing;									\n\
			Num = [1, 1][0];									\n\
			Num = [1, nothing][0];								\n\
			Num = [nothing][0];									\n\
			var $Num?? = Num;									\n\
			$Num = nothing;										\n\
			$Num = 1;											\n\
			$Num = [1, 1][0];									\n\
			$Num = [1, nothing][0];								\n\
			$Num = [1, Num][0];									\n\
			$Num = [nothing, Num][0];							\n\
			$Num = [nothing][0];								\n\
			$Num = [Num][0];									\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	ValidSafeListIndexAccessAndReturningValidTypes,
	"every MyClass is:															\n\
		Num? -- intListBecomesNum(Num[]) { return Num[?0]; }					\n\
		Text? -- stringListBecomesString(Text[]) { return Text[?1]; }			\n\
		Text[]? -- stringListListBecomesString(Text[][]) { return Text[?1]; }	\n\
		Num[]? -- intListListBecomesNum(Num[][]) { return Num[?1]; }			\n\
	",
	PTT_VALID
);
