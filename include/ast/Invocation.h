/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Invocation.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_INVOCATION
#define HEADER_AST_INVOCATION

#include "ExpressionNode.h"
#include "TypeAnalyzer.h"
#include "node.h"
#include "ErrorTracker.h"
#include <boost/ptr_container/ptr_vector.hpp>

namespace wake {

	namespace ast {

		class Invocation : public ExpressionNode {

			public:
				Invocation(ExpressionNode* lambdaExpr, std::vector<ExpressionNode*> argumentExprs, Node* node, TypeAnalyzer* analyzer, ErrorTracker* errors)
					: lambdaExpr(lambdaExpr), node(node), analyzer(analyzer), errors(errors) {
						for(std::vector<ExpressionNode*>::iterator it = argumentExprs.begin(); it != argumentExprs.end(); ++it) this->argumentExprs.push_back(*it);
				};

				PureType<wake::QUALIFIED>* typeCheck(bool forceArrayIdentifier);


			private:
				std::auto_ptr<ExpressionNode> lambdaExpr;
				boost::ptr_vector<ExpressionNode> argumentExprs;
				Node* node;
				TypeAnalyzer* analyzer;
				ErrorTracker* errors;

		};

	}

}

#endif
