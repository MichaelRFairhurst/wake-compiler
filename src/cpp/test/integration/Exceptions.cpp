/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Exceptions.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

PTT_TEST_CASE(
	TestThrowNonExceptionIsTypeError,
	"every MyClass is:						\n\
		throwThis() {						\n\
			throw this;						\n\
		}									\n\
		throwNum() {						\n\
			throw 5;						\n\
		}									\n\
		throwText() {						\n\
			throw 'test';					\n\
		}									\n\
		throwBool() {						\n\
			throw true;						\n\
		}									\n\
		throwList() {						\n\
			var Exception[] = [];			\n\
			throw Exception[];				\n\
		}									\n\
		throwOptional() {					\n\
			var Exception? = nothing;		\n\
			throw Exception;				\n\
		}									\n\
		throwFunction(function fn()) {		\n\
			throw function;					\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestCatchExceptionIsNotTypeError,
	"every MyException (an Exception) is:	\n\
	every MyClass is:						\n\
		provides Exception, MyException;	\n\
		catchException() {					\n\
			try {} catch(Exception) {}		\n\
		}									\n\
		catchMyException() {				\n\
			try {} catch(MyException) {}	\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestCatchNonExceptionIsTypeError,
	"every MyClass is:							\n\
		catchThis() {							\n\
			try {} catch(MyClass) {}			\n\
		}										\n\
		catchNum() {							\n\
			try {} catch(Num) {}				\n\
		}										\n\
		catchText() {							\n\
			try {} catch(Text) {}				\n\
		}										\n\
		catchBool() {							\n\
			try {} catch(Bool) {}				\n\
		}										\n\
		catchList() {							\n\
			try {} catch(Exception[]) {}		\n\
		}										\n\
		catchFunction() {						\n\
			try {} catch(fn(Num) aliases) {}	\n\
		}										\n\
		catchOptional() {						\n\
			try {} catch(Exception?) {}			\n\
		}",
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
	PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestThrowExceptionIsNotTypeError,
	"every MyException (an Exception) is:	\n\
	every MyClass is:						\n\
		provides Exception, MyException;	\n\
		throwException() {					\n\
			throw Exception from this;		\n\
		}									\n\
		throwMyException() {				\n\
			throw MyException from this;		\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestMethodWithThrowIsExhaustiveReturns,
	"every MyClass is:						\n\
		provides Exception;					\n\
		Num -- throwInsteadOfReturn() {		\n\
			throw Exception from this;		\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	TestCatchUnknownClassIsError,
	"every MyClass is:						\n\
		catchBlah() {						\n\
			try { } catch(Blah) { }			\n\
		}",
	PTT_EXPECT(CLASSNAME_NOT_FOUND)
);

PTT_TEST_CASE(
	TestThrowErroneousExpression,
	"every MyClass is:						\n\
		throwNonexistMethodCall() {			\n\
			throw iDontExist();				\n\
		}",
	PTT_EXPECT(PROPERTY_OR_METHOD_NOT_FOUND)
	// and nothing else
);

PTT_TEST_CASE(
	TestTryBlockAndCatchBlockErrorsAreDiscovered,
	"every MyClass is:						\n\
		errorsInTry() {						\n\
			try {							\n\
				5 + true;					\n\
			}								\n\
		}									\n\
		errorsInCatch() {					\n\
			try {} catch(Exception) {		\n\
				5 + true;					\n\
			}								\n\
		}",
		PTT_EXPECT(TYPE_ERROR)
		PTT_EXPECT(TYPE_ERROR)
);

PTT_TEST_CASE(
	TestCaughtExceptionVariablesAreUsable,
	"every MyException (an Exception) is:		\n\
		needs public Exception;					\n\
	every MyClass is:							\n\
		useException() {						\n\
			try { } catch(Exception) {			\n\
				Exception.getStackTrace();		\n\
			}									\n\
		}										\n\
		useMyException() {						\n\
			try { } catch(MyException) {		\n\
				MyException.Exception;			\n\
			}									\n\
		}",
	PTT_VALID
);

PTT_TEST_CASE(
	testReturnWithinATryOrACatchIsNotExhaustive,
	"every MyClass is:						\n\
		Num -- returnInTry() {				\n\
			try {							\n\
				return 5;					\n\
			} catch(Exception) { }			\n\
		}									\n\
		Num -- returnInCatch() {			\n\
			try { } catch(Exception) {		\n\
				return 5;					\n\
			}								\n\
		}",
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
	PTT_EXPECT(INEXHAUSTIVE_RETURNS)
);

PTT_TEST_CASE(
	testReturnWithinATryAndItsCatchIsExhaustive,
	"every MyClass is:						\n\
		Num -- returnInTry() {				\n\
			try {							\n\
				return 5;					\n\
			} catch(Exception) {			\n\
				return 5;					\n\
			}								\n\
		}",
	PTT_VALID
);

