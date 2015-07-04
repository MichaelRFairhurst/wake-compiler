/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * TypeAnalyzer.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "TypeAnalyzer.h"
#include "ClassSpaceSymbolTable.h"
#include "ReadOnlyPropertySymbolTable.h"
#include "CompilationExceptions.h"
#include <memory>

bool TypeAnalyzer::isASubtypeOfB(string a, string b) {
	try {
		if(a == b) return true;

		ReadOnlyPropertySymbolTable* a_data = reference->findFullyQualified(a);

		for(map<string, bool>::const_iterator it = a_data->getParentage().begin(); it != a_data->getParentage().end(); ++it) {
			PropertySymbolTable* a_child_data = reference->findFullyQualifiedModifiable(it->first);
			if(isASubtypeOfB(a_child_data->getAsPureType()->getFQClassname(), b)) return true;
		}

	} catch(SymbolNotFoundException* e) {
		delete e;
	}

	return false;
}

bool TypeAnalyzer::isASubtypeOfB(PureType<wake::QUALIFIED>* a, PureType<wake::QUALIFIED>* b) {
	//if(a == NULL || b == NULL) return false;
	if(a->type == TYPE_MATCHALL || b->type == TYPE_MATCHALL) return true;
	if(a->type != b->type) {
		if(b->type == TYPE_OPTIONAL) {
			return isASubtypeOfB(a, b->typedata.optional.contained);
		}
		 // TODO: parameterized types need logic here
		return false;
	}

	if(a->type == TYPE_LAMBDA) {

		// if one or the other is a null pointer, or has no args, and the other one has arguments.
		if((a->typedata.lambda.arguments == NULL || !a->typedata.lambda.arguments->typecount) != (b->typedata.lambda.arguments == NULL || !b->typedata.lambda.arguments->typecount))
			return false;

		// Bool -- fn() is a subtype of void -- fn(), since the subtype will simply ignore the returnval
		// however, void --fn() is not a subtype of Bool -- fn() as you probably guessed
		if(a->typedata.lambda.returntype == NULL && b->typedata.lambda.returntype != NULL)
			return false;
		else if(b->typedata.lambda.returntype != NULL && !isASubtypeOfB(a->typedata.lambda.returntype, b->typedata.lambda.returntype)) // ChildClass -- fn() is a subtype of ParentClass -- fn()
			return false;

		if(a->typedata.lambda.arguments != NULL && a->typedata.lambda.arguments->typecount) {
			// A fn taking 3 arguments is not a subtype of a fn taking 2 or 4
			if(a->typedata.lambda.arguments->typecount != b->typedata.lambda.arguments->typecount)
				return false;

			int i;
			for(i = 0; i < a->typedata.lambda.arguments->typecount; i++)
			if(!isASubtypeOfB(b->typedata.lambda.arguments->types[i], a->typedata.lambda.arguments->types[i])) // fn(ParentClass) is a subtype of fn(ChildClass), but not vice versa! (contravariance)
				return false;
		}

		return true;

	} else if(a->type == TYPE_CLASS) {
		// check if one pointer exists and the other is null: !ptr == 0 and !NULL == 1
		if(!a->typedata._class.parameters != !b->typedata._class.parameters) {
			return false;
		}

		if(a->typedata._class.parameters) { // Here if A is not null, neither is B
			int len = a->typedata._class.parameters->typecount;
			if(b->typedata._class.parameters->typecount != len) {
				return false;
			}

			for(int i = 0; i < len; i++)
			if(!isAExactlyB(a->typedata._class.parameters->types[i], b->typedata._class.parameters->types[i]))
				return false;
		}

		if(a->getFQClassname() == b->getFQClassname()) {
			return true;
		}

		try {
			if(isASubtypeOfB(a->getFQClassname(), b->getFQClassname())) return true;
		} catch(SymbolNotFoundException* e) {
			delete e;
		}

		return false;
	} else if(a->type == TYPE_PARAMETERIZED || a->type == TYPE_PARAMETERIZED_ARG) {
		if(a->typedata.parameterized.label == string(b->typedata.parameterized.label)) return true;
		// TODO: lower/upper bounds comparison
		return false;
	} else if(a->type == TYPE_LIST) {
		return isAExactlyB(a->typedata.list.contained, b->typedata.list.contained);
	} else if(a->type == TYPE_OPTIONAL) {
		return isASubtypeOfB(a->typedata.optional.contained, b->typedata.optional.contained);
	}

}

