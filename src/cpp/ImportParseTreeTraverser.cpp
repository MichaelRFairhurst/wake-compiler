#include "ImportParseTreeTraverser.h"

void ImportParseTreeTraverser::traverse(Node* tree, ClassSpaceSymbolTable& classes, LibraryLoader& l, std::string importpath) {
	switch(tree->node_type) {

		case NT_PROGRAM:
		case NT_IMPORTSET:
		case NT_IMPORT:
			for(int i = 0; i < tree->subnodes; i++) {
				traverse(tree->node_data.nodes[i], classes, l, importpath);
			}
			break;

		case NT_IMPORTPATH:
			// TODO use actual path
			l.loadImport(tree->node_data.string, importpath, classes);
			break;

		//case NT_IMPORTTARGET:
			//throw "Not supported yet";
	}

}
