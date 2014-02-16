#include <algorithm>
#include "ObjectFileHeaderData.h"

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

void ObjectFileHeaderData::addNeed(std::string classname, std::string need) {
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
