/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Throw.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_THROW
#define HEADER_AST_THROW

#include "ast/StatementNode.h"
#include "ast/ExpressionNode.h"
#include "ErrorTracker.h"
#include "TypeAnalyzer.h"
#include <memory>

namespace wake {

	namespace ast {

		class Throw : public StatementNode {

			public:
				Throw(ExpressionNode* exception, TypeAnalyzer* analyzer, ErrorTracker* errors) : exception(exception), errors(errors), analyzer(analyzer) {};

				void typeCheck();

				bool exhaustiveReturns();

				~Throw(){};

			private:
				std::auto_ptr<ExpressionNode> exception;
				ErrorTracker* errors;
				TypeAnalyzer* analyzer;

		};

	}

}

#endif
