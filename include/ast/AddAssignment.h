/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * AddAssignment.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

namespace wake::ast {

	class AddAssignment : public Add {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}