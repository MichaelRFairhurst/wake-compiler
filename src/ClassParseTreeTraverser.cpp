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

void ClassParseTreeTraverser::firstPass(Node* tree) {
	switch(tree->node_type) {
		case NT_CLASSBODY:
			{
				int i = 0;
				while(i < tree->subnodes) {
					firstPass(tree->node_data.nodes[i]);
					i++;
				}
				checkCtorArgs(tree);
			}
			break;

		case NT_PROVISIONS:
		case NT_INJECTION_ARG:
			{
				int i = 0;
				while(i < tree->subnodes) {
					firstPass(tree->node_data.nodes[i]);
					i++;
				}
			}
			break;

		case NT_PROVISION:
			try {
				propertysymtable->addProvision(tree->node_data.nodes[0]->node_data.type, tree);
				objectsymtable->assertTypeIsValid(tree->node_data.nodes[0]->node_data.type);
			} catch(SemanticError* e) {
				e->token = tree;
				errors->addError(e);
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

void ClassParseTreeTraverser::secondPass(Node* tree) {
	switch(tree->node_type) {
		case NT_CLASSBODY:
			{
				/*int i = 0;
				while(i < tree->subnodes) {
					secondPass(tree->node_data.nodes[i]);
					i++;
				}*/
				scopesymtable->pushScope();
				loadCtorArgs(tree);
				loadProperties(tree);
				typechecker->setThisContext(classname);
				typeCheckMethods(tree);
				scopesymtable->popScope();
			}
			break;
	}
}


void ClassParseTreeTraverser::checkCtorArgs(Node* tree) {
	switch(tree->node_type) {
		case NT_CLASSBODY:
			{
				bool found_ctor = false;
				for(int i = 0; i < tree->subnodes; i++)
				if(tree->node_data.nodes[i]->node_type == NT_CTOR) {
					if(found_ctor) errors->addError(new SemanticError(MULTIPLE_METHOD_DEFINITION, "multiple definitions of constructor", tree));
					else checkCtorArgs(tree->node_data.nodes[i]->node_data.nodes[0]);
					found_ctor = true;
				}
			}
			break;

		case NT_CTOR_ARGS:
			try {
				for(int i = 0; i < tree->subnodes; i++) {
					objectsymtable->assertTypeIsValid(tree->node_data.nodes[i]->node_data.type);
					objectsymtable->getAnalyzer()->assertNeedIsNotCircular(classname, tree->node_data.nodes[i]->node_data.type);
					propertysymtable->addNeed(tree->node_data.nodes[i]->node_data.type);
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

void ClassParseTreeTraverser::loadCtorArgs(Node* tree) {
	switch(tree->node_type) {
		case NT_CLASSBODY:
			{
				for(int i = 0; i < tree->subnodes; i++)
				if(tree->node_data.nodes[i]->node_type == NT_CTOR) {
					loadCtorArgs(tree->node_data.nodes[i]->node_data.nodes[0]);
					return;
				}
			}
			break;

		case NT_CTOR_ARGS:
			try {
				for(int i = 0; i < tree->subnodes; i++) {
					Type* need = tree->node_data.nodes[i]->node_data.type;
					scopesymtable->add(need);
				}
			} catch(SymbolNotFoundException* e) {
				delete e;
			} catch(SemanticError* e) {
				delete e;
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
			{
				int i = 0;
				while(i < tree->subnodes) {
					typeCheckMethods(tree->node_data.nodes[i]);
					i++;
				}
			}
			break;

		case NT_PROVISION:
			if(tree->subnodes > 1) {
				Type* provision = tree->node_data.nodes[0]->node_data.type;
				Node* served = tree->node_data.nodes[1];
				switch(served->node_type) {
					case NT_BLOCK:
						errors->addError(new SemanticError(TYPE_ERROR, "Not implemented", tree));
						return;

					case NT_STRINGLIT:
						if(!objectsymtable->getAnalyzer()->isPrimitiveTypeText(provision))
							errors->addError(new SemanticError(TYPE_ERROR, "Bound a Text value to something that is not a Text", tree));
						break;
					case NT_NUMBERLIT:
						if(!objectsymtable->getAnalyzer()->isPrimitiveTypeInt(provision))
							errors->addError(new SemanticError(TYPE_ERROR, "Bound an Int value to something that is not an Int", tree));
						break;
					case NT_TYPEDATA:
						try {
							objectsymtable->assertTypeIsValid(served->node_data.type);

							if(!objectsymtable->getAnalyzer()->isASubtypeOfB(served->node_data.type, provision))
								errors->addError(new SemanticError(TYPE_ERROR, "Bound class is not a subtype of provided class", tree));
							else objectsymtable->getAnalyzer()->assertClassCanProvide(classname, served->node_data.type); // Test that this class Ctor is recursively provided
						} catch(SemanticError* e) {
							e->token = tree;
							errors->addError(e);
						}
						break;

					case NT_INJECTION:
						try {
							int i;
							string otherclassname = served->node_data.nodes[0]->node_data.string;
							vector<Type*>* needs = objectsymtable->find(otherclassname)->getNeeds();
							Node* arguments = served->node_data.nodes[1];
							if(needs->size() == arguments->subnodes)
							for(i = 0; i < arguments->subnodes; i++) {
								Type* required = needs->at(i);
								Type* actual;
								switch(arguments->node_data.nodes[i]->node_type) {
									case NT_TYPEDATA:
										{
											actual = copyType(arguments->node_data.nodes[i]->node_data.type);
											objectsymtable->assertTypeIsValid(actual);
										}
										break;
									case NT_STRINGLIT:
										actual = MakeType(TYPE_CLASS);
										actual->typedata._class.classname = "Text";
										break;
									case NT_NUMBERLIT:
										actual = MakeType(TYPE_CLASS);
										actual->typedata._class.classname = "Int";
										break;
								}

								if(required->specialty != NULL && (actual->specialty == NULL || string(required->specialty) != actual->specialty))
									errors->addError(new SemanticError(WARNING, "Injected a class with specialized dependencies...it may not be looking for what you're giving it!", tree));

								if(!objectsymtable->getAnalyzer()->isASubtypeOfB(required, actual)) {
									errors->addError(new SemanticError(TYPE_ERROR, "Injection is not a proper subtype for class dependencies", tree));
								} else if(arguments->node_data.nodes[i]->node_type == NT_TYPEDATA) {
									objectsymtable->getAnalyzer()->assertClassCanProvide(classname, actual);
								}
							} else {
								errors->addError(new SemanticError(MISMATCHED_INJECTION, "Too many or too few injectioned depndencies", tree));
							}
						} catch(SemanticError* e) {
							e->token = tree;
							errors->addError(e);
						} catch(SymbolNotFoundException* e) {
							errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
							delete e;
						}
						break;
				}
			} else {
				try {
					// if we merely 'provide SomeClass' we must check we provide all its dependencies
					objectsymtable->getAnalyzer()->assertClassCanProvide(classname, tree->node_data.nodes[0]->node_data.type);
				} catch(SemanticError *e) {
					e->token = tree;
					errors->addError(e);
				} catch(SymbolNotFoundException* e) {
					//errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
					//Already caught :)
					delete e;
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
				AddSubNode(tree, MakeNodeFromString(NT_COMPILER_HINT, strdup(name.c_str())));

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
