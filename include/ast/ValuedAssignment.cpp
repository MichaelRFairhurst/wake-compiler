/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ValuedAssignment.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

namespace wake::ast {

	class ValuedAssignment : public Assignment {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
