#include "ClassParseTreeTraverser.h"
#include "CompilationExceptions.h"

/**
 * This class makes several passes to have the proper type info at the proper times.
 * First it gathers all method signatures.
 * Then it loads the ctor args into the sym table
 * Then it loads the properties into the sym table in order of declaration, typechecking each
 * Then it type checks the methods, provisions, and ctor body
 */

ClassParseTreeTraverser::ClassParseTreeTraverser(ErrorTracker* errors, ObjectSymbolTable* objectsymtable, ScopeSymbolTable* scopesymtable, string classname, TypeChecker* typechecker, MethodSignatureParseTreeTraverser* methodanalyzer) {
	this->errors = errors;
	this->scopesymtable = scopesymtable;
	this->objectsymtable = objectsymtable;
	this->classname = classname;
	this->typechecker = typechecker;
	this->methodanalyzer = methodanalyzer;
	propertysymtable = objectsymtable->find(classname);
}

void ClassParseTreeTraverser::traverse(Node* tree) {
	switch(tree->node_type) {
		case NT_CLASSBODY:
			{
				int i = 0;
				while(i < tree->subnodes) {
					traverse(tree->node_data.nodes[i]);
					i++;
				}
				scopesymtable->pushScope();
				loadCtorArgs(tree);
				loadProperties(tree);
				typechecker->setThisContext(classname);
				typeCheckMethods(tree);
				scopesymtable->popScope();
			}
			break;

		case NT_PROVISIONS:
		case NT_INJECTED_CTOR_ARG:
			{
				int i = 0;
				while(i < tree->subnodes) {
					traverse(tree->node_data.nodes[i]);
					i++;
				}
			}
			break;

		case NT_PROVISION:
		case NT_PROVIDING:
			try {
				traverse(tree->node_data.nodes[0]);
				if(tree->subnodes > 1) traverse(tree->node_data.nodes[1]);
			} catch(SymbolNotFoundException* e) {
				errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
				delete e;
			}
			break;

		case NT_TYPEDATA:
			{
				Type* injection = tree->node_data.type;
				if(injection->type == TYPE_CLASS) objectsymtable->find(injection->typedata._class.classname);
			}
			break;

		case NT_INJECTED_CTOR:
			try {
				objectsymtable->find(tree->node_data.nodes[0]->node_data.string);
				traverse(tree->node_data.nodes[1]);
			} catch(SymbolNotFoundException* e) {
				errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
				delete e;
			}
			break;

		case NT_METHOD_DECLARATION:
			try {
				vector<pair<string, TypeArray*> >* methodname = methodanalyzer->getName(tree);

				try {
					propertysymtable->addMethod(methodanalyzer->getReturn(tree), methodname, methodanalyzer->getBody(tree));
				} catch(SemanticError* e) {
					delete methodname;
					throw e;
				}

				delete methodname;
			} catch(SemanticError* e) {
				e->token = tree;
				errors->addError(e);
			}

	}
}

void ClassParseTreeTraverser::loadCtorArgs(Node* tree) {
	switch(tree->node_type) {
		case NT_CLASSBODY:
			{
				bool found_ctor = false;
				for(int i = 0; i < tree->subnodes; i++)
				if(tree->node_data.nodes[i]->node_type == NT_CTOR) {
					if(found_ctor) errors->addError(new SemanticError(MULTIPLE_METHOD_DEFINITION, "multiple definitions of constructor", tree));
					else loadCtorArgs(tree->node_data.nodes[i]->node_data.nodes[0]);
					found_ctor = true;
				}
			}
			break;

		case NT_CTOR_ARGS:
			try {
				for(int i = 0; i < tree->subnodes; i++) {
					objectsymtable->assertTypeIsValid(tree->node_data.nodes[i]->node_data.type);
					scopesymtable->add(tree->node_data.nodes[i]->node_data.type);
				}
			} catch(SymbolNotFoundException* e) {
				errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
				delete e;
			} catch(SemanticError* e) {
				e->token = tree;
				errors->addError(e);
			}
	}
}

void ClassParseTreeTraverser::loadProperties(Node* tree) {
	switch(tree->node_type) {
		case NT_CLASSBODY:
			{
				int i = tree->subnodes;
				while(i > 0) {
					i--;
					loadProperties(tree->node_data.nodes[i]);
				}
			}
			break;

		case NT_PROPERTY:
			try {
				if(tree->node_data.nodes[0]->node_type == NT_ASSIGNMENT) {
					scopesymtable->add(tree->node_data.nodes[0]->node_data.nodes[0]->node_data.type);
					typechecker->setReturnType(NULL);
					typechecker->check(tree->node_data.nodes[0]);
				} else {
					objectsymtable->assertTypeIsValid(tree->node_data.nodes[0]->node_data.type);
					scopesymtable->add(tree->node_data.nodes[0]->node_data.type);
				}
			} catch(SymbolNotFoundException* e) {
				errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
				delete e;
			} catch(SemanticError* e) {
				e->token = tree;
				errors->addError(e);
			}
			break;
	}
}

void ClassParseTreeTraverser::typeCheckMethods(Node* tree) {
	switch(tree->node_type) {
		case NT_CLASSBODY:
		case NT_PROVISIONS:
		case NT_INJECTED_CTOR_ARG:
			{
				int i = 0;
				while(i < tree->subnodes) {
					typeCheckMethods(tree->node_data.nodes[i]);
					i++;
				}
			}
			break;

		case NT_METHOD_DECLARATION:
			try {
				Node* methodbody = methodanalyzer->getBody(tree);
				vector<pair<string, TypeArray*> >* methodname;
				Type* methodreturn;
				string name;

				try { // If this throws we can't type check
					methodreturn = methodanalyzer->getReturn(tree);
					methodname = methodanalyzer->getName(tree);
					name = propertysymtable->getSymbolNameOf(methodname); // TODO include return type
					delete methodname;
				} catch(SemanticError* e) {
					delete e;
					return;
				}

				Type* method = propertysymtable->get(name);
				errors->pushContext("In declaration of 'every " + classname + "' method " + name);

				// Begin Method Scope For Type Analysis
				scopesymtable->pushScope();
				int i;
				for(i = 0; i < method->typedata.lambda.arguments->typecount; i++) {
					scopesymtable->add(method->typedata.lambda.arguments->types[i]);
				}

				try {
					// Run Type Analysis
					typechecker->setReturnType(methodanalyzer->getReturn(tree));
					typechecker->check(methodbody);
				} catch(SemanticError* e) {
					e->token = tree;
					errors->addError(e);
				}

				// Pop Method Scope
				scopesymtable->popScope();
				errors->popContext();

			} catch(SemanticError* e) {
				e->token = tree;
				errors->addError(e);
			}
			break;

		case NT_CTOR:
			if(tree->subnodes == 1) return;

			scopesymtable->pushScope();
			typechecker->setReturnType(NULL);
			typechecker->check(tree->node_data.nodes[1]);
			scopesymtable->popScope();
			break;
	}
}
