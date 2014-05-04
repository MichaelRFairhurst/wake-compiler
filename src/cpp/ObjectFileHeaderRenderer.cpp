#include <iostream>
#include <utility>
#include <vector>
#include "ObjectFileHeaderRenderer.h"
#include "ObjectFileHeaderData.h"

void ObjectFileHeaderRenderer::writeOut(std::ostream& out, ObjectFileHeaderData* data) {
	out << "%-CLASSES-%";
	std::vector<std::pair<int, std::string> > classusages = data->getClassUsages();
	for(std::vector<std::pair<int, std::string> >::iterator it = classusages.begin(); it != classusages.end(); ++it) {
		out << it->first << " %" << it->second << "% ";
	}

	out << "%-PROPERTIES-%";
	std::vector<std::pair<int, std::string> > propertyusages = data->getPropertyUsages();
	for(std::vector<std::pair<int, std::string> >::iterator it = propertyusages.begin(); it != propertyusages.end(); ++it) {
		out << it->first << " %" << it->second << "% ";
	}

	out << "%-END-%\n";
}