bool TypeAnalyzer::isAExactlyB(PureType<wake::QUALIFIED>* a, PureType<wake::QUALIFIED>* b) {
	if(a->type == TYPE_MATCHALL || b->type == TYPE_MATCHALL) return true;
	if(a->type != b->type) return false;
	if(a->type == TYPE_CLASS) {
		if(a->typedata._class.classname != string(b->typedata._class.classname)) return false;
		if(!a->typedata._class.parameters != !b->typedata._class.parameters) return false; // tests both or neither are null. Tests back this up.
		if(a->typedata._class.parameters != NULL) {
			if(a->typedata._class.parameters->typecount != b->typedata._class.parameters->typecount)
				return false;

			for(int i = 0; i < a->typedata._class.parameters->typecount; i++)
			if(!isAExactlyB(a->typedata._class.parameters->types[i], b->typedata._class.parameters->types[i]))
				return false;
		}
	} else if(a->type == TYPE_PARAMETERIZED || a->type == TYPE_PARAMETERIZED_ARG) {
		// no need to check lower/upper bound, should always be the same for all labels
		return a->typedata.parameterized.label == string(b->typedata.parameterized.label);
	} else if(a->type == TYPE_LAMBDA) {
		if(!a->typedata.lambda.returntype != !b->typedata.lambda.returntype) return false; // tests both or neither are null. Tests back this up
		if(!a->typedata.lambda.arguments != !b->typedata.lambda.arguments) return false; // tests both or neither are null. Tests back this up
		if(a->typedata.lambda.returntype != NULL && !isAExactlyB(a->typedata.lambda.returntype, b->typedata.lambda.returntype)) return false;
		if(a->typedata.lambda.arguments != NULL) {
			if(a->typedata.lambda.arguments->typecount != b->typedata.lambda.arguments->typecount)
				return false;

			for(int i = 0; i < a->typedata.lambda.arguments->typecount; i++)
			if(!isAExactlyB(a->typedata.lambda.arguments->types[i], b->typedata.lambda.arguments->types[i]))
				return false;
		}
	} else if(a->type == TYPE_LIST) {
		return isAExactlyB(a->typedata.list.contained, b->typedata.list.contained);
	} else if(a->type == TYPE_OPTIONAL) {
		return isAExactlyB(a->typedata.optional.contained, b->typedata.optional.contained);
	}
	return true;
}

void TypeAnalyzer::assertFQNeedIsNotCircular(string classname, string otherclassname) {
	if(otherclassname == classname)
		throw new SemanticError(CIRCULAR_DEPENDENCIES, "Created by the need for class " + otherclassname);

	vector<SpecializableVarDecl<wake::QUALIFIED>*>* recurse = reference->findFullyQualified(otherclassname)->getNeeds();
	for(vector<SpecializableVarDecl<wake::QUALIFIED>*>::iterator it = recurse->begin(); it != recurse->end(); ++it)
	try {
		if((*it)->decl.typedata.type == TYPE_CLASS) {
			string fqname = (*it)->decl.typedata.getFQClassname();
			assertFQNeedIsNotCircular(classname, fqname);
		}
	} catch(SemanticError* e) {
		e->msg += ", as required by " + otherclassname;
		throw e;
	}
}

void TypeAnalyzer::assertFQClassCanBeProvided(string fqclassname) {
	ReadOnlyPropertySymbolTable* bound = reference->findFullyQualified(fqclassname);
	if(bound->isAbstract())
		throw new SemanticError(ABSTRACT_PROVISION);
}

