/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * MultDivSubAssignment.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

namespace wake::ast {

	class MultDivSubAssignment : public MultDivSub {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
