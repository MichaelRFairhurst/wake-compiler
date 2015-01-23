/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * LambdaReturn.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_LAMBDA_RETURN
#define HEADER_AST_LAMBDA_RETURN

#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"
#include "UnifyingType.h"
#include "type.h"
#include <memory>

namespace wake {

	namespace ast {

		class LambdaReturn : public StatementNode {

			public:
				LambdaReturn(ExpressionNode* value, UnifyingType* returntype)
					: value(value), returntype(returntype) {};

				void typeCheck();

				~LambdaReturn(){};

			private:
				std::auto_ptr<ExpressionNode> value;
				UnifyingType* returntype;

		};

	}

}

#endif
