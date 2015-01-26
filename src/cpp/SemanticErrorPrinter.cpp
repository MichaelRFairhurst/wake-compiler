/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * SemanticErrorPrinter.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include <iostream>
#include "SemanticErrorPrinter.h"
#include "error.h"

void SemanticErrorPrinter::print(SemanticError* e) {

	if(e->token != NULL) error_print_error_token(&e->token->loc);
	if(e->token != NULL) cout << "[ ERROR on line " << e->token->line << ", column " << e->token->col << " ]" << endl;

	if(e->context != "") cout << "[ -CONTEXT: " << e->context << " ]" << endl;

	cout << "[ -TYPE: ";
	switch(e->code) {
		case MULTIPLE_CLASS_DEFINITION:
			cout << "Class is defined twice";
			break;
		case SELF_INHERITANCE:
			cout << "Class inherits from itself";
			break;
		case CIRCULAR_INHERITANCE:
			cout << "Circular inheritance formation";
			break;
		case MORE_THAN_ONE_SUBCLASS:
			cout << "Muliple subclassing with 'a/an' is not allowed. Perhaps use 'capable' instead?";
			break;
		case MULTIPLE_INHERITANCE:
			cout << "Class inherits twice from the same class";
			break;
		case MULTIPLE_METHOD_DEFINITION:
			cout << "Multiple methods for one class defined with the same signature";
			break;
		case CLASSNAME_NOT_FOUND:
			cout << "Class by this name not found";
			break;
		case ALIAS_REQUIRED:
			cout << "Variable cannot be referenced by type and requires an alias";
			break;
		case SYMBOL_ALREADY_DEFINED:
			cout << "Variable declaration would overwrite previous. Either use a new alias, or shadow this declaration, or shadow the previous declaration";
			break;
		case SYMBOL_NOT_DEFINED:
			cout << "Variable (by class name, shadow, or alias) is not recognized";
			break;
		case TYPE_ERROR:
			cout << "Invalid type";
			break;
		case PROPERTY_OR_METHOD_NOT_FOUND:
			cout << "Property or method on class is not valid";
			break;
		case ILLEGAL_CASE:
			cout << "Use of 'case' or 'default' outside of a switch block";
			break;
		case ILLEGAL_CONTINUE:
			cout << "Use of 'continue' outside of a loop";
			break;
		case ILLEGAL_BREAK:
			cout << "Use of 'break' outside of a loop or switch block";
			break;
		case INEXHAUSTIVE_RETURNS:
			cout << "Inexhaustive returns; not all execution paths of function guarantee a return value";
			break;
		case WARNING:
			cout << "Warning";
			break;
		case MISMATCHED_INJECTION:
			cout << "Invalid injection mapping";
			break;
		case DUPLICATE_PROPERTY_DEFINITION:
			cout << "This class already has a property, method, or provision by the same name";
			break;
		case CIRCULAR_DEPENDENCIES:
			cout << "Circular dependency formation";
			break;
		case INVALID_ASSIGNMENT:
			cout << "Invalid assignment; you can only assign to variables and variable indices";
			break;
		case ABSTRACT_PROVISION:
			cout << "Provided class does not define all of its methods or parent/interface methods";
			break;
		case DIRECT_USE_OF_OPTIONAL_TYPE:
			cout << "Direct usage of an optional type. Wrap it in a T exists { } statement";
			break;
		case EXISTS_ON_NONOPTIONAL_TYPE:
			cout << "Calling T exists { } on a nonoptional type, statement does nothing";
			break;
		case USE_OF_ASSIGNMENT_VALUE:
			cout << "Using a void method return, or the result of an assignment, as a value. (Use ':=' if you didn't intend to compare with '==')";
			break;
		case INVALID_GENERIC_TYPE:
			cout << "Using a non-generic type as a generic, a generic type as a non-generic, or wrong number of types used in generic. @TODO split these up...";
			break;
		case BAD_IMPORT:
			cout << "Import could not be found within the table directory. Has the file been compiled yet, or was the import misspelled?";
			break;
		case INVALID_CHILD_RETURN_TYPE:
			cout << "Child class must return the same type or a supertype as their parent classes";
			break;
		case OPTIONAL_USE_OF_NONOPTIONAL_TYPE:
			cout << "Using a type which is not optional as if it were optional";
			break;
		case TYPE_INFERENCE_FAILURE:
			cout << "Type inference failed";
			break;
		default:
			cout << "bad error code provided";
	}
	cout << " ]" << endl;

	if(e->msg != "") cout << "[ -MSG: " << e->msg << " ]" << endl;

	cout << endl;
}
