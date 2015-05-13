/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ExpectedTypeExpression.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_EXPECTED_TYPE_EXPRESSION
#define HEADER_EXPECTED_TYPE_EXPRESSION

#include "ExpressionNode.h"

namespace wake {

	namespace ast {

		class ExpectedTypeExpression : public ExpressionNode {

			public:
				virtual PureType<wake::QUALIFIED>* typeCheckExpecting(PureType<wake::QUALIFIED>* hint)=0;
				virtual PureType<wake::QUALIFIED>* typeCheck(bool forceArrayIdentifier)=0;
				virtual ~ExpectedTypeExpression(){};

		};

	}

}

#endif
