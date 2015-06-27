/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ControlFlow.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	CatchesTypeErrorsWithinIfElseAndInvertWorks,
	"every MyClass is:													\n\
		failInIf() { if true then 5 + 'illegal'; }							\n\
		failInElse() { if true then 5; else 5 + 'illegal'; }				\n\
		failInIfInverted() { if !true then 5 + 'illegal'; }					\n\
		failInElseInverted() { if !true then 5; else 5 + 'illegal'; }		\n\
		failInWhile() { while !true do 5 + 'illegal'; }					\n\
		failInWhileInverted() { while !true do 5 + 'illegal'; }			\n\
		failInFor() { for(5; !true; 5) 5 + 'illegal'; }					\n\
		failInForInverted() { for(5; !true; 5) 5 + 'illegal'; }			\n\
		failInForInit() { for(5 + 'illegal'; !true; 5) 5; }				\n\
		failInForInitInverted() { for( 5 + 'illegal'; !true; 5) 5; }	\n\
		failInForIncr() { for(5; !true; 5 + 'illegal') 5; }				\n\
		failInForIncrInverted() { for(5; !true; 5 + 'illegal') 5; }		\n\
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
)

PTT_TEST_CASE(
	ValidIfConditions,
	"every MyClass is:											\n\
		truthLiterals() { if true then 5; if false then 5; }			\n\
		truthNumLiteral() { if 1 then 5; }			\n\
		truthVariable(Bool) { if Bool then 5; }					\n\
		numVariable(Num) { if Num then 5; }					\n\
		truthAlias(Bool b) { if b then 5; }						\n\
		numAlias(Num n) { if n then 5; }						\n\
		truthLiteralsWhile() { while true do 5; while false do 5; }	\n\
		truthVariableWhile(Bool) { while Bool do 5; }			\n\
		truthAliasWhile(Bool b) { while b do 5; }				\n\
		numLiteralWhile() { while 5 do 5; }	\n\
		numVariableWhile(Num) { while Num do 5; }			\n\
		numAliasWhile(Num n) { while n do 5; }				\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	CaseNotWithinSwitchIsError,
	"every MyClass is: afn() { case 5: }",
	PTT_EXPECT(ILLEGAL_CASE)
)

PTT_TEST_CASE(
	DefaultCaseNotWithinSwitchIsError,
	"every MyClass is: afn() { default: }",
	PTT_EXPECT(ILLEGAL_CASE)
)

/* SWITCH CASE LOGIC IS PENDING
PTT_TEST_CASE(
	CaseWithinSwitchIsOk,
	"every MyClass is: afn() { switch(5) { case 1: case 2: default: } }",
	PTT_VALID
)

PTT_TEST_CASE(
	CaseWithinSwitchAfterClosingPriorSwitchIsOk,
	"every MyClass is: afn() { switch(5) { case 1: switch (8) { case 6: } case 2: default: } }",
	PTT_VALID
)

PTT_TEST_CASE(
	BreakWithinSwitchAfterClosingPriorSwitchIsOk,
	"every MyClass is: afn() { switch(5) { case 1: break; switch (8) { case 6: break; } case 2: break; default: } }",
	PTT_VALID
)*/

PTT_TEST_CASE(
	BreakNotWithinSwitchForOrWhileIsError,
	"every MyClass is: afn() { break; }",
	PTT_EXPECT(ILLEGAL_BREAK)
)

PTT_TEST_CASE(
	ContinueNotWithinForOrWhileIsError,
	"every MyClass is: afn() { continue; }",
	PTT_EXPECT(ILLEGAL_CONTINUE)
)

PTT_TEST_CASE(
	BreakAndContinueWithinForOrWhileIsOK,
	"every MyClass is: afn() { while true { while true { break; continue; } for(5; true; 5) { break; continue; }; break; continue; } }",
	PTT_VALID
)

PTT_TEST_CASE(
	InexhaustiveReturnFromIfsWithoutElses,
	"every MyClass is: Bool -- afn() { if true then return true; if true then return true; if true then return true; }",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
)

PTT_TEST_CASE(
	InexhaustiveReturnFromIfsWithoutReturns,
	"every MyClass is: Bool -- afn() { if true then 5; else return true; if true then 5; else return true; }",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
)

PTT_TEST_CASE(
	InexhaustiveReturnFromElsesWithoutReturns,
	"every MyClass is: Bool -- afn() { if true then return true; else 5; if true then return true; else 5; }",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
)

PTT_TEST_CASE(
	InexhaustiveReturnFromReturningWithinLoopsOnly,
	"every MyClass is: Bool -- afn() { while true do return true; for(1; true; 1) return true; }",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
)

PTT_TEST_CASE(
	InexhaustiveReturnFromNoReturn,
	"every MyClass is: Bool -- afn() { 5 + 5; 6 + 6; 8 + 9 + 7; }",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
)

PTT_TEST_CASE(
	InexhaustiveReturnComplicated,
	"every MyClass is:				\n\
		Bool -- afn() {			\n\
			if true {				\n\
				if true {			\n\
					5 + 5;			\n\
					6 + 6;			\n\
					return true;	\n\
				}					\n\
			} else {				\n\
				5 + 6;				\n\
				7 == 10;			\n\
				while true {		\n\
					return true;	\n\
				}					\n\
			}						\n\
			6 + 7;					\n\
		}",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
)

