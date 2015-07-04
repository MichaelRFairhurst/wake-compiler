/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Char.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ast/Char.h"
#include "TypeError.h"
#include "string.h"

using namespace wake;

PureType<QUALIFIED>* ast::Char::typeCheck(bool forceArrayLiteral) {
	PureType<QUALIFIED> ret(TYPE_CLASS);
	ret.typedata._class.classname = strdup("Char");
	ret.typedata._class.modulename = strdup("lang");

	return new PureType<QUALIFIED>(ret);
}
