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
		case NT_IMPORT:
			for(int i = 0; i < tree->subnodes; i++) {
				traverse(tree->node_data.nodes[i], classes, l, errors, importpath);
			}
			break;

		case NT_IMPORTPATH:
			// TODO use actual path
			try {
				if(!l.loadImport(tree->node_data.string, importpath, classes)) {
					errors.addError(new SemanticError(BAD_IMPORT, "Could not import class by name " + string(tree->node_data.string), tree));
				}
			} catch(string errormsg) {
				errors.addError(new SemanticError(BAD_IMPORT, "Error importing class by name " + string(tree->node_data.string) + ": " + errormsg, tree));
			}
			break;

		//case NT_IMPORTTARGET:
			//throw "Not supported yet";
	}

}
