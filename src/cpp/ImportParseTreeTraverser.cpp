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
				break;
			}
	}

}
