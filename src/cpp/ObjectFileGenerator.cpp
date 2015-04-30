/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ObjectFileGenerator.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By: Nathan Fairhurst
 *
 **************************************************/

#include "ObjectFileGenerator.h"
#include <sstream>
#include <iomanip>
#include <limits>
#include "MethodSignatureParseTreeTraverser.h"
#include <boost/ptr_container/ptr_vector.hpp>
#include <memory>

void ObjectFileGenerator::generate(Node* tree) {
	if(forceArrayIdentifier && tree->node_type != NT_TYPEDATA) {
		forceArrayIdentifier = false;
	}

	switch(tree->node_type) {
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
					generate(tree->node_data.nodes[i]);
					file << ";";
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
			file << "new ";
			header->addClassUsage(file.tellp(), classes->getFullyQualifiedClassname(tree->node_data.nodes[1]->node_data.string));
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << ")";
			break;

		case NT_CAST:
			generate(tree->node_data.nodes[1]);
			break;

		case NT_CLASS:
			{
				file << "function ";
				table.pushScope();
				classname = tree->node_data.nodes[0]->node_data.type->typedata._class.classname;
				header->addClassUsage(file.tellp(), classes->getFullyQualifiedClassname(classname));
				file << "(";

				vector<SpecializableVarDecl*>* needs = classes->findByImportedName(classname)->getNeeds();
				for(vector<SpecializableVarDecl*>::iterator it = needs->begin(); it != needs->end(); ++it) {
					table.add(&(*it)->decl);
					if(it != needs->begin()) file << ",";
					VarRef ref = (*it)->decl.createVarRef();
					file << table.getAddress(&ref);
				}

				file << "){";

				for(vector<SpecializableVarDecl*>::iterator it = needs->begin(); it != needs->end(); ++it) {
					file << "this.";
					VarRef ref = (*it)->decl.createVarRef();
					header->addPropertyUsage(file.tellp(), ref.toString());
					file << "=" << table.getAddress(&ref) << ";";
				}

				generate(tree->node_data.nodes[1]);
				if(tree->subnodes > 2) generate(tree->node_data.nodes[2]);

				// Generate the exception runtime class checking
				// @Todo not generate this when not an exception
				// and either don't allow this to be overridden or don't declare it
				// when its defined
				file << "this.";
				header->addPropertyUsage(file.tellp(), "isExceptionType(Text)");
				file << "=function(a){return [";
				const map<string, bool> parentage = classes->findByImportedName(classname)->getParentage();
				// @Todo only print exception classes
				for(map<string, bool>::const_iterator it = parentage.begin(); it != parentage.end(); ++it) {
					file << "'";
					header->addClassUsage(file.tellp(), classes->getFullyQualifiedClassname(it->first));
					file << "',";
				}
				file << "'";
				header->addClassUsage(file.tellp(), classes->getFullyQualifiedClassname(classname));
				file << "'].indexOf(a)!==-1;};";

				// End exception class type checking
				file << "};";
				table.popScope();
			}
			break;

		case NT_TRY:
			file << "try{";
			generate(tree->node_data.nodes[0]);
			file << "}";
			if(tree->subnodes == 2) generate(tree->node_data.nodes[1]);
			break;

		case NT_CATCH:
			table.pushScope();
			{
				VarDecl* decl = tree->node_data.nodes[0]->node_data.var_decl;
				VarRef ref = decl->createVarRef();
				table.add(decl);
				file << "catch(" << table.getAddress(&ref) << "){";

				// wrap in a wake exception if its not one
				file << "if(!" << table.getAddress(&ref) << ".";
				header->addPropertyUsage(file.tellp(), "isExceptionType(Text)");
				file << ")" << table.getAddress(&ref) << "=new ";
				header->addClassUsage(file.tellp(), "std.Exception");
				file << "(" << table.getAddress(&ref) << ");";

				// check its type
				file << "if(" << table.getAddress(&ref) << ".";
				header->addPropertyUsage(file.tellp(), "isExceptionType(Text)");
				file << "('";
				header->addClassUsage(file.tellp(), classes->getFullyQualifiedClassname(tree->node_data.nodes[0]->node_data.type->typedata._class.classname));
				file << "')){";

				// run catch statement
				generate(tree->node_data.nodes[1]);

				// or rethrow it
				file << "}else{throw " << table.getAddress(&ref) << ";}}";
				table.popScope();
			}
			break;

		case NT_THROW:
			{
				table.pushScope();
				auto_ptr<PureType> exception(new PureType(TYPE_MATCHALL));
				std::stringstream exceptionname;
				exceptionname << exception.get();
				table.add(exceptionname.str(), exception.get());
				file << "var " << table.getAddress(exceptionname.str()) << "=";
				generate(tree->node_data.nodes[0]);
				file << ";" << table.getAddress(exceptionname.str()) << ".";
				header->addPropertyUsage(file.tellp(), "preThrow()");
				file << "();";
				file << "throw " << table.getAddress(exceptionname.str());
				table.popScope();
			}
			break;

		case NT_PROVISION:
			file << "this.";
			header->addPropertyUsage(file.tellp(), tree->node_data.nodes[tree->subnodes - 1]->node_data.string);
			file << "=";
			if(tree->subnodes == 2) { // one for type, on for hint
				string provisionname = tree->node_data.nodes[0]->node_data.type->typedata._class.classname;
				file << "function(){return new ";
				header->addClassUsage(file.tellp(), classes->getFullyQualifiedClassname(provisionname));
				file << "(";
				vector<SpecializableVarDecl*>* needs = classes->findByImportedName(provisionname)->getNeeds();
				for(vector<SpecializableVarDecl*>::iterator it = needs->begin(); it != needs->end(); ++it) {
					SpecializablePureType spType;
					spType.specialty = strdup((*it)->specialty);
					spType.typedata = (*it)->decl.typedata;
					vector<PureType*> no_args;
					if(it != needs->begin()) file << ",";
					file << "this.";
					header->addPropertyUsage(file.tellp(), spType.toProvisionSymbol(no_args));
					file << "()";
				}
				file << ");";
			} else {
				if(tree->node_data.nodes[1]->node_type == NT_SPECIALIZABLE_TYPEDATA) {
					file << "function(){return this.";
					vector<PureType*> no_args;
					header->addPropertyUsage(file.tellp(), tree->node_data.nodes[1]->node_data.specializable_pure_type->toProvisionSymbol(no_args));
					file << "();";
				} else if(tree->node_data.nodes[1]->node_type == NT_INJECTION) {
					file << "function(";
					vector<PureType*> no_args;
					Node* inj = tree->node_data.nodes[1]->node_data.nodes[1];
					table.pushScope();

					bool first = true;
					for(int i = 0; i < inj->subnodes; i++) {
						if(inj->node_data.nodes[i]->node_type == NT_INJECTION_ARG) {
							if(first) first = false;
							else file << ",";
							VarDecl decl;
							decl.typedata = *inj->node_data.nodes[i]->node_data.nodes[0]->node_data.type;
							decl.alias = strdup(string('$', i).c_str()); // unique name for each
							table.add(&decl);
							VarRef ref = decl.createVarRef();
							file << table.getAddress(&ref);
						}
					}

					file << "){return new ";
					string provisionname = tree->node_data.nodes[0]->node_data.type->typedata._class.classname;
					header->addClassUsage(file.tellp(), classes->getFullyQualifiedClassname(provisionname));
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
								header->addPropertyUsage(file.tellp(), inj->node_data.nodes[i]->node_data.specializable_pure_type->toProvisionSymbol(no_args));
								file << "()";
							}
						}
					file << ");";
					table.popScope();
				} else if(tree->node_data.nodes[1]->node_type == NT_STRINGLIT || tree->node_data.nodes[1]->node_type == NT_NUMBERLIT || tree->node_data.nodes[1]->node_type == NT_BOOLLIT) {
					file << "function(){return ";
					generate(tree->node_data.nodes[1]);
				} else if(tree->node_data.nodes[1]->node_type == NT_PROVISION_BEHAVIOR) {
					file << "function(";

					table.pushScope();

					for(int i = 0; i < tree->node_data.nodes[1]->subnodes; i++) {
						if(i != 0) file << ",";
						table.add(tree->node_data.nodes[1]->node_data.nodes[i]->node_data.var_decl);
						VarRef ref = tree->node_data.nodes[1]->node_data.nodes[i]->node_data.var_decl->createVarRef();
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
					vector<PureType*> no_args;
					header->addPropertyUsage(file.tellp(), tree->node_data.nodes[i]->node_data.specializable_pure_type->toProvisionSymbol(no_args));
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
				string name = tree->node_data.nodes[tree->subnodes - 1]->node_data.string;
				vector<VarDecl*> arguments = traverser.getArgDecls(tree);
				int i;

				file << "this.";
				header->addPropertyUsage(file.tellp(), name);
				file << "=function(";

				for(i = 0; i < arguments.size(); i++) {
					if(i != 0) file << ",";
					table.add(arguments[i]);
					VarRef ref = arguments[i]->createVarRef();
					file << table.getAddress(&ref);
				}

				file << "){";
				if(tree->subnodes > 0) generate(tree->node_data.nodes[0]);
				if(tree->subnodes > 1) generate(tree->node_data.nodes[1]);
				if(tree->subnodes > 2) generate(tree->node_data.nodes[2]);
				if(tree->subnodes > 3) generate(tree->node_data.nodes[3]);
				file << "};";
				table.popScope();
			}

			break;

		case NT_LAMBDA_DECLARATION:
			{
				table.pushScope();
				boost::ptr_vector<PureType> cleanup;

				file << "(function(";
				for(int i = 0; i < tree->node_data.nodes[0]->subnodes; i++)
				if(tree->node_data.nodes[0]->node_data.nodes[i]->node_type == NT_VAR_DECL_DATA) {
					if(i != 0) file << ",";
					table.add(tree->node_data.nodes[0]->node_data.nodes[i]->node_data.var_decl);
					VarRef ref = tree->node_data.nodes[0]->node_data.nodes[i]->node_data.var_decl->createVarRef();
					file << table.getAddress(&ref);
				} else {
					if(i != 0) file << ",";
					cleanup.push_back(new PureType(TYPE_MATCHALL));
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
				string name = tree->node_data.nodes[tree->subnodes - 1]->node_data.string;

				file << ".";
				header->addPropertyUsage(file.tellp(), name);
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

				file << ")";
			}
			break;

		case NT_EARLYBAILOUT_METHOD_INVOCATION:
			{
				string methodclass = tree->node_data.nodes[tree->subnodes - 2]->node_data.string;
				string name = tree->node_data.nodes[tree->subnodes - 1]->node_data.string;

				table.pushScope();
				auto_ptr<PureType> safechecked(new PureType(TYPE_MATCHALL));
				std::stringstream safecheckedname;
				safecheckedname << safechecked.get();
				table.add(safecheckedname.str(), safechecked.get());

				file << "(function(" << table.getAddress(safecheckedname.str()) << "){";
				file << "return " << table.getAddress(safecheckedname.str()) << "===null?null:(";
				file << table.getAddress(safecheckedname.str());
				file << ".";
				header->addPropertyUsage(file.tellp(), name);
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
				auto_ptr<PureType> safechecked(new PureType(TYPE_MATCHALL));
				std::stringstream safecheckedname;
				safecheckedname << safechecked.get();
				table.add(safecheckedname.str(), safechecked.get());

				file << "(function(" << table.getAddress(safecheckedname.str()) << "){";
				file << "return " << table.getAddress(safecheckedname.str()) << "===null?null:";
				file << table.getAddress(safecheckedname.str());
				file << ".";
				string name = tree->node_data.nodes[tree->subnodes - 1]->node_data.string;
				header->addPropertyUsage(file.tellp(), name);

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

				file << ".";
				header->addPropertyUsage(file.tellp(), name);
			}
			break;

		case NT_RETRIEVAL:
			{
				generate(tree->node_data.nodes[2]);
				string provisionSymbol = tree->node_data.nodes[tree->subnodes - 1]->node_data.string;

				file << ".";
				header->addPropertyUsage(file.tellp(), provisionSymbol);
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
				file << "this.";
				string propname = tree->node_data.nodes[0]->node_data.nodes[0]->node_data.var_ref->toString();
				header->addPropertyUsage(file.tellp(), propname);
				file << "=";
				generate(tree->node_data.nodes[0]->node_data.nodes[1]);
				file << ";";
			}
			break;

		case NT_DECLARATION:
			table.add(tree->node_data.nodes[0]->node_data.var_decl);
			file << "var ";
			// FALL THROUGH
		case NT_ASSIGNMENT:
			generate(tree->node_data.nodes[0]);
			file << "=";
			generate(tree->node_data.nodes[1]);
			break;

		case NT_VALUED_ASSIGNMENT:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "=";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_ADD_ASSIGNMENT:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "+=";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_SUB_ASSIGNMENT:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "-=";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_MULT_ASSIGNMENT:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "*=";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_DIV_ASSIGNMENT:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "/=";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_ARRAY_DECLARATION:
			file << "[";
			if(tree->subnodes) {
				int i;
				for(i = 0; i < tree->subnodes; i++) {
					if(i != 0) file << ",";
					generate(tree->node_data.nodes[i]);
				}

			}
			file << "]";
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
			header->addClassUsage(file.tellp(), classes->getFullyQualifiedClassname(tree->node_data.nodes[0]->node_data.type->typedata._class.classname));
			file << ".call(this";
			PropertySymbolTable* proptable = classes->findByImportedNameModifiable(tree->node_data.nodes[0]->node_data.type->typedata._class.classname);
			for(vector<SpecializableVarDecl*>::iterator it = proptable->getNeeds()->begin(); it != proptable->getNeeds()->end(); ++it) {
				file << ",";
				VarRef ref = (*it)->decl.createVarRef();
				file << table.getAddress(&ref);
			}
			file << ");";
			}
			break;

		case NT_ARRAY_ACCESS_LVAL:
		case NT_TYPESAFE_ARRAY_ACCESS:
			file << "(";
			forceArrayIdentifier = true;
			generate(tree->node_data.nodes[0]);
			forceArrayIdentifier = false;
			file << ")[";
			generate(tree->node_data.nodes[1]);
			file << "]";
			break;

		case NT_ARRAY_ACCESS:
			file << "$A(";
			forceArrayIdentifier = true;
			generate(tree->node_data.nodes[0]);
			forceArrayIdentifier = false;
			file << ",";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_EXISTS:
			file << "if(";
			generate(tree->node_data.nodes[0]);
			file << "!=null){";
			generate(tree->node_data.nodes[1]);
			file << "}";
			if(tree->subnodes > 2) {
				file << "else{";
				generate(tree->node_data.nodes[2]);
				file << "};";
			}
			break;


		case NT_IF_ELSE:
			file << "if(";
			generate(tree->node_data.nodes[0]);
			file << "){";
			generate(tree->node_data.nodes[1]);
			if(tree->subnodes > 2) {
				file << "}else{";
				generate(tree->node_data.nodes[2]);
			}
			file << "}";
			break;

		case NT_WHILE:
			file << "while(";
			generate(tree->node_data.nodes[0]);
			file << "){";
			generate(tree->node_data.nodes[1]);
			file << "}";
			break;

		case NT_FOR:
			file << "for(";
			table.pushScope();
			generate(tree->node_data.nodes[0]);
			file << ";";
			generate(tree->node_data.nodes[1]);
			file << ";";
			generate(tree->node_data.nodes[2]);
			file << "){";
			generate(tree->node_data.nodes[3]);
			table.popScope();
			file << "}";
			break;

		case NT_FOREACH:
		case NT_FOREACHIN:
			{
				table.pushScope();
				Node** nodebase = tree->node_type == NT_FOREACHIN ? tree->node_data.nodes + 1: tree->node_data.nodes;
				bool iterating_expression = nodebase[0]->node_type != NT_ALIAS && nodebase[0]->node_type != NT_TYPEDATA;
				std::stringstream valuestorename;
				auto_ptr<PureType> latch;
				if(iterating_expression) {
					latch.reset(new PureType(TYPE_MATCHALL));
					valuestorename << latch.get();
					table.add(valuestorename.str(), latch.get());
					file << "var " << table.getAddress(valuestorename.str()) << "=";
					generate(nodebase[0]);
					file << ";";
				}

				auto_ptr<PureType> indexer(new PureType(TYPE_MATCHALL));
				auto_ptr<PureType> lowered(new PureType(TYPE_MATCHALL));
				std::stringstream indexername;
				indexername << indexer.get();
				table.add(indexername.str(), indexer.get());
				table.add(nodebase[2]->node_data.string, lowered.get());
				file << "for(var " << table.getAddress(indexername.str()) << "=0;";
				file << table.getAddress(indexername.str()) << " < ";
				if(iterating_expression) file << table.getAddress(valuestorename.str());
				else generate(nodebase[0]);
				file << ".length; " << table.getAddress(indexername.str()) << "++){";
				file << "var " << table.getAddress(nodebase[2]->node_data.string) << "=";
				if(iterating_expression) file << table.getAddress(valuestorename.str());
				else generate(nodebase[0]);
				file << "[" << table.getAddress(indexername.str()) << "];";
				generate(nodebase[1]);
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
			file << ")?(";
			generate(tree->node_data.nodes[0]);
			file << "):(";
			generate(tree->node_data.nodes[2]);
			file << "))";
			break;

		case NT_RETURN:
			file << "return ";
			if(tree->subnodes) generate(tree->node_data.nodes[0]);
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
			file << "*";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_DIVIDE:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "/";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_MOD:
			file << "$B(";
			generate(tree->node_data.nodes[0]);
			file << ",";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_MODNATIVE:
		case NT_MODALT:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "%";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_ADD:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "+";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_SUBTRACT:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "-";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_BITSHIFTLEFT:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "<<";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_BITSHIFTRIGHT:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << ">>";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_LESSTHAN:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "<";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_GREATERTHAN:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << ">";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_LESSTHANEQUAL:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "<=";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_GREATERTHANEQUAL:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << ">=";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_EQUALITY:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "==";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_INEQUALITY:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "!=";
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
			file << "&";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_BITXOR:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "^";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_BITOR:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "|";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_AND:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "&&";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_OR:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "||";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_NUMBERLIT:
			file << std::setprecision (std::numeric_limits<double>::digits10 + 1) << tree->node_data.number;
			break;

		case NT_BOOLLIT: file << (tree->node_data.number ? "1" : "0"); break;
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

		case NT_NOTHING: file << "null"; break;
	}
}
