/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * VarDeclarations.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	TestDeclareAnNum,
	"every MyClass is: Num -- myfn() { var Num = 5; return Num; }",
	PTT_VALID
)

PTT_TEST_CASE(
	TestDeclareAnNumWithBadType,
	"every MyClass is: myfn() { var Num = 'test'; }",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	TestDeclareAnNumWithUnknownType,
	"every MyClass is: myfn() { var LNum = 'test'; }",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	TestDeclareAnNumIsScoped,
	"every MyClass is: Num -- myfn() { if(true) { var Num = 5; } return Num; }",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	TestVarDeclarationInForLoopIsTemporary,
	"every MyClass is: method() {		\n\
		for(var Num = 0; true; 0) { } 	\n\
		Num += 1;						\n\
	}",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
);

PTT_TEST_CASE(
	testVarsWithinBlockAndWithinCatchAreIsolateScopes,
	"every MyClass is:							\n\
		declareVarsInTryUnusableOutside() {		\n\
			try {								\n\
				var Num = 5;					\n\
			} catch(Exception) {				\n\
				Num += 1;						\n\
			}									\n\
			Num += 5;							\n\
		}										\n\
		declareVarsInCatchUnusableOutside() {	\n\
			try {								\n\
			} catch(Exception) {				\n\
				var Num = 5;					\n\
			}									\n\
			Num += 5;							\n\
		}",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
);