void TypeAnalyzer::assertFQClassCanProvide(string provider, SpecializablePureType<wake::QUALIFIED>* binding) {
	string symname = binding->typedata.toString() + "<-";
	if(binding->specialty != NULL) symname += binding->specialty;
	if(!reference->findFullyQualified(provider)->find(symname))
		throw new SemanticError(PROPERTY_OR_METHOD_NOT_FOUND, symname + " not found on class " + provider);

	if(reference->findFullyQualifiedModifiable(provider)->isBehavioralProvision(symname)) {
		return;
	}

	if(binding->typedata.type != TYPE_CLASS) {
		return;
	}

	string fqname = binding->typedata.typedata._class.modulename == NULL ? "" : binding->typedata.typedata._class.modulename;
	if(fqname.size()) {
		fqname += ".";
	}
	fqname += binding->typedata.typedata._class.classname;

	vector<SpecializableVarDecl<wake::QUALIFIED>*>* recurse = reference->findFullyQualified(fqname)->getNeeds();
	for(vector<SpecializableVarDecl<wake::QUALIFIED>*>::iterator it = recurse->begin(); it != recurse->end(); ++it)
	try {
		SpecializablePureType<wake::QUALIFIED> recursable;
		recursable.specialty = (*it)->specialty == NULL ? NULL : strdup((*it)->specialty);
		recursable.typedata = (*it)->decl.typedata;
		assertFQClassCanProvide(provider, &recursable);
	} catch(SemanticError* e) {
		e->msg += ", when trying to provide " + symname;
		throw e;
	}
}

