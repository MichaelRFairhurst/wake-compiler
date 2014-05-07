#ifndef HEADER_IMPORT_PARSE_TREE_TRAVERSER
#define HEADER_IMPORT_PARSE_TREE_TRAVERSER

#include "LibraryLoader.h"
#include "ClassSpaceSymbolTable.h"
#include <string>
#include "ErrorTracker.h"

extern "C" {
	#include "tree.h"
}

class ImportParseTreeTraverser {

	public:
		void traverse(Node* tree, ClassSpaceSymbolTable& o, LibraryLoader& l, ErrorTracker& errors, std::string dirname);

};

#endif
