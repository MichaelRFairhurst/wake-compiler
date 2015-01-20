/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Ternary.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

// THIS FILE IS NOT YET USED!
// this ast node is represented in the old ast/new ast crossover
// see OtherExpression for the real code!
//
// and maybe take this as an opportunity to convert this node
// from the compatibility layer to the new typesafe OO ast

namespace wake::ast {

	class Ternary : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
