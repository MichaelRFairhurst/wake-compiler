#ifndef HEADER_AST_CREATOR
#define HEADER_AST_CREATOR

#include "ErrorTracker.h"
#include "ClassSpaceSymbolTable.h"
#include "ScopeSymbolTable.h"
#include "MethodSignatureParseTreeTraverser.h"
#include "Type.h"
#include "node.h"

namespace wake {

	class AstCreator {

		public:
			AstCreator(ErrorTracker* errors, ClassSpaceSymbolTable* objectsymtable, ScopeSymbolTable* scopesymtable, MethodSignatureParseTreeTraverser* methodanalyzer, Type* returntype, Type* thiscontext, const vector<Type>& parameterizedtypes)
				: errors(errors), objectsymtable(objectsymtable), scopesymtable(scopesymtable), methodanalyzer(methodanalyzer), returntype(returntype), thiscontext(thiscontext), parameterizedtypes(parameterizedtypes);

			StatementNode* generateStatementAst(Node* node);
			ExpressionNode* generateExpressionAst(Node* node, bool mustBeUsable);

		private:
			MethodSignatureParseTreeTraverser* methodanalyzer;
			ErrorTracker* errors;
			ClassSpaceSymbolTable* classestable;
			ScopeSymbolTable* scopesymtable;
			Type* returntype;
			Type* thiscontext;
			vector<Type*> parameterizedtypes;

	}

}

#endif
