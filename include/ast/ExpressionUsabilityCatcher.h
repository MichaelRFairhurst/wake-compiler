/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ExpressionUsabilityCatcher.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_EXPRESSION_USABLE_CATCHER
#define HEADER_AST_EXPRESSION_USABLE_CATCHER

#include "ast/ExpressionNode.h"
#include "ErrorTracker.h"
#include <memory>

namespace wake {

	namespace ast {

		class ExpressionUsabilityCatcher : public ExpressionNode {

			public:
				ExpressionUsabilityCatcher(ExpressionNode* child, ErrorTracker* errors, Node* node) : child(child), errors(errors), node(node) {};

				PureType* typeCheck(bool forceArrayIdentifier);

				ExpressionUsabilityCatcher(){};

			private:
				Node* node;
				std::auto_ptr<ExpressionNode> child;
				ErrorTracker* errors;

		};

	}

}

#endif
