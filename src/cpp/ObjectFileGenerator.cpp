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
 * Revised By:
 *
 **************************************************/

#include "ObjectFileGenerator.h"
#include <sstream>

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
		case NT_LAMBDA_INVOCATION:
		case NT_INJECTION:
			{
				int i;
				for(i = 0; i < tree->subnodes; i++)
					generate(tree->node_data.nodes[i]);
			}
			break;

		case NT_AUTOBOX:
			file << "new ";
			header->addClassUsage(file.tellp(), tree->node_data.nodes[1]->node_data.string);
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
				header->addClassUsage(file.tellp(), classname);
				file << "(";

				vector<Type*>* needs = classes->find(classname)->getNeeds();
				for(vector<Type*>::iterator it = needs->begin(); it != needs->end(); ++it) {
					table.add(*it);
					if(it != needs->begin()) file << ",";
					file << table.getAddress(*it);
				}

				file << "){";

				for(vector<Type*>::iterator it = needs->begin(); it != needs->end(); ++it) {
					file << "this.";
					header->addPropertyUsage(file.tellp(), table.getNameForType(*it));
					file << "=" << table.getAddress(*it) << ";";
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
				const map<string, bool> parentage = classes->find(classname)->getParentage();
				// @Todo only print exception classes
				for(map<string, bool>::const_iterator it = parentage.begin(); it != parentage.end(); ++it) {
					file << "'";
					header->addClassUsage(file.tellp(), it->first);
					file << "',";
				}
				file << "'";
				header->addClassUsage(file.tellp(), classname);
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
			table.add(tree->node_data.nodes[0]->node_data.type);
			file << "catch(" << table.getAddress(tree->node_data.nodes[0]->node_data.type) << "){";

			// wrap in a wake exception if its not one
			file << "if(!" << table.getAddress(tree->node_data.nodes[0]->node_data.type) << ".";
			header->addPropertyUsage(file.tellp(), "isExceptionType(Text)");
			file << ")" << table.getAddress(tree->node_data.nodes[0]->node_data.type) << "=new ";
			header->addClassUsage(file.tellp(), "Exception");
			file << "(" << table.getAddress(tree->node_data.nodes[0]->node_data.type) << ");";

			// check its type
			file << "if(" << table.getAddress(tree->node_data.nodes[0]->node_data.type) << ".";
			header->addPropertyUsage(file.tellp(), "isExceptionType(Text)");
			file << "('";
			header->addClassUsage(file.tellp(), tree->node_data.nodes[0]->node_data.type->typedata._class.classname);
			file << "')){";

			// run catch statement
			generate(tree->node_data.nodes[1]);

			// or rethrow it
			file << "}else{throw " << table.getAddress(tree->node_data.nodes[0]->node_data.type) << ";}}";
			table.popScope();
			break;

		case NT_THROW:
			{
				table.pushScope();
				Type* exception = MakeType(TYPE_MATCHALL);
				std::stringstream exceptionname;
				exceptionname << exception;
				table.add(exceptionname.str(), exception);
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
				header->addClassUsage(file.tellp(), provisionname);
				file << "(";
				vector<Type*>* needs = classes->find(provisionname)->getNeeds();
				for(vector<Type*>::iterator it = needs->begin(); it != needs->end(); ++it) {
					vector<Type*> no_args;
					if(it != needs->begin()) file << ",";
					file << "this.";
					header->addPropertyUsage(file.tellp(), classes->find(classname)->getProvisionSymbol(*it, no_args));
					file << "()";
				}
				file << ");";
			} else {
				if(tree->node_data.nodes[1]->node_type == NT_TYPEDATA) {
					file << "function(){return this.";
					vector<Type*> no_args;
					header->addPropertyUsage(file.tellp(), classes->find(classname)->getProvisionSymbol(tree->node_data.nodes[1]->node_data.type, no_args));
					file << "();";
				} else if(tree->node_data.nodes[1]->node_type == NT_INJECTION) {
					file << "function(";
					vector<Type*> no_args;
					Node* inj = tree->node_data.nodes[1]->node_data.nodes[1];
					table.pushScope();

					bool first = true;
					for(int i = 0; i < inj->subnodes; i++) {
						if(inj->node_data.nodes[i]->node_type == NT_INJECTION_ARG) {
							if(first) first = false;
							else file << ",";
							inj->node_data.nodes[i]->node_data.nodes[0]->node_data.type->alias = strdup(string('$', i).c_str()); // a variable name guarenteed unique
							table.add(inj->node_data.nodes[i]->node_data.nodes[0]->node_data.type);
							file << table.getAddress(inj->node_data.nodes[i]->node_data.nodes[0]->node_data.type);
						}
					}

					file << "){return new ";
					string provisionname = tree->node_data.nodes[0]->node_data.type->typedata._class.classname;
					header->addClassUsage(file.tellp(), provisionname);
					file << "(";
						for(int i = 0; i < inj->subnodes; i++) {
							if(i != 0) {
								file << ",";
							}

							if(inj->node_data.nodes[i]->node_type == NT_INJECTION_ARG) {
								file << table.getAddress(inj->node_data.nodes[i]->node_data.nodes[0]->node_data.type);
							} else {
								file << "this.";
								header->addPropertyUsage(file.tellp(), classes->find(classname)->getProvisionSymbol(inj->node_data.nodes[i]->node_data.type, no_args));
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

					TypeArray* argarray = tree->node_data.nodes[1]->node_data.nodes[0]->node_data.typearray;
					for(int i = 0; i < argarray->typecount; i++) {
						if(i != 0) file << ",";
						table.add(argarray->types[i]);
						file << table.getAddress(argarray->types[i]);
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
					vector<Type*> no_args;
					header->addPropertyUsage(file.tellp(), classes->find(classname)->getProvisionSymbol(tree->node_data.nodes[i]->node_data.type, no_args));
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
				if(tree->node_data.nodes[i]->node_type == NT_METHOD_DECLARATION || tree->node_data.nodes[i]->node_type == NT_PROVISIONS)
					generate(tree->node_data.nodes[i]);

				// TODO generate ctor body
			}
			break;

		case NT_METHOD_DECLARATION:
			table.pushScope();
			{
				string name = tree->node_data.nodes[tree->subnodes - 1]->node_data.string;
				TypeArray* arguments = (*classes->find(classname)->find(name))->typedata.lambda.arguments;
				int i;

				//file << "this." << classes->find(classname)->getAddress(name) << "=function(";
				file << "this.";
				header->addPropertyUsage(file.tellp(), name);
				file << "=function(";

				for(i = 0; i < arguments->typecount; i++) {
					if(i != 0) file << ",";
					table.add(arguments->types[i]);
					file << table.getAddress(arguments->types[i]);
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

		//case NT_METHOD_RETURN_TYPE:
		case NT_METHOD_NAME:
			break;

		case NT_METHOD_INVOCATION:
			{
				generate(tree->node_data.nodes[0]);
				string methodclass = tree->node_data.nodes[tree->subnodes - 2]->node_data.string;
				string name = tree->node_data.nodes[tree->subnodes - 1]->node_data.string;

				//file << "." << classes->find(methodclass)->getAddress(name) << "(";
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
				string propname = table.getNameForType(tree->node_data.nodes[0]->node_data.nodes[0]->node_data.type);
				header->addPropertyUsage(file.tellp(), propname);
				file << "=";
				generate(tree->node_data.nodes[0]->node_data.nodes[1]);
				file << ";";
			}
			break;

		case NT_DECLARATION:
			table.add(tree->node_data.nodes[0]->node_data.type);
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
				for(i = 0; i < tree->node_data.nodes[0]->subnodes; i++) {
					if(i != 0) file << ",";
					generate(tree->node_data.nodes[0]->node_data.nodes[i]);
				}

			}
			file << "]";
			break;

		case NT_TYPEDATA:
			{
				Type type = *tree->node_data.type;
				if(forceArrayIdentifier) {
					if(type.type == TYPE_LIST) {
						type.typedata.list.levels++;
					} else {
						type = Type(TYPE_LIST);
						type.typedata.list.levels = 1;
						type.typedata.list.contained = new Type(*tree->node_data.type);
					}
				}
				file << table.getAddress(&type);
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
			header->addClassUsage(file.tellp(), tree->node_data.nodes[0]->node_data.type->typedata._class.classname);
			file << ".call(this";
			PropertySymbolTable* proptable = classes->findModifiable(tree->node_data.nodes[0]->node_data.type->typedata._class.classname);
			for(vector<Type*>::iterator it = proptable->getNeeds()->begin(); it != proptable->getNeeds()->end(); ++it) {
				file << ",";
				file << table.getAddress(*it);
			}
			file << ");";
			}
			break;

		case NT_ARRAY_ACCESS_LVAL:
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
				if(iterating_expression) {
					Type* valuestore = MakeType(TYPE_MATCHALL);
					valuestorename << valuestore;
					table.add(valuestorename.str(), valuestore);
					file << "var " << table.getAddress(valuestorename.str()) << "=";
					generate(nodebase[0]);
					file << ";";
				}

				Type* indexer = MakeType(TYPE_MATCHALL);
				Type* lowered = MakeType(TYPE_MATCHALL);
				std::stringstream indexername;
				indexername << indexer;
				table.add(indexername.str(), indexer);
				table.add(nodebase[2]->node_data.string, lowered);
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
			file << "(";
			generate(tree->node_data.nodes[1]);
			file << ")?(";
			generate(tree->node_data.nodes[0]);
			file << "):(";
			generate(tree->node_data.nodes[2]);
			file << ")";
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

		case NT_AND:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "&";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_OR:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << "|";
			generate(tree->node_data.nodes[1]);
			file << ")";
			break;

		case NT_NUMBERLIT: file << tree->node_data.number; break;
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
