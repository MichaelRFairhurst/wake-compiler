/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ObjectFileHeaderData.h
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#ifndef HEADER_OBJECTFILEHEADERDATA
#define HEADER_OBJECTFILEHEADERDATA

#include <string>
#include <vector>
#include <utility>
#include "objectfile.h"

class ObjectFileHeaderData {

	public:
		ObjectFileHeaderData(objectfile*);
		ObjectFileHeaderData();

		void setFilename(std::string filename);
		std::string getFilename();
		void addClassUsage(int location, std::string symbol);
		void addPropertyUsage(int location, std::string symbol);

		std::vector<std::pair<std::string, std::vector<std::string> > > getNeeds();
		std::vector<std::string> getClasses();
		std::vector<std::string> getProperties();
		std::vector<std::pair<int, std::string> > getPropertyUsages();
		std::vector<std::pair<int, std::string> > getClassUsages();

	private:
		std::string filename;
		std::vector<std::pair<std::string, std::vector<std::string> > > needs;
		std::vector<std::string> classes;
		std::vector<std::string> properties;
		std::vector<std::pair<int, std::string> > propertyusages;
		std::vector<std::pair<int, std::string> > classusages;
};

#endif
