/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ArrayDeclaration.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_ARRAY_DECLARATION
#define HEADER_AST_ARRAY_DECLARATION

#include "ast/ExpressionNode.h"

namespace wake {

	namespace ast {

		class ArrayDeclaration : public ExpressionNode {

			public:
				Type* typeCheck(bool forceArrayIdentifier);

		};

	}

}

#endif
