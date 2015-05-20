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
#include "TypeAnalyzer.h"
#include <memory>

namespace wake {

	namespace ast {

		class Return : public StatementNode {

			public:
				Return(ExpressionNode* value, PureType<wake::QUALIFIED>* returntype, TypeAnalyzer* analyzer)
					: value(value), returntype(returntype), analyzer(analyzer) {};

				void typeCheck();

				bool exhaustiveReturns();

				~Return(){};

			private:
				std::auto_ptr<ExpressionNode> value;
				PureType<wake::QUALIFIED>* returntype;
				TypeAnalyzer* analyzer;

		};

	}

}

#endif
