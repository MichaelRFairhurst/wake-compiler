/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Provisions.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	ProvideClassNotExists,
	"every ClassA is: provides NonExistClass;",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	RetrieveFromErrorWorks,
	"every ClassA is: myMethod() { Num from Oastnhunsh; }",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	ProvideBindToClassNotExists,
	"every ClassA is: provides ClassA <- NonExistClass;",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	ProvideBindToClassConstructorNotExists,
	"every ClassA is: provides ClassA <- NonExistClass(ClassA);",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	ProvideBindToClassConstructorArgumentNotExists,
	"every ClassB is: needs ClassA; every ClassA is: provides ClassB <- ClassB(NonExistClass);",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	ProvideBindToClassConstructorArgumentTwoNotExists,
	"every ClassB is: needs ClassA, ClassA also; every ClassA is: provides ClassA, ClassB <- ClassB(ClassA, NonExistClass);",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	TestProvideNumOnNonNumIsBadNewsBears,
	"every MyClass is:				\n\
		provides MyClass <- 4.0;	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	TestProvideIntOnNonIntIsBadNewsBears,
	"every MyClass is:			\n\
		provides MyClass <- 4;	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	TestProvideBoolOnNonBoolIsBadNewsBears,
	"every MyClass is:				\n\
		provides MyClass <- \\n;	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	TestProvideCharOnNonCharIsBadNewsBears,
	"every MyClass is:				\n\
		provides MyClass <- true;	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	TestProvideTextOnNonTextIsBadNewsBears,
	"every MyClass is:				\n\
		provides MyClass <- 'text';	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	TestProvideNotSubclassIsBadNewsBears,
	"every ParentClass is:					\n\
	every MyClass (a ParentClass) is:		\n\
		provides MyClass <- ParentClass;	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	ValidProvisions,
	"every ParentClass is:					\n\
	every MyClass (a ParentClass) is:		\n\
		provides							\n\
			Num <- 4.0,						\n\
			Int <- 4,						\n\
			Char <- \\n,					\n\
			Bool <- true,					\n\
			Text <- 'Test',					\n\
			ParentClass <- MyClass,			\n\
			MyClass <- MyClass;				\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	InjectCtorInvalidSubtype,
	"every ClassA is:						\n\
	every ClassB is:						\n\
		needs ClassA;						\n\
		provides ClassB <- ClassB(ClassB);	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	InjectCtorIsTooFewDependencies,
	"every ClassA is:						\n\
	every ClassB is:						\n\
		needs Num, ClassA;					\n\
		provides ClassB <- ClassB(5);		\n\
	",
	PTT_EXPECT(MISMATCHED_INJECTION)
)

PTT_TEST_CASE(
	InjectCtorIsTooManyDependencies,
	"every ClassA is:								\n\
	every ClassB is:								\n\
		needs ClassA;								\n\
		provides ClassB <- ClassB(ClassB, ClassB);	\n\
	",
	PTT_EXPECT(MISMATCHED_INJECTION)
)

PTT_TEST_CASE(
	InjectCtorValid,
	"every ClassA is:								\n\
	every ClassB is:								\n\
		needs ClassA;								\n\
		provides ClassA, ClassB <- ClassB(ClassA);	\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	InjectCtorSpecializationWarning,
	"every ClassA is:								\n\
	every ClassB is:								\n\
		needs ClassA:Specified;					\n\
		provides ClassA, ClassB <- ClassB(ClassA);	\n\
	",
	PTT_EXPECT(WARNING)
)

