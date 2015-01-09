#ifndef HEADER_AST_THROW
#define HEADER_AST_THROW

#include "ast/StatementNode.h"
#include "Type.h"
#include "ErrorTracker.h"
#include "TypeAnalyzer.h"

namespace wake {

	namespace ast {

		class Throw : public StatementNode {

			public:
				Throw(ExpressionNode* exception, TypeAnalyzer* analyzer, ErrorTracker* errors) : errors(errors), analyzer(analyzer);
				typeCheck();

			private:
				ErrorTracker* errors;
				TypeAnalyzer* analyzer;

		}

	}

}

#endif
