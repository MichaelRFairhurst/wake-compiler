/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Return.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_RETURN
#define HEADER_AST_RETURN

#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"
#include "ast/TypeAnalyzer.h"
#include "ast/Type.h"

namespace wake {

	namespace ast {

		class Return : public StatementNode {

			public:
				Return(ExpressionNode* value, Type* returntype, TypeAnalyzer* analyzer)
					: value(value), returntype(returntype), analyzeer(analyzer);
				typeCheck();

			private:
				ExpressionNode* value;
				Type* returntype;
				TypeAnalyzer* analyzer;

		};

	}

}

#endif