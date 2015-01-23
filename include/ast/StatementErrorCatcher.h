/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * StatementErrorCatcher.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_STATEMENT_ERROR_CATCHER
#define HEADER_STATEMENT_ERROR_CATCHER

#include "ast/StatementNode.h"
#include "ErrorTracker.h"
#include <memory>

namespace wake {

	namespace ast {

		class StatementErrorCatcher : public StatementNode {

			public:
				StatementErrorCatcher(StatementNode* wrapped, Node* node, ErrorTracker* errors) : wrapped(wrapped), errors(errors), node(node) {};

				void typeCheck();

				bool exhaustiveReturns();

				~StatementErrorCatcher(){};

			private:
				std::auto_ptr<StatementNode> wrapped;
				ErrorTracker* errors;
				Node* node;

		};

	}

}

#endif
