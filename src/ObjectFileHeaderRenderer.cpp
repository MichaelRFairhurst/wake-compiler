#include <iostream>
#include <utility>
#include <vector>
#include "ObjectFileHeaderRenderer.h"
#include "ObjectFileHeaderData.h"

void ObjectFileHeaderRenderer::writeOut(std::ostream& out, ObjectFileHeaderData* data) {
	out << "%-CLASSES-%";
	std::vector<std::string> classes = data->getClasses();
	for(std::vector<std::string>::iterator it = classes.begin(); it != classes.end(); ++it) {
		out << "{" << *it << "} ";
	}

	out << "%-PROPERTIES-%";
	std::vector<std::string> properties = data->getProperties();
	for(std::vector<std::string>::iterator it = properties.begin(); it != properties.end(); ++it) {
		out << "{" << *it << "} ";
	}

	out << "%-USAGES-%";

	int classusei = 0;
	int propertyusei = 0;

	std::vector<std::pair<int, std::string> > classusages = data->getClassUsages();
	std::vector<std::pair<int, std::string> > propertyusages = data->getPropertyUsages();
	while(classusei < classusages.size() || propertyusei < propertyusages.size()) {
		std::pair<int, std::string> pair;

		if(classusei == classusages.size()) {
			pair = propertyusages[propertyusei++];
		} else if(propertyusei == propertyusages.size()) {
			pair = classusages[classusei++];
		} else {
			int classuseloc = classusages[classusei].first;
			int propertyuseloc = propertyusages[propertyusei].first;
			if(classuseloc < propertyuseloc) pair = classusages[classusei++];
			else pair = propertyusages[propertyusei++];
		}

		out << pair.first << " {" << pair.second << "} ";
	}

	out << "%-END-%\n";
}
