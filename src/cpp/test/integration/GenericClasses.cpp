/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * GenericClasses.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	TestParameterizedClassReturningType,
	"every MyClass{T, E} is:	\n\
		T -- myMethod(T) {		\n\
			return T;			\n\
		}						\n\
								\n\
		E -- myMethod(E) {		\n\
			return E;			\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestParameterizedClassReturningWrongType,
	"every MyClass{T, E} is:	\n\
		E -- returnClass() {	\n\
			return this;		\n\
		}						\n\
								\n\
		E -- returnBool() {		\n\
			return true;		\n\
		}						\n\
								\n\
		E -- returnText() {		\n\
			return 'Test';		\n\
		}						\n\
								\n\
		E -- returnNum() {		\n\
			return 5;			\n\
		}						\n\
								\n\
		E -- myMethod(T) {		\n\
			return T;			\n\
		}						\n\
								\n\
		T -- myMethod(E) {		\n\
			return E;			\n\
		}",

	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestUseConcreteParameterizedClasses,
	"every MyClass{T} is:								\n\
		MyClass{Num} -- takeWithNum(MyClass{Num}) {		\n\
			return MyClass;								\n\
		}												\n\
		MyClass{Text} -- takeWithText(MyClass{Text}) {	\n\
			return MyClass;								\n\
		}												\n\
		MyClass{T} -- takeWithT(MyClass{T}) {			\n\
			return MyClass;								\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestUseConcreteParameterizedClassesIncorrectly,
	"every MyClass{T} is:									\n\
		MyClass{Text} -- returnNumAsText(MyClass{Num}) {	\n\
			return MyClass;									\n\
		}													\n\
		MyClass{Text} -- returnTAsText(MyClass{T}) {		\n\
			return MyClass;									\n\
		}													\n\
		MyClass{T} -- returnNumAsT(MyClass{Num}) {			\n\
			return MyClass;									\n\
		}													\n\
		MyClass{T} -- returnTextAsT(MyClass{Text}) {		\n\
			return MyClass;									\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestGenericMethodReturnTypesHaveChanged,
	"every MyClass{T} is:													\n\
		T -- returnATypeT() {												\n\
			return returnATypeT();											\n\
		}																	\n\
		Num -- returnATypeNum(MyClass{Num}) {								\n\
			return MyClass.returnATypeT();									\n\
		}																	\n\
		Text -- returnAText(MyClass{Text}) {								\n\
			return MyClass.returnATypeT();									\n\
		}																	\n\
		Bool -- returnABool(MyClass{Bool}) {								\n\
			return MyClass.returnATypeT();									\n\
		}																	\n\
		MyClass{T} -- returnAMyClassT(MyClass{MyClass{T}}) {				\n\
			return MyClass.returnATypeT();									\n\
		}																	\n\
		Num? -- returnOptionalNumArray(MyClass{Num?}) {						\n\
			return MyClass.returnATypeT();									\n\
		}																	\n\
		Num[] -- returnNumArray(MyClass{Num[]}) {							\n\
			return MyClass.returnATypeT();									\n\
		}																	\n\
		T[] -- returnTArray(MyClass{T[]}) {									\n\
			return MyClass.returnATypeT();									\n\
		}																	\n\
		T? -- returnOptionalT(MyClass{T?}) {								\n\
			return MyClass.returnATypeT();									\n\
		}																	\n\
		Num? -- returnOptionalNum(MyClass{Num?}) {							\n\
			return MyClass.returnATypeT();									\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestGenericNeedsAndProperties,
	"every Generic{T} is:														\n\
	every MyClass{T} is:														\n\
		needs T plainneed,														\n\
			T? optionalneed,													\n\
			T[] arrayedneed,													\n\
			Generic{T} containedneed,											\n\
			T -- fn() returnedneed,												\n\
			T? -- fn() returnedoptionalneed,									\n\
			T[] -- fn() returnedarrayedneed,									\n\
			Generic{T} -- fn() returnedcontainedneed,							\n\
			fn(T) argumentneed,													\n\
			fn(T?) argumentoptionalneed,										\n\
			fn(T[]) argumentarrayedneed,										\n\
			fn(Generic{T}) argumentcontainedneed;								\n\
		with T plainprop = plainneed;											\n\
		with T? optionalprop = optionalneed;									\n\
		with T[] arrayedprop = arrayedneed;										\n\
		with Generic{T} containedprop = containedneed;							\n\
		with T -- fn() returnedprop = returnedneed;								\n\
		with T? -- fn() returnedoptionalprop = returnedoptionalneed;			\n\
		with T[] -- fn() returnedarrayedprop = returnedarrayedneed;				\n\
		with Generic{T} -- fn() returnedcontainedprop = returnedcontainedneed;	\n\
		with fn(T) argumentprop = argumentneed;									\n\
		with fn(T?) argumentoptionalprop = argumentoptionalneed;				\n\
		with fn(T[]) argumentarrayedprop = argumentarrayedneed;					\n\
		with fn(Generic{T}) argumentcontainedprop = argumentcontainedneed;		\n\
		",
		PTT_VALID
);

