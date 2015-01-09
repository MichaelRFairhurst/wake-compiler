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
		case NT_FOR:
			{
			}
			break;

		case NT_RETURN:
			if(returntype == NULL) {
				if(tree->subnodes == 0) break;

				ret = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);
				expectedstring = "VOID";
				erroneousstring = analyzer->getNameForType(ret);
				throw string("Method is not allowed to return anything");
			}

			if(tree->subnodes == 0) {
				expectedstring = analyzer->getNameForType(returntype);
				erroneousstring = "VOID";
				throw string("Method is not allowed to return without a value");
			}

			ret = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);
			if(!analyzer->isASubtypeOfB(ret, returntype)) {
				expectedstring = analyzer->getNameForType(returntype);
				erroneousstring = analyzer->getNameForType(ret);
				throw string("Return type is incompatible with method signature");
			}
			break;

		case NT_EXISTS:
			try {
				ret = typeCheckUsable(tree->node_data.nodes[0], forceArrayIdentifier);

				if(ret->type != TYPE_OPTIONAL) {
					errors->addError(new SemanticError(EXISTS_ON_NONOPTIONAL_TYPE, "exists { } statement uses a nonoptional type", tree)); // @todo better error message!
					break;
				}

				Type real(TYPE_MATCHALL);
				real = *ret->typedata.optional.contained;
				if(ret->alias != NULL) real.alias = strdup(ret->alias);

				Type* orig;
				if(tree->node_data.nodes[0]->node_type == NT_MEMBER_ACCESS) {
					errors->addError(new SemanticError(TYPE_ERROR, "Calling exists { } on a property is illegal as it is a shared reference and therefore might be unset amid the scope", tree));
					break;
				} else {
					orig = *scopesymtable->find(ret);
					scopesymtable->addOverwriting(&real);
				}

				delete typeCheck(tree->node_data.nodes[1], forceArrayIdentifier);

				if(tree->node_data.nodes[0]->node_type != NT_MEMBER_ACCESS) {
					scopesymtable->addOverwriting(orig);
				}

				if(tree->subnodes > 2) {
					delete typeCheck(tree->node_data.nodes[2], forceArrayIdentifier);
				}
			} catch(SymbolNotFoundException* e) {
				errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
				delete e;
			}
			break;

		case NT_FOREACH:
		case NT_FOREACHIN:
			{
				Node** nodebase = tree->node_type == NT_FOREACH ? tree->node_data.nodes : tree->node_data.nodes + 1;

				ret = typeCheckUsable(nodebase[0], forceArrayIdentifier);

				if(ret->type == TYPE_OPTIONAL) {
					errors->addError(new SemanticError(DIRECT_USE_OF_OPTIONAL_TYPE, "Iterating over optional type. You must first wrap object in an exists { } clause.", tree));
					delete ret;
					ret = MakeType(TYPE_MATCHALL);
					break;
				}

				// no sense checking the contents of the loop if we're missing a variable
				if(ret->type == TYPE_MATCHALL) break;

				if(ret->type != TYPE_LIST) {
					errors->addError(new SemanticError(TYPE_ERROR, "Calling foreach over something that is not a list", tree));
				} else {
					Type lowered(*ret->typedata.list.contained);

					if(lowered.alias != NULL) free(lowered.alias);

					if(tree->node_type == NT_FOREACH) {
						lowered.alias = NULL;
					} else {
						if(tree->node_data.nodes[0]->node_type == NT_ALIAS) {
							lowered.alias = strdup(tree->node_data.nodes[0]->node_data.string);
						} else {
							Type* actual = tree->node_data.nodes[0]->node_data.type;
							lowered.alias = NULL;
							if(!analyzer->isASubtypeOfB(actual, &lowered)) {
								expectedstring = analyzer->getNameForType(&lowered);
								erroneousstring = analyzer->getNameForType(actual);
								throw string("Declaration of item within foreach does not match the item's type");
							}

							lowered = *actual; // preserve shadow and other goodies
						}
					}

					scopesymtable->pushScope();
					scopesymtable->add(&lowered);
					delete typeCheck(nodebase[1], forceArrayIdentifier);
					scopesymtable->popScope();

					AddSubNode(tree, MakeNodeFromString(NT_COMPILER_HINT, strdup(scopesymtable->getNameForType(&lowered).c_str()), tree->loc));
				}
			}
			break;

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
				if(tree->node_type == NT_BLOCK || tree->node_type == NT_CATCH || tree->node_type == NT_TRY) scopesymtable->popScope();
			} catch(SemanticError *e) {
				if(tree->node_type == NT_BLOCK || tree->node_type == NT_CATCH || tree->node_type == NT_TRY) scopesymtable->popScope();
				throw e;
			}
	}

	return ret;
}
