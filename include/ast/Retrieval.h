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
#include "ClassSpaceSymbolTable.h"
#include "TypeAnalyzer.h"
#include "ErrorTracker.h"
#include <memory>
#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>

namespace wake {

	namespace ast {

		class Retrieval : public ExpressionNode {

			public:
				Retrieval(ExpressionNode* providerExp, SpecializablePureType<wake::QUALIFIED>* retrievalType, std::vector<ExpressionNode*> argumentExprs, Node* node, ClassSpaceSymbolTable* classestable, TypeAnalyzer* analyzer, ErrorTracker* errors)
					: providerExp(providerExp), retrievalType(retrievalType), node(node), classestable(classestable), analyzer(analyzer), errors(errors) {
						for(std::vector<ExpressionNode*>::iterator it = argumentExprs.begin(); it != argumentExprs.end(); ++it) this->argumentExprs.push_back(*it);
					};

				~Retrieval(){};

				PureType<wake::QUALIFIED>* typeCheck(bool forceArrayIdentifier);

			private:
				std::auto_ptr<ExpressionNode> providerExp;
				SpecializablePureType<wake::QUALIFIED>* retrievalType;
				boost::ptr_vector<ExpressionNode> argumentExprs;
				Node* node;
				ClassSpaceSymbolTable* classestable;
				TypeAnalyzer* analyzer;
				ErrorTracker* errors;

		};

	}

}

#endif