boost::optional<PureType<wake::QUALIFIED>*> TypeAnalyzer::getCommonSubtypeOf(PureType<wake::QUALIFIED>* a, PureType<wake::QUALIFIED>* b) {
	if(a->type == TYPE_UNUSABLE || b->type == TYPE_UNUSABLE) return boost::optional<PureType<wake::QUALIFIED>*>(); // this maybe should return TYPE_UNUSABLE...but unusable types aren't necessarily similar at all
	if(a->type == TYPE_MATCHALL) return boost::optional<PureType<wake::QUALIFIED>*>(new PureType<wake::QUALIFIED>(*b));
	if(b->type == TYPE_MATCHALL) return boost::optional<PureType<wake::QUALIFIED>*>(new PureType<wake::QUALIFIED>(*a));

	if(a->type == TYPE_OPTIONAL || b->type == TYPE_OPTIONAL) {
		PureType<wake::QUALIFIED>* optional = a->type == TYPE_OPTIONAL ? a : b;
		PureType<wake::QUALIFIED>* other = a == optional ? b : a;
		if(other->type == TYPE_OPTIONAL) other = other->typedata.optional.contained;

		// [nothing, Text?] is common to type Text?
		// [X, Y?] is common to type Z? where Z is the common type to [X, Y]
		boost::optional<PureType<wake::QUALIFIED>*> nonoptcommon = getCommonSubtypeOf(optional->typedata.optional.contained, other);

		if(!nonoptcommon) {
			return nonoptcommon;
		}

		PureType<wake::QUALIFIED>* common = new PureType<wake::QUALIFIED>(TYPE_OPTIONAL);

		// [Text?, Text] becomes Text?
		common->typedata.optional.contained = *nonoptcommon;
		return boost::optional<PureType<wake::QUALIFIED>*>(common);
	}

	if(a->type != b->type) return boost::optional<PureType<wake::QUALIFIED>*>();

	if(a->type == TYPE_LIST) {
		// [Text[], Num[]] and [Printer[], DisabledPrinter[]] are both common to nothing
		if(!isAExactlyB(a, b)) return boost::optional<PureType<wake::QUALIFIED>*>();
		// but [Text[], Text[]] is common to Text[]
		return boost::optional<PureType<wake::QUALIFIED>*>(new PureType<wake::QUALIFIED>(*a));

	} else if(a->type == TYPE_CLASS) {
		// check if one pointer exists and the other is null: !ptr == 0 and !NULL == 1
		if(!a->typedata._class.parameters != !b->typedata._class.parameters) {
			return boost::optional<PureType<wake::QUALIFIED>*>();
		}

		if(a->typedata._class.parameters) { // Here if A is not null, neither is B
			int len = a->typedata._class.parameters->typecount;
			if(b->typedata._class.parameters->typecount != len) {
				return boost::optional<PureType<wake::QUALIFIED>*>();
			}

			for(int i = 0; i < len; i++)
			if(!isAExactlyB(a->typedata._class.parameters->types[i], b->typedata._class.parameters->types[i]))
				return boost::optional<PureType<wake::QUALIFIED>*>();
		}

		if(isASubtypeOfB(a, b)) return boost::optional<PureType<wake::QUALIFIED>*>(new PureType<wake::QUALIFIED>(*b));

		if(isASubtypeOfB(b, a)) return boost::optional<PureType<wake::QUALIFIED>*>(new PureType<wake::QUALIFIED>(*a));

		boost::optional<pair<int, string> > common = getCommonFQClassnamesWithDepth(a->getFQClassname(), b->getFQClassname(), 0);
		if(!common || common->second == "") {
			return boost::optional<PureType<wake::QUALIFIED>*>();
		} else {
			PureType<wake::QUALIFIED>* classType = new PureType<wake::QUALIFIED>(TYPE_CLASS);
			classType->typedata._class.classname = strdup(common->second.c_str());
			return boost::optional<PureType<wake::QUALIFIED>*>(classType);
		}
	} else if(a->type == TYPE_PARAMETERIZED || a->type == TYPE_PARAMETERIZED_ARG) {
		if(a->typedata.parameterized.label == string(b->typedata.parameterized.label)) return boost::optional<PureType<wake::QUALIFIED>*>(new PureType<wake::QUALIFIED>(*a));
		// TODO: lower/upper bounds comparison
		return boost::optional<PureType<wake::QUALIFIED>*>();
	}

	if(a->type == TYPE_LAMBDA) {

		// if one or the other is a null, and the other one has arguments
		if((a->typedata.lambda.arguments == NULL || !a->typedata.lambda.arguments->typecount) != (b->typedata.lambda.arguments == NULL || !b->typedata.lambda.arguments->typecount))
			return boost::optional<PureType<wake::QUALIFIED>*>();

		if(a->typedata.lambda.arguments != NULL && a->typedata.lambda.arguments->typecount) {
			// A fn taking 3 arguments is not a subtype of a fn taking 2 or 4
			if(a->typedata.lambda.arguments->typecount != b->typedata.lambda.arguments->typecount)
				return boost::optional<PureType<wake::QUALIFIED>*>();

			int i;
			for(i = 0; i < a->typedata.lambda.arguments->typecount; i++)
			if(!isAExactlyB(a->typedata.lambda.arguments->types[i], b->typedata.lambda.arguments->types[i]))
				return boost::optional<PureType<wake::QUALIFIED>*>();
		}

		// Bool -- fn() is a subtype of void -- fn(), since the subtype will simply ignore the returnval
		// however, void --fn() is not a subtype of Bool -- fn() as you probably guessed
		if(a->typedata.lambda.returntype == NULL || b->typedata.lambda.returntype == NULL) {
			PureType<wake::QUALIFIED>* newlambda = new PureType<wake::QUALIFIED>(*a);

			if(newlambda->typedata.lambda.returntype) {
				delete newlambda->typedata.lambda.returntype;
			}

			newlambda->typedata.lambda.returntype = NULL;
			return boost::optional<PureType<wake::QUALIFIED>*>(newlambda);
		}


		boost::optional<PureType<wake::QUALIFIED>*> commonReturn = getCommonSubtypeOf(a->typedata.lambda.returntype, b->typedata.lambda.returntype);
		if(!commonReturn) {
			PureType<wake::QUALIFIED>* newlambda = new PureType<wake::QUALIFIED>(*a);
			delete newlambda->typedata.lambda.returntype;
			newlambda->typedata.lambda.returntype = NULL;
			return boost::optional<PureType<wake::QUALIFIED>*>(newlambda);
		}

		PureType<wake::QUALIFIED>* newlambda = new PureType<wake::QUALIFIED>(*a);
		delete newlambda->typedata.lambda.returntype;
		newlambda->typedata.lambda.returntype = *commonReturn;;
		return boost::optional<PureType<wake::QUALIFIED>*>(newlambda);

	}
}

