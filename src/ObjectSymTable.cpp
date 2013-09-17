#include "ObjectSymTable.h"
#include <stdio.h>
#include <algorithm>


ObjectSymTable::ObjectSymTable() {
	errors = new vector<SemanticError>();
}

bool ObjectSymTable::isASubtypeOfB(string a, string b) {
	if(a == b) return true;

	std::map<string,string*>::iterator subit = subclasses.find(a);
	if(subit != subclasses.end()) {
		if(isASubtypeOfB(*subit->second, b)) return true;
	}

	std::map<string,vector<string*>*>::iterator implit = interfaces.find(a);
	if(implit != interfaces.end()) {
		vector<string*>* impls = implit->second;
		for(vector<string*>::iterator it = impls->begin(); it != impls->end(); ++it) {
			if(isASubtypeOfB(**it, b)) return true;
		}
	}

	return false;
}

void ObjectSymTable::build(Node* tree) {
	switch(tree->node_type) {
		case NT_PROGRAM:
			build(tree->node_data.nodes[0]);
			if(tree->subnodes > 1) build(tree->node_data.nodes[1]);
			secondPass(tree->node_data.nodes[0]);
			if(tree->subnodes > 1) secondPass(tree->node_data.nodes[1]);
			break;
		case NT_INHERITANCESET:
		case NT_CLASSSET:
			{
				int i = 0;
				while(i < tree->subnodes) {
					build(tree->node_data.nodes[i]);
					i++;
				}
			}
			break;
		case NT_CLASS:
			build(tree->node_data.nodes[0]);
			build(tree->node_data.nodes[1]);
			break;
		case NT_CLASSNAME:
			{
				string name(tree->node_data.string);
				if(std::find(classes.begin(), classes.end(), name) == classes.end()) {
					classes.push_back(name);
					addingclass.assign(name);
				} else {
					string temp = "Multiple declarations of 'every ";
					temp += name;
					temp += "'";
					SemanticError error(tree, temp);
					errors->push_back(error);
				}
			}
			break;
		case NT_INTERFACE:
			{
				string *child = new string();child->assign(tree->node_data.string);
				vector<string*>* children;

				if(!interfaces.count(addingclass)) {
					if(isASubtypeOfB(*child, addingclass)) {
						std::map<string,string*>::iterator searcher = subclasses.find(addingclass);
						string temp = "In declaration of 'every ";
						temp += addingclass;
						temp += "', circular relationship would be formed by 'capable ";
						temp += *searcher->second;
						temp += "'";
						SemanticError error(tree, temp);
						errors->push_back(error);
					} else {
						children = new vector<string*>();
						interfaces[addingclass] = children;
						children->push_back(child);
					}
				} else {
					std::map<string,vector<string*>*>::iterator searcher = interfaces.find(addingclass);
					children = searcher->second;

					bool duplicate = false;
					for(int i = 0; i < children->size(); i++)
					if(*children->at(i) == *child) duplicate = true;

					if(!duplicate) {
						children->push_back(child);
					} else {
						string temp = "In declaration of 'every ";
						temp += addingclass;
						temp += "', listed more than once as 'capable ";
						temp += *child;
						temp += "'";
						SemanticError error(tree, temp);
						errors->push_back(error);
					}
				}
			}
			break;
		case NT_SUBCLASS:
			{
				string *child = new string();child->assign(tree->node_data.string);

				if(!subclasses.count(addingclass)) {
					if(isASubtypeOfB(*child, addingclass)) {
						string temp = "In declaration of 'every ";
						temp += addingclass;
						temp += "', circular relationship would be formed by 'a/an ";
						temp += *child;
						temp += "'";
						SemanticError error(tree, temp);
						errors->push_back(error);
					} else {
						subclasses[addingclass] = child;
					}

				} else {
					std::map<string,string*>::iterator searcher = subclasses.find(addingclass);
					string temp = "In declaration of 'every ";
					temp += addingclass;
					temp += "', already a/an ";
					temp += *searcher->second;
					temp += ", and therefore cannot also be a/an ";
					temp += *child;
					temp += ". Perhaps use 'capable' instead?";
					SemanticError error(tree, temp);
					errors->push_back(error);
				}
			}
	}
}

void ObjectSymTable::secondPass(Node* tree) {
	switch(tree->node_type) {
		case NT_INHERITANCESET:
		case NT_CLASSSET:
			{
				int i = 0;
				while(i < tree->subnodes) {
					secondPass(tree->node_data.nodes[i]);
					i++;
				}
			}
			break;
		case NT_CLASS:
			secondPass(tree->node_data.nodes[0]);
			secondPass(tree->node_data.nodes[1]);
			break;
		case NT_CLASSNAME:
			addingclass.assign(tree->node_data.string);
			break;
		case NT_INTERFACE:
		case NT_SUBCLASS:
			{
				string temp = tree->node_data.string;

				if(std::find(classes.begin(), classes.end(), temp) == classes.end()) {
					temp.assign("In declaration of 'every ");
					temp += addingclass;
					temp += "', cannot find subclass for 'a/an ";
					temp += tree->node_data.string;
					temp += "'";
					SemanticError error(tree, temp);
					errors->push_back(error);

					// cleanup
					temp = tree->node_data.string;
				}

				if(tree->node_type == NT_SUBCLASS) break;

				if(!subclasses.count(addingclass)) break;

				std::map<string,string*>::iterator searcher = subclasses.find(addingclass);
				if(*searcher->second == temp) {
					temp.assign("In declaration of 'every ");
					temp += addingclass;
					temp += "', declared as both 'a/an' and 'capable ";
					temp += tree->node_data.string;
					temp += "'";
					SemanticError error(tree, temp);
					errors->push_back(error);
				}
			}
			break;
	}
}

vector<SemanticError>* ObjectSymTable::getErrors() {
	return errors;
}
