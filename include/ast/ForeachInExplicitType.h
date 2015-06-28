/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ForeachInExplicitType.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_FOREACH_IN_EXPLICIT_TYPE
#define HEADER_FOREACH_IN_EXPLICIT_TYPE

#include "ast/Foreach.h"
#include "TypeAnalyzer.h"

namespace wake {

	namespace ast {

		class ForeachInExplicitType : public Foreach {

			public:
				ForeachInExplicitType(ExpressionNode* iterable, StatementNode* body, ClassVarRef* explicitType, Node* node, ScopeSymbolTable* scopesymtable, ErrorTracker* errors, TypeAnalyzer* analyzer)
					: Foreach(iterable, body, node, scopesymtable, errors), analyzer(analyzer), explicitType(explicitType) {};

				VarDecl<wake::QUALIFIED> getIterationVarDecl(PureType<wake::QUALIFIED>* iterableType);

				~ForeachInExplicitType(){};

			private:
				ClassVarRef* explicitType;
				TypeAnalyzer* analyzer;

		};

	}

}

#endif
