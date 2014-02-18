#ifndef HEADER_OBJECTFILE
#define HEADER_OBJECTFILE

typedef struct {
	char* symbol;
	int pos;
} symbolusage;

typedef struct {
	int classusagecount;
	symbolusage** classusages;
	int propertyusagecount;
	symbolusage** propertyusages;
	char* filename;
} objectfile;

#endif
