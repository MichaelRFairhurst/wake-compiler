#include <stdio.h>
#include "Linker.h"

using namespace std;

extern "C" {
	#include "objectfile.tab.h"
	#include "objectfile.h"
	extern int objectfilelex_destroy  (void);
	extern int objectfileparse();
	extern void objectfile_scan_string(const char* string);
	extern objectfile* myobjectfile;
	extern FILE *objectfilein;
}

void Linker::loadObject(string filename) {
	myobjectfile = (objectfile*) malloc(sizeof(objectfile));
	myobjectfile->classcount = 0;
	myobjectfile->propertycount = 0;
	myobjectfile->usagecount = 0;

	objectfilein = fopen(filename.c_str(), "r");
	objectfileparse();
	fclose(objectfilein);

	objectFiles = (objectfile**) realloc(objectFiles, sizeof(objectfile*) * ++objectFileCount);
	objectFiles[objectFileCount - 1] = myobjectfile;
	files.push_back(filename);
}

void Linker::write(fstream& objectfile) {
	for(vector<string>::iterator it = files.begin(); it != files.end(); ++it) {
		objectfilein = fopen(it->c_str(), "r");
		fclose(objectfilein);
	}
}
