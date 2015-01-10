/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * EarlyBailoutMemberAccess.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

namespace wake::ast {

	class EarlyBailoutMemberAccess : public MemberAccessBase {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	};

}