PTT_TEST_CASE(
	TestBadParameterizationsInPropertiesAndNeeds,
	"every Generic{T} is:										\n\
	every MyClass is:											\n\
		needs Generic,											\n\
			Generic{MyClass, MyClass, MyClass},					\n\
			Generic{Generic{MyClass, MyClass}},					\n\
			MyClass{Generic{MyClass}},							\n\
			MyClass{Generic{MyClass}, Generic{MyClass}};		\n\
		with Generic = 1;										\n\
		with Generic{MyClass, MyClass, MyClass} = 1;			\n\
		with Generic{Generic{MyClass, MyClass}} = 1;			\n\
		with MyClass{Generic{MyClass}} = 1;						\n\
		with MyClass{Generic{MyClass}, Generic{MyClass}} = 1;	\n\
	",
	PTT_EXPECT(INVALID_GENERIC_TYPE)
	PTT_EXPECT(INVALID_GENERIC_TYPE)
	PTT_EXPECT(INVALID_GENERIC_TYPE)
	PTT_EXPECT(INVALID_GENERIC_TYPE)
	PTT_EXPECT(INVALID_GENERIC_TYPE)
	PTT_EXPECT(INVALID_GENERIC_TYPE)
	PTT_EXPECT(INVALID_GENERIC_TYPE)
	PTT_EXPECT(INVALID_GENERIC_TYPE)
	PTT_EXPECT(INVALID_GENERIC_TYPE)
	PTT_EXPECT(INVALID_GENERIC_TYPE)
);

