/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * UnsafeCast.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_UNSAFE_CAST
#define HEADER_AST_UNSAFE_CAST

#include "ast/ExpressionNode.h"
#include "node.h"
#include "ClassSpaceSymbolTable.h"
#include "ErrorTracker.h"

namespace wake {

	namespace ast {

		class UnsafeCast : public ExpressionNode {

			public:
				UnsafeCast(ExpressionNode* valueExpr, PureType<wake::QUALIFIED>* castedType, Node* node, ClassSpaceSymbolTable* classestable, ErrorTracker* errors)
				: valueExpr(valueExpr), castedType(castedType), errors(errors), node(node), classestable(classestable) {};

				PureType<wake::QUALIFIED>* typeCheck(bool forceArrayIdentifier);

			private:
				auto_ptr<ExpressionNode> valueExpr;
				PureType<wake::QUALIFIED>* castedType;
				ErrorTracker* errors;
				ClassSpaceSymbolTable* classestable;
				Node* node;

		};

	}

}

#endif
