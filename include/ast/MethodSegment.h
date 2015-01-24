/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * MethodSegment.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_METHOD_SEGMENT
#define HEADER_AST_METHOD_SEGMENT

#include <string>
#include <vector>
#include "ast/ExpressionNode.h"
#include "ast/ExpectedTypeExpression.h"

namespace wake {

	namespace ast {

		class MethodSegment {

			public:
				MethodSegment(std::string name, std::vector<std::pair<ExpressionNode*, ExpectedTypeExpression*> > arguments)
				: name(name), arguments(arguments) {};

				~MethodSegment();

				std::string name;
				std::vector<std::pair<ExpressionNode*, ExpectedTypeExpression*> > arguments;
		};

	}

}

#endif