PTT_TEST_CASE(
	ExhaustiveReturnComplicated,
	"every MyClass is:				\n\
		Bool -- afn() {			\n\
			if true {				\n\
				if true {			\n\
					5 + 5;			\n\
					6 + 6;			\n\
					return true;	\n\
				} else {			\n\
					7 / 16;			\n\
				}					\n\
				return true;		\n\
			} else {				\n\
				5 + 6;				\n\
				7 == 10;			\n\
				return true;		\n\
			}						\n\
			6 + 7;					\n\
	}",
	PTT_VALID
)

PTT_TEST_CASE(
	InexhaustiveReturnsOkWithVoidReturnType,
	"every MyClass is: afn() { } ",
	PTT_VALID
)

PTT_TEST_CASE(
	ExistsElseStatementIsTypeChecked,
	"every MyClass is:				\n\
		myMethod() {				\n\
			var Num? = 4;			\n\
			if Num exists {} else { \n\
				Num + 4;			\n\
			}						\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	ReturningWithinExistsIsNotExhaustive,
	"every MyClass is:				\n\
		Bool -- myMethod(Num?) {	\n\
			if Num exists {			\n\
				return true;		\n\
			}						\n\
		}",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
);

PTT_TEST_CASE(
	ReturningWithinExistsElseIsNotExhaustive,
	"every MyClass is:				\n\
		Bool -- myMethod(Num?) {	\n\
			if Num exists {			\n\
			} else {				\n\
				return true;		\n\
			}						\n\
		}",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
);

PTT_TEST_CASE(
	ReturningWithinExistsWithAnElseIsNotExhaustive,
	"every MyClass is:				\n\
		Bool -- myMethod(Num?) {	\n\
			if Num exists {			\n\
				return true;		\n\
			} else {				\n\
			}						\n\
		}",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
);

PTT_TEST_CASE(
	ReturningWithinExistsAndElseIsExhaustive,
	"every MyClass is:				\n\
		Bool -- myMethod(Num?) {	\n\
			if Num exists {			\n\
				return true;		\n\
			} else {				\n\
				return false;		\n\
			}						\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestForeachAddsLowerVariables,
	"every MyClass is:						\n\
		method() {							\n\
			var Num[] = [];					\n\
			foreach Num[] {				\n\
				Num += Num[0];				\n\
			}								\n\
			var Text[] = [];				\n\
			foreach Text[] {				\n\
				Text += Text[0];			\n\
			}								\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestForeachWontOverwriteLowerVariables,
	"every MyClass is:						\n\
		lowerArrayArray() {					\n\
			var Num[][] = [];				\n\
			foreach Num[][] { }			\n\
		}									\n\
		overwriteVar() {					\n\
			var Num[] = [];					\n\
			var Num = 3;					\n\
			foreach Num[] { }				\n\
		}",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
);

PTT_TEST_CASE(
	TestForeachOnNonArray,
	"every MyClass is:						\n\
		onThis() { foreach this { } }		\n\
		on(Num) { foreach Num { } }		\n\
		on(Text) { foreach Text { } }		\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestForeachOnAnAliasNoNamespaceClash,
	"every MyClass is:						\n\
		myMethod() {						\n\
			var aliased Num[] = [];			\n\
			foreach aliased { Num + 5; }	\n\
		}",
);

PTT_TEST_CASE(
	TestForeachOnAnAliasNamespaceClash,
	"every MyClass is:						\n\
		myMethod() {						\n\
			var aliased Num[] = [];			\n\
			var Num = 5;					\n\
			foreach aliased { }			\n\
		}",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
);

PTT_TEST_CASE(
	TestAbstractMethodsDontComplainAboutExhaustiveReturns,
	"every MyClass is:		\n\
		Text -- method();",
	PTT_VALID
);

PTT_TEST_CASE(
	TestForeachInValid,
	"every MyClass is:					\n\
		myMethod(Text[]) {				\n\
			foreach Text[] as text {	\n\
				text + 'another text';	\n\
			}							\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestTypeErrorsWithinForeachAreCaught,
	"every MyClass is:					\n\
		myMethod(Text[]) {				\n\
			foreach Text[] as text {	\n\
				5 + 'another text';		\n\
			}							\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestNonSubtypeinForeachIsTypeError,
	"every MyClass is:					\n\
		myMethod(Text[]) {				\n\
			foreach Text[] as Num {		\n\
				aaaa;					\n\
			}							\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestRedefinitionOfAliasInForeachInIsCaught,
	"every MyClass is:					\n\
		myMethod(Text[], Text text) {	\n\
			foreach Text[] as text {	\n\
			}							\n\
		}",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
);

PTT_TEST_CASE(
	TestRedefinitionOfTypeInForeachInIsCaught,
	"every MyClass is:					\n\
		myMethod(Text[], Text) {		\n\
			foreach Text[] as Text {	\n\
			}							\n\
		}",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
);

PTT_TEST_CASE(
	TestForeachShadowKeepsShadow,
	"every MyClass is:					\n\
		Text -- myMethod($Text[]) {		\n\
			foreach $Text[] {			\n\
				return $Text;			\n\
			}							\n\
			return \"Blah\";			\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestValidTernaries,
	"every MyClass is:											\n\
		method() {												\n\
			var Bool = true if true else true;					\n\
			var Num = 1 if 2 else 3;							\n\
			var Text = 'hey' if Bool else 'hello';				\n\
			var MyClass? = nothing if 1 else this;				\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestInvalidTernaries,
	"every MyClass is:											\n\
		method() {												\n\
			var Num = 1 if 'hey' else 3;						\n\
			var $Num = 'hey' if true else 'hello';				\n\
			var $$Num = 'hey' if true else 1;					\n\
			var MyClass = this if this else true;				\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

