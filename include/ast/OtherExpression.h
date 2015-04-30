/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * OtherExpression.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_OTHER_EXPRESSION
#define HEADER_AST_OTHER_EXPRESSION

#include <vector>
#include "ast/ExpressionNode.h"
#include "ErrorTracker.h"
#include "ClassSpaceSymbolTable.h"
#include "ScopeSymbolTable.h"
#include "MethodSignatureParseTreeTraverser.h"
#include "PureType.h"
#include "node.h"
#include <memory>
#include <boost/ptr_container/ptr_vector.hpp>

namespace wake {

	namespace ast {

		class OtherExpression : public ExpressionNode {

			public:
				OtherExpression(
					Node* node,
					std::vector<ExpressionNode*> children,
					ErrorTracker* errors,
					ClassSpaceSymbolTable* classestable,
					ScopeSymbolTable* scopesymtable,
					MethodSignatureParseTreeTraverser* methodanalyzer,
					PureType* thiscontext,
					PureType* returntype,
					const vector<PureType*>& parameterizedtypes
				) : node(node),
					errors(errors),
					classestable(classestable),
					scopesymtable(scopesymtable),
					methodanalyzer(methodanalyzer),
					returntype(returntype),
					thiscontext(thiscontext),
					parameterizedtypes(parameterizedtypes
				) {
					for(std::vector<ExpressionNode*>::iterator it = children.begin(); it != children.end(); ++it) this->children.push_back(*it);
				};

				PureType* typeCheck(bool forceArrayIdentifier);

				~OtherExpression(){};

			private:
				bool isValidLValue(Node* tree);
				PureType* typeCheckMemberAccess(Node* n, PureType& subject, bool forceArrayIdentifier);

				Node* node;
				boost::ptr_vector<ExpressionNode> children;
				MethodSignatureParseTreeTraverser* methodanalyzer;
				ErrorTracker* errors;
				ClassSpaceSymbolTable* classestable;
				ScopeSymbolTable* scopesymtable;
				PureType* returntype;
				PureType* thiscontext;
				vector<PureType*> parameterizedtypes;

		};

	}

}

#endif
