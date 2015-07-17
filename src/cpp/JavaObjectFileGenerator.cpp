/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * JavaObjectFileGenerator.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst Nathan Fairhurst
 * Revised By:
 *
 **************************************************/

#include "JavaObjectFileGenerator.h"
#include <sstream>
#include <iomanip>
#include <limits>
#include "MethodSignatureParseTreeTraverser.h"
#include <boost/ptr_container/ptr_vector.hpp>
#include <memory>

using namespace wake;

void JavaObjectFileGenerator::generate(Node* tree) {
	if(forceArrayIdentifier && tree->node_type != NT_VAR_REF) {
		forceArrayIdentifier = false;
	}

	switch(tree->node_type) {
		case NT_MODULE:
			if(tree->node_data.string != string("")) {
				file << "package " << tree->node_data.string << ";\n\n";
			}
		case NT_IMPORT:
		case NT_IMPORTSET:
		case NT_IMPORTTARGET:
		case NT_IMPORTPATH:
		case NT_ABSTRACT_METHOD:
		case NT_TYPE_ARRAY:
		case NT_CURRIED:
		case NT_CASE:
		case NT_DEFAULTCASE:
		case NT_INCREMENT:
		case NT_DECREMENT:
		case NT_SWITCH:
			return;

		case NT_ANNOTATED_CLASS:
		case NT_ANNOTATED_METHOD:
		case NT_ANNOTATED_TYPE:
			generate(tree->node_data.nodes[0]);
			break;

		case NT_PROGRAM:
			{
				int i;
				for(i = 0; i < tree->subnodes; i++)
					generate(tree->node_data.nodes[i]);
			}
			break;

		case NT_RETRIEVALS_STATEMENTS:
			{
				int i;
				for(i = 0; i < tree->subnodes; i++) {
					file << "\t\t";
					generate(tree->node_data.nodes[i]);
					file << ";\n\t\t";
				}
			}
			break;

		case NT_BLOCK:
			{
				table.pushScope();
				int i;
				for(i = 0; i < tree->subnodes; i++)
					generate(tree->node_data.nodes[i]);
				table.popScope();

			}
			break;

		case NT_VALUES:
		case NT_CLASSSET:
		case NT_EXPRESSIONS:
		case NT_INHERITANCESET:
		case NT_PROVISIONS:
		case NT_CTOR:
		case NT_PARENT:
		case NT_INJECTION:
			{
				int i;
				for(i = 0; i < tree->subnodes; i++)
					generate(tree->node_data.nodes[i]);
			}
			break;

		case NT_AUTOBOX:
			file << "new " << classes->getFullyQualifiedClassname(tree->node_data.nodes[1]->node_data.string) << "(";
			generate(tree->node_data.nodes[0]);
			file << ")";
			break;

		case NT_CAST:
		case NT_UNSAFE_CAST:
			generate(tree->node_data.nodes[1]);
			break;

		case NT_CLASS:
			{
				file << "public class ";
				classname = tree->node_data.nodes[0]->node_data.pure_type->typedata._class.classname;
				file << classname;

				file << " {\n";

				vector<SpecializableVarDecl<QUALIFIED>*>* needs = classes->findByImportedName(classname)->getNeeds();
				for(vector<SpecializableVarDecl<QUALIFIED>*>::iterator it = needs->begin(); it != needs->end(); ++it) {
					VarRef ref = (*it)->decl.createVarRef();
					file << "\tpublic " << toJavaTypeInformation((*it)->decl.typedata) +  " " << toJavaIdentifier(ref) << ";\n";
				}

				file << "\tpublic ";
				file << classname << "(";
				table.pushScope();

				for(vector<SpecializableVarDecl<QUALIFIED>*>::iterator it = needs->begin(); it != needs->end(); ++it) {
					table.add(&(*it)->decl);
					if(it != needs->begin()) file << ",";

					VarRef ref = (*it)->decl.createVarRef();
					file << toJavaTypeInformation((*it)->decl.typedata) << " " << table.getAddress(&ref);
				}

				file << ") {\n";

				for(vector<SpecializableVarDecl<QUALIFIED>*>::iterator it = needs->begin(); it != needs->end(); ++it) {
					VarRef ref = (*it)->decl.createVarRef();
					file << "\t\tthis." << toJavaIdentifier(ref) << " = " << table.getAddress(&ref) << ";\n";
				}
				file << "\t}\n\n";
				table.popScope();

				generate(tree->node_data.nodes[1]);
				if(tree->subnodes > 2) generate(tree->node_data.nodes[2]);

				file << "};";
				table.popScope();
			}
			break;

		case NT_TRY:
			file << "\ttry {\n";
			generate(tree->node_data.nodes[0]);
			file << "\t}";
			if(tree->subnodes == 2) generate(tree->node_data.nodes[1]);
			else file << "\n";
			break;

		case NT_CATCH:
			table.pushScope();
			{
				// its ok that we have to manually cast, its been qualified earlier
				VarDecl<QUALIFIED>* decl = (VarDecl<QUALIFIED>*) tree->node_data.nodes[0]->node_data.var_decl;
				VarRef ref = decl->createVarRef();
				table.add(decl);
				file << " catch(" << toJavaTypeInformation(decl->typedata) << " " << table.getAddress(&ref) << ") {\n";
				// run catch statement
				generate(tree->node_data.nodes[1]);

				file << "\t}\n";
				table.popScope();
			}
			break;

		case NT_THROW:
			{
				file << "\tthrow ";
				generate(tree->node_data.nodes[0]);
				file << ";\n";
			}
			break;

		case NT_PROVISION:
			file << "this.";
			//header->addPropertyUsage(file.tellp(), tree->node_data.nodes[tree->subnodes - 1]->node_data.string);
			file << "=";
			if(tree->subnodes == 2) { // one for pure_type, on for hint
				string provisionname = tree->node_data.nodes[0]->node_data.pure_type->typedata._class.classname;
				file << "function(){return new ";
				//header->addClassUsage(file.tellp(), classes->getFullyQualifiedClassname(provisionname));
				file << "(";
				vector<SpecializableVarDecl<QUALIFIED>*>* needs = classes->findByImportedName(provisionname)->getNeeds();
				for(vector<SpecializableVarDecl<QUALIFIED>*>::iterator it = needs->begin(); it != needs->end(); ++it) {
					SpecializablePureType<QUALIFIED> spType;
					spType.specialty = (*it)->specialty == NULL ? NULL : strdup((*it)->specialty);
					spType.typedata = (*it)->decl.typedata;
					vector<PureType<QUALIFIED>*> no_args;
					if(it != needs->begin()) file << ",";
					file << "this.";
					//header->addPropertyUsage(file.tellp(), spType.toProvisionSymbol(no_args));
					file << "()";
				}
				file << ");";
			} else {
				if(tree->node_data.nodes[1]->node_type == NT_SPECIALIZABLE_TYPEDATA) {
					file << "function(){return this.";
					vector<PureType<QUALIFIED>*> no_args;
					//header->addPropertyUsage(file.tellp(), tree->node_data.nodes[1]->node_data.specializable_pure_type->toProvisionSymbol(no_args));
					file << "();";
				} else if(tree->node_data.nodes[1]->node_type == NT_INJECTION) {
					file << "function(";
					vector<PureType<QUALIFIED>*> no_args;
					Node* inj = tree->node_data.nodes[1]->node_data.nodes[1];
					table.pushScope();

					bool first = true;
					for(int i = 0; i < inj->subnodes; i++) {
						if(inj->node_data.nodes[i]->node_type == NT_INJECTION_ARG) {
							if(first) first = false;
							else file << ",";
							VarDecl<QUALIFIED> decl;
							// its ok that we have to cast, its been qualified elswhere
							decl.typedata = *(PureType<QUALIFIED>*) inj->node_data.nodes[i]->node_data.nodes[0]->node_data.pure_type;
							decl.alias = strdup(string('$', i).c_str()); // unique name for each
							table.add(&decl);
							VarRef ref = decl.createVarRef();
							file << table.getAddress(&ref);
						}
					}

					file << "){return new ";
					string provisionname = tree->node_data.nodes[0]->node_data.pure_type->typedata._class.classname;
					//header->addClassUsage(file.tellp(), classes->getFullyQualifiedClassname(provisionname));
					file << "(";
						for(int i = 0; i < inj->subnodes; i++) {
							if(i != 0) {
								file << ",";
							}

							if(inj->node_data.nodes[i]->node_type == NT_INJECTION_ARG) {
								VarRef ref(strdup(string('$', i).c_str()));
								file << table.getAddress(&ref);
							} else {
								file << "this.";
								//header->addPropertyUsage(file.tellp(), inj->node_data.nodes[i]->node_data.specializable_pure_type->toProvisionSymbol(no_args));
								file << "()";
							}
						}
					file << ");";
					table.popScope();
				} else if(tree->node_data.nodes[1]->node_type == NT_STRINGLIT
					|| tree->node_data.nodes[1]->node_type == NT_NUMBERLIT
					|| tree->node_data.nodes[1]->node_type == NT_BOOLLIT
					|| tree->node_data.nodes[1]->node_type == NT_INTEGERLIT
					|| tree->node_data.nodes[1]->node_type == NT_CHARLIT
				) {
					file << "function(){return ";
					generate(tree->node_data.nodes[1]);
				} else if(tree->node_data.nodes[1]->node_type == NT_PROVISION_BEHAVIOR) {
					file << "function(";

					table.pushScope();

					for(int i = 0; i < tree->node_data.nodes[1]->node_data.nodes[0]->subnodes; i++) {
						if(i != 0) file << ",";
						// its ok that we have to cast, its been qualified elsewhere
						table.add((VarDecl<QUALIFIED>*) tree->node_data.nodes[1]->node_data.nodes[0]->node_data.nodes[i]->node_data.var_decl);
						VarRef ref = tree->node_data.nodes[1]->node_data.nodes[0]->node_data.nodes[i]->node_data.var_decl->createVarRef();
						file << table.getAddress(&ref);
					}

					file << "){";
					generate(tree->node_data.nodes[1]->node_data.nodes[1]);
					table.popScope();
				}
			}
			file << "};";
			break;

		case NT_SUBINJECTIONS:
			{
				for(int i = 0; i < tree->subnodes; i++) {
					if(i != 0) {
						file << ",";
					}

					file << "this.";
					vector<PureType<QUALIFIED>*> no_args;
					//header->addPropertyUsage(file.tellp(), tree->node_data.nodes[i]->node_data.specializable_pure_type->toProvisionSymbol(no_args));
					file << "()";
				}
			}
			break;

		case NT_CTOR_ARGS:
			break;

		case NT_CLASSBODY:
			{
				int i;
				// Put ctor args in scope
				for(i = 0; i < tree->subnodes; i++)
				if(tree->node_data.nodes[i]->node_type == NT_CTOR)
					generate(tree->node_data.nodes[i]);

				// Generate properties
				for(i = 0; i < tree->subnodes; i++)
				if(tree->node_data.nodes[i]->node_type == NT_PROPERTY)
					generate(tree->node_data.nodes[i]);

				file << "\n";

				// Generate methods & provisions
				for(i = 0; i < tree->subnodes; i++)
				if(tree->node_data.nodes[i]->node_type == NT_METHOD_DECLARATION || tree->node_data.nodes[i]->node_type == NT_ANNOTATED_METHOD || tree->node_data.nodes[i]->node_type == NT_PROVISIONS)
					generate(tree->node_data.nodes[i]);

				// TODO generate ctor body
			}
			break;

		case NT_METHOD_DECLARATION:
			{
				MethodSignatureParseTreeTraverser traverser(classes);
				table.pushScope();
				string name;
				vector<VarDecl<QUALIFIED>*> arguments = traverser.getArgDecls(tree);
				int i;

				file << "\tpublic ";

				PureType<wake::QUALIFIED>* ret = traverser.getReturn(tree);
				if(ret == NULL) {
					file << "void";
				} else {
					file << toJavaTypeInformation(*ret);
				}

				vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >* segments = traverser.getName(tree);
				for(vector<pair<string, PureTypeArray<wake::QUALIFIED>*> >::iterator it = segments->begin(); it != segments->end(); ++it) {
					name += it->first;
					for(int i = 0; it != --segments->end() && i < it->second->typecount; ++i) {
						name += "_";
					}
				}

				file << " " << name << "(";

				for(i = 0; i < arguments.size(); i++) {
					if(i != 0) file << ",";
					table.add(arguments[i]);
					VarRef ref = arguments[i]->createVarRef();
					file << toJavaTypeInformation(arguments[i]->typedata) << " " << table.getAddress(&ref);
				}

				file << ") {\n";
				if(tree->subnodes > 0) generate(tree->node_data.nodes[0]);
				if(tree->subnodes > 1) generate(tree->node_data.nodes[1]);
				if(tree->subnodes > 2) generate(tree->node_data.nodes[2]);
				if(tree->subnodes > 3) generate(tree->node_data.nodes[3]);
				file << "\n\t}\n\n";
				table.popScope();
			}

			break;

		case NT_LAMBDA_DECLARATION:
			{
				table.pushScope();
				boost::ptr_vector<PureType<QUALIFIED> > cleanup;

				file << "(function(";
				for(int i = 0; i < tree->node_data.nodes[0]->subnodes; i++)
				if(tree->node_data.nodes[0]->node_data.nodes[i]->node_type == NT_VAR_DECL_DATA) {
					if(i != 0) file << ",";
					// its ok that we have to cast, its been qualified elsewhere
					table.add((VarDecl<QUALIFIED>*) tree->node_data.nodes[0]->node_data.nodes[i]->node_data.var_decl);
					VarRef ref = tree->node_data.nodes[0]->node_data.nodes[i]->node_data.var_decl->createVarRef();
					file << table.getAddress(&ref);
				} else {
					if(i != 0) file << ",";
					cleanup.push_back(new PureType<QUALIFIED>(TYPE_MATCHALL));
					table.add(string(tree->node_data.nodes[0]->node_data.nodes[i]->node_data.string), &cleanup[cleanup.size() - 1]);
					VarRef ref(strdup(tree->node_data.nodes[0]->node_data.nodes[i]->node_data.string));
					file << table.getAddress(&ref);
				}

				file << "){";
				generate(tree->node_data.nodes[1]);
				file << "}).bind(this)";
				table.popScope();
			}
			break;

		case NT_LAMBDA_INVOCATION:
			{
				file << "(";
				generate(tree->node_data.nodes[0]);

				file << ")(";
				if(tree->subnodes == 2)
				for(int i = 0; i < tree->node_data.nodes[1]->subnodes; i++) {
					if(i != 0) {
						file << ",";
					}
					generate(tree->node_data.nodes[1]->node_data.nodes[i]);
				}
				file << ")";
			}
			break;

		//case NT_METHOD_RETURN_TYPE:
		case NT_METHOD_NAME:
			break;

		case NT_METHOD_INVOCATION:
			{
				generate(tree->node_data.nodes[0]);
				string methodclass = tree->node_data.nodes[tree->subnodes - 2]->node_data.string;
				string name;

				int i;
				for(i = 0; i < tree->node_data.nodes[1]->subnodes; ++i) {
					name += tree->node_data.nodes[1]->node_data.nodes[i]->node_data.string;
					i += 1;
					for(int b = 0; i < tree->node_data.nodes[1]->subnodes - 1 && b < tree->node_data.nodes[1]->node_data.nodes[i]->subnodes; b++) {
						name += "_";
					}
				}

				file << "." << name << "(";

				int argnum = 0;
				for(i = 1; i < tree->node_data.nodes[1]->subnodes; i += 2) {
					for(int b = 0; b < tree->node_data.nodes[1]->node_data.nodes[i]->subnodes; b++) {
						if(argnum != 0) file << ",";
						generate(tree->node_data.nodes[1]->node_data.nodes[i]->node_data.nodes[b]);
						argnum++;
					}
				}

				file << ")";
			}
			break;

		case NT_EARLYBAILOUT_METHOD_INVOCATION:
			{
				string methodclass = tree->node_data.nodes[tree->subnodes - 2]->node_data.string;
				string name = tree->node_data.nodes[tree->subnodes - 1]->node_data.string;

				table.pushScope();
				auto_ptr<PureType<QUALIFIED> > safechecked(new PureType<QUALIFIED>(TYPE_MATCHALL));
				std::stringstream safecheckedname;
				safecheckedname << safechecked.get();
				table.add(safecheckedname.str(), safechecked.get());

				file << "(function(" << table.getAddress(safecheckedname.str()) << "){";
				file << "return " << table.getAddress(safecheckedname.str()) << "===null?null:(";
				file << table.getAddress(safecheckedname.str());
				file << ".";
				//header->addPropertyUsage(file.tellp(), name);
				file << "(";

				int argnum = 0;
				int i;
				for(i = 1; i < tree->node_data.nodes[1]->subnodes; i += 2) {
					for(int b = 0; b < tree->node_data.nodes[1]->node_data.nodes[i]->subnodes; b++) {
						if(argnum != 0) file << ",";
						generate(tree->node_data.nodes[1]->node_data.nodes[i]->node_data.nodes[b]);
						argnum++;
					}
				}

				file << "));})(";
				generate(tree->node_data.nodes[0]);
				file << ")";
				table.popScope();
			}
			break;

		case NT_EARLYBAILOUT_MEMBER_ACCESS:
			{

				table.pushScope();
				auto_ptr<PureType<QUALIFIED> > safechecked(new PureType<QUALIFIED>(TYPE_MATCHALL));
				std::stringstream safecheckedname;
				safecheckedname << safechecked.get();
				table.add(safecheckedname.str(), safechecked.get());

				file << "(function(" << table.getAddress(safecheckedname.str()) << "){";
				file << "return " << table.getAddress(safecheckedname.str()) << "===null?null:";
				file << table.getAddress(safecheckedname.str());
				file << ".";
				string name = tree->node_data.nodes[tree->subnodes - 1]->node_data.string;
				//header->addPropertyUsage(file.tellp(), name);

				file << ";})(";
				generate(tree->node_data.nodes[0]);
				file << ")";
				table.popScope();
			}
			break;

		case NT_MEMBER_ACCESS:
			{
				generate(tree->node_data.nodes[0]);
				string name = tree->node_data.nodes[tree->subnodes - 1]->node_data.string;

				file << "." << name;
			}
			break;

		case NT_RETRIEVAL:
			{
				generate(tree->node_data.nodes[2]);
				string provisionSymbol = tree->node_data.nodes[tree->subnodes - 1]->node_data.string;

				file << ".";
				//header->addPropertyUsage(file.tellp(), provisionSymbol);
				file << "(";
				for(int i = 0; i < tree->node_data.nodes[1]->subnodes; i++) {
					if(i != 0) file << ",";
					generate(tree->node_data.nodes[1]->node_data.nodes[i]);
				}
				file << ")";
			}
			break;

		case NT_PROPERTY:
			{
				file << "\tpublic " << toJavaTypeInformation(tree->node_data.nodes[0]->node_data.nodes[0]->node_data.var_decl->typedata) << " " << toJavaIdentifier(tree->node_data.nodes[0]->node_data.nodes[0]->node_data.var_decl->createVarRef());
				file << " = ";
				generate(tree->node_data.nodes[0]->node_data.nodes[1]);
				file << ";\n";
			}
			break;

		case NT_TYPEINFER_DECLARATION:
			{
				VarDecl<QUALIFIED> decl;
				decl.typedata = PureType<QUALIFIED>(TYPE_MATCHALL);
				decl.alias = strdup(tree->node_data.nodes[0]->node_data.string);
				table.add(new VarDecl<QUALIFIED>(decl));

				VarRef ref = decl.createVarRef();
				file << toJavaTypeInformation(decl.typedata) << " " << table.getAddress(&ref);
				file << " = ";
				generate(tree->node_data.nodes[1]);
			}
			break;

		case NT_DECLARATION:
			{
				// its ok that we have to cast, its been qualified elsewhere
				table.add((VarDecl<QUALIFIED>*) tree->node_data.nodes[0]->node_data.var_decl);
				VarRef ref = tree->node_data.nodes[0]->node_data.var_decl->createVarRef();
				file << toJavaTypeInformation(tree->node_data.nodes[0]->node_data.var_decl->typedata) << " " << table.getAddress(&ref) << " = ";
				generate(tree->node_data.nodes[1]);
			}
			break;

		case NT_ASSIGNMENT:
			generate(tree->node_data.nodes[0]);
			file << " = ";
			generate(tree->node_data.nodes[1]);
			break;

		case NT_VALUED_ASSIGNMENT:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " = ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_ADD_ASSIGNMENT:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " += ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_SUB_ASSIGNMENT:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " -= ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_MULT_ASSIGNMENT:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " *= ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_DIV_ASSIGNMENT:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " /= ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_ARRAY_DECLARATION:
			file << "((java.util.List) java.util.Arrays.asList(";
			if(tree->subnodes) {
				int i;
				for(i = 0; i < tree->subnodes; i++) {
					if(i != 0) file << ",";
					generate(tree->node_data.nodes[i]);
				}

			}
			file << "))";
			break;

		case NT_VAR_DECL_DATA:
			{
				VarDecl<UNQUALIFIED> decl = *tree->node_data.var_decl;
				VarRef ref = decl.createVarRef();
				file << table.getAddress(&ref);
			}
			break;

		case NT_VAR_REF:
			{
				VarRef ref = *tree->node_data.var_ref;
				if(forceArrayIdentifier && ref._class != NULL) {
					ref._class->arrayed++;
				}
				file << table.getAddress(&ref);
			}
			break;

		case NT_ALIAS:
			{
				string name = tree->node_data.string;
				file << table.getAddress(name);
			}
			break;

		case NT_SUBCLASS:
			{
			//header->addClassUsage(file.tellp(), classes->getFullyQualifiedClassname(tree->node_data.nodes[0]->node_data.pure_type->typedata._class.classname));
			file << ".call(this";
			PropertySymbolTable* proptable = classes->findByImportedNameModifiable(tree->node_data.nodes[0]->node_data.pure_type->typedata._class.classname);
			for(vector<SpecializableVarDecl<QUALIFIED>*>::iterator it = proptable->getNeeds()->begin(); it != proptable->getNeeds()->end(); ++it) {
				file << ",";
				VarRef ref = (*it)->decl.createVarRef();
				file << table.getAddress(&ref);
			}
			file << ");";
			}
			break;

		case NT_ARRAY_ACCESS_LVAL:
		case NT_TYPESAFE_ARRAY_ACCESS:
		case NT_ARRAY_ACCESS:
			file << "(";
			forceArrayIdentifier = true;
			generate(tree->node_data.nodes[0]);
			forceArrayIdentifier = false;
			file << ")[";
			generate(tree->node_data.nodes[1]);
			file << "]";
			break;

		case NT_EXISTS:
			file << "\tif (";
			generate(tree->node_data.nodes[0]);
			file << " != null) {\n";
			generate(tree->node_data.nodes[1]);
			if(tree->subnodes > 2) {
				file << "} else {\n";
				generate(tree->node_data.nodes[2]);
			}
			file << "}\n";
			break;


		case NT_IF_ELSE:
			file << "if (";
			generate(tree->node_data.nodes[0]);
			file << ") {\n";
			generate(tree->node_data.nodes[1]);
			if(tree->subnodes > 2) {
				file << "} else {\n";
				generate(tree->node_data.nodes[2]);
			}
			file << "}\n";
			break;

		case NT_WHILE:
			file << "while(";
			generate(tree->node_data.nodes[0]);
			file << ") {\n";
			generate(tree->node_data.nodes[1]);
			file << "}\n";
			break;

		case NT_FOR:
			file << "for (";
			table.pushScope();
			generate(tree->node_data.nodes[0]);
			file << "; ";
			generate(tree->node_data.nodes[1]);
			file << "; ";
			generate(tree->node_data.nodes[2]);
			file << ") {\n";
			generate(tree->node_data.nodes[3]);
			table.popScope();
			file << "}\n";
			break;

		case NT_FOREACH:
		case NT_FOREACHIN:
		case NT_FOREACHAT:
		case NT_FOREACHINAT:
			{
				table.pushScope();
				Node* iterationValue;
				Node* iterationVar;
				Node* iterationIndexVar = NULL;
				Node* iterationBody;
				if(tree->node_type == NT_FOREACHIN) {
					iterationValue = tree->node_data.nodes[1];
					iterationBody = tree->node_data.nodes[2];
					iterationVar = tree->node_data.nodes[3];
				} else if(tree->node_type == NT_FOREACHINAT) {
					iterationValue = tree->node_data.nodes[1];
					iterationBody = tree->node_data.nodes[3];
					iterationVar = tree->node_data.nodes[4];
					iterationIndexVar = tree->node_data.nodes[5];
				} else if(tree->node_type == NT_FOREACHAT) {
					iterationValue = tree->node_data.nodes[0];
					iterationBody = tree->node_data.nodes[2];
					iterationVar = tree->node_data.nodes[3];
					iterationIndexVar = tree->node_data.nodes[4];
				} else {
					iterationValue = tree->node_data.nodes[0];
					iterationBody = tree->node_data.nodes[1];
					iterationVar = tree->node_data.nodes[2];
				}

				bool iterating_expression = iterationValue->node_type != NT_ALIAS && iterationValue->node_type != NT_VAR_REF;
				std::stringstream valuestorename;
				auto_ptr<PureType<QUALIFIED> > latch;
				if(iterating_expression) {
					latch.reset(new PureType<QUALIFIED>(TYPE_MATCHALL));
					valuestorename << latch.get();
					table.add(valuestorename.str(), latch.get());
					file << "var " << table.getAddress(valuestorename.str()) << "=";
					generate(iterationValue);
					file << ";";
				}

				auto_ptr<PureType<QUALIFIED> > indexer(new PureType<QUALIFIED>(TYPE_MATCHALL));
				auto_ptr<PureType<QUALIFIED> > lowered(new PureType<QUALIFIED>(TYPE_MATCHALL));
				std::stringstream indexername;
				if(iterationIndexVar == NULL) {
					indexername << indexer.get();
				} else {
					indexername << iterationIndexVar->node_data.string;
				}
				table.add(indexername.str(), indexer.get());
				table.add(iterationVar->node_data.string, lowered.get());
				file << "for(var " << table.getAddress(indexername.str()) << "=0;";
				file << table.getAddress(indexername.str()) << " < ";
				if(iterating_expression) file << table.getAddress(valuestorename.str());
				else generate(iterationValue);
				file << ".length; " << table.getAddress(indexername.str()) << "++){";
				file << "var " << table.getAddress(iterationVar->node_data.string) << "=";
				if(iterating_expression) file << table.getAddress(valuestorename.str());
				else generate(iterationValue);
				file << "[" << table.getAddress(indexername.str()) << "];";
				generate(iterationBody);
				file << "}";
				table.popScope();
			}
			break;

		case NT_BREAK: file << "break;"; break;
		case NT_CONTINUE: file << "continue;"; break;
		case NT_THIS: file << "this"; break;

		case NT_IF_THEN_ELSE:
			file << "((";
			generate(tree->node_data.nodes[1]);
			file << ") ? (";
			generate(tree->node_data.nodes[0]);
			file << ") : (";
			generate(tree->node_data.nodes[2]);
			file << "))";
			break;

		case NT_RETURN:
			file << "return";
			if(tree->subnodes) {
				file << " ";
				 generate(tree->node_data.nodes[0]);
			}
			//file << ";";
			break;

		case NT_INVERT:
			file << "(!";
			generate(tree->node_data.nodes[0]);
			file << ")";
			break;

		case NT_MULTIPLY:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " * ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_DIVIDE:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " / ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_MOD:
			file << "$B(";
			generate(tree->node_data.nodes[0]);
			file << " , ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_MODNATIVE:
		case NT_MODALT:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " % ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_ADD:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " + ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_SUBTRACT:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " - ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_BITSHIFTLEFT:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " << ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_BITSHIFTRIGHT:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " >> ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_LESSTHAN:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " < ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_GREATERTHAN:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " > ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_LESSTHANEQUAL:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " <= ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_GREATERTHANEQUAL:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " >= ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_EQUALITY:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " == ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_INEQUALITY:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " != ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_BITNOT:
			file << "(~";
			generate(tree->node_data.nodes[0]);
			file << ")";
			break;

		case NT_BITAND:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " & ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_BITXOR:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " ^ ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_BITOR:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " | ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_AND:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " && ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_OR:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << " || ";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_INTEGERLIT:
			file << tree->node_data.integer;
			break;

		case NT_NUMBERLIT:
			file << std::setprecision (std::numeric_limits<double>::digits10 + 2) << tree->node_data.number;
			break;

		case NT_BOOLLIT: file << (tree->node_data.number ? "true" : "false"); break;
		case NT_STRINGLIT:
			file << '"';
			for(int i = 0; i < strlen(tree->node_data.string); i++) {
				switch(tree->node_data.string[i]) {
					case '\n': file << "\\n"; break;
					case '"': file << "\\\""; break;
					default:
					file << tree->node_data.string[i];
				}
			}
			file << '"';
			break;
		case NT_CHARLIT:
			file << '\'';
			for(int i = 0; i < strlen(tree->node_data.string); i++) {
				switch(tree->node_data.string[i]) {
					case '\n': file << "\\n"; break;
					case '\'': file << "\\\'"; break;
					default:
					file << tree->node_data.string[i];
				}
			}
			file << '\'';
			break;

		case NT_NOTHING: file << "null"; break;

		case NT_REQUIRED_PARENS:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << ")";
			break;
	}
}

