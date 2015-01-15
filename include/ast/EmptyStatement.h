/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * EmptyStatement.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_EMPTY_STATEMENT
#define HEADER_AST_EMPTY_STATEMENT

#include "ast/StatementNode.h"

namespace wake {

	namespace ast {

		class EmptyStatement : public StatementNode {

			void typeCheck() {};

		};

	}

}

#endif
