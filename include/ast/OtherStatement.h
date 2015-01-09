#ifndef AST_OTHER_STATEMENT
#define AST_OTHER_STATEMENT

#include <vector>
#include "ast/StatementNode.h"
#include "ErrorTracker.h"
#include "ClassSpaceSymbolTable.h"
#include "ScopeSymbolTable.h"
#include "MethodSignatureParseTreeTraverser.h"
#include "Type.h"
#include "node.h"

namespace wake {

	namespace ast {

		class OtherStatement : public StatementNode {

			public:
				OtherStatement(Node* node, std::vector<ExpressionNode*> children, ErrorTracker* errors, ClassSpaceSymbolTable* objectsymtable, ScopeSymbolTable* scopesymtable, MethodSignatureParseTreeTraverser* methodanalyzer, Type* returntype, Type* thiscontext, const vector<Type>& parameterizedtypes)
					: node(node), children(children), errors(errors), objectsymtable(objectsymtable), scopesymtable(scopesymtable), methodanalyzer(methodanalyzer), returntype(returntype), thiscontext(thiscontext), parameterizedtypes(parameterizedtypes);
				typeCheck(bool forceArrayIdentifier);

			private:
				Node* node;
				std::vector<StatementNode*> children;
				MethodSignatureParseTreeTraverser* methodanalyzer;
				ErrorTracker* errors;
				ClassSpaceSymbolTable* classestable;
				ScopeSymbolTable* scopesymtable;
				Type* returntype;
				Type* thiscontext;
				vector<Type*> parameterizedtypes;

		}

	}

}

#endif
