#include <stdio.h>
#include <string.h>
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
	myobjectfile->classusagecount = 0;
	myobjectfile->propertyusagecount = 0;
	myobjectfile->classusages = NULL;
	myobjectfile->propertyusages = NULL;
	myobjectfile->filename = strdup(filename.c_str());

	objectfilein = fopen(filename.c_str(), "r");
	objectfileparse();
	fclose(objectfilein);
	objectfilelex_destroy();

	files.push_back(new ObjectFileHeaderData(myobjectfile));
	free(myobjectfile->classusages);
	free(myobjectfile->propertyusages);
	free(myobjectfile);
}

void Linker::write(ostream& outfile) {
	outfile << "(function(){";
	for(vector<ObjectFileHeaderData*>::iterator file = files.begin(); file != files.end(); ++file) {
		/* Read whole file into memory */
		/* @TODO not read whole file into memory */
		std::ifstream ifs((*file)->getFilename().c_str());

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

		int classusei, propusei, i;
		std::vector<std::pair<int, std::string> > classUsages = (*file)->getClassUsages();
		std::vector<std::pair<int, std::string> > propUsages = (*file)->getPropertyUsages();
		for(i = 0, classusei = 0, propusei = 0; it != buffer.end(); ++it, i++) {
			if(classusei < classUsages.size() && classUsages[classusei].first == i) {
				if(!classTable.symbolExists(classUsages[classusei].second)) classTable.addSymbol(classUsages[classusei].second);
				outfile << "_" << classTable.getAddress(classUsages[classusei].second);
				classusei++;
			} else if(propusei < (*file)->getPropertyUsages().size() && (*file)->getPropertyUsages()[propusei].first == i) {
				if(!propertyTable.symbolExists(propUsages[propusei].second)) propertyTable.addSymbol(propUsages[propusei].second);
				outfile << propertyTable.getAddress(propUsages[propusei].second);
				propusei++;
			}

			outfile << *it;
		}

	}

	outfile << "})();";
}
