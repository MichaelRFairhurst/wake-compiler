#ifndef HEADER_OBJECTFILEHEADERDATA
#define HEADER_OBJECTFILEHEADERDATA

#include <string>
#include <vector>
#include <utility>

class ObjectFileHeaderData {

	public:
		void addClassUsage(int location, std::string symbol);
		void addPropertyUsage(int location, std::string symbol);
		void addNeed(std::string classname, std::string need);

		std::vector<std::pair<std::string, std::vector<std::string> > > getNeeds();
		std::vector<std::string> getClasses();
		std::vector<std::string> getProperties();
		std::vector<std::pair<int, std::string> > getPropertyUsages();
		std::vector<std::pair<int, std::string> > getClassUsages();

	private:
		std::vector<std::pair<std::string, std::vector<std::string> > > needs;
		std::vector<std::string> classes;
		std::vector<std::string> properties;
		std::vector<std::pair<int, std::string> > propertyusages;
		std::vector<std::pair<int, std::string> > classusages;
};

#endif
