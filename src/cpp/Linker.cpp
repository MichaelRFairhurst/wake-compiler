/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * Linker.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include <stdio.h>
#include <string.h>
#include <algorithm>
#include "Linker.h"
#include "TableFileReader.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>

using namespace std;
using namespace boost::filesystem;

extern "C" {
	#include "objectfile.tab.h"
	#include "objectfile.h"
	extern int objectfilelex_destroy  (void);
	extern int objectfileparse();
	extern void objectfile_scan_string(const char* string);
	extern objectfile* myobjectfile;
	extern FILE *objectfilein;
}

void Linker::loadTables(string dirname, ClassSpaceSymbolTable& table) {
	if (!exists(dirname)) throw string("Directory " + dirname + " does not exist");

	TableFileReader reader;
	fstream file;
	directory_iterator end_itr; // default construction yields past-the-end

	for(directory_iterator itr(dirname); itr != end_itr; ++itr)
	if(!is_directory(itr->status())) {
		string fname = itr->path().string();
		if(fname.substr(fname.size() - 6) != ".table") continue;
		file.open(fname.c_str(), fstream::in | fstream::binary);
		PropertySymbolTable* ptable = table.getEmptyPropertySymbolTable();
		reader.read(ptable, file);
		table.importClass(ptable);
		file.close();
	}
	else
	for(directory_iterator moduleitr(*itr); moduleitr != end_itr; ++moduleitr) {
		string fname = moduleitr->path().string();
		if(fname.substr(fname.size() - 6) != ".table") continue;
		file.open(fname.c_str(), fstream::in | fstream::binary);
		PropertySymbolTable* ptable = table.getEmptyPropertySymbolTable();
		reader.read(ptable, file);
		table.importClass(ptable);
		file.close();
	}
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

		const char* seq = "%-END-%\n";
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
}

void Linker::writeDebugSymbols(ostream& outfile) {
	// Write a string like "Printer|b|Champion|c||getName()|b|getParent()|c|write(Num)|d"
	// where || delimits when class symbols become property symbols
	outfile << "var $DBGS=\"";
	const std::map<std::string, std::string> classaddresses = classTable.getAddresses();
	const std::map<std::string, std::string> propaddresses = propertyTable.getAddresses();

	for(std::map<std::string, std::string>::const_iterator it = classaddresses.begin(); it != classaddresses.end(); ++it) {
		outfile << it->first << "|" << it->second << "|";
	}

	for(std::map<std::string, std::string>::const_iterator it = propaddresses.begin(); it != propaddresses.end(); ++it) {
		outfile << "|" << it->first << "|" << it->second;
	}

	outfile << "\";";
}

void Linker::setMain(ostream& file, string classname, string methodname, ClassSpaceSymbolTable& table) {
	file << "try{";

	file << "(";
	generateRecursiveConstructors(file, classname, table);
	file << ").";
	file << propertyTable.getAddress(methodname);
	file << "();";

	file << "}catch(e){";
	file << "if(e.";
	file << propertyTable.getAddress("getStacktrace()");
	file << ")console.log(e.";
	file << propertyTable.getAddress("getStacktrace()");
	file << "());else throw e;}";

	file << "})();";
}

void Linker::generateRecursiveConstructors(ostream& file, string ctedclass, ClassSpaceSymbolTable& table) {
	file << "new ";
	file << "_" << classTable.getAddress(ctedclass);
	file << "(";

	vector<Type*>* needs = table.find(ctedclass)->getNeeds();
	for(vector<Type*>::iterator it = needs->begin(); it != needs->end(); ++it) {
		if(it != needs->begin()) file << ",";
		generateRecursiveConstructors(file, (*it)->typedata._class.classname, table);
	}

	file << ")";
}
