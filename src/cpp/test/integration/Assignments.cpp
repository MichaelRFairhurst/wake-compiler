/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Assignments.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	InvalidAssignments,
	"every UnrelatedClass is:												\n\
	every ParentClass is:													\n\
	every MyClass (a ParentClass) is:										\n\
		assignBoolTo(Num) { Num = true; }									\n\
		assignTextTo(Num) { Num = 'text'; }									\n\
		assignA(MyClass)To(Num) { Num = MyClass; }							\n\
		assignBoolTo(Text) { Text = true; }								\n\
		assignNumTo(Text) { Text = 5; }										\n\
		assignA(MyClass)To(Text) { Text = MyClass; }						\n\
		assignAn(UnrelatedClass)To(MyClass) { MyClass = UnrelatedClass; }	\n\
		assignA(ParentClass)To(MyClass) { MyClass = ParentClass; }			\n\
	",
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
	ValidAssignments,
	"every ParentClass is:											\n\
	every MyClass (a ParentClass) is:								\n\
		assignBoolTo(Bool) { Bool = true; }							\n\
		assignTextTo(Text) { Text = 'text'; }						\n\
		assignNumTo(Num) { Num = 5; }								\n\
		assign(MyClass c)To(MyClass b) { c = b; }					\n\
		assignA(MyClass)To(ParentClass) { ParentClass = MyClass; }	\n\
	",
	PTT_VALID
)

PTT_TEST_CASE(
	InvalidLeftHandAssignments,
	"every MyClass is:						\n\
		provides MyClass;					\n\
		invalidAssignments() {				\n\
			this = 5;						\n\
			invalidAssignments() = 5;		\n\
			this.invalidAssignments() = 5;	\n\
			(MyClass from this) = 5;			\n\
		}",
	PTT_EXPECT(INVALID_ASSIGNMENT)
	PTT_EXPECT(INVALID_ASSIGNMENT)
	PTT_EXPECT(INVALID_ASSIGNMENT)
	PTT_EXPECT(INVALID_ASSIGNMENT)
);

PTT_TEST_CASE(
	UsingNormalAssignmentAsValueIsNotAllowed,
	"every MyClass is:												\n\
		with Num = 0;												\n\
		with Bool = false;											\n\
		with MyClass? = nothing;									\n\
		inAddition() { (Num = 3) + 2; }								\n\
		inSubtraction() { (Num = 3) - 2; }							\n\
		inDivision() { (Num = 3) / 2; }								\n\
		inMultiplication() { (Num = 3) * 2; }						\n\
		inAssignment() { Num = (Num = 3); }							\n\
		Num -- inReturn() { return Num = 3; }						\n\
		inMethodSubject() { (MyClass = this).inMethodSubject(); }	\n\
		inEquivalence() { (Num = 5) == 5; }							\n\
		inLT() { (Num = 5) <= 5; }									\n\
		inGT() { (Num = 5) >= 5; }									\n\
		inIfStmt() { if(Bool = true) {} }							\n\
		inWhileStmt() { while(Bool = true) {} }						\n\
		inMethodArg($Num) { inMethodArg(Num = 5); }					\n\
	",
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND) // the  uninferencable type
	PTT_EXPECT(USE_OF_ASSIGNMENT_VALUE)
);

PTT_TEST_CASE(
	UsingValuedAssignmentAsValueIsOK,
	"every MyClass is:															\n\
		with Num = 0;															\n\
		with Bool = false;														\n\
		inAddition() { (Num := 3) + 2; }										\n\
		inSubtraction() { (Num := 3) - 2; }										\n\
		inDivision() { (Num := 3) / 2; }										\n\
		inMultiplication() { (Num := 3) * 2; }									\n\
		inAssignment() { Num = (Num := 3); }									\n\
		Num -- inReturn() { return Num := 3; }									\n\
		inMethodSubject(MyClass) { (MyClass := this).inMethodSubject(this); }	\n\
		inMethodArg($Num) { inMethodArg(Num := 5); }							\n\
		inEquivalence() { (Num := 5) == 5; }									\n\
		inLT() { (Num := 5) <= 5; }												\n\
		inGT() { (Num := 5) >= 5; }												\n\
		inIfStmt() { if(Bool := true) {} }										\n\
		inWhileStmt() { while(Bool := true) {} }								\n\
	",
	PTT_VALID
);

