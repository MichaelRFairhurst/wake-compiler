/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Cast.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

namespace wake::ast {

	class Cast : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
