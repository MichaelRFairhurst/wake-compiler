/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * SemanticError.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include <stdio.h>
#include "SemanticError.h"

SemanticError::SemanticError(SemanticErrorCode errorcode) {
	code = errorcode;
}

SemanticError::SemanticError(SemanticErrorCode errorcode, string errormsg) {
	code = errorcode;
	msg = errormsg;
}

SemanticError::SemanticError(SemanticErrorCode errorcode, string errormsg, Node* errortoken) {
	code = errorcode;
	msg = errormsg;
	token = errortoken;
}

SemanticError::SemanticError(SemanticErrorCode errorcode, string errormsg, Node* errortoken, string errorcontext) {
	code = errorcode;
	msg = errormsg;
	token = errortoken;
	context = errorcontext;
}
