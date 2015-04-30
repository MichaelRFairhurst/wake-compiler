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
				ForeachInExplicitType(ExpressionNode* iterable, StatementNode* body, PureType* explicitType, Node* node, ScopeSymbolTable* scopesymtable, ErrorTracker* errors, TypeAnalyzer* analyzer)
					: Foreach(iterable, body, node, scopesymtable, errors), analyzer(analyzer), explicitType(explicitType) {};

				PureType getIterationType(PureType* iterableType);

				~ForeachInExplicitType(){};

			private:
				PureType* explicitType;
				TypeAnalyzer* analyzer;

		};

	}

}

#endif
