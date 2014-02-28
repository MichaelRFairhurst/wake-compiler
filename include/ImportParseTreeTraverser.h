#ifndef HEADER_IMPORT_PARSE_TREE_TRAVERSER
#define HEADER_IMPORT_PARSE_TREE_TRAVERSER

#include "LibraryLoader.h"
#include "ObjectSymbolTable.h"
#include <string>

extern "C" {
	#include "tree.h"
}

class ImportParseTreeTraverser {

	public:
		void traverse(Node* tree, ObjectSymbolTable& o, LibraryLoader& l, std::string dirname);

};

#endif
