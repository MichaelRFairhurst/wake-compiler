#include "CodeGenerator.h"

void CodeGenerator::generate(Node* tree) {
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
		case NT_IF_THEN_ELSE:
		case NT_INCREMENT:
		case NT_DECREMENT:
		case NT_SWITCH:
			return;

		case NT_PROGRAM:
			file << "(function() {";
			{
				// StdLib lol
				file << "var printqueue='';Printer=function(){this.a=function(b){if(typeof process!='undefined'&&typeof process.stdout!='undefined'&&typeof process.stdout.write!='undefined')process.stdout.write(''+b);else printqueue+=''+b;};this.b=this.a;this.c=function(b){if(typeof process!='undefined'&&typeof process.stdout!='undefined'&&typeof process.stdout.write!='undefined')return this.a(b+'\\n');console.log(printqueue+b);printqueue='';};this.d=this.c;};System=function(){this.e=function(a){if(typeof process!='undefined'&&typeof process.exit!='undefined')process.exit(a);}};";
				int i;
				for(i = 0; i < tree->subnodes; i++)
					generate(tree->node_data.nodes[i]);
			}
			//file << "})();";
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
		case NT_INHERITANCE:
		case NT_INHERITANCESET:
		case NT_PROVISIONS:
		case NT_INTERFACE:
		case NT_INJECTION:
		case NT_INJECTION_ARGS:
		case NT_INJECTION_ARG:
		case NT_CTOR:
		case NT_PARENT:
		case NT_CLASSNAME:
		case NT_MEMBER_ACCESS:
		case NT_LAMBDA_INVOCATION:
		case NT_CAST:
			{
				int i;
				for(i = 0; i < tree->subnodes; i++)
					generate(tree->node_data.nodes[i]);
			}
			break;

		case NT_CLASS:
			{
				table.pushScope();
				classname = tree->node_data.nodes[0]->node_data.string;
				file << classname << "=function(";

				vector<Type*>* needs = objects->find(classname)->getNeeds();
				for(vector<Type*>::iterator it = needs->begin(); it != needs->end(); ++it) {
					table.add(*it);
					if(it != needs->begin()) file << ",";
					file << table.getAddress(*it);
				}

				file << "){";
				generate(tree->node_data.nodes[1]);
				if(tree->subnodes > 2) generate(tree->node_data.nodes[2]);
				file << "};";
				table.popScope();
			}
			break;

		case NT_PROVISION:
			file << "this." << objects->find(classname)->getProvisionAddress(tree->node_data.nodes[0]->node_data.type) << "=function(){";
			if(tree->subnodes == 1) {
				string provisionname = tree->node_data.nodes[0]->node_data.type->typedata._class.classname;
				file << "return new " << provisionname << "(";
				vector<Type*>* needs = objects->find(provisionname)->getNeeds();
				for(vector<Type*>::iterator it = needs->begin(); it != needs->end(); ++it) {
					if(it != needs->begin()) file << ",";
					file << "this." << objects->find(classname)->getProvisionAddress(*it) << "()";
				}
				file << ");";
			} else {
				if(tree->node_data.nodes[1]->node_type == NT_TYPEDATA) {
					file << "return this." << objects->find(classname)->getProvisionAddress(tree->node_data.nodes[1]->node_data.type) << "();";
				}
			}
			file << "};";
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
				TypeArray* arguments = objects->find(classname)->get(name)->typedata.lambda.arguments;
				int i;

				file << "this." << objects->find(classname)->getAddress(name) << "=function(";

				for(i = 0; i < arguments->typecount; i++) {
					if(i != 0) file << ",";
					table.add(arguments->types[i]);
					file << table.getAddress(arguments->types[i]);
				}

				file << "){";
				if(tree->subnodes > 0) generate(tree->node_data.nodes[0]);
				if(tree->subnodes > 1) generate(tree->node_data.nodes[1]);
				if(tree->subnodes > 2) generate(tree->node_data.nodes[2]);
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

				file << "." << objects->find(methodclass)->getAddress(name) << "(";

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

		case NT_RETRIEVAL:
			{
				generate(tree->node_data.nodes[2]);
				string providerclass = tree->node_data.nodes[tree->subnodes - 1]->node_data.string;

				file << "." << objects->find(providerclass)->getProvisionAddress(tree->node_data.nodes[0]->node_data.type) << "()";
			}
			break;

		case NT_PROPERTY:
			table.add(tree->node_data.nodes[0]->node_data.nodes[0]->node_data.type);
			file << "var ";
			generate(tree->node_data.nodes[0]);
			file << ";";
			break;

		case NT_DECLARATION:
			table.add(tree->node_data.nodes[0]->node_data.type);
			file << "var ";
			// FALL THROUGH
		case NT_ASSIGNMENT:
			{
				Node* assignednode = tree->node_data.nodes[0];
				//if(assignednode->node_type == NT_ALIAS) {
					//file << table.getAddress(string("@") + assignednode->node_data.string) << "=";
				//} else {
					//file << table.getAddress(assignednode->node_data.type) << "=";
				//}
				generate(assignednode);
				file << "=";
			}
			generate(tree->node_data.nodes[1]);
			//if(tree->node_type == NT_DECLARATION) file << ";";
			break;

		case NT_ARRAY_DECLARATION:
			file << "[";
			if(tree->subnodes) {
				int i;
				for(i = 0; i < tree->subnodes; i++)
					generate(tree->node_data.nodes[i]);
			}
			file << "]";
			break;

		case NT_TYPEDATA:
			file << table.getAddress(tree->node_data.type);
			break;

		case NT_ALIAS:
			{
				string name = "@";
				name += tree->node_data.string;
				file << table.getAddress(name);
			}
			break;

		case NT_SUBCLASS:
			file << "this.prototype = new " << tree->node_data.string << "();";
			break;

		case NT_ARRAY_ACCESS:
			file << "(";
			generate(tree->node_data.nodes[0]);
			file << ")[";
			generate(tree->node_data.nodes[1]);
			file << "]";
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
			generate(tree->node_data.nodes[0]);
			file << ";";
			generate(tree->node_data.nodes[1]);
			file << ";";
			generate(tree->node_data.nodes[2]);
			file << "){";
			generate(tree->node_data.nodes[3]);
			file << "}";
			break;

		case NT_BREAK: file << "break;"; break;
		case NT_CONTINUE: file << "continue;"; break;
		case NT_THIS: file << "this"; break;

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
		case NT_TRUTHLIT: file << (tree->node_data.number ? "1" : "0"); break;
		case NT_STRINGLIT: file << '"'; file << tree->node_data.string; file << '"'; break;
	}
}

void CodeGenerator::setMain(string classname, string methodname) {
	file << "(";
	generateRecursiveConstructors(classname);
	file << ")." << objects->find(classname)->getAddress(methodname) << "();})();";
}

/*
void CodeGenerator::generateRecursiveProvisions(Type* provision) {
	file << "new " << provision->typedata._class.classname << "(";

	vector<Type*>* needs = objects->find(ctedclass)->getNeeds();
	for(vector<Type*>::iterator it = needs->begin(); it != needs->end(); ++it) {
		file << "this." << objects->find(classname)->getProvisionAddress(it) << "()";
		if(it != needs->begin()) file << ",";
		generateRecursiveConstructors(*it);
	}
	file << ");";
}*/

void CodeGenerator::generateRecursiveConstructors(string ctedclass) {
	file << "new " << ctedclass << "(";

	vector<Type*>* needs = objects->find(ctedclass)->getNeeds();
	for(vector<Type*>::iterator it = needs->begin(); it != needs->end(); ++it) {
		if(it != needs->begin()) file << ",";
		generateRecursiveConstructors((*it)->typedata._class.classname);
	}

	file << ")";
}

CodeGenerator::~CodeGenerator() {
	file.close();
}
