/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Assignment.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

namespace wake::ast {

	class Assignment : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
