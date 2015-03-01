/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Properties.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	InvalidPropertyClassIsNotFoundError,
	"every MyClass is: with NonExist = 4;\n",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
)

PTT_TEST_CASE(
	assignNonExistClassToPropertyIsNotFoundError,
	"every MyClass is: with MyClass = NonExist;\n",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	AssignNonSubTypeToPropertyIsTypeError,
	"every Unrelated is: every MyClass is: with MyClass = Unrelated;",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	AssignMultiplePropertiesRightOrderIsntOK,
	"every MyClass is: with Num = 5; with $Num = Num; with $$Num = $Num;",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	AssignMultiplePropertiesLeftOrderIsntOK,
	"every MyClass is: with Num = $Num; with $Num = $$Num; with $$Num = 5;",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	AssignPropertyFromCtorIsOK,
	"every MyClass is: with Num too = Num; needs Num;",
	PTT_VALID
)

PTT_TEST_CASE(
	TestUsePublicPropertiesOnOtherObjectAndThis,
	"every MyClass is:						\n\
		with public Num = 3;				\n\
		with public Num aliased = 3;		\n\
		myMethod(MyClass) {					\n\
			MyClass.Num + MyClass.aliased;	\n\
			MyClass.Num = 3;				\n\
			MyClass.aliased = 3;			\n\
			this.Num + this.aliased;		\n\
			this.Num = 3;					\n\
			this.aliased = 3;				\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	AccessPropertyOnTypeErrorIsntSegfault,
	"every MyClass is: myMethod() { Stou.lalala; }",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
)

PTT_TEST_CASE(
	TestUsePublicPropertiesOnOtherObjectAndThisInvalid,
	"every MyClass is:							\n\
		with public Num = 3;					\n\
		with public Num aliased = 3;			\n\
		myMethod(MyClass) {						\n\
			MyClass.Num == this;				\n\
			MyClass.Num.nonExistMethod();		\n\
			MyClass.aliased == this;			\n\
			MyClass.aliased.nonExistMethod();	\n\
			MyClass.Num = this;					\n\
			MyClass.Num = true;					\n\
			MyClass.aliased = this;				\n\
			MyClass.aliased = true;				\n\
			this.Num == this;					\n\
			this.Num.nonExistMethod();			\n\
			this.aliased == this;				\n\
			this.aliased.nonExistMethod();		\n\
			this.Num = this;					\n\
			this.Num = true;					\n\
			this.aliased = this;				\n\
			this.aliased = true;				\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	MemberAccessOnOptionalType,
	"every MyClass is:						\n\
		with public Text hello = '';		\n\
		method() {							\n\
		var MyClass? = nothing;				\n\
		MyClass.hello;						\n\
	}",
	PTT_EXPECT(DIRECT_USE_OF_OPTIONAL_TYPE)
);

PTT_TEST_CASE(
	TestDuplicatePropertiesThrowsAlreadyDefined,
	"every MyClass is: needs Num one, Num one;",
	PTT_EXPECT(SYMBOL_ALREADY_DEFINED)
);

