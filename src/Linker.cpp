#include <stdio.h>
#include <algorithm>
#include "Linker.h"
#include <fstream>

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
	myobjectfile->classes = NULL;
	myobjectfile->properties = NULL;
	myobjectfile->usages = NULL;
	myobjectfile->filename = "bin/finaltest.js";//cstrdup(filename.c_str());

	objectfilein = fopen(filename.c_str(), "r");
	objectfileparse();
	fclose(objectfilein);

	files.push_back(myobjectfile);
}

void Linker::write(ostream& outfile) {
	for(vector<objectfile*>::iterator file = files.begin(); file != files.end(); ++file) {
		/* Read whole file into memory */
		/* @TODO not read whole file into memory */
		std::ifstream ifs((*file)->filename);

		ifs.seekg(0, std::ios::end);
		std::streamsize f_size = ifs.tellg();
		ifs.seekg(0, std::ios::beg);

		std::vector<char> buffer(f_size);
		ifs.read(buffer.data(), f_size);

		char* seq = "%-END-%\n";
		std::vector<char>::iterator it;

		/* get the starting point of code */
		it = std::search(buffer.begin(), buffer.end(), seq, seq + 7);

		it = it + 8;

		int usagei, i;
		for(i = 0, usagei = 0; it != buffer.end(); ++it, i++) {
			if(usagei < (*file)->usagecount && (*file)->usages[usagei]->pos == i) {
				//outfile << (*file)->usages[usagei]->symbol; // @TODO useg symbol table to address
				outfile << "METHOD"; // test valid syntax
				usagei++;
			}

			outfile << *it;
		}

	}

}
