/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Retrieval.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_RETRIEVAL
#define HEADER_AST_RETRIEVAL

#include "ast/ExpressionNode.h"
#include <vector>
#include "ClassSpaceSymbolTable.h"
#include "TypeAnalyzer.h"
#include "ErrorTracker.h"

namespace wake {

	namespace ast {

		class Retrieval : public ExpressionNode {

			public:
				Retrieval(ExpressionNode* providerExp, Type* retrievalType, std::vector<ExpressionNode*> argumentExprs, Node* node, ClassSpaceSymbolTable* classestable, TypeAnalyzer* analyzer, ErrorTracker* errors)
					: providerExp(providerExp), retrievalType(retrievalType), argumentExprs(argumentExprs), node(node), classestable(classestable), analyzer(analyzer), errors(errors) {};

				Type* typeCheck(bool forceArrayIdentifier);

			private:
				ExpressionNode* providerExp;
				Type* retrievalType;
				std::vector<ExpressionNode*> argumentExprs;
				Node* node;
				ClassSpaceSymbolTable* classestable;
				TypeAnalyzer* analyzer;
				ErrorTracker* errors;

		};

	}

}

#endif