boost::optional<pair<int, string> > TypeAnalyzer::getCommonFQClassnamesWithDepth(string a, string b, int depth) {
	try {
		ReadOnlyPropertySymbolTable* a_data = reference->findFullyQualified(a);

		boost::optional<boost::optional<pair<int, string> > > contender;
		bool duplicated = false;
		for(map<string, bool>::const_iterator it = a_data->getParentage().begin(); it != a_data->getParentage().end(); ++it) {
			string lowered = it->first;
			if(lowered == b) {
				if(contender && (*contender)->first == depth) {
					//cout << "Checking " << a.typedata._class.classname << "/" << b.typedata._class.classname << ", " << depth;
					//cout << " is duped!" << endl;
					return boost::optional<pair<int, string> >(pair<int, string>(depth, ""));
				}

				boost::optional<pair<int, string> > found(pair<int, string>(depth, lowered));
				contender = boost::optional<boost::optional<pair<int, string> > >(found);
				duplicated = false;
			} else {
				boost::optional<pair<int, string> > recurse = getCommonFQClassnamesWithDepth(b, lowered, depth + 1);

				if(recurse) {
					if(contender && (*contender)->first == recurse->first) {
						if((*contender)->second != "" && recurse->second != "" && (*contender)->second != recurse->second) {
							duplicated = true;
						}
					} else {
						if(contender && (*contender)->first > recurse->first) {
						} else if(!contender || recurse->second != "") {
							contender = boost::optional<boost::optional<pair<int, string> > >(recurse);
						}
						if(recurse->second != "") duplicated = false;
					}
				}

				recurse = getCommonFQClassnamesWithDepth(lowered, b, depth + 1);

				if(recurse) {
					if(contender && (*contender)->first == recurse->first) {
						if((*contender)->second != "" && recurse->second != "" && (*contender)->second != string(recurse->second)) {
							duplicated = true;
						}
					} else {
						if(contender && (*contender)->first > recurse->first) {
						} else if(!contender || recurse->second != "") {
							contender = boost::optional<boost::optional<pair<int, string> > >(recurse);
						}
						if(recurse->second != "") duplicated = false;
					}
				}
			}
		}
		//cout << "Checking " << a.typedata._class.classname << "/" << b.typedata._class.classname << ", " << depth;

		if(duplicated) {
			//cout << " has dupe parents!" << endl;
			return boost::optional<pair<int, string> >(pair<int, string>(depth, ""));
		}

		if(!contender) {
			//cout << " has no child matches!" << endl;
			return boost::optional<pair<int, string> >();
		}
		//cout << " has match " << (*contender)->first << ", " << ((*contender)->second ? (*contender)->second->typedata._class.classname : "NULL") << "!" << endl;
		return *contender;
	} catch(SymbolNotFoundException* e) {
		delete e;
	}
	return boost::optional<pair<int, string> >();
}

bool TypeAnalyzer::isPrimitiveTypeNum(PureType<wake::QUALIFIED>* type) {
	if(type->type == TYPE_MATCHALL) return true;
	return type->type == TYPE_CLASS && type->typedata._class.classname == string("Num") && type->typedata._class.modulename == string("lang");
}

bool TypeAnalyzer::isPrimitiveTypeText(PureType<wake::QUALIFIED>* type) {
	if(type->type == TYPE_MATCHALL) return true;
	return type->type == TYPE_CLASS && type->typedata._class.classname == string("Text") && type->typedata._class.modulename == string("lang");
}

bool TypeAnalyzer::isPrimitiveTypeBool(PureType<wake::QUALIFIED>* type) {
	if(type->type == TYPE_MATCHALL) return true;
	return type->type == TYPE_CLASS && type->typedata._class.classname == string("Bool") && type->typedata._class.modulename == string("lang");
}

bool TypeAnalyzer::isPrimitiveTypeChar(PureType<wake::QUALIFIED>* type) {
	if(type->type == TYPE_MATCHALL) return true;
	return type->type == TYPE_CLASS && type->typedata._class.classname == string("Char") && type->typedata._class.modulename == string("lang");
}

