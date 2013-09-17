#ifndef HEADER_OBJECT_SYM_TABLE
#define HEADER_OBJECT_SYM_TABLE

#include <string>
#include <map>
#include <vector>
#include "SemanticError.h"

extern "C" {
	#include "type.h"
	#include "node.h"
	#include "tree.h"
}

using namespace std;

class ObjectSymTable {
	public:
		ObjectSymTable();
		bool isASubtypeOfB(string a, string b);
		void build(Node* tree);
		vector<SemanticError>* getErrors();

	private:
		void secondPass(Node* tree);
		std::string addingclass;
		std::vector<string> classes;
		map<string, string*> subclasses;
		map<string, vector<string*>*> interfaces;
		vector<SemanticError>* errors;
};

#endif
