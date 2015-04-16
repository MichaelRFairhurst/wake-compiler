/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ClassParseTreeTraverser.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ClassParseTreeTraverser.h"
#include "CompilationExceptions.h"
#include "TypeParameterizer.h"
#include "AnnotationTreeTraverser.h"
#include "Annotation.h"
#include <memory>
#include <boost/lexical_cast.hpp>

/**
 * This class makes several passes to have the proper type info at the proper times.
 * First it gathers all method signatures.
 * Then it loads the ctor args into the sym table
 * Then it loads the properties into the sym table in order of declaration, typechecking each
 * Then it type checks the methods, provisions, and ctor body
 */

ClassParseTreeTraverser::ClassParseTreeTraverser(ErrorTracker* errors, ClassSpaceSymbolTable* classestable, ScopeSymbolTable* scopesymtable, string classname, TypeChecker* typechecker, MethodSignatureParseTreeTraverser* methodanalyzer, PropertySymbolTable* propertysymtable, bool is_extern) {
	this->errors = errors;
	this->scopesymtable = scopesymtable;
	this->classestable = classestable;
	this->classname = classname;
	this->typechecker = typechecker;
	this->methodanalyzer = methodanalyzer;
	this->propertysymtable = propertysymtable;
	this->is_extern = is_extern;
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
		case NT_SUBINJECTION:
			{
				int i = 0;
				while(i < tree->subnodes) {
					firstPass(tree->node_data.nodes[i]);
					i++;
				}
			}
			break;

		case NT_ANNOTATED_PROVISION:
		case NT_PROVISION:
			try {
				vector<Annotation*> annotations;
				if(tree->node_type == NT_ANNOTATED_PROVISION) {
					AnnotationTreeTraverser traverser;
					annotations = traverser.getAnnotations(tree->node_data.nodes[1]);
					tree = tree->node_data.nodes[0];
				}
				int flags = 0;
				std::vector<PureType*> arguments;
				if(tree->subnodes > 1) {
					if(tree->node_data.nodes[1]->node_type == NT_PROVISION_BEHAVIOR) {
						flags |= PROPERTY_BLOCKPROVISION;
						Node* argnodes = tree->node_data.nodes[1]->node_data.nodes[0];
						for(int i = 0; i < argnodes->node_data.typearray->typecount; i++) {
							arguments.push_back(argnodes->node_data.typearray->types[i]);
						}
					} else if(tree->node_data.nodes[1]->node_type == NT_INJECTION) {
						Node* subinjections = tree->node_data.nodes[1]->node_data.nodes[1];
						for(int i = 0; i < subinjections->subnodes; i++) {
							if(subinjections->node_data.nodes[i]->node_type == NT_INJECTION_ARG) {
								arguments.push_back(subinjections->node_data.nodes[i]->node_data.nodes[0]->node_data.pure_type);
							}
						}
					}
				}

				if(is_extern) {
					flags |= PROPERTY_BLOCKPROVISION;
				}

				boost::optional<SemanticError*> error = propertysymtable->addProvision(tree->node_data.nodes[0]->node_data.specializable_pure_type, arguments, flags, annotations);
				addSubNode(tree, makeNodeFromString(NT_COMPILER_HINT, strdup(tree->node_data.nodes[0]->node_data.specializable_pure_type->toProvisionSymbol(arguments).c_str()), tree->loc));
				if(error) {
					(*error)->token = tree;
					errors->addError(*error);
				}
				classestable->assertTypeIsValid(&tree->node_data.nodes[0]->node_data.specializable_pure_type->typedata);
			} catch(SemanticError* e) {
				e->token = tree;
				errors->addError(e);
			}
			break;

		case NT_ANNOTATED_METHOD:
		case NT_METHOD_DECLARATION:
			try {
				vector<Annotation*> annotations;
				if(tree->node_type == NT_ANNOTATED_METHOD) {
					AnnotationTreeTraverser traverser;
					annotations = traverser.getAnnotations(tree->node_data.nodes[1]);
					tree = tree->node_data.nodes[0];
				}
				vector<PureType*> parameterizedtypes = propertysymtable->getParameters();

				Node* lastnode = tree->node_data.nodes[tree->subnodes - 1];
				vector<string> usedGenericNames;
				for(std::vector<PureType*>::iterator it = parameterizedtypes.begin(); it != parameterizedtypes.end(); ++it) {
					usedGenericNames.push_back((*it)->typedata.parameterized.label);
				}

				if(lastnode->node_type == NT_TYPE_ARRAY)
				for(int i = 0; i < lastnode->node_data.typearray->typecount; i++)
				if(std::find(usedGenericNames.begin(), usedGenericNames.end(), lastnode->node_data.typearray->types[i]->typedata.parameterized.label) == usedGenericNames.end()) {
					lastnode->node_data.pure_type_array->types[i]->type = TYPE_PARAMETERIZED_ARG;
					parameterizedtypes.push_back(lastnode->node_data.typearray->types[i]);
					usedGenericNames.push_back(lastnode->node_data.typearray->types[i]->typedata.parameterized.label);
				} else {
					errors->addError(new SemanticError(GENERIC_TYPE_COLLISION, string("Generic type with label ") + lastnode->node_data.typearray->types[i]->typedata.parameterized.label + " is declared more than once", tree));
				}

				methodanalyzer->convertParameterizedTypes(tree, parameterizedtypes);
				vector<pair<string, PureTypeArray*> >* methodname = methodanalyzer->getName(tree);

				int methodFlags = methodanalyzer->getFlags(tree);
				if(is_extern) {
					methodFlags = methodFlags & ~ PROPERTY_ABSTRACT; // remove abstract flag on extern classes
				}
				boost::optional<SemanticError*> error = propertysymtable->addMethod(methodanalyzer->getReturn(tree), methodname, methodFlags, annotations);
				if(error) {
					(*error)->token = tree;
					errors->addError(*error);
				}

				delete methodname;
			} catch(SemanticError* e) {
				e->token = tree;
				errors->addError(e);
			}
			break;

		case NT_ANNOTATED_PROPERTY:
		case NT_PROPERTY:
			try {
				vector<Annotation*> annotations;
				if(tree->node_type == NT_ANNOTATED_PROPERTY) {
					AnnotationTreeTraverser traverser;
					annotations = traverser.getAnnotations(tree->node_data.nodes[1]);
					tree = tree->node_data.nodes[0];
				}
				TypeParameterizer parameterizer;
				throw "fixme";
				//parameterizer.rewriteClasstypesToParameterizedtypeByLabel(tree->node_data.nodes[0]->node_data.nodes[0]->node_data.var_decl->typedata, propertysymtable->getParameters());
				VarDecl decl;
				throw "fixme";
				//Type prop = *tree->node_data.nodes[0]->node_data.nodes[0]->node_data.type;
				classestable->assertTypeIsValid(&decl.typedata);
				boost::optional<SemanticError*> error = propertysymtable->addProperty(&decl, tree->subnodes == 2 ? PROPERTY_PUBLIC : 0, annotations);
				if(error) {
					(*error)->token = tree;
					errors->addError(*error);
				}
			} catch(SemanticError* e) {
				e->token = tree;
				errors->addError(e);
			}
			break;

	}
}

