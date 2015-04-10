/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ExpressionNode.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_EXPRESSION_NODE
#define HEADER_EXPRESSION_NODE

#include "PureType.h"

namespace wake {

	namespace ast {

		class ExpressionNode {

			public:
				virtual PureType* typeCheck(bool forceArrayIdentifier)=0;
				virtual ~ExpressionNode(){};

			private:
		};

	}

}

#endif
