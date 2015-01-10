/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ArrayAccessLval.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

namespace wake::ast {

	class ArrayAccessLval : public ArrayAccess {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