bool TypeAnalyzer::isAutoboxedType(PureType<wake::QUALIFIED>* type, PureType<wake::QUALIFIED>** boxed) {
	if(type->type == TYPE_MATCHALL) return false;

	// shouldn't be here...throw exception?
	if(type->type == TYPE_UNUSABLE) return false;

	// need to return a List<T>
	if(type->type == TYPE_LIST) {
		*boxed = new PureType<wake::QUALIFIED>(TYPE_CLASS);
		PureType<wake::QUALIFIED>* loweredtype = new PureType<wake::QUALIFIED>(*type->typedata.list.contained);
		(*boxed)->typedata._class.modulename = strdup("lang");
		(*boxed)->typedata._class.classname = strdup("List");
		(*boxed)->typedata._class.parameters = new PureTypeArray<wake::QUALIFIED>();

		// the template is just for static checks, we can safely cast these
		addPureTypeToPureTypeArray(
			(PureType<wake::UNQUALIFIED>*) loweredtype,
			(PureTypeArray<wake::UNQUALIFIED>*) (*boxed)->typedata._class.parameters
		);
		return true;
	}

	// need to return a generic autoboxed type according to arguments/return
	if(type->type == TYPE_LAMBDA) {
		*boxed = new PureType<wake::QUALIFIED>(TYPE_CLASS);
		(*boxed)->typedata._class.classname = strdup("lambda");
		return true;
	}

	// shouldn't ever get here,..unless I decide to make, say, T?.exists() and/or T?.applyIfExists(fn(T))
	if(type->type == TYPE_OPTIONAL) return false;

	if(isPrimitiveTypeBool(type)) {
		*boxed = new PureType<wake::QUALIFIED>(TYPE_CLASS);
		(*boxed)->typedata._class.modulename = strdup("lang");
		(*boxed)->typedata._class.classname = strdup("Bool");
		return true;
	}

	if(isPrimitiveTypeText(type)) {
		*boxed = new PureType<wake::QUALIFIED>(TYPE_CLASS);
		(*boxed)->typedata._class.modulename = strdup("lang");
		(*boxed)->typedata._class.classname = strdup("Text");
		return true;
	}

	if(isPrimitiveTypeNum(type)) {
		*boxed = new PureType<wake::QUALIFIED>(TYPE_CLASS);
		(*boxed)->typedata._class.modulename = strdup("lang");
		(*boxed)->typedata._class.classname = strdup("Num");
		return true;
	}

	if(isPrimitiveTypeChar(type)) {
		*boxed = new PureType<wake::QUALIFIED>(TYPE_CLASS);
		(*boxed)->typedata._class.modulename = strdup("lang");
		(*boxed)->typedata._class.classname = strdup("Char");
		return true;
	}

	return false;
}

bool TypeAnalyzer::isException(PureType<wake::QUALIFIED>* type) {
	PureType<wake::QUALIFIED> exception(TYPE_CLASS);
	exception.typedata._class.modulename = strdup("lang");
	exception.typedata._class.classname = strdup("Exception");
	bool ret = isASubtypeOfB(type, &exception);
	return ret;
}

int TypeAnalyzer::getArrayReferenceLevel(PureType<wake::QUALIFIED>& type) {
	if(type.type == TYPE_LIST) return 1 + getArrayReferenceLevel(*type.typedata.list.contained);
	if(type.type == TYPE_OPTIONAL) return getArrayReferenceLevel(*type.typedata.optional.contained);
	return 0;
}

bool TypeAnalyzer::hasArgParameterization(PureType<wake::QUALIFIED>* type) {
	if(type == NULL) return false;
	switch(type->type) {
		case TYPE_OPTIONAL:
			return hasArgParameterization(type->typedata.optional.contained);

		case TYPE_LIST:
			return hasArgParameterization(type->typedata.list.contained);

		case TYPE_PARAMETERIZED_ARG:
			return true;

		case TYPE_CLASS:
			return hasArgParameterization(type->typedata._class.parameters);

		case TYPE_LAMBDA:
			if(hasArgParameterization(type->typedata.lambda.arguments)) {
				return true;
			}
			return type->typedata.lambda.returntype != NULL && hasArgParameterization(type->typedata.lambda.returntype);

		default:
			return false;
	}
}

bool TypeAnalyzer::hasArgParameterization(PureTypeArray<wake::QUALIFIED>* typearray) {
	if(typearray == NULL) {
		return false;
	}

	for(int i = 0; i < typearray->typecount; i++)
	if(hasArgParameterization(typearray->types[i])) {
		return true;
	}

	return false;
}
