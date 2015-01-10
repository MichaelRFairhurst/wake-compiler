/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * MemberAccess.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

namespace wake::ast {

	class MemberAccess : public MemberAccessBase {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