PTT_TEST_CASE(
	InjectCtorSpecializationMatchedNotWarning,
	"every ClassA is:											\n\
	every ClassB is:											\n\
		needs ClassA:Specified;								\n\
		provides												\n\
			ClassA:Specified,									\n\
			ClassB <- ClassB(ClassA:Specified);				\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	InjectCtorWithNumNotNeeded,
	"every ClassA is:					\n\
	every ClassB is:					\n\
		needs ClassA;					\n\
		provides ClassB <- ClassB(6.0);	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	InjectCtorWithIntNotNeeded,
	"every ClassA is:					\n\
	every ClassB is:					\n\
		needs ClassA;					\n\
		provides ClassB <- ClassB(6);	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	InjectCtorWithCharNotNeeded,
	"every ClassA is:					\n\
	every ClassB is:					\n\
		needs ClassA;					\n\
		provides ClassB <- ClassB(\\n);	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	InjectCtorWithBoolNotNeeded,
	"every ClassA is:						\n\
	every ClassB is:						\n\
		needs ClassA;						\n\
		provides ClassB <- ClassB(true);	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	InjectCtorWithTextNotNeeded,
	"every ClassA is:						\n\
	every ClassB is:						\n\
		needs ClassA;						\n\
		provides ClassB <- ClassB('test');	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
)

PTT_TEST_CASE(
	InjectCtorWithTextAndNumAndIntAndCharAndBoolOK,
	"every ClassA is:											\n\
	every ClassB is:											\n\
		needs Num, Text, Int, Char, Bool;						\n\
		provides ClassB <- ClassB(5.0, 'test', 5, \\n, true);	\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	DuplicateProvisionNoSpecialtyIsError,
	"every ClassA is:				\n\
		provides ClassA, ClassA;	\n\
	",
	PTT_EXPECT(DUPLICATE_PROPERTY_DEFINITION)
)

PTT_TEST_CASE(
	DuplicateProvisionWithSpecialtyIsError,
	"every ClassA is:							\n\
		provides ClassA:Poop, ClassA:Poop;	\n\
	",
	PTT_EXPECT(DUPLICATE_PROPERTY_DEFINITION)
)

PTT_TEST_CASE(
	DuplicateProvisionWithDifferentSpecialtyIsOk,
	"every ClassA is:							\n\
		provides ClassA:Poop, ClassA:NotPoop;	\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	RetrieveFromClassValid,
	"every MyClass is: provides MyClass; MyClass -- method() { return MyClass from this; }",
	PTT_VALID
)

PTT_TEST_CASE(
	RetrieveFromClassInvalid,
	"every MyClass is: MyClass -- method() { return MyClass from this; }",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
)

PTT_TEST_CASE(
	BindToCtorNonexistProviding,
	"every MyClass is: needs Int, Text; provides Int <- 5, MyClass <- MyClass(Int, Text);",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
)

PTT_TEST_CASE(
	RetrieveFromClassCtordSpecialtiedValid,
	"every MyDB is:										\n\
														\n\
		needs Int:Port, Text:Username;				\n\
														\n\
		provides										\n\
			Int:Port <- 55,							\n\
			Text:Username <- 'Bilbo',					\n\
			MyDB <- MyDB(Int:Port, Text:Username);	\n\
														\n\
		MyDB -- cloneMe() {								\n\
			return MyDB from this;						\n\
		}												\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	InjectionsAndProvisionsRequireTransitiveProvidability,
	"every ClassA is:							\n\
		needs Int:Port;						\n\
	every ClassB is:							\n\
		needs ClassA, Text:Stuff;				\n\
	every ClassC is:							\n\
		needs ClassB, Int:ID;					\n\
	every Provider is:							\n\
		provides								\n\
			ClassB,								\n\
			ClassC,								\n\
			Int:ID,							\n\
			Text:Stuff;						\n\
	",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
)

PTT_TEST_CASE(
	InjectionsAndProvisionsRequireTransitiveProvidabilityOK,
	"every ClassA is:					\n\
		needs Int:Port;				\n\
										\n\
	every ClassB is:					\n\
		needs ClassA, Text:Stuff;		\n\
										\n\
	every ClassC is:					\n\
		needs ClassB, Int:ID;			\n\
										\n\
	every Provider is:					\n\
		provides						\n\
			ClassC,						\n\
			Int:ID,					\n\
			Text:Stuff,				\n\
			Int:Port,					\n\
			ClassB,						\n\
			ClassA;						\n\
	",
	PTT_VALID
)

