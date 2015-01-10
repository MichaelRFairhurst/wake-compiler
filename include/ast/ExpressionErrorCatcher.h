/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ExpressionErrorCatcher.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

namespace wake::ast {

	class ExpressionErrorCatcher : public ExpressionNode {

		public:
			ExpressionErrorCatcher(ExpressionNode* child);
			Type* typeCheck(bool forceArrayIdentifier);


		private:
			ExpressionNode* child;

	};

}
