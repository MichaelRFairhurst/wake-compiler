/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * MemberInvocation.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_AST_METHOD_INVOCATION
#define HEADER_AST_METHOD_INVOCATION

#include "ast/MethodInvocationBase.h"
#include "ScopeSymbolTable.h"

namespace wake {

	class AstCreator;

	namespace ast {

		class MethodInvocation : public MethodInvocationBase {

			public:
				MethodInvocation(ExpressionNode* subjectExpr, std::vector<MethodSegment*> methodSegments, Node* node, AstCreator* astCreator, ScopeSymbolTable* scopesymtable, ClassSpaceSymbolTable* classestable, ErrorTracker* errors)
				: subjectExpr(subjectExpr), astCreator(astCreator), scopesymtable(scopesymtable), MethodInvocationBase(methodSegments, node, classestable, errors) {};

				Type* typeCheck(bool forceArrayIdentifier);

			private:
				auto_ptr<ExpressionNode> subjectExpr;
				AstCreator* astCreator;
				ScopeSymbolTable* scopesymtable;

		};

	}

}

#endif
