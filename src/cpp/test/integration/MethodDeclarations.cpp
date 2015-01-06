/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * MethodDeclarations.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	MethodDeclaredTwiceSimple,
	"every ClassA is: getName() {} getName() {}",
	PTT_EXPECT(MULTIPLE_METHOD_DEFINITION)
)

PTT_TEST_CASE(
	MethodDeclaredTwiceComplex,
	"every ClassA is:										\n\
		getName(											\n\
			ClassA,											\n\
			ClassA b,										\n\
			ClassA -- fn(									\n\
				ClassA,										\n\
				ClassA -- fn()								\n\
			) alias1										\n\
		)FromEmptyFunc(										\n\
			ClassA -- fn() alias2							\n\
		) {}												\n\
															\n\
		getName(											\n\
			ClassA,											\n\
			ClassA c,										\n\
			ClassA -- fn(									\n\
				ClassA,										\n\
				ClassA -- fn()								\n\
			) alias3										\n\
		)FromEmptyFunc(										\n\
			ClassA -- fn() alias4							\n\
		) {}												\n\
		",
	PTT_EXPECT(MULTIPLE_METHOD_DEFINITION)
)

PTT_TEST_CASE(
	MethodWithNonExistReturn,
	"every MyClass is: NonExist -- badMethod() {}",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodWithNonExistArgument,
	"every MyClass is: MyClass -- badMethod(NonExist) {}",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodWithLambdaReturnWithInvalidReturn,
	"every MyClass is: NonExist -- fn() -- badMethod() {}",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodWithLambdaReturnWithInvalidArgument,
	"every MyClass is: MyClass -- fn(NonExist) -- badMethod() {}",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodWithLambdaArgumentWithInvalidReturn,
	"every MyClass is: badMethod( NonExist -- fn() aliased ) {}",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodWithLambdaArgumentWithInvalidArgument,
	"every MyClass is: badMethod( MyClass -- fn( NonExist ) aliased ) {}",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	MethodDefinedWithVariablesSameClassname,
	"every MyClass is: methodWithDualArgs( MyClass, MyClass ) {}",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
)

PTT_TEST_CASE(
	MethodDefinedWithVariablesSameClassnameOneArrayedOK,
	"every MyClass is: methodWithDualArgs( MyClass, MyClass[] ) {}",
	PTT_VALID
)

PTT_TEST_CASE(
	MethodDefinedWithVariablesSameClassnameDifferentArrayed,
	"every MyClass is: methodWithDualArgs( MyClass[], MyClass[][] ) {}",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
)

PTT_TEST_CASE(
	MethodDefinedWithVariablesSameAlias,
	"every MyClass is: methodWithDualArgs( MyClass b, MyClass b ) {}",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
)

PTT_TEST_CASE(
	MethodDefinedWithVariablesSameShadow,
	"every MyClass is: methodWithDualArgs( $MyClass, $MyClass ) {}",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
)

PTT_TEST_CASE(
	MethodUsesNonexistAlias,
	"every MyClass is: methodWithEmptyScope() { b; }",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	MethodUsesNonInstanceBoundClass,
	"every MyClass is: methodWithEmptyScope() { MyClass; }",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	MethodUsesNonInstanceBoundShadow,
	"every MyClass is: methodWithEmptyScope(MyClass) { $MyClass; }",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	MethodUsesClassnameForAliasBoundClass,
	"every MyClass is: methodWithEmptyScope(MyClass b) { MyClass; }",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	TestMethodCannotBeExtendedAsVoid,
	"every ParentClass is:					\n\
		Text -- method() { return ""; }		\n\
	every ChildClass (a ParentClass) is:	\n\
		method() {}							\n\
	",
	PTT_EXPECT(TYPE_ERROR) // it complains about the return type once like this
	PTT_EXPECT(INVALID_CHILD_RETURN_TYPE)
);

PTT_TEST_CASE(
	TestMethodCannotBeExtendedOrthogonally,
	"every ParentClass is:					\n\
		Text -- method() { return ''; }		\n\
	every ChildClass (a ParentClass) is:	\n\
		Num -- method() { return 2; }		\n\
	",
	PTT_EXPECT(INVALID_CHILD_RETURN_TYPE)
);

PTT_TEST_CASE(
	TestVoidMethodCanBeExtendedToReturn,
	"every ParentClass is:					\n\
		method() { }						\n\
	every ChildClass (a ParentClass) is:	\n\
		Num -- method() { return 2; }		\n\
	",
	PTT_VALID
);

PTT_TEST_CASE(
	TestMethodCannotBeExtendedToReturnCovariantType,
	"every ParentClass is:												\n\
		ChildClass -- method(ChildClass, ParentClass) {					\n\
			return ChildClass;											\n\
		}																\n\
	every ChildClass (a ParentClass) is:								\n\
		ParentClass -- method(ChildClass, ParentClass) {				\n\
			return ParentClass;											\n\
		}																\n\
	",
	PTT_EXPECT(INVALID_CHILD_RETURN_TYPE)
);

PTT_TEST_CASE(
	TestMethodCanBeExtendedToReturnContravariantType,
	"every ParentClass is:												\n\
		ParentClass -- method(ChildClass, ParentClass) {				\n\
			return ParentClass;											\n\
		}																\n\
	every ChildClass (a ParentClass) is:								\n\
		ChildClass -- method(ChildClass, ParentClass) {					\n\
			return ChildClass;											\n\
		}																\n\
	",
	PTT_VALID
);

