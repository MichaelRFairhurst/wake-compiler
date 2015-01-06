/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * EarlyBailoutOperators.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	TestEarlyBailoutMemberAccessNotValidOnNonOptionals,
	"every MyClass is:											\n\
		myMethod() {											\n\
			1.?Text;											\n\
			'hey'.?Text;										\n\
			true.?Text; 										\n\
		}",
	PTT_EXPECT(OPTIONAL_USE_OF_NONOPTIONAL_TYPE)
	PTT_EXPECT(OPTIONAL_USE_OF_NONOPTIONAL_TYPE)
	PTT_EXPECT(OPTIONAL_USE_OF_NONOPTIONAL_TYPE)
);

PTT_TEST_CASE(
	TestEarlyBailoutMethodCallNotValidOnNonOptionals,
	"every MyClass is:											\n\
		myMethod() {											\n\
			1.?orMaxOf(3);										\n\
			'hey'.?split(',');									\n\
			true.?toString(); 									\n\
		}",
	PTT_EXPECT(OPTIONAL_USE_OF_NONOPTIONAL_TYPE)
	PTT_EXPECT(OPTIONAL_USE_OF_NONOPTIONAL_TYPE)
	PTT_EXPECT(OPTIONAL_USE_OF_NONOPTIONAL_TYPE)
);

PTT_TEST_CASE(
	TestEarlyBailoutAccessUnknownPropertyIsUnknownProp,
	"every MyClass is:											\n\
		myMethod(Num?, Text?, Bool?) {							\n\
			Num.?rrrrRRRr;										\n\
			Text.?rrrrRRRr;										\n\
			Bool.?rrrrRRRr; 									\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	TestEarlyBailoutCallUnknownMethodIsTypeError,
	"every MyClass is:											\n\
		myMethod(Num?, Text?, Bool?) {							\n\
			Num.?rrrrRRR(3);									\n\
			Text.?rrrrRRR(',');									\n\
			Bool.?rrrrRRRr(); 									\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	TestEarlyBailoutAccessReturnsOptionalWrappedMemberType,
	"every MyClass is:											\n\
		with public Num = 1;									\n\
		with public Text = 'hey';								\n\
		with public Num[] = [1];								\n\
		with public Text[] = ['hey'];							\n\
		myMethod(MyClass?, $MyClass??) {						\n\
			var Num? = MyClass.?Num; 							\n\
			var Text? = MyClass.?Text; 							\n\
			var Num[]? = MyClass.?Num[]; 						\n\
			var Text[]? = MyClass.?Text[]; 						\n\
			Num?? = $MyClass.?Num; 								\n\
			Text?? = $MyClass.?Text; 							\n\
			Num[]?? = $MyClass.?Num[]; 							\n\
			Text[]?? = $MyClass.?Text[]; 						\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestEarlyBailoutReturnsOptionalWrappedReturnType,
	"every MyClass is:											\n\
		Num -- getNum() { return 1; }							\n\
		Text -- getText() { return 'hey'; }						\n\
		Num[] -- getNumList() { return [1]; }					\n\
		Text[] -- getTextList() { return ['hey']; }				\n\
		myMethod(MyClass?, $MyClass?? ) {						\n\
			var Num? = MyClass.?getNum(); 						\n\
			var Text? = MyClass.?getText(); 					\n\
			var Num[]? = MyClass.?getNumList(); 				\n\
			var Text[]? = MyClass.?getTextList(); 				\n\
			Num? = $MyClass.?getNum();							\n\
			Text? = $MyClass.?getText();						\n\
			Num[]? = $MyClass.?getNumList();					\n\
			Text[]? = $MyClass.?getTextList();					\n\
		}",
	PTT_VALID
);

