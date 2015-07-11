/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ImportParseTreeTraverser.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "ImportParseTreeTraverser.h"
#include "SemanticError.h"

void ImportParseTreeTraverser::traverse(Node* tree, ClassSpaceSymbolTable& classes, LibraryLoader& l, ErrorTracker& errors, std::string importpath) {
	switch(tree->node_type) {

		case NT_PROGRAM:
		case NT_IMPORTSET:
			for(int i = 0; i < tree->subnodes; i++) {
				traverse(tree->node_data.nodes[i], classes, l, errors, importpath);
			}
			break;

		case NT_IMPORT:
			{
				std::string moduledirectory = "";
				std::string importname = tree->node_data.nodes[1]->node_data.string;

				if(strlen(tree->node_data.nodes[0]->node_data.string)) {
					moduledirectory += tree->node_data.nodes[0]->node_data.string + string("/");
				}

				// TODO use actual path
				try {
					if(!l.loadImport(moduledirectory + importname, importpath, classes)) {
						errors.addError(new SemanticError(BAD_IMPORT, "Could not import class by name " + moduledirectory + importname, tree));
					}
				} catch(string errormsg) {
					errors.addError(new SemanticError(BAD_IMPORT, "Error importing class by name " + moduledirectory + importname + ": " + errormsg, tree));
				}
			}
			break;
	}

}

void ImportParseTreeTraverser::prepImports(Node* tree, ClassSpaceSymbolTable& classes) {
	switch(tree->node_type) {
		case NT_PROGRAM:
		case NT_IMPORTSET:
			for(int i = 0; i < tree->subnodes; i++) {
				prepImports(tree->node_data.nodes[i], classes);
			}
			break;

		case NT_IMPORT:
			{
				std::string modulename = tree->node_data.nodes[0]->node_data.string;
				std::string classname = tree->node_data.nodes[1]->node_data.string;
				classes.prepImport(modulename, classname);
			}
			break;
	}
}

std::vector<std::pair<std::string, std::string> > ImportParseTreeTraverser::gatherImports(Node* tree) {
	switch(tree->node_type) {

		case NT_PROGRAM:
		case NT_IMPORTSET:
			{
				std::vector<std::pair<std::string, std::string> > res;
				for(int i = 0; i < tree->subnodes; i++) {
					std::vector<std::pair<std::string, std::string> > res2 = gatherImports(tree->node_data.nodes[i]);
					for(std::vector<std::pair<std::string, std::string> >::iterator it = res2.begin(); it != res2.end(); ++it) {
						res.push_back(*it);
					}
				}

				return res;
			}

		case NT_IMPORT:
			{
				std::vector<std::pair<std::string, std::string> > res;
				std::string module = "";
				std::string importname = tree->node_data.nodes[1]->node_data.string;

				if(strlen(tree->node_data.nodes[0]->node_data.string)) {
					module = tree->node_data.nodes[0]->node_data.string;
				}

				res.push_back(std::pair<std::string, std::string>(module, importname));
				return res;
			}
		default:
			return std::vector<std::pair<std::string, std::string> >();
	}

}