PTT_TEST_CASE(
	TestAccessArrayedClassMember,
	"every MyClass is:							\n\
		with public Text[] = [];				\n\
		myMethod(MyClass) {						\n\
			MyClass.Text[2] + 'mustbetext';		\n\
			var Text[] = MyClass.Text[];		\n\
			this.Text[2] + 'mustbetext';		\n\
			Text[] = this.Text[];				\n\
			(MyClass).Text[2] + 'mustbetext';	\n\
			Text[] = (MyClass).Text[]; 			\n\
			(this).Text[2] + 'mustbetext';		\n\
			Text[] = (this).Text[]; 			\n\
			(MyClass.Text[2]) + 'mustbetext';	\n\
			Text[] = (MyClass.Text[]); 			\n\
			(this.Text[2]) + 'mustbetext';		\n\
			Text[] = (this.Text[]); 			\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestAccessArrayedMemberAsNonArrayedIsException,
	"every MyClass is:						\n\
		with public Text[] = [];			\n\
		myMethod(MyClass) {					\n\
			MyClass.Text.getSize(); 		\n\
			this.Text.getSize(); 			\n\
			MyClass.Text + 'doesntexist';	\n\
			this.Text + 'doesntexist';		\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	TestAccessNonArrayedMemberAsArrayedIsException,
	"every MyClass is:							\n\
		with public Text = '';					\n\
		myMethod(MyClass) {						\n\
			MyClass.Text[].getSize();			\n\
			this.Text[].getSize();				\n\
			MyClass.Text[2] + 'doesntexist';	\n\
			this.Text[2] + 'doesntexist';		\n\
			(MyClass).Text[].getSize(); 		\n\
			(this).Text[].getSize(); 			\n\
			(MyClass).Text[2] + 'doesntexist';	\n\
			(this).Text[2] + 'doesntexist';		\n\
			(MyClass.Text[]).getSize(); 		\n\
			(this.Text[]).getSize(); 			\n\
			(MyClass.Text[2]) + 'doesntexist';	\n\
			(this.Text[2]) + 'doesntexist';		\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
);

PTT_TEST_CASE(
	TestMemberAccessNotMatchingArrayCountIsException,
	"every MyClass is:						\n\
		with public Text[][] = [];			\n\
		myMethod(MyClass) {					\n\
			MyClass.Text[]; 				\n\
		}",
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
);

PTT_TEST_CASE(
	TestMemberAccessChainPositiveTestCase,
	"every MyClass is:															\n\
		with public MyClass[] = [];												\n\
		MyClass[] -- getMyClassList(Num) {										\n\
			return MyClass[];													\n\
		}																		\n\
		myMethod() {															\n\
			var Num = 0;														\n\
			MyClass[2].MyClass[2].getMyClassList(Num)[2].MyClass[0].MyClass[0]; \n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestMemberAccessMultipleArrayedLevelPositive,
	"every MyClass is:						\n\
		with public Text[][] = [];			\n\
		myMethod(MyClass) {					\n\
			var $Text[][] = Text[][];		\n\
			var $$Text[] = Text[0];			\n\
			$Text[][] = MyClass.Text[][];	\n\
			$$Text[] = MyClass.Text[0];		\n\
			$Text[][] = this.Text[][];		\n\
			$$Text[] = this.Text[0];		\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestMemberAccessMultipleArrayedLevelNegative,
	"every MyClass is:						\n\
		with public Text[][] = [];			\n\
		myMethod(MyClass) {					\n\
			var $Text[][] = Text[];			\n\
			var $$Text[] = Text;			\n\
			$Text[][] = MyClass.Text[];		\n\
			$$Text[] = MyClass.Text;		\n\
			$Text[][] = this.Text[];		\n\
			$$Text[] = this.Text;			\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
	PTT_EXPECT(SYMBOL_NOT_DEFINED)
);

PTT_TEST_CASE(
	TestDeclareMultipleArrayedMembersNamespaceConflict,
	"every MyClass is:					\n\
		with public Text[] = [];		\n\
		with public Text[][] = [];",
	PTT_EXPECT(DUPLICATE_PROPERTY_DEFINITION)
);

PTT_TEST_CASE(
	TestAccessAliasedPropertyArrayUsesAliasNoBrackets,
	"every MyClass is:					\n\
		with public Text[] arr = [];	\n\
		myMethod() {					\n\
			var Text[] = this.arr;		\n\
			var Text = this.arr[0];		\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestAccessPrivateMemberIsInvalid,
	"every MyClass is:		\n\
		needs Num;			\n\
		with $Num = 4;		\n\
		myMethod(MyClass) {	\n\
			MyClass.Num; 	\n\
			MyClass.$Num; 	\n\
		}",
	PTT_EXPECT(PRIVATE_ACCESS)
	PTT_EXPECT(PRIVATE_ACCESS)
);