/*
PTT_TEST_CASE(
	CircularDependenciesAreError,
	"every CircularA is: needs CircularB;	\n\
	every CircularB is: needs CircularC;	\n\
	every CircularC is: needs CircularA;",
	PTT_EXPECT(CIRCULAR_DEPENDENCIES)
);

PTT_TEST_CASE(
	SelfDependencyIsError,
	"every Circular is: needs Circular;",
	PTT_EXPECT(CIRCULAR_DEPENDENCIES)
);
*/

PTT_TEST_CASE(
	TestParentNeedsAreNecessaryToProvide,
	"every MyClass is: needs Text:hey;	\n\
	every SubClass (a MyClass) is:		\n\
		provides SubClass;",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	TestGetWithSpecialtyInMethod,
	"every MyClass is:							\n\
		provides MyClass:Specialized;			\n\
		method() {								\n\
			var MyClass:Specialized from this;	\n\
			MyClass:Specialized from this;		\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestGetWithoutSpecialtyInMethod,
	"every MyClass is:						\n\
		provides MyClass:Specialized;		\n\
		method() {							\n\
			var MyClass from this;			\n\
		}									\n\
		methodTwo() {						\n\
			MyClass from this;				\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	TestBehavioralProvisions,
	"every MyClass is:					\n\
		provides Int:Test <- { return 5; };",
	PTT_VALID
);

PTT_TEST_CASE(
	TestInvalidBehavioralProvision,
	"every MyClass is:							\n\
		provides Int:Test <- { return 'not int'; };",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestUseThisInBehavioralProvisions,
	"every MyClass is:							\n\
		provides MyClass <- { return this; };",
	PTT_VALID
);

PTT_TEST_CASE(
	TestProvidesBlockDoesntNeedTransitiveDeps,
	"every MyClass is:							\n\
		needs Text:SomeNeed;					\n\
		provides MyClass <- { return this; };",
	PTT_VALID
);

PTT_TEST_CASE(
	TestProvisionArgumentNotSubtypeOfNeed,
	"every MyClass is:						\n\
		needs Int;							\n\
		provides MyClass <- MyClass(?Text);	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestProvisionArgumentSubtypeOfNeed,
	"every MyClass is:						\n\
		needs Int;							\n\
		provides MyClass <- MyClass(?Int);	\n\
	",
	PTT_VALID
);

PTT_TEST_CASE(
	TestProvisionContravariantArgumentIsInvalid,
	"every ClassA is:							\n\
	every ClassB (a ClassA) is:					\n\
	every MyClass is:							\n\
		needs ClassB;							\n\
		provides MyClass <- MyClass(?ClassA);	\n\
	",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestProvisionCovariantArgumentIsValid,
	"every ClassA is:							\n\
	every ClassB (a ClassA) is:					\n\
	every MyClass is:							\n\
		needs ClassA;							\n\
		provides MyClass <- MyClass(?ClassB);	\n\
	",
	PTT_VALID
);

PTT_TEST_CASE(
	TestProvisionContravariantInjectionIsInvalid,
	"every ClassA is:							\n\
	every ClassB (a ClassA) is:					\n\
	every MyClass is:							\n\
		needs ClassB;							\n\
		provides MyClass <- MyClass(ClassA),	\n\
			ClassA;								\n\
	",
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestProvisionCovariantInjectionIsValid,
	"every ClassA is:							\n\
	every ClassB (a ClassA) is:					\n\
	every MyClass is:							\n\
		needs ClassA;							\n\
		provides MyClass <- MyClass(ClassB),	\n\
			ClassB;								\n\
	",
	PTT_VALID
);

PTT_TEST_CASE(
	TestProvisionArgumentNonexistClass,
	"every MyClass is:							\n\
		needs Int;								\n\
		provides MyClass <- MyClass(?Nurmm);	\n\
	",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
);

