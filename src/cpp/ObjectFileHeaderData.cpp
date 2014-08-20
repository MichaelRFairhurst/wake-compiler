/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * ObjectFileHeaderData.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include <algorithm>
#include "ObjectFileHeaderData.h"

ObjectFileHeaderData::ObjectFileHeaderData() {}
ObjectFileHeaderData::ObjectFileHeaderData(objectfile* object) {
	int i;
	for(i = 0; i < object->classusagecount; i++)
		addClassUsage(object->classusages[i]->pos, object->classusages[i]->symbol);

	for(i = 0; i < object->propertyusagecount; i++)
		addPropertyUsage(object->propertyusages[i]->pos, object->propertyusages[i]->symbol);

	setFilename(object->filename);
}

void ObjectFileHeaderData::addClassUsage(int location, std::string symbol) {
	classusages.push_back(std::pair<int, std::string>(location, symbol));
	std::vector<std::string>::iterator it = find(classes.begin(), classes.end(), symbol);
	if(it == classes.end()) classes.push_back(symbol);
}

void ObjectFileHeaderData::addPropertyUsage(int location, std::string symbol) {
	propertyusages.push_back(std::pair<int, std::string>(location, symbol));
	std::vector<std::string>::iterator it = find(properties.begin(), properties.end(), symbol);
	if(it == properties.end()) properties.push_back(symbol);
}

void ObjectFileHeaderData::setFilename(std::string filename) {
	this->filename = filename;
}

std::string ObjectFileHeaderData::getFilename() {
	return filename;
}

std::vector<std::string> ObjectFileHeaderData::getClasses() {
	return classes;
}

std::vector<std::string> ObjectFileHeaderData::getProperties() {
	return properties;
}

std::vector<std::pair<int, std::string> > ObjectFileHeaderData::getPropertyUsages() {
	return propertyusages;
}

std::vector<std::pair<int, std::string> > ObjectFileHeaderData::getClassUsages() {
	return classusages;
}

std::vector<std::pair<std::string, std::vector<std::string> > > ObjectFileHeaderData::getNeeds() {
	return needs;
}
