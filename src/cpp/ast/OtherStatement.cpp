#include "Type.h"
#include "TypeAnalyzer.h"
#include "TypeParameterizer.h"
#include "ClassSpaceSymbolTable.h"
#include "ast/StatementNode.h"
#include "ast/OtherStatement.h"

OtherStatement::OtherStatement(StatementNode** children) {
	this->children = children;
}

Type* OtherStatement:typeCheck(bool forceArrayIdentifiers) {
	TypeAnalyzer* analyzer = classestable->getAnalyzer();
	Type* ret = NULL;
	string erroneousstring;
	string expectedstring;

	// read Printer as Printer[] from ARRAY_ACCESS nodes
	// but not if there are ANY nodes before the next TYPEDATA
	if(forceArrayIdentifier && !(tree->node_type == NT_TYPEDATA || tree->node_type == NT_MEMBER_ACCESS)) {
		forceArrayIdentifier = false;
	}

	switch(tree->node_type) {
		// Ignoring these for now
		case NT_SWITCH:
		case NT_CURRIED:
		case NT_INCREMENT:
		case NT_DECREMENT:
		// These require a common-ancestor function
		case NT_TYPE_ARRAY:
		case NT_VALUES:
			throw string("Not supported yet");

		// These have tests so can't throw, but won't compile anyway as long as switch throws
		case NT_DEFAULTCASE:
		case NT_CASE:

		// these don't have types
		case NT_BLOCK:
		case NT_TRY:
			scopesymtable->pushScope();
			// FALL THROUGH!
		case NT_RETRIEVALS_STATEMENTS:
		case NT_BREAK:
		case NT_CONTINUE:
			try {
				int i = 0;
				while(i < tree->subnodes) {
					if(i > 0) delete ret;
					ret = typeCheck(tree->node_data.nodes[i], forceArrayIdentifier);
					i++;
				}
				if(tree->node_type == NT_BLOCK || tree->node_type == NT_TRY) scopesymtable->popScope();
			} catch(SemanticError *e) {
				if(tree->node_type == NT_BLOCK || tree->node_type == NT_TRY) scopesymtable->popScope();
				throw e;
			}
	}

	return ret;
}
