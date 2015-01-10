/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * EarlyBailoutMethodInvecation.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

namespace wake::ast {

	class EarlyBailoutMethodInvocation : public MethodInvocationBase {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	};

}
