#include <stdio.h>
#include "SemanticError.h"

SemanticError::SemanticError(Node* errortoken, string errormsg) {
	token = errortoken;
	char temp[30];
	sprintf(temp, "[ On line %d, column %d ]\n[ : ", errortoken->line, errortoken->col);
	msg = temp + errormsg;
	msg += " ]";
}
