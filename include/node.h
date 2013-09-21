#ifndef HEADER_NODE
#define HEADER_NODE

typedef struct Node {
	int line;
	int col;
	int node_type;
	int subnodes;
	union {
		char* string;
		float number;
		struct Node** nodes;
		struct Type* type;
		struct TypeArray* typearray;
	} node_data;
} Node;

#endif