PTT_TEST_CASE(
	TestUseGenericMethodsCorrectly,
	"every Generic{T1, T2} is:					\n\
		type1(T1) {}							\n\
		optional1(T1?) {}						\n\
		arrayed1(T1[]) {}						\n\
		type2(T2) {}							\n\
		optional2(T2?) {}						\n\
		arrayed2(T2[]) {}						\n\
		contained(Generic{T1, T2}) {}			\n\
	every MyClass is:							\n\
		needs Num,								\n\
			Num? optionalNum,					\n\
			Num[] intArray,						\n\
			Text,								\n\
			Text? optionalText,					\n\
			Text[] textArray;					\n\
		testNumText(Generic{Num,Text}) {		\n\
			Generic.type1(Num);					\n\
			Generic.optional1(optionalNum);		\n\
			Generic.arrayed1(intArray);			\n\
			Generic.type2(Text);				\n\
			Generic.optional2(optionalText);	\n\
			Generic.arrayed2(textArray);		\n\
			Generic.contained(Generic);			\n\
		}										\n\
		testTextNum(Generic{Text,Num}) {		\n\
			Generic.type1(Text);				\n\
			Generic.optional1(optionalText);	\n\
			Generic.arrayed1(textArray);		\n\
			Generic.type2(Num);					\n\
			Generic.optional2(optionalNum);		\n\
			Generic.arrayed2(intArray);			\n\
			Generic.contained(Generic);			\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestCanProvideAndRetrieveValidGenericImplementations,
	"every MyClass{T} is:						\n\
		provides								\n\
			MyClass{Num},						\n\
			MyClass{Text},						\n\
			MyClass{Bool},						\n\
			MyClass{Num?},						\n\
			MyClass{Text?},						\n\
			MyClass{Bool?},						\n\
			MyClass{Num[]},						\n\
			MyClass{Text[]},					\n\
			MyClass{Bool[]},					\n\
			MyClass{MyClass{Num}},				\n\
			MyClass{MyClass{Text}},				\n\
			MyClass{MyClass{Bool}},				\n\
			MyClass{MyClass{Num?}},				\n\
			MyClass{MyClass{Text?}},			\n\
			MyClass{MyClass{Bool?}},			\n\
			MyClass{MyClass{Num[]}},			\n\
			MyClass{MyClass{Text[]}},			\n\
			MyClass{MyClass{Bool[]}};			\n\
		useProvisions() {						\n\
			MyClass{Num} from this;				\n\
			MyClass{Text} from this;				\n\
			MyClass{Bool} from this;				\n\
			MyClass{Num?} from this;				\n\
			MyClass{Text?} from this;				\n\
			MyClass{Bool?} from this;				\n\
			MyClass{Num[]} from this;				\n\
			MyClass{Text[]} from this;			\n\
			MyClass{Bool[]} from this;			\n\
			MyClass{MyClass{Num}} from this;		\n\
			MyClass{MyClass{Text}} from this;		\n\
			MyClass{MyClass{Bool}} from this;		\n\
			MyClass{MyClass{Num?}} from this;		\n\
			MyClass{MyClass{Text?}} from this;	\n\
			MyClass{MyClass{Bool?}} from this;	\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestUseGenericThingsInMethods,
	"every MyClass{A, B, C, D} is:										\n\
		needs A, B, C, D,												\n\
			A? optA, B? optB, C? optC, D? optD,							\n\
			A[] arrA, B[] arrB, C[] arrC, D[] arrD;						\n\
		with A propA = A;												\n\
		with B propB = B;												\n\
		with C propC = C;												\n\
		with D propD = D;												\n\
		with A? propoptA = optA;										\n\
		with B? propoptB = optB;										\n\
		with C? propoptC = optC;										\n\
		with D? propoptD = optD;										\n\
		with A[] proparrA = arrA;										\n\
		with B[] proparrB = arrB;										\n\
		with C[] proparrC = arrC;										\n\
		with D[] proparrD = arrD;										\n\
		useThem() {														\n\
			var A varA = A; varA = propA;								\n\
			var B varB = B; varB = propB;								\n\
			var C varC = C; varC = propC;								\n\
			var D varD = D; varD = propD;								\n\
			var A? varoptA = optA; varoptA = propoptA;					\n\
			var B? varoptB = optB; varoptB = propoptB;					\n\
			var C? varoptC = optC; varoptC = propoptC;					\n\
			var D? varoptD = optD; varoptD = propoptD;					\n\
			var A[] vararrA = arrA; vararrA = proparrA;					\n\
			var B[] vararrB = arrB; vararrB = proparrB;					\n\
			var C[] vararrC = arrC; vararrC = proparrC;					\n\
			var D[] vararrD = arrD; vararrD = proparrD;					\n\
			// test the types were right								\n\
			optA = nothing; optB = nothing;								\n\
			optC = nothing; optD = nothing;								\n\
			propoptA = nothing; propoptB = nothing;						\n\
			propoptC = nothing; propoptD = nothing;						\n\
			varoptA = nothing; varoptB = nothing;						\n\
			varoptC = nothing; varoptD = nothing;						\n\
			arrA[0]; arrB[0];											\n\
			arrC[0]; arrD[0];											\n\
			proparrA[0]; proparrB[0];									\n\
			proparrC[0]; proparrD[0];									\n\
			vararrA[0]; vararrB[0];										\n\
			vararrC[0]; vararrD[0];										\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestGenericListCanBeReferencedWithoutGenericParams,
	"every MyClass is:											\n\
		with $List{Text}[] = [];								\n\
		myMethod() {											\n\
			var List{Text}[] = [];								\n\
			List[] = [];										\n\
			$List[] = [];										\n\
			this.$List[] = [];									\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestDoubleDeclarationOfClassGenericIsError,
	"every MyClass{T, T} is:",
	PTT_EXPECT(GENERIC_TYPE_COLLISION)
);