void ClassParseTreeTraverser::secondPass(Node* tree) {
	switch(tree->node_type) {
		case NT_CLASSBODY:
			{
				scopesymtable->pushScope();
				for(vector<SpecializableVarDecl*>::iterator it = propertysymtable->getNeeds()->begin(); it != propertysymtable->getNeeds()->end(); ++it) {
					try {
						scopesymtable->add(&(*it)->decl);
					} catch(SemanticError* e) {
						errors->addError(e);
					}
				}
				typechecker->setParameterizedTypes(propertysymtable->getParameters());
				typeCheckProperties(tree);
				scopesymtable->popScope();
				scopesymtable->pushScope();
				//auto_ptr<Type> thiscontext(propertysymtable->getAsType());
				typechecker->setThisContext(propertysymtable->getAsPureType());
				typeCheckMethods(tree);
				//typechecker->setThisContext(NULL);

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
			{
				TypeParameterizer parameterizer;
				for(int i = 0; i < tree->subnodes; i++) {
					try {
						vector<Annotation*> annotations;
						Node* needNode = tree->node_data.nodes[i];
						Node* typeNode;

						if(needNode->node_data.nodes[0]->node_type == NT_ANNOTATED_TYPE) {
							typeNode = needNode->node_data.nodes[0]->node_data.nodes[0];
							AnnotationTreeTraverser annotationTraverser;
							annotations = annotationTraverser.getAnnotations(needNode->node_data.nodes[1]);
						} else {
							typeNode = needNode->node_data.nodes[0];
						}

						SpecializableVarDecl needDecl = *typeNode->node_data.specializable_var_decl;
						throw "fixme";
						//parameterizer.rewriteClasstypesToParameterizedtypeByLabel(&needDecl.decl.typedata, propertysymtable->getParameters());
						PureType* needtype = &needDecl.decl.typedata;
						classestable->assertTypeIsValid(needtype);
						//classestable->getAnalyzer()->assertNeedIsNotCircular(classname, needtype); DISABLED because we can't tell without importing everything
						propertysymtable->addNeed(&needDecl, needNode->node_data.nodes[1]->node_type == NT_PUBLIC ? PROPERTY_PUBLIC : 0, annotations);
					} catch(SymbolNotFoundException* e) {
						errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
						delete e;
					} catch(SemanticError* e) {
						e->token = tree;
						errors->addError(e);
					}
				}
			}
	}
}

void ClassParseTreeTraverser::typeCheckProperties(Node* tree) {
	switch(tree->node_type) {
		case NT_CLASSBODY:
			{
				int i = tree->subnodes;
				while(i > 0) {
					i--;
					typeCheckProperties(tree->node_data.nodes[i]);
				}
			}
			break;

		case NT_ANNOTATED_PROPERTY:
			typeCheckProperties(tree->node_data.nodes[0]);
			break;

		case NT_PROPERTY:
			scopesymtable->pushScope();
			try {
				if(tree->node_data.nodes[0]->node_type == NT_ASSIGNMENT) {
					scopesymtable->add(tree->node_data.nodes[0]->node_data.nodes[0]->node_data.var_decl);
					typechecker->setReturnType(NULL);
					typechecker->check(tree->node_data.nodes[0]);
				} else {
					classestable->assertTypeIsValid(&tree->node_data.nodes[0]->node_data.var_decl->typedata);
					scopesymtable->add(tree->node_data.nodes[0]->node_data.var_decl);
				}
			} catch(SymbolNotFoundException* e) {
				errors->addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
				delete e;
			} catch(SemanticError* e) {
				e->token = tree;
				errors->addError(e);
			}
			scopesymtable->popScope();
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

		case NT_ANNOTATED_PROVISION:
			typeCheckMethods(tree->node_data.nodes[0]);
			break;

		case NT_PROVISION:
			if(tree->subnodes > 2) { // this time we use > 2 because we have the hint as the second node
				if(is_extern) {
					errors->addError(new SemanticError(EXTERN_METHOD_BODY, "Provisions cannot be defined in extern classes, since the implementation is not generated by the wake compiler.", tree));
					break;
				}
				SpecializablePureType* provision = tree->node_data.nodes[0]->node_data.specializable_pure_type;
				Node* served = tree->node_data.nodes[1];
				switch(served->node_type) {
					case NT_PROVISION_BEHAVIOR:
						{
							// @TODO get arguments for symbol name
							vector<VarDecl*> arguments;
							vector<PureType*> argumentsPure;

							Node* argnodes = served->node_data.nodes[0];
							for(int i = 0; i < argnodes->subnodes; i++) {
								arguments.push_back(argnodes->node_data.nodes[i]->node_data.var_decl);
								argumentsPure.push_back(&argnodes->node_data.nodes[i]->node_data.var_decl->typedata);
							}

							errors->pushContext("In declaration of 'every " + classname + "' provision " + provision->toProvisionSymbol(argumentsPure));

							// Begin Method Scope For Type Analysis
							scopesymtable->pushScope();

							for(vector<VarDecl*>::iterator it = arguments.begin(); it != arguments.end(); ++it) {
								scopesymtable->add(*it);
							}

							Node* block = served->node_data.nodes[served->subnodes == 1 ? 0 : 1];

							try {
								// Run Type Analysis
								typechecker->setReturnType(&provision->typedata);
								typechecker->check(block);
							} catch(SemanticError* e) {
								e->token = tree;
								errors->addError(e);
							}

							// Pop Method Scope
							scopesymtable->popScope();
							errors->popContext();
						}
						break;

					case NT_STRINGLIT:
						if(!classestable->getAnalyzer()->isPrimitiveTypeText(&provision->typedata))
							errors->addError(new SemanticError(TYPE_ERROR, "Bound a Text value to something that is not a Text", tree));
						break;
					case NT_NUMBERLIT:
						if(!classestable->getAnalyzer()->isPrimitiveTypeNum(&provision->typedata))
							errors->addError(new SemanticError(TYPE_ERROR, "Bound an Num value to something that is not an Num", tree));
						break;
					case NT_TYPEDATA:
						try {
							PureType* servedType = served->node_data.pure_type;
							classestable->assertTypeIsValid(servedType);

							if(!classestable->getAnalyzer()->isASubtypeOfB(servedType, &provision->typedata)) {
								errors->addError(new SemanticError(TYPE_ERROR, "Bound class is not a subtype of provided class", tree));
							} else if(servedType->type == TYPE_CLASS) {
								classestable->setModulesOnType(servedType);
								classestable->getAnalyzer()->assertFQClassCanBeProvided(served->node_data.pure_type->getFQClassname()); // Test that this class isn't abstract
								SpecializablePureType servedTypeSp;
								servedTypeSp.typedata = *servedType;
								classestable->getAnalyzer()->assertFQClassCanProvide(classname, &servedTypeSp); // Test that this class Ctor is recursively provided
							}
						} catch(SemanticError* e) {
							e->token = tree;
							errors->addError(e);
						}
						break;

					case NT_INJECTION:
						try {
							int i;
							string otherclassname = served->node_data.nodes[0]->node_data.string;
							vector<SpecializableVarDecl*>* needs = classestable->findByImportedName(otherclassname)->getNeeds();
							Node* injections = served->node_data.nodes[1];
							if(needs->size() == injections->subnodes)
							for(i = 0; i < injections->subnodes; i++) {
								SpecializableVarDecl* required = needs->at(i);
								SpecializablePureType actual;
								switch(injections->node_data.nodes[i]->node_type) {
									case NT_SPECIALIZABLE_TYPEDATA:
										{
											actual = *injections->node_data.nodes[i]->node_data.specializable_pure_type;
											classestable->assertTypeIsValid(&actual.typedata);
										}
										break;
									case NT_STRINGLIT:
										actual.typedata = PureType(TYPE_CLASS);
										actual.typedata.typedata._class.classname = strdup("Text");
										break;
									case NT_NUMBERLIT:
										actual.typedata = PureType(TYPE_CLASS);
										actual.typedata.typedata._class.classname = strdup("Num");
										break;

									case NT_INJECTION_ARG:
										{
											actual = *injections->node_data.nodes[i]->node_data.nodes[0]->node_data.specializable_pure_type;
											classestable->assertTypeIsValid(&actual.typedata);
										}
										break;
								}

								if(required->specialty != NULL && (actual.specialty == NULL || string(required->specialty) != actual.specialty))
									errors->addError(new SemanticError(WARNING, "Injected a class with specialized dependencies...it may not be looking for what you're giving it!", tree));

								if(!classestable->getAnalyzer()->isASubtypeOfB(&actual.typedata, &required->decl.typedata)) {
									errors->addError(new SemanticError(TYPE_ERROR, "Injection is not a proper subtype for class dependencies", tree));
								} else if(injections->node_data.nodes[i]->node_type == NT_TYPEDATA) {
									classestable->getAnalyzer()->assertFQClassCanProvide(classname, &actual);
								}
							} else {
								errors->addError(new SemanticError(MISMATCHED_INJECTION, "Too many or too few injected dependencies", tree));
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
					SpecializablePureType* provided = tree->node_data.nodes[0]->node_data.specializable_pure_type;
					if(provided->typedata.type == TYPE_CLASS) {
						classestable->getAnalyzer()->assertFQClassCanBeProvided(provided->typedata.getFQClassname()); // Test that this class isn't abstract
					}
					classestable->getAnalyzer()->assertFQClassCanProvide(classname, tree->node_data.nodes[0]->node_data.specializable_pure_type);
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

		case NT_ANNOTATED_METHOD:
			typeCheckMethods(tree->node_data.nodes[0]);
			break;

		case NT_METHOD_DECLARATION:
			try {
				Node* methodbody = methodanalyzer->getBody(tree);

				vector<pair<string, PureTypeArray*> >* methodname;
				PureType* methodreturn;
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

				boost::optional<PureType*> optmethod = propertysymtable->find(name);
				if(!optmethod) return;
				PureType* method = *optmethod;

				errors->pushContext("In declaration of 'every " + classname + "' method " + name);
				addSubNode(tree, makeNodeFromString(NT_COMPILER_HINT, strdup(name.c_str()), tree->loc));

				vector<PureType*> parameterizedtypes = propertysymtable->getParameters();

				Node* lastnode = tree->node_data.nodes[tree->subnodes - 1];
				if(lastnode->node_type == NT_TYPE_ARRAY)
				for(int i = 0; i < lastnode->node_data.typearray->typecount; i++) {
					parameterizedtypes.push_back(lastnode->node_data.typearray->types[i]);
				}

				typechecker->setParameterizedTypes(parameterizedtypes);

				// Begin Method Scope For Type Analysis
				scopesymtable->pushScope();
				vector<VarDecl*> argDecls = methodanalyzer->getArgDecls(tree);
				for(std::vector<VarDecl*>::iterator it = argDecls.begin(); it != argDecls.end(); ++it) {
					scopesymtable->add(*it);
				}

				if(is_extern && methodbody->node_type != NT_ABSTRACT_METHOD) {
					errors->addError(new SemanticError(EXTERN_METHOD_BODY, "Cannot define a method on an extern class, since the wake compiler does not generate the implementation source.", tree));
				} else {

					try {
						// Run Type Analysis
						typechecker->setReturnType(methodanalyzer->getReturn(tree));
						typechecker->check(methodbody);
					} catch(SemanticError* e) {
						e->token = tree;
						errors->addError(e);
					}

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
