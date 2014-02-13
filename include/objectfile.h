#ifndef HEADER_OBJECTFILE
#define HEADER_OBJECTFILE

typedef struct {
	char* symbol;
	int pos;
} symbolusage;

typedef struct {
	char* filename;
	int classcount;
	char** classes;
	int propertycount;
	char** properties;
	int usagecount;
	symbolusage** usages;
} objectfile;

#endif