string JavaObjectFileGenerator::toJavaTypeInformation(PureType<wake::UNQUALIFIED> type) {
	return toJavaTypeInformation(*reinterpret_cast<PureType<QUALIFIED>*>(&type));
}

string JavaObjectFileGenerator::toJavaTypeInformation(PureType<wake::QUALIFIED> type, bool forceBoxedTypes) {
	switch(type.type) {
		case TYPE_CLASS:
			if(typeanalyzer.isPrimitiveTypeInt(&type)) return forceBoxedTypes ? "Integer" : "int";
			if(typeanalyzer.isPrimitiveTypeNum(&type)) return forceBoxedTypes ? "Double" : "double";
			if(typeanalyzer.isPrimitiveTypeChar(&type)) return forceBoxedTypes ? "Character" : "char";
			if(typeanalyzer.isPrimitiveTypeBool(&type)) return forceBoxedTypes ? "Boolean" : "boolean";
			if(typeanalyzer.isPrimitiveTypeText(&type)) return "String";
			// List and Exception are both happy without fq
			if(type.typedata._class.modulename == NULL || type.typedata._class.modulename == "lang") return type.typedata._class.classname;
			return type.typedata._class.modulename + string(".") + type.typedata._class.classname;

		case TYPE_OPTIONAL:
			return toJavaTypeInformation(*type.typedata.optional.contained, true);

		case TYPE_LIST:
			return "java.util.List<" + toJavaTypeInformation(*type.typedata.list.contained, true) + ">";
	}

	return "TEMP";
}

string JavaObjectFileGenerator::toJavaIdentifier(const VarRef& ref) {
	if(ref.alias != NULL) {
		return ref.alias;
	}

	string ident;
	for(int i = 0; i < ref._class->shadow; i++) {
		ident += '$';
	}

	ident += ref._class->classname;

	for(int i = 0; i < ref._class->arrayed; i++) {
		ident += "_List";
	}

	return ident;
}
