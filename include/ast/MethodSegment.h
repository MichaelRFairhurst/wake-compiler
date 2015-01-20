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
#include <boost/ptr_container/ptr_vector.hpp>
#include "ast/ExpressionNode.h"

namespace wake {

	namespace ast {

		class MethodSegment {

			public:
				MethodSegment(string name, std::vector<ExpressionNode*> arguments)
				: name(name) {
					for(std::vector<ExpressionNode*>::iterator it = arguments.begin(); it != arguments.end(); ++it) this->arguments.push_back(*it);
				};

				string name;
				boost::ptr_vector<ExpressionNode> arguments;
		};

	}

}

#endif