PTT_TEST_CASE(
	TestCallProvisionWithNoArgsIsSymbolNotFound,
	"every MyClass is:							\n\
		needs Text;								\n\
		provides MyClass <- MyClass(?Text);		\n\
		myMethod() {							\n\
			var MyClass from this;				\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	TestCallProvisionWithTooManyArgsIsSymbolNotFound,
	"every MyClass is:								\n\
		needs Text;									\n\
		provides MyClass <- MyClass(?Text);			\n\
		myMethod() {								\n\
			var MyClass('Text', 'Text') from this;	\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	TestCallProvisionWithWrongArgsIsSymbolNotFound,
	"every MyClass is:								\n\
		needs Text;									\n\
		provides MyClass <- MyClass(?Text);			\n\
		myMethod() {								\n\
			var MyClass(5) from this;				\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	TestCallProvisionWithCorrectArgsIsCorrect,
	"every MyClass is:								\n\
		needs Text;									\n\
		provides MyClass <- MyClass(?Text);			\n\
		myMethod() {								\n\
			var MyClass('Hey') from this;			\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestCallOverloadedProvisionIsCorrect,
	"every MyClass is:								\n\
		needs Text;									\n\
		provides MyClass <- MyClass(?Text);			\n\
		provides MyClass <- MyClass('hey');			\n\
		myMethod() {								\n\
			var MyClass('Hey') from this;			\n\
			var $MyClass from this;					\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestSubtypeProvisionDontNeedToProvideNeeds,
	"every ClassA is:											\n\
		needs ClassB;											\n\
	every ClassB is:											\n\
	every ParentClassA (capable ClassA) is:						\n\
	every Provider is:											\n\
		provides ClassA <- ParentClassA;						\n\
		provides ParentClassA;									\n\
		",
	PTT_VALID
);

PTT_TEST_CASE(
	TestInjectionProvisionDontNeedToProvideNeeds,
	"every ClassA is:											\n\
		needs ClassB;											\n\
	every ClassB is:											\n\
	every Provider is:											\n\
		provides ClassA <- ClassA(ClassB:test);					\n\
		provides ClassB:test;									\n\
		",
	PTT_VALID
);

PTT_TEST_CASE(
	TestBehavioralProvisionUsingArguments,
	"every ClassA is:											\n\
	every ClassB is:											\n\
	every MyClass is:											\n\
		needs ClassA, ClassB;									\n\
		provides MyClass <- (ClassA, ClassB) {					\n\
			return this;										\n\
		};														\n\
	",
);

PTT_TEST_CASE(
	TestBehavioralProvisionTypeChecksArguments,
	"every MyClass is:											\n\
		provides MyClass <- (Num, Text, Bool) {					\n\
			Num + 'hey';										\n\
			Text + 5;											\n\
			Bool + 'hey';										\n\
			return this;										\n\
		};														\n\
	",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestBehavioralProvisionTypeChecksArgumentsPositive,
	"every ClassA is:											\n\
	every ClassB is:											\n\
	every MyClass is:											\n\
		needs ClassA, ClassB;									\n\
		provides MyClass <- (Num, Text, Bool) {					\n\
			Num + 5;											\n\
			Text + 'hey';										\n\
			Bool = false;										\n\
			return this;										\n\
		};														\n\
	",
	PTT_VALID
);

PTT_TEST_CASE(
	TestUseBehavioralProvision,
	"every ClassA is:											\n\
	every ClassB is:											\n\
	every MyClass is:											\n\
		needs ClassA, ClassB;									\n\
		provides MyClass <- (Num, Text, Bool) {					\n\
			return MyClass(Num, Text, Bool) from this;			\n\
		};														\n\
	",
	PTT_VALID
);

PTT_TEST_CASE(
	TestRetrievalWithinRetrieval,
	"every ClassA is:																			\n\
	every ClassB is:																			\n\
		needs ClassA;																			\n\
	every MyClass is:																			\n\
		needs ClassB;																			\n\
		provides MyClass <- MyClass(?ClassB),													\n\
			ClassB <- ClassB(?ClassA),															\n\
			ClassA,																				\n\
			MyClass <- { return this; };														\n\
		method() {																				\n\
			MyClass(ClassB(ClassA from this) from this) from MyClass from MyClass from this;	\n\
		}																						\n\
	",
	PTT_VALID
);

