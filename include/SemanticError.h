/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * SemanticError.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_SEMANTIC_ERROR
#define HEADER_SEMANTIC_ERROR

#include <string>
using namespace std;

extern "C" {
	#include "type.h"
	#include "node.h"
}

enum SemanticErrorCode {
	MULTIPLE_CLASS_DEFINITION,		// defining class A twice
	SELF_INHERITANCE,				// A implements or extends A
	CIRCULAR_INHERITANCE,			// A inherits from B inherits from A
	MORE_THAN_ONE_SUBCLASS,			// A extends B and C
	MULTIPLE_INHERITANCE,			// A inherits from B twice
	MULTIPLE_METHOD_DEFINITION,		// A.methodA() defined twice
	CLASSNAME_NOT_FOUND,			// accepting, extending, providing, or instantiating an unknown type
	SYMBOL_ALREADY_DEFINED,			// declaring a variable twice
	SYMBOL_NOT_DEFINED,				// using an unknown alias, shadow, or classname with no known instance
	TYPE_ERROR,						// adding intelligence to George Bush
	PROPERTY_OR_METHOD_NOT_FOUND,	// MyClass.asntoehuaobeiuaou() is probably a typo
	ILLEGAL_CASE,					// Using 'case x:' or 'default:' outside a switch
	ILLEGAL_BREAK,					// Using break outside a loop or switch
	ILLEGAL_CONTINUE,				// Using continue outside a loop or switch
	INEXHAUSTIVE_RETURNS,			// returning conditionally in a typed function with a returnval
	WARNING,						// Plain text warning. Not sure if this is the best place for them
	MISMATCHED_INJECTION,			// provides HasThreeArgs <- HasThreeArgs(Only, Two);
	DUPLICATE_PROPERTY_DEFINITION,	// provides AClass, AClass;
	CIRCULAR_DEPENDENCIES,			// A needs B, B needs C, C needs A, shit for everyone. Or A needs A.
	INVALID_ASSIGNMENT,				// @it.doIt() = 5; is pushing it, bro
	ABSTRACT_PROVISION,				// Providing a class directly that doesn't implement all of its methods
	DIRECT_USE_OF_OPTIONAL_TYPE,	// :Printer = nothing;
	EXISTS_ON_NONOPTIONAL_TYPE,		// needs Printer; Printer exists { }
	USE_OF_ASSIGNMENT_VALUE,		// a = b = c; --- should be a = b := c;
	INVALID_GENERIC_TYPE,			// Printer{Int} or List or List{String, String} etc
	BAD_IMPORT,						// Import sanhutohesuntho;
};

class SemanticError {

	public:
		SemanticError(SemanticErrorCode errorcode);
		SemanticError(SemanticErrorCode errorcode, string errormsg);
		SemanticError(SemanticErrorCode errorcode, string errormsg, Node* errortoken);
		SemanticError(SemanticErrorCode errorcode, string errormsg, Node* errortoken, string errorcontext);
		SemanticErrorCode code;
		string msg;
		string context;
		Node* token;

};

#endif
