/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * BlockOrTry.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

namespace wake::ast {

	class BlockOrTry : public StatementNode {

		public:
			typeCheck(bool forceArrayIdentifier);

	}

}
